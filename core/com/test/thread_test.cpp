// thread_test.cpp
// Thread 类单元测试

#include "thread.h"

#include <gtest/gtest.h>

#include <chrono>
#include <thread>
#include <vector>

#include "task.h"

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

// ==================== Thread 测试 ====================

// 测试线程对象的创建和基本功能
TEST(ThreadTest, CreateAndSetup) {
  Thread thread;
  thread.id_ = 1;

  // 测试线程安装
  EXPECT_TRUE(thread.Setup());
}

// 测试向线程添加任务
TEST(ThreadTest, AddTask) {
  Thread thread;
  thread.id_ = 1;
  thread.Setup();

  SimpleTask* task = new SimpleTask();
  thread.AddTask(task);

  // 测试任务的 thread_id 是否正确设置
  EXPECT_EQ(task->thread_id(), 1);
  EXPECT_NE(task->base(), nullptr);

  delete task;
}

// 测试线程激活功能
TEST(ThreadTest, ActivateThread) {
  Thread thread;
  thread.id_ = 1;
  thread.Setup();
  thread.Start();

  SimpleTask* task = new SimpleTask();
  thread.AddTask(task);

  // 激活线程处理任务
  thread.Activate();

  // 等待线程处理任务
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // 任务应该被初始化
  EXPECT_TRUE(task->IsInitCalled());

  delete task;
}

// 测试多个任务的处理
TEST(ThreadTest, MultipleTasksProcessing) {
  Thread thread;
  thread.id_ = 1;
  thread.Setup();
  thread.Start();

  const int task_count = 5;
  std::vector<SimpleTask*> tasks;

  for (int i = 0; i < task_count; ++i) {
    SimpleTask* task = new SimpleTask();
    tasks.push_back(task);
    thread.AddTask(task);
  }

  // 激活线程处理任务
  for (int i = 0; i < task_count; ++i) {
    thread.Activate();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  // 等待所有任务处理完成
  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  // 检查所有任务是否都被初始化
  for (auto* task : tasks) {
    EXPECT_TRUE(task->IsInitCalled());
    delete task;
  }
}

// 测试线程 ID 设置
TEST(ThreadTest, ThreadIdSetting) {
  Thread thread;
  thread.id_ = 42;

  EXPECT_EQ(thread.id_, 42);

  thread.id_ = 100;
  EXPECT_EQ(thread.id_, 100);
}

// 测试空任务添加
TEST(ThreadTest, AddNullTask) {
  Thread thread;
  thread.id_ = 1;
  thread.Setup();

  // 添加空任务不应该崩溃
  thread.AddTask(nullptr);

  // 线程仍应该可以正常工作
  SimpleTask* task = new SimpleTask();
  thread.AddTask(task);
  EXPECT_EQ(task->thread_id(), 1);

  delete task;
}
