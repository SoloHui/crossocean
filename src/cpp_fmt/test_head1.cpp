#include "test_head1.h"

// 源文件实现可以导入命名空间
using namespace std;

TestHead1::TestHead1(const string& name, int x1) : name_(name), x1_(x1) {
  // 不做实际意义的初始化，与业务逻辑无关

  // Init(); 函数实现业务逻辑初始化
}

bool TestHead1::Init() {
  // 业务相关的初始化
  index_ = 100;
  return true;  // 初始化成功
}

// 实现接口类的纯虚函数
void TestHead1::Execute() {}

// 其他成员函数的实现
void TestHead1::AddServer(const string& server) {
  name_ += server;  // 仅做示例
}
