#include "hthread.h"

#include <iostream>
#include <thread>

using namespace std;

HThread::HThread() {}

HThread::~HThread() {}

void HThread::Start() {
  // 启动线程，绑定Main函数为线程入口
  thread t(&HThread::Main, this);
  // 分离线程，使其在后台运行
  t.detach();
}

void HThread::Main() {
  // 线程执行的任务
  // 这里可以添加具体的任务逻辑

  cout << "Thread " << id_ << " is running." << endl;
}