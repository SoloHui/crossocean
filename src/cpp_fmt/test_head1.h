/*
 * 解决头文件在一个`cpp`中多次引用
 */
#ifndef TEST_HEAD1_H
#define TEST_HEAD1_H
// 头文件中尽量不要使用`using namespace std;`，减少命名空间污染
#include <string>

#include "i_thread.h"

/**
 * @brief 声明次序
 *
 * 1. 构造函数
 * 2. 析构函数
 * 3. 其他公有成员函数
 * 4. 保护成员函数
 * 5. 私有成员函数
 * 6. 公有成员变量
 * 7. 保护成员变量
 * 8. 私有成员变量
 *
 */

// 结构体当中不包含方法，如果要用方法换成类
struct MyStruct {
  int x = 0;
  int y = 0;
};

class TestHead1 : public IThread {
 public:
  // 构造函数
  TestHead1(const std::string& name, int x1);

  // 析构函数
  ~TestHead1() override = default;

  // 初始化函数（业务相关）
  bool Init();
  // 其他公有成员函数
  void Execute() override;  // 实现接口类的纯虚函数
  void AddServer(const std::string& server);

  // 数据成员
  int index_ = 0;

  // 成员读写函数
  int set_x1(int x) { return x1_ = x; }
  int x1() const { return x1_; }
  void set_name(const std::string& name) { name_ = name; }
  const std::string& name() const { return name_; }

 protected:
  int x1_;

 private:
  //  使用`std::`前缀，明确标识命名空间
  std::string name_;
};

#endif  // TEST_HEAD1_H
