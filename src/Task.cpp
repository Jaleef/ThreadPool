// copyright 2025 闫佳乐
#include "Task.h"
#include "Result.h"

Task::Task(): result_(nullptr) {}

void Task::exec() {
  if (result_ != nullptr) {
    result_->setVal(run());   // 这里发生多态调用
  }
}

void Task::setResult(Result *res) {
  result_ = res;
}
