/**
 * @file hthread_pool.cpp
 * @author L.J.H (3414467112@qq.com)
 * @brief `ThreadPool`类实现
 * @date 2025-10-24
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "include/thread_pool.h"

#include <iostream>
#include <thread>

#include "task.h"
#include "thread.h"
using namespace std;
_USING_CROSSOCEAN_NAMESPACE_

/**
 * @brief 初始化线程池
 *
 * @details
 * 初始化`thread_count`个线程, 并启动线程池中所有线程, 创建`event_base`,
 * 并在线程中开始接收消息
 *
 * @param thread_num 线程数量
 */
void ThreadPool::Init(int thread_num) {
  thread_num_ = thread_num;
  // 根据线程数量创建线程对象并存储到容器中
  for (int i = 0; i < thread_num_; ++i) {
    Thread* thread = new Thread();
    cout << "ThreadPool::Init() ThreadPool: Created thread " << i + 1 << endl;
    // 启动线程(编号从1开始)
    thread->id_ = i + 1;
    thread->Start();
    // 将线程对象添加到容器中
    threads_.push_back(thread);
    this_thread::sleep_for(std::chrono::milliseconds(10));  // 模拟线程启动延迟
  }
}

/**
 * @brief 分发任务到线程
 *
 * @details
 * 分发任务到线程中执行, 会调用`task`的`Init`函数进行任务初始化,
 * 任务会轮询分发到线程池中的各个线程
 *
 * @param task 任务指针
 */
void ThreadPool::Dispatch(Task* task) {
  if (!task) {
    cerr << "ThreadPool::Dispatch() Invalid task." << endl;
    return;
  }
  // 简单的轮询分发任务给线程
  if (threads_.empty()) {
    cerr << "ThreadPool::Dispatch() No threads available to dispatch task."
         << endl;
    return;
  }
  int thread_index = (last_thread_index_ + 1) % thread_num_;
  last_thread_index_ = thread_index;
  Thread* thread = threads_[thread_index];

  // 将任务添加到线程的任务列表
  thread->AddTask(task);

  // 激活线程执行任务
  thread->Activate();
  cout << "ThreadPool::Dispatch() Dispatched task to thread " << thread->id_
       << endl;
}