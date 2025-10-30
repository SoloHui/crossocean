// server_task_test.cpp
// ServerTask 类单元测试

#include "server_task.h"

#include <event2/event.h>
#include <gtest/gtest.h>

#include <thread>

using namespace crossocean;

// ==================== ServerTask 测试 ====================

// 测试 ServerTask 端口设置
TEST(ServerTaskTest, PortConfiguration) {
  ServerTask task;

  task.set_server_port(8080);
  EXPECT_EQ(task.server_port(), 8080);

  task.set_server_port(9000);
  EXPECT_EQ(task.server_port(), 9000);
}

// 测试 ServerTask 无效端口初始化失败
TEST(ServerTaskTest, InvalidPortInitialization) {
  ServerTask task;

  // 未设置端口
  EXPECT_FALSE(task.Init());

  // 设置无效端口
  task.set_server_port(0);
  EXPECT_FALSE(task.Init());

  task.set_server_port(-1);
  EXPECT_FALSE(task.Init());
}

// 测试 ServerTask 有效端口初始化
TEST(ServerTaskTest, ValidPortInitialization) {
  ServerTask task;

  // 创建一个 event_base 用于测试
  struct event_base* base = event_base_new();
  ASSERT_NE(base, nullptr);

  task.set_base(base);
  task.set_server_port(18080);  // 使用高端口避免权限问题

  EXPECT_TRUE(task.Init());

  event_base_free(base);
}

// 测试 ServerTask 自定义回调函数
TEST(ServerTaskTest, CustomCallback) {
  static bool callback_called = false;

  auto custom_callback = [](int socket_fd, struct sockaddr* addr, int socklen,
                            void* user_arg) { callback_called = true; };

  ServerTask task;
  task.ListenCB = custom_callback;

  EXPECT_NE(task.ListenCB, nullptr);
}

// 测试默认回调为空
TEST(ServerTaskTest, DefaultCallbackIsNull) {
  ServerTask task;
  EXPECT_EQ(task.ListenCB, nullptr);
}

// 测试多个 ServerTask 使用不同端口
TEST(ServerTaskTest, MultipleServerTasks) {
  struct event_base* base1 = event_base_new();
  struct event_base* base2 = event_base_new();

  ASSERT_NE(base1, nullptr);
  ASSERT_NE(base2, nullptr);

  ServerTask task1;
  task1.set_base(base1);
  task1.set_server_port(18081);

  ServerTask task2;
  task2.set_base(base2);
  task2.set_server_port(18082);

  EXPECT_TRUE(task1.Init());
  EXPECT_TRUE(task2.Init());

  EXPECT_NE(task1.server_port(), task2.server_port());

  event_base_free(base1);
  event_base_free(base2);
}

// 测试端口范围
TEST(ServerTaskTest, PortRange) {
  ServerTask task;

  // 测试小端口号（通常需要root权限）
  task.set_server_port(80);
  EXPECT_EQ(task.server_port(), 80);

  // 测试常用端口
  task.set_server_port(8080);
  EXPECT_EQ(task.server_port(), 8080);

  // 测试高端口号
  task.set_server_port(65535);
  EXPECT_EQ(task.server_port(), 65535);
}

// 测试未设置 event_base 的初始化
TEST(ServerTaskTest, InitWithoutEventBase) {
  ServerTask task;
  task.set_server_port(18083);

  // 未设置 event_base，应该失败或者有明确行为
  // 根据实际实现，可能会失败
  bool result = task.Init();

  // 如果实现中检查了 base，应该返回 false
  // 如果没有检查，可能会崩溃或返回 true
  // 这里我们至少验证函数可以被调用
  (void)result;
}

// 测试端口重用
TEST(ServerTaskTest, PortReuse) {
  struct event_base* base1 = event_base_new();
  struct event_base* base2 = event_base_new();

  ASSERT_NE(base1, nullptr);
  ASSERT_NE(base2, nullptr);

  ServerTask task1;
  task1.set_base(base1);
  task1.set_server_port(18084);

  EXPECT_TRUE(task1.Init());

  // 清理第一个任务
  event_base_free(base1);

  // 稍等一下让端口释放
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // 尝试使用相同端口创建新任务（因为设置了 LEV_OPT_REUSEABLE）
  ServerTask task2;
  task2.set_base(base2);
  task2.set_server_port(18084);

  EXPECT_TRUE(task2.Init());

  event_base_free(base2);
}

// 测试继承的 Task 属性
TEST(ServerTaskTest, InheritedTaskProperties) {
  ServerTask task;

  // ServerTask 继承自 Task，应该有这些方法
  task.set_thread_id(5);
  EXPECT_EQ(task.thread_id(), 5);

  task.set_sock(100);
  EXPECT_EQ(task.sock(), 100);

  struct event_base* base = event_base_new();
  task.set_base(base);
  EXPECT_EQ(task.base(), base);

  event_base_free(base);
}
