#ifndef TEST_HEAD2_H
#define TEST_HEAD2_H

// #include "test_head1.h"
// 前置声明，减少头文件依赖

class TestHead1;

class TestHead2 {
 public:
  TestHead2();

 protected:
  //  使用成员变量指针导入，而不是通过继承调用其成员接口
  //  默认需要初始化为`nullptr`
  TestHead1* th1 = nullptr;
};

#endif  // TEST_HEAD2_H
