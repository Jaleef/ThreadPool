// Copyright 2024
#include <chrono>
#include <iostream>
#include <thread>

#include "Task.h"
#include "ThreadPool.h"

class MyTask : public Task {
 public:
  MyTask(int begin, int end) : begin_(begin), end_(end) {}

  // 如何设计run函数的返回值, 可以返回任意类型
  void run() {
    std::cout << "begin threadFunc tid: " << std::this_thread::get_id()
              << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(10));
    std::cout << "end threadFunc tid: " << std::this_thread::get_id()
              << std::endl;
  }

 private:
  int begin_;
  int end_;
};
int main(int argc, char* argv[]) {
  ThreadPool pool;
  pool.start(4);

  pool.submitTask(std::make_shared<MyTask>());
  pool.submitTask(std::make_shared<MyTask>());
  pool.submitTask(std::make_shared<MyTask>());
  pool.submitTask(std::make_shared<MyTask>());

  pool.submitTask(std::make_shared<MyTask>());
  pool.submitTask(std::make_shared<MyTask>());
  pool.submitTask(std::make_shared<MyTask>());
  pool.submitTask(std::make_shared<MyTask>());

  pool.submitTask(std::make_shared<MyTask>());
  pool.submitTask(std::make_shared<MyTask>());
  pool.submitTask(std::make_shared<MyTask>());
  pool.submitTask(std::make_shared<MyTask>());

  getchar();
  return 0;
}
