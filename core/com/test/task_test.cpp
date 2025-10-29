// task_test.cpp
// Task 类单元测试

#include "task.h"

#include <gtest/gtest.h>

using namespace crossocean;

// 测试用的简单任务类
class SimpleTask : public Task {
 public:
  bool Init() override {
    init_called_ = true;
    return true;
  }

  bool IsInitCalled() const { return init_called_; }

 private:
  bool init_called_ = false;
};

// 测试用的失败任务类
class FailTask : public Task {
 public:
  bool Init() override { return false; }
};

// 测试用的计数任务类
class CounterTask : public Task {
 public:
  bool Init() override {
    counter_++;
    return true;
  }

  static int GetCounter() { return counter_; }
  static void ResetCounter() { counter_ = 0; }

 private:
  static int counter_;
};

int CounterTask::counter_ = 0;

// ==================== Task 测试 ====================

// 测试 Task 基本属性的 getter 和 setter
TEST(TaskTest, GettersAndSetters) {
  SimpleTask task;

  // 测试 thread_id
  task.set_thread_id(5);
  EXPECT_EQ(task.thread_id(), 5);

  // 测试 sock
  task.set_sock(100);
  EXPECT_EQ(task.sock(), 100);

  // 测试 base
  struct event_base* base = (struct event_base*)0x1234;
  task.set_base(base);
  EXPECT_EQ(task.base(), base);
}

// 测试 Task 的初始化
TEST(TaskTest, Initialization) {
  SimpleTask task;
  EXPECT_TRUE(task.Init());
  EXPECT_TRUE(task.IsInitCalled());
}

// 测试失败任务
TEST(TaskTest, FailedInitialization) {
  FailTask task;
  EXPECT_FALSE(task.Init());
}

// 测试默认值
TEST(TaskTest, DefaultValues) {
  SimpleTask task;

  // 默认值应该为0或nullptr
  EXPECT_EQ(task.thread_id(), 0);
  EXPECT_EQ(task.sock(), 0);
  EXPECT_EQ(task.base(), nullptr);
}

// 测试 thread_id 边界值
TEST(TaskTest, ThreadIdBoundaryValues) {
  SimpleTask task;

  // 测试负数
  task.set_thread_id(-1);
  EXPECT_EQ(task.thread_id(), -1);

  // 测试零
  task.set_thread_id(0);
  EXPECT_EQ(task.thread_id(), 0);

  // 测试正数
  task.set_thread_id(999);
  EXPECT_EQ(task.thread_id(), 999);
}

// 测试 socket 边界值
TEST(TaskTest, SocketBoundaryValues) {
  SimpleTask task;

  // 测试负数（无效socket）
  task.set_sock(-1);
  EXPECT_EQ(task.sock(), -1);

  // 测试零
  task.set_sock(0);
  EXPECT_EQ(task.sock(), 0);

  // 测试大数值
  task.set_sock(65535);
  EXPECT_EQ(task.sock(), 65535);
}

// 测试多次初始化
TEST(TaskTest, MultipleInitializations) {
  CounterTask::ResetCounter();

  CounterTask task;
  EXPECT_EQ(CounterTask::GetCounter(), 0);

  task.Init();
  EXPECT_EQ(CounterTask::GetCounter(), 1);

  task.Init();
  EXPECT_EQ(CounterTask::GetCounter(), 2);

  task.Init();
  EXPECT_EQ(CounterTask::GetCounter(), 3);
}

// 测试属性链式设置
TEST(TaskTest, ChainedSetters) {
  SimpleTask task;

  task.set_thread_id(10);
  task.set_sock(200);
  task.set_base((struct event_base*)0x5678);

  EXPECT_EQ(task.thread_id(), 10);
  EXPECT_EQ(task.sock(), 200);
  EXPECT_EQ(task.base(), (struct event_base*)0x5678);
}
