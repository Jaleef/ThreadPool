// copyright
#include <functional>
#include <iostream>
#include <thread>
#include <memory>

#include "ThreadPool.h"

constexpr int TASK_MAX_THRESHOLD = 1024;  // 任务队列的最大阈值

ThreadPool::ThreadPool() :
  initThreadSize_{4},
  taskSize_{0},
  taskQueueMaxThreshold_{TASK_MAX_THRESHOLD},
  poolMode_{ThreadPoolMode::MODE_FIXED}
{}

ThreadPool::~ThreadPool()
{}

void ThreadPool::setThreadPoolMode(ThreadPoolMode mode) {
  poolMode_ = mode;
}

void ThreadPool::setInitThreadSize(int size) {
  initThreadSize_ = size;
}

void ThreadPool::setTaskQueueMaxThreshold(int threshold) {
  taskQueueMaxThreshold_ = threshold;
}

void ThreadPool::submitTask(std::shared_ptr<Task> task) {
}

void ThreadPool::start(int initThreadSize) {
  // 设置初始线程个数
  setInitThreadSize(initThreadSize);

  // 创建所有的线程
  for (int i = 0 ; i < initThreadSize_ ; ++i) {
    threads_.emplace_back(
      std::make_unique<Thread>([this] { this->threadFunc(); }));
  }

  // 启动所有线程
  for (int i = 0 ; i < initThreadSize_ ; ++i) {
    threads_[i]->start();
  }
}

void ThreadPool::threadFunc() {
  std::cout << "begin thread function" << std::endl;
  std::cout << std::this_thread::get_id() << std::endl;
  std::cout << "end thread function" << std::endl;
}
