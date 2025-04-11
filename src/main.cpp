// Copyright 2024
#include <chrono>
#include <iostream>
#include <thread>

#include "Any.h"
#include "Task.h"
#include "ThreadPool.h"

class MyTask : public Task {
 public:
  MyTask(int begin, int end) : begin_(begin), end_(end) {}

  // 如何设计run函数的返回值, 可以返回任意类型
  Any run() {
    std::this_thread::sleep_for(std::chrono::seconds(5));
    uint32_t sum;
    for (int i = begin_; i < end_; ++i) {
      sum += i;
    }
    std::cout << sum << std::endl;
    return Any(sum);
  }

 private:
  int begin_;
  int end_;
};
int main(int argc, char* argv[]) {
  ThreadPool pool;
  // pool.setThreadPoolMode(ThreadPoolMode::MODE_CACHED);

  pool.start(4);

  uint32_t sum = 0;
  Result res1 = pool.submitTask(std::make_shared<MyTask>(1, 10000));

  // Result res2 = pool.submitTask(std::make_shared<MyTask>(10001, 20000));

  // Result res3 = pool.submitTask(std::make_shared<MyTask>(20001, 30000));

  // pool.submitTask(std::make_shared<MyTask>(20001, 30000));
  // pool.submitTask(std::make_shared<MyTask>(20001, 30000));
  // pool.submitTask(std::make_shared<MyTask>(20001, 30000));

  std::cout << "main over" << std::endl;
  // std::this_thread::sleep_for(std::chrono::seconds(100));
  return 0;
}
