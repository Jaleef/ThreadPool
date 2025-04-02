// copyright
#ifndef ANY_H_
#define ANY_H_

#include <memory>

class Any {
 public:
  // 默认构造函数
  Any() = default;
  ~Any() = default;

  // 禁止拷贝构造函数和拷贝赋值函数
  Any(const Any&) = delete;
  Any& operator=(const Any&) = delete;

  // 默认移动构造函数
  Any(Any&&) = default;
  Any& operator=(Any&&) = default;

  template <typename T>
  explicit Any(T data) : base_{std::make_unique<Derive<T>>(data)} {}

  template <typename T>
  T cast_() {
    // 怎么从base_中找到它所指向的Derive对象并取出data成员变量
    // 基类指针 转为 派生类指针
    Derive<T>* pd = dynamic_cast<Derive<T>*>(base_.get());
    if (pd == nullptr) {
      throw "type incompatible!";
    }
    return pd->data_;
  }

 private:
  // 基类类型
  class Base {
   public:
    virtual ~Base() = default;
  };

  // 派生类类型
  template <typename T>
  class Derive : public Base {
   public:
    explicit Derive(T data) : data_{data} {}

    T data_;
  };

 private:
  std::unique_ptr<Base> base_;
};

#endif  // ANY_H_
