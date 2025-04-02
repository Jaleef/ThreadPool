// copyright
#ifndef TASK_H_
#define TASK_H_

#include "Any.h"

class Result;

// 任务抽象基类
class Task {
 public:
  Task();
  ~Task() = default;

  // 用户可以自定义任意任务类型, 从Task继承, 重写run方法
  virtual Any run() = 0;

  void exec();

  void setResult(Result *res);

 private:
  Result *result_;  // Result对象的生命周期 远大于 Task的
};

#endif  // TASK_H_
