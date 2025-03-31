// copyright
#ifndef TASK_H_
#define TASK_H_

// 任务抽象基类
class Task {
 public:
  // 用户可以自定义任意任务类型, 从Task继承, 重写run方法
  virtual void run() = 0;
};


#endif  // TASK_H_
