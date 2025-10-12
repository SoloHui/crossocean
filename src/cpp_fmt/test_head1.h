/*
 * 解决头文件在一个`cpp`中多次引用
 */
#ifndef TEST_HEAD1_H
#define TEST_HEAD1_H
// 头文件中尽量不要使用`using namespace std;`，减少命名空间污染
#include <string>

class TestHead1 {
 public:
  TestHead1();

 protected:
  //  使用`std::`前缀，明确标识命名空间
  std::string name;

  int x1;
};

#endif  // TEST_HEAD1_H
