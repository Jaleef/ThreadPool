// copyright
#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

#include <condition_variable>
#include <mutex>

class Semaphore {
 public:
  explicit Semaphore(int limit = 0) : resLimit_(0) {}

  ~Semaphore() = default;

  // 获取一个信号量资源
  void wait();

  // 增加一个信号量资源
  void post();

 private:
  int resLimit_;                  // 资源限制
  std::mutex mutex_;              // 互斥锁
  std::condition_variable cond_;  // 条件变量
};

#endif  // SEMAPHORE_H_
