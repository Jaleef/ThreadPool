// copyright
#ifndef THREAD_H_
#define THREAD_H_

#include <functional>

using ThreadFunc = std::function<void(int)>;

class Thread {
 public:
  explicit Thread(ThreadFunc func);

  ~Thread();

  // 启动线程
  void start();

  // 获取线程id
  int getThreadId() const;

 private:
  ThreadFunc func_;
  static int generatedId_;  // 线程ID的生成器
  int threadId_;            // 线程ID
};

#endif  // THREAD_H_
