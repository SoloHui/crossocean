/**
 * @file hthread.cpp
 * @author L.J.H (3414467112@qq.com)
 * @brief `Thread`类实现
 * @date 2025-10-24
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "thread.h"

#include <event2/event.h>
#include <event2/thread.h>

#include <iostream>
#include <thread>

#include "task.h"

#ifndef _WIN32  // Unix/Linux 系统
#include <unistd.h>
#endif

using namespace std;
_USING_CROSSOCEAN_NAMESPACE_

Thread::Thread() {}

Thread::~Thread() {}

/**
 * @brief 启动线程
 *
 * @details 安装线程, 启动线程并绑定入口函数, 然后分离线程使其在后台运行
 *
 */
void Thread::Start() {
  // 安装线程,初始化 event_base 和管道监听事件用于激活线程
  Setup();

  // 启动线程，绑定Main函数为线程入口
  std::thread t(&Thread::Main, this);

  // 分离线程，使其在后台运行
  t.detach();
}

/**
 * @brief 线程入口函数
 *
 * @details 线程执行的任务逻辑, 运行事件循环等待事件发生
 *
 */
void Thread::Main() {
  // 线程执行的任务
  // 这里可以添加具体的任务逻辑

  cout << "Thread::Main() Thread " << id_ << " begin." << endl;
  // 运行事件循环，等待事件发生
  event_base_dispatch(base_);
  event_base_free(base_);
  cout << "Thread::Main() Thread " << id_ << " end." << endl;
}

/**
 * @brief 激活线程任务的回调函数
 *
 * @param fd	管道读取端文件描述符
 * @param events	事件类型
 * @param arg	回调函数参数(传入线程对象指针)
 */
static void NotifyCB(evutil_socket_t fd, short events, void* arg) {
  // 静态函数(只在本文件使用) 作为管道可读事件的回调
  Thread* thread = static_cast<Thread*>(arg);
  thread->Notify(fd, events);
}

/**
 * @brief 安装线程, 初始化 event_base 和管道监听事件用于激活线程
 *
 * @return true 安装成功
 * @return false 安装失败
 */
bool Thread::Setup() {
  // 初始化 event_base 和管道监听事件用于激活线程

  // Windows 用配对 socketpair 模拟管道
#ifdef _WIN32
  // 创建一个 配对 socketpair 可以相互通信
  // fds[0] 用于读， fds[1] 用于写
  evutil_socket_t fds[2];
  if (evutil_socketpair(AF_INET, SOCK_STREAM, 0, fds) < 0) {
    cerr << "Thread::Setup() Failed to create socketpair." << endl;
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
    cerr << "Thread::Setup() Failed to create pipe." << endl;
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
    cerr << "Thread::Setup() Failed to create event_base." << endl;
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

/**
 * @brief 收到主线程发出的激活消息
 *
 * @details 获取任务并执行
 *
 * @param fd  管道读取端文件描述符
 * @param events  事件类型
 */
void Thread::Notify(evutil_socket_t fd, short events) {
  // 水平触发模式下循环读取数据(直到接受完毕)
  char buf[1] = {0};
#ifdef _WIN32
  // Windows 下用 recv 读取数据
  int re = recv(fd, buf, 1, 0);
#else
  // Unix/Linux 下用 read 读取数据
  ssize_t re = read(fd, buf, 1);
#endif
  // 读取数据以清除事件
  if (re <= 0) {
    return;
  }
  // 在这里处理线程被激活后的任务
  cout << "Thread::Notify() Thread " << id_ << " activated." << endl;

  Task* task = nullptr;
  // 线程安全获取任务列表
  tasks_mutex_.lock();
  if (tasks_.empty()) {
    tasks_mutex_.unlock();
    cout << "Thread::Notify() Thread " << id_ << " has no tasks." << endl;
    return;
  }
  task = tasks_.front();  // 先进先出
  tasks_.pop_front();     // 从任务列表中移除任务
  tasks_mutex_.unlock();

  // 处理任务
  cout << "Thread::Notify() Thread " << id_ << " processing task." << endl;
  task->Init();
}

/**
 * @brief 激活线程
 *
 * @details 向线程发送激活消息(通过管道或者`socketpair`写入数据)
 *
 */
void Thread::Activate() {
  // 向线程发送激活消息(通过管道写入数据)
  char buf[1] = {'c'};  // 发送一个字节的数据作为激活信号
#ifdef _WIN32
  // Windows 下用 send 发送数据
  int re = send(notify_send_fd_, buf, 1, 0);
#else
  // Unix/Linux 下用 write 发送数据
  ssize_t re = write(notify_send_fd_, buf, 1);
#endif
  if (re <= 0) {
    cerr << "Thread::Activate() Thread " << id_
         << " failed to send activate signal." << endl;
  }
}

/**
 * @brief 添加任务到线程
 *
 * @param task	任务对象指针
 */
void Thread::AddTask(Task* task) {
  // 添加处理的任务
  // 一个线程同时可以处理多个任务, 共用一个 event_base
  if (!task) {
    cerr << "Thread::AddTask() Invalid task." << endl;
    return;
  }
  task->set_base(base_);
  task->set_thread_id(id_);
  // 线程安全添加任务到任务列表
  tasks_mutex_.lock();
  tasks_.push_back(task);
  tasks_mutex_.unlock();
}