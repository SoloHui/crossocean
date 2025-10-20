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
    threads_.push_back(thread);
    cout << "HThreadPool: Created thread " << i << endl;
    this_thread::sleep_for(std::chrono::milliseconds(10));  // 模拟线程启动延迟
  }
}