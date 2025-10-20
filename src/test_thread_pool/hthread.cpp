#include "hthread.h"

#include <event2/event.h>
#include <event2/thread.h>

#include <iostream>
#include <thread>

#ifndef _WIN32  // Unix/Linux 系统
#include <unistd.h>
#endif

using namespace std;

HThread::HThread() {}

HThread::~HThread() {}

void HThread::Start() {
  // 安装线程,初始化 event_base 和管道监听事件用于激活线程
  Setup();

  // 启动线程，绑定Main函数为线程入口
  thread t(&HThread::Main, this);

  // 分离线程，使其在后台运行
  t.detach();
}

void HThread::Main() {
  // 线程执行的任务
  // 这里可以添加具体的任务逻辑

  cout << "HThread::Main() Thread " << id_ << " begin." << endl;
  // 运行事件循环，等待事件发生
  event_base_dispatch(base_);
  event_base_free(base_);
  cout << "HThread::Main() Thread " << id_ << " end." << endl;
}

static void NotifyCB(evutil_socket_t fd, short events, void* arg) {
  // 静态函数(只在本文件使用) 作为管道可读事件的回调
  HThread* thread = static_cast<HThread*>(arg);
  thread->Notify(fd, events);
}

bool HThread::Setup() {
  // 初始化 event_base 和管道监听事件用于激活线程

  // Windows 用配对 socketpair 模拟管道
#ifdef _WIN32
  // 创建一个 配对 socketpair 可以相互通信
  // fds[0] 用于读， fds[1] 用于写
  evutil_socket_t fds[2];
  if (evutil_socketpair(AF_INET, SOCK_STREAM, 0, fds) < 0) {
    cerr << "HThread::Setup() Failed to create socketpair." << endl;
    return false;
  }
  // 设置非阻塞模式
  evutil_make_socket_nonblocking(fds[0]);
  evutil_make_socket_nonblocking(fds[1]);
#else
  // Unix/Linux 用管道
  // 不能用 send/recv 只能用 read/write
  int fds[2];
  if (pipe(fds) == -1) {
    cerr << "HThread::Setup() Failed to create pipe." << endl;
    return false;
  }
#endif

  // 读取绑定到 event_base 上，写入要保存
  notify_send_fd_ = fds[1];

  // 创建 event_base 对象(无锁版)
  event_config* ev_conf = event_config_new();
  event_config_set_flag(ev_conf, EVENT_BASE_FLAG_NOLOCK);
  // 创建 libevent 上下文
  base_ = event_base_new_with_config(ev_conf);
  // 释放配置对象
  event_config_free(ev_conf);

  if (!base_) {
    cerr << "HThread::Setup() Failed to create event_base." << endl;
    return false;
  }

  // 添加管道监听事件到 event_base,用于激活线程执行任务
  event* notify_event =
      event_new(base_,                 // event_base
                fds[0],                // 读取端
                EV_READ | EV_PERSIST,  // 监听可读事件且持续监听
                NotifyCB,              // 事件回调函数
                this);                 // 回调函数参数(传入当前线程对象指针)
  event_add(notify_event, nullptr);
  return true;
}

void HThread::Notify(evutil_socket_t fd, short events) {
  // 水平触发模式下循环读取数据(直到接受完毕)
  char buf[1] = {0};
#ifdef _WIN32
  // Windows 下用 recv 读取数据
  ssize_t re = recv(fd, buf, 1, 0);
#else
  // Unix/Linux 下用 read 读取数据
  ssize_t re = read(fd, buf, 1);
#endif
  // 读取数据以清除事件
  if (re <= 0) {
    return;
  }
  // 在这里处理线程被激活后的任务
  cout << "HThread::Notify() Thread " << id_ << " activated." << endl;
}
