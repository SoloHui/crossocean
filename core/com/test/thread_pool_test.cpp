// thread_pool_test.cpp
// ThreadPool 类单元测试

#include "include/thread_pool.h"

#include <gtest/gtest.h>

#include <chrono>
#include <map>
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

// ==================== ThreadPool 测试 ====================

// 测试线程池的初始化
TEST(ThreadPoolTest, Initialization) {
  ThreadPool* pool = ThreadPool::GetInstance();
  ASSERT_NE(pool, nullptr);

  // 初始化线程池（使用2个线程）
  pool->Init(2);

  // 给线程时间启动
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

// 测试获取单例实例
TEST(ThreadPoolTest, GetInstance) {
  ThreadPool* pool1 = ThreadPool::GetInstance();
  ThreadPool* pool2 = ThreadPool::GetInstance();

  // 验证是同一个实例
  EXPECT_EQ(pool1, pool2);
  ASSERT_NE(pool1, nullptr);
}

// 测试任务分发
TEST(ThreadPoolTest, DispatchTask) {
  ThreadPool* pool = ThreadPool::GetInstance();
  pool->Init(2);

  SimpleTask* task = new SimpleTask();
  pool->Dispatch(task);

  // 等待任务处理
  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  EXPECT_TRUE(task->IsInitCalled());
  EXPECT_GT(task->thread_id(), 0);
  EXPECT_LE(task->thread_id(), 2);

  delete task;
}

// 测试多任务轮询分发
TEST(ThreadPoolTest, RoundRobinDispatch) {
  ThreadPool* pool = ThreadPool::GetInstance();
  pool->Init(3);

  const int task_count = 9;
  std::vector<SimpleTask*> tasks;

  for (int i = 0; i < task_count; ++i) {
    SimpleTask* task = new SimpleTask();
    tasks.push_back(task);
    pool->Dispatch(task);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }

  // 等待所有任务处理完成
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  // 检查所有任务是否都被处理
  std::map<int, int> thread_task_count;
  for (auto* task : tasks) {
    EXPECT_TRUE(task->IsInitCalled());
    thread_task_count[task->thread_id()]++;
    delete task;
  }

  // 验证任务分发是否均匀（轮询策略）
  for (const auto& pair : thread_task_count) {
    EXPECT_EQ(pair.second, 3);  // 9个任务分给3个线程，每个线程应该得到3个任务
  }
}

// 测试空任务分发
TEST(ThreadPoolTest, DispatchNullTask) {
  ThreadPool* pool = ThreadPool::GetInstance();
  pool->Init(2);

  // 分发空任务不应该崩溃
  pool->Dispatch(nullptr);

  // 线程池仍应该可以正常工作
  SimpleTask* task = new SimpleTask();
  pool->Dispatch(task);

  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  EXPECT_TRUE(task->IsInitCalled());

  delete task;
}

// 测试不同线程数量的初始化
TEST(ThreadPoolTest, DifferentThreadCounts) {
  ThreadPool* pool = ThreadPool::GetInstance();

  // 测试单线程
  pool->Init(1);
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  SimpleTask* task1 = new SimpleTask();
  pool->Dispatch(task1);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  EXPECT_TRUE(task1->IsInitCalled());
  EXPECT_EQ(task1->thread_id(), 1);

  delete task1;
}

// 测试大量任务分发
TEST(ThreadPoolTest, ManyTasksDispatch) {
  ThreadPool* pool = ThreadPool::GetInstance();
  pool->Init(4);

  const int task_count = 20;
  std::vector<SimpleTask*> tasks;

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
    EXPECT_GT(task->thread_id(), 0);
    EXPECT_LE(task->thread_id(), 4);
    delete task;
  }

  EXPECT_EQ(success_count, task_count);
}
