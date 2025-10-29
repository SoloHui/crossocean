// main.cpp
// 测试主入口

#include <event2/thread.h>
#include <gtest/gtest.h>

// 测试主函数
// argc: 命令行参数数量
// argv: 命令行参数数组
// 返回: 测试结果（0表示成功）
int main(int argc, char** argv) {
#ifdef _WIN32
  // Windows 下初始化 libevent 的多线程支持
  evthread_use_windows_threads();
#else
  // Linux 下初始化 libevent 的多线程支持
  evthread_use_pthreads();
#endif

  // 初始化 Google Test
  ::testing::InitGoogleTest(&argc, argv);

  // 运行所有测试
  return RUN_ALL_TESTS();
}
