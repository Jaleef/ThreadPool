// copyright
#ifndef THREAD_H_
#define THREAD_H_

#include <functional>

using ThreadFunc = std::function<void()>;

class Thread {
 public:
  explicit Thread(ThreadFunc func);

  ~Thread();

  // 启动线程
  void start();

 private:
  ThreadFunc func_;
};

#endif  // THREAD_H_
