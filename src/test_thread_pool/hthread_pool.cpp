#include "hthread_pool.h"

#include <iostream>
#include <thread>

#include "hthread.h"

using namespace std;

void HThreadPool::Init(int thread_num) {
  thread_num_ = thread_num;
  // 根据线程数量创建线程对象并存储到容器中
  for (int i = 0; i < thread_num_; ++i) {
    HThread* thread = new HThread();
    cout << "HThreadPool::Init() HThreadPool: Created thread " << i + 1 << endl;
    // 启动线程(编号从1开始)
    thread->id_ = i + 1;
    thread->Start();
    // 将线程对象添加到容器中
    threads_.push_back(thread);
    this_thread::sleep_for(std::chrono::milliseconds(10));  // 模拟线程启动延迟
  }
}

void HThreadPool::Dispatch(HTask* task) {
  // 简单的轮询分发任务给线程
  if (!task) {
    cerr << "HThreadPool::Dispatch() Invalid task." << endl;
    return;
  }
  // 简单的轮询分发任务给线程
  if (threads_.empty()) {
    cerr << "HThreadPool::Dispatch() No threads available to dispatch task."
         << endl;
    return;
  }
  int thread_index = (last_thread_index_ + 1) % thread_num_;
  last_thread_index_ = thread_index;
  HThread* thread = threads_[thread_index];
  // 激活线程执行任务
  thread->Activate();
  cout << "HThreadPool::Dispatch() Dispatched task to thread " << thread->id_
       << endl;
}