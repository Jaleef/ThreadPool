// copyright
#include <thread>

#include "Thread.h"

void Thread::start() {
  // 创建一个线程来执行一个线程函数
  std::thread t(func_);
  t.detach();  // 分离线程
}

Thread::Thread(ThreadFunc func):
  func_{func} {}
