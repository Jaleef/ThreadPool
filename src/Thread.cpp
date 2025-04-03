// copyright
#include "Thread.h"

#include <thread>

int Thread::generatedId_ = 0;  // 静态成员变量初始化

void Thread::start() {
  // 创建一个线程来执行一个线程函数
  std::thread t(func_, threadId_);
  t.detach();  // 分离线程
}

Thread::Thread(ThreadFunc func) : func_{func}, threadId_{generatedId_++} {}

Thread::~Thread() {}

int Thread::getThreadId() const { return threadId_; }
