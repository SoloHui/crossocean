// main.cpp
// 测试主入口
#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#else
#include <signal.h>
#endif

#include <event2/thread.h>
#include <gtest/gtest.h>

// 测试主函数
// argc: 命令行参数数量
// argv: 命令行参数数组
// 返回: 测试结果（0表示成功）
int main(int argc, char** argv) {
#ifdef _WIN32
  // 初始化`socket`
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);
#else
  // 忽略`SIGPIPE`信号, 防止程序因向已关闭的socket写数据而终止
  if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
    std::cerr << "main(): signal SIGPIPE error" << std::endl;
    return -1;
  }
#endif

  // 初始化 Google Test
  ::testing::InitGoogleTest(&argc, argv);

  // 运行所有测试
  return RUN_ALL_TESTS();
}
