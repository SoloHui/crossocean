#ifndef I_THREAD_H
#define I_THREAD_H

// 接口类 以`I`开头
class IThread {
 public:
  // 接口类只有纯虚函数和静态成员函数
  virtual ~IThread() = default;
  virtual void Execute() = 0;  // 纯虚函数
};

#endif  // I_THREAD_H
