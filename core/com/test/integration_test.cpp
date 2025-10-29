// integration_test.cpp
// 集成测试

#include <gtest/gtest.h>

#include <chrono>
#include <thread>
#include <vector>

#include "include/thread_pool.h"
#include "server_task.h"
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

// ==================== 集成测试 ====================

// 集成测试：线程池 + ServerTask
TEST(IntegrationTest, ThreadPoolWithServerTask) {
  ThreadPool* pool = ThreadPool::GetInstance();
  pool->Init(2);

  ServerTask* task = new ServerTask();
  task->set_server_port(18085);

  pool->Dispatch(task);

  // 等待任务处理
  std::this_thread::sleep_for(std::chrono::milliseconds(300));

  EXPECT_GT(task->thread_id(), 0);
  EXPECT_NE(task->base(), nullptr);

  delete task;
}

// 集成测试：并发任务处理
TEST(IntegrationTest, ConcurrentTaskProcessing) {
  ThreadPool* pool = ThreadPool::GetInstance();
  pool->Init(4);

  const int task_count = 12;
  std::vector<SimpleTask*> tasks;

  // 快速分发大量任务
  for (int i = 0; i < task_count; ++i) {
    SimpleTask* task = new SimpleTask();
    tasks.push_back(task);
    pool->Dispatch(task);
  }

  // 等待所有任务处理完成
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  // 验证所有任务都被处理
  int success_count = 0;
  for (auto* task : tasks) {
    if (task->IsInitCalled()) {
      success_count++;
    }
    delete task;
  }

  EXPECT_EQ(success_count, task_count);
}

// 集成测试：混合任务类型
TEST(IntegrationTest, MixedTaskTypes) {
  ThreadPool* pool = ThreadPool::GetInstance();
  pool->Init(3);

  std::vector<Task*> tasks;

  // 添加简单任务
  for (int i = 0; i < 3; ++i) {
    SimpleTask* task = new SimpleTask();
    tasks.push_back(task);
    pool->Dispatch(task);
  }

  // 添加服务器任务
  for (int i = 0; i < 3; ++i) {
    ServerTask* task = new ServerTask();
    task->set_server_port(18086 + i);
    tasks.push_back(task);
    pool->Dispatch(task);
  }

  // 等待所有任务处理完成
  std::this_thread::sleep_for(std::chrono::milliseconds(800));

  // 验证所有任务都有 thread_id
  for (auto* task : tasks) {
    EXPECT_GT(task->thread_id(), 0);
    EXPECT_LE(task->thread_id(), 3);
    delete task;
  }
}

// 集成测试：多个服务器任务
TEST(IntegrationTest, MultipleServerTasks) {
  ThreadPool* pool = ThreadPool::GetInstance();
  pool->Init(4);

  std::vector<ServerTask*> tasks;

  // 创建多个服务器任务监听不同端口
  for (int i = 0; i < 4; ++i) {
    ServerTask* task = new ServerTask();
    task->set_server_port(18090 + i);
    tasks.push_back(task);
    pool->Dispatch(task);
  }

  // 等待所有任务处理完成
  std::this_thread::sleep_for(std::chrono::milliseconds(600));

  // 验证所有服务器任务都成功初始化
  for (auto* task : tasks) {
    EXPECT_GT(task->thread_id(), 0);
    EXPECT_NE(task->base(), nullptr);
    delete task;
  }
}

// 集成测试：顺序和并发混合
TEST(IntegrationTest, SequentialAndConcurrent) {
  ThreadPool* pool = ThreadPool::GetInstance();
  pool->Init(2);

  // 第一批任务
  std::vector<SimpleTask*> batch1;
  for (int i = 0; i < 4; ++i) {
    SimpleTask* task = new SimpleTask();
    batch1.push_back(task);
    pool->Dispatch(task);
  }

  // 等待第一批完成
  std::this_thread::sleep_for(std::chrono::milliseconds(400));

  // 验证第一批
  for (auto* task : batch1) {
    EXPECT_TRUE(task->IsInitCalled());
  }

  // 第二批任务
  std::vector<SimpleTask*> batch2;
  for (int i = 0; i < 4; ++i) {
    SimpleTask* task = new SimpleTask();
    batch2.push_back(task);
    pool->Dispatch(task);
  }

  // 等待第二批完成
  std::this_thread::sleep_for(std::chrono::milliseconds(400));

  // 验证第二批
  for (auto* task : batch2) {
    EXPECT_TRUE(task->IsInitCalled());
  }

  // 清理
  for (auto* task : batch1) delete task;
  for (auto* task : batch2) delete task;
}

// 集成测试：压力测试
TEST(IntegrationTest, StressTest) {
  ThreadPool* pool = ThreadPool::GetInstance();
  pool->Init(8);

  const int task_count = 50;
  std::vector<SimpleTask*> tasks;

  // 快速分发大量任务
  for (int i = 0; i < task_count; ++i) {
    SimpleTask* task = new SimpleTask();
    tasks.push_back(task);
    pool->Dispatch(task);
  }

  // 等待所有任务处理完成
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  // 统计成功处理的任务数
  int success_count = 0;
  for (auto* task : tasks) {
    if (task->IsInitCalled()) {
      success_count++;
    }
    delete task;
  }

  // 至少应该有大部分任务成功处理
  EXPECT_GT(success_count, task_count * 0.9);
}
