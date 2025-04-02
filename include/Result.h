// copyright
#ifndef RESULT_H_
#define RESULT_H_
#include <atomic>
#include <memory>

#include "Any.h"
#include "Semaphore.h"

class Task;

// 实现接收提交到线程池的task任务执行完后的返回值类型Result
class Result {
 public:
  explicit Result(std::shared_ptr<Task> task, bool isValid = true);

  ~Result() = default;

  // setVal方法, 获取任务执行完的返回值
  void setVal(Any any);

  // get方法, 用户调用这个方法获取task的返回值
  Any get();

 private:
  Any any_;                     // 存储任务的返回值
  Semaphore sem_;               // 线程间通信的信号量
  std::shared_ptr<Task> task_;  // 指向对应获取返回值的任务对象
  std::atomic<bool> isValid_;   // 返回值是否有效
};

#endif  // RESULT_H_
