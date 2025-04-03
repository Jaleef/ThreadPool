// copyright
#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <unordered_map>
#include <vector>

#include "Result.h"
#include "Task.h"
#include "Thread.h"

// 线程池支持的模式
enum class ThreadPoolMode {
  MODE_FIXED,   // Fixed thread number
  MODE_CACHED,  // dynamic thread number
};

class ThreadPool {
 public:
  ThreadPool();
  ~ThreadPool();

  // 设置线程池的模式
  void setThreadPoolMode(ThreadPoolMode mode);

  // 设置task任务队列上限阈值
  void setTaskQueueMaxThreshold(int threshold);

  // 设置线程池的初始线程数
  void setInitThreadSize(int size);

  // 设置cache模式下线程个数的最大阈值
  void setThreadMaxThreshold(int threshold);

  // 给线程池提交任务
  Result submitTask(std::shared_ptr<Task> task);

  // 启动线程池
  void start(int initThreadSize = 4);

  ThreadPool(const ThreadPool &) = delete;  // 禁止拷贝构造函数
  ThreadPool &operator=(const ThreadPool &) = delete;  // 禁止拷贝赋值运算符

 private:
  // 定义线程函数
  void threadFunc(int threadId = 0);

  // 检查pool的运行状态
  bool checkRunningState() const;

 private:
  // std::vector<std::unique_ptr<Thread>> threads_{};  // 线程池中的线程列表
  std::unordered_map<int, std::unique_ptr<Thread>> threads_;  // 线程列表
  std::size_t initThreadSize_{};          // 线程池的初始线程数
  std::atomic<int> threadCurrentSize_{};  // 线程池当前的线程数
  std::atomic<int> idleThreadSize_{};     // 线程池中空闲线程的数量
  int threadMaxThreshold_{};              // 线程池的最大线程数

  std::queue<std::shared_ptr<Task>> taskQueue_{};  // 任务队列
  std::atomic<int> taskSize_{};  // 任务队列中的任务数
  int taskQueueMaxThreshold_{};  // 任务队列的最大阈值

  std::mutex taskQueueMutex_{};         // 任务队列的互斥锁
  std::condition_variable notFull_{};   // 任务队列非满条件变量
  std::condition_variable notEmpty_{};  // 任务队列非空条件变量

  std::condition_variable exitCond_{};  // 等待线程资源全部回收

  ThreadPoolMode poolMode_;         // 线程池的模式
  std::atomic<bool> isPoolRunning_;  //  表示当前线程池的启动状态
};

#endif  // THREADPOOL_H_
