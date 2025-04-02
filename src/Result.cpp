// copyright
#include "Result.h"
#include "Task.h"

#include <utility>

Result::Result(std::shared_ptr<Task> task, bool isValid)
    : task_(task), isValid_(isValid) {
  task_->setResult(this);
}

Any Result::get() {
  if (!isValid_) {
    return Any("");
  }

  sem_.wait();  // task任务如果没有执行完, 这里会阻塞用户线程
  return std::move(any_);
}

void Result::setVal(Any any) {
  // 存储task返回值
  this->any_ = std::move(any);
  sem_.post();  // 已经获取任务的返回值, 增加信号量资源
}
