/**
 * @file hthread.h
 * @author L.J.H (3414467112@qq.com)
 * @brief HThread类声明
 * @date 2025-10-19
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

#include <event2/util.h>
class HThread {
 public:
  HThread();
  ~HThread();

  // 启动线程
  void Start();

  // 线程入口函数
  void Main();

  // 安装线程,初始化 event_base 和管道监听事件用于激活线程
  bool Setup();

  // 收到主线程发出的激活消息(线程池的分发任务)
  void Notify(evutil_socket_t fd, short events);

  // 线程激活
  void Activate();

  // 线程编号
  int id_;

 private:
  int notify_send_fd_ = 0;
  struct event_base* base_ = nullptr;
};