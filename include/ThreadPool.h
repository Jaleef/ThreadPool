// copyright
#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#include <vector>
#include <queue>
#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>


#include "Thread.h"
#include "Task.h"

// 线程池支持的模式
enum class ThreadPoolMode {
  MODE_FIXED,  // Fixed thread number
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

  // 给线程池提交任务
  void submitTask(std::shared_ptr<Task> task);

  // 启动线程池
  void start(int initThreadSize = 4);

  ThreadPool(const ThreadPool &) = delete;  // 禁止拷贝构造函数
  ThreadPool& operator=(const ThreadPool &) = delete;  // 禁止拷贝赋值运算符

 private:
  std::vector<Thread *> threads_{};  // 线程池中的线程列表
  std::size_t initThreadSize_{};  // 线程池的初始线程数

  std::queue<std::shared_ptr<Task>> taskQueue_{};  // 任务队列
  std::atomic<int> taskSize_{};  // 任务队列中的任务数
  int taskQueueMaxThreshold_{};  // 任务队列的最大阈值

  std::mutex taskQueueMutex_{};  // 任务队列的互斥锁
  std::condition_variable notFull_{};  // 任务队列非满条件变量
  std::condition_variable notEmpty_{};  // 任务队列非空条件变量

  ThreadPoolMode poolMode_;  // 线程池的模式
};

#endif  // THREADPOOL_H_
