// copyright
#include "ThreadPool.h"

#include <functional>
#include <iostream>
#include <memory>
#include <thread>
#include <utility>

constexpr int TASK_MAX_THRESHOLD = 1024;  // 默认任务队列的最大阈值
constexpr int MAX_THREAD_SIZE = 10;       // 默认线程池的最大线程数
constexpr int THREAD_MAX_IDLE_TIME = 10;  // 默认线程的最大空闲时间

ThreadPool::ThreadPool()
    : initThreadSize_{4},
      taskSize_{0},
      threadMaxThreshold_{MAX_THREAD_SIZE},
      threadCurrentSize_{0},
      taskQueueMaxThreshold_{TASK_MAX_THRESHOLD},
      poolMode_{ThreadPoolMode::MODE_FIXED},
      isPoolRunning{false},
      idleThreadSize_{0} {}

ThreadPool::~ThreadPool() {
  // 线程池析构函数
  isPoolRunning = false;
  notEmpty_.notify_all();  // 唤醒所有的线程

  // 等待线程池里所有的线程返回
  // 有两种状态: 阻塞 & 执行任务中
  std::unique_lock<std::mutex> lock(taskQueueMutex_);
  exitCond_.wait(lock, [this]() -> bool { return threads_.size() == 0; });
}

void ThreadPool::setThreadPoolMode(ThreadPoolMode mode) {
  if (checkRunningState()) {
    std::cerr << "线程池已经启动, 不能修改线程池的模式" << std::endl;
    return;
  }
  poolMode_ = mode;
}

void ThreadPool::setInitThreadSize(int size) {
  if (checkRunningState()) {
    std::cerr << "线程池已经启动, 不能修改初始线程数" << std::endl;
    return;
  }
  initThreadSize_ = size;
}

void ThreadPool::setTaskQueueMaxThreshold(int threshold) {
  if (checkRunningState()) {
    std::cerr << "线程池已经启动, 不能修改任务队列的最大阈值" << std::endl;
    return;
  }
  taskQueueMaxThreshold_ = threshold;
}

void ThreadPool::setThreadMaxThreshold(int threshold) {
  if (checkRunningState()) {
    std::cerr << "线程池已经启动, 不能修改线程池的最大线程数" << std::endl;
    return;
  }
  if (poolMode_ != ThreadPoolMode::MODE_CACHED) {
    std::cerr << "当前线程池的模式不是cache, 不能设置最大线程数" << std::endl;
    return;
  }
  threadMaxThreshold_ = threshold;
}

Result ThreadPool::submitTask(std::shared_ptr<Task> task) {
  // 获取锁
  std::unique_lock<std::mutex> lock(taskQueueMutex_);

  // 线程的通信 等待任务队列有空余
  // 用户提交任务, 最长不阻塞超过1s, 否则判断提交任务失败
  if (!notFull_.wait_for(lock, std::chrono::seconds(1), [this]() -> bool {
        return taskQueue_.size() < taskQueueMaxThreshold_;
      })) {
    // 表示notFull_等待1s后条件仍没有满足
    std::cerr << "task queue is full, submit task fail." << std::endl;
    return Result(task, false);
  }

  // 如果有空余 把任务放入任务队列中
  taskQueue_.emplace(task);
  ++taskSize_;

  // 因为新放了任务 任务队列肯定不空了 在notEmpty_上通知一个线程
  notEmpty_.notify_all();

  // cache模式, 任务处理比较紧急, 场景：小而快的任务
  // 需要根据任务数量和空闲线程数量，判断是否需要创建新的线程出来
  if (poolMode_ == ThreadPoolMode::MODE_CACHED && taskSize_ > idleThreadSize_ &&
      threadCurrentSize_ < threadMaxThreshold_) {
    std::cout << "create new thread" << std::endl;

    // 创建新的线程
    auto ptr = std::make_unique<Thread>(
        [this](int threadId) { this->threadFunc(threadId); });
    int threadId = ptr->getThreadId();
    threads_.emplace(threadId, std::move(ptr));

    // 启动线程
    threads_[threadId]->start();

    // 线程启动成功, 当前线程数量和空闲线程数量加1
    threadCurrentSize_++;
    idleThreadSize_++;
  }

  // 返回任务的Result对象
  return Result(task);
}

void ThreadPool::start(int initThreadSize) {
  // 设置初始线程个数
  setInitThreadSize(initThreadSize);
  threadCurrentSize_ = initThreadSize_;

  // 设置线程池的运行状态
  isPoolRunning = true;

  // 创建所有的线程
  for (int i = 0; i < initThreadSize_; ++i) {
    auto ptr = std::make_unique<Thread>(
        [this](int threadId) { this->threadFunc(threadId); });
    int threadId = ptr->getThreadId();
    threads_.emplace(threadId, std::move(ptr));
  }

  // 启动所有线程
  for (auto& thread : threads_) {
    thread.second->start();
    // 线程启动成功, 空闲线程数量加1
    idleThreadSize_++;
  }
}

void ThreadPool::threadFunc(int threadId) {
  // 记录上一次线程执行的时间
  auto lastTime = std::chrono::high_resolution_clock::now();

  while (isPoolRunning) {
    std::shared_ptr<Task> task;
    {
      // 先获取锁
      std::unique_lock<std::mutex> lock(taskQueueMutex_);

      std::cout << "tid: " << std::this_thread::get_id() << "尝试获取任务..."
                << std::endl;

      // cache模式下, 创建过多的线程需要回收
      // 超过initThreadSize_的线程, 需要回收
      // 按照 当前时间 - 上一次线程执行的事件 > 60s的标准来判断回收
      // 每一秒返回一次
      while (taskQueue_.size() == 0) {
        if (poolMode_ == ThreadPoolMode::MODE_CACHED) {
          // 条件变量, 超时返回了
          if (std::cv_status::timeout ==
              notEmpty_.wait_for(lock, std::chrono::seconds(1))) {
            auto now = std::chrono::high_resolution_clock::now();
            auto dur = std::chrono::duration_cast<std::chrono::seconds>(
                now - lastTime);

            // 如果当前线程的空闲时间超过60s, 需要回收
            if (dur.count() >= THREAD_MAX_IDLE_TIME &&
                threadCurrentSize_ > initThreadSize_) {
              // 回收线程
              // 把线程对象从线程列表容器中删除
              // 记录线程数量相关变量的值修改
              threads_.erase(threadId);

              threadCurrentSize_--;
              idleThreadSize_--;

              std::cout << "threadid: " << std::this_thread::get_id() << " exit"
                        << std::endl;
              // 退出线程
              return;
            }
          }
        } else {
          // 非cache模式, 等待notEmpty条件
          notEmpty_.wait(lock,
                         [this]() -> bool { return taskQueue_.size() > 0; });
        }

        // 线程池要结束, 回收线程资源
        if (!isPoolRunning) {
          break;
        }
      }

      // 取出一个任务后空闲线程数量减1
      idleThreadSize_--;

      std::cout << "tid: " << std::this_thread::get_id() << "获取任务成功..."
                << std::endl;

      // 从任务队列中取出一个任务出来
      task = taskQueue_.front();
      taskQueue_.pop();
      taskSize_--;

      // 如果依然有剩余任务, 继续通知其他线程执行任务
      if (taskQueue_.size() > 0) {
        notEmpty_.notify_all();
      }

      // 取出一个任务, 进行通知
      notFull_.notify_all();
    }

    // 当前线程负责执行这个任务
    if (task != nullptr) {
      task->exec();
    }

    // 执行完任务后, 线程数量加1
    idleThreadSize_++;

    // 更新上次执行时间
    lastTime = std::chrono::high_resolution_clock::now();
  }

  // 线程池要结束, 回收线程资源
  threads_.erase(threadId);
  std::cout << "threadid: " << std::this_thread::get_id() << " exit"
            << std::endl;
  exitCond_.notify_all();  // 唤醒所有的exitCond条件变量阻塞的线程
}

bool ThreadPool::checkRunningState() const { return isPoolRunning; }
