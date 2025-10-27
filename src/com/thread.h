/**
 * @file hthread.h
 * @author L.J.H (3414467112@qq.com)
 * @brief `Thread`类声明
 * @date 2025-10-24
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef THREAD_H
#define THREAD_H
#include <event2/util.h>

#include <list>
#include <mutex>

#include "crossocean.h"

_CROSSOCEAN_NAMESPACE_

class Task;

class Thread {
 public:
  Thread();
  ~Thread();

  /**
   * @brief 启动线程
   *
   * @details 安装线程, 启动线程并绑定入口函数, 然后分离线程使其在后台运行
   *
   */
  void Start();

  /**
   * @brief 线程入口函数
   *
   * @details 线程执行的任务逻辑, 运行事件循环等待事件发生
   *
   */
  void Main();

  /**
   * @brief 安装线程, 初始化 event_base 和管道监听事件用于激活线程
   *
   * @return true 安装成功
   * @return false 安装失败
   */
  bool Setup();

  /**
   * @brief 收到主线程发出的激活消息
   *
   * @details 获取任务并执行
   *
   * @param fd  管道读取端文件描述符
   * @param events  事件类型
   */
  void Notify(evutil_socket_t fd, short events);

  /**
   * @brief 激活线程
   *
   * @details 向线程发送激活消息(通过管道或者`socketpair`写入数据)
   *
   */
  void Activate();

  /**
   * @brief 添加任务到线程
   *
   * @param task	任务对象指针
   */
  void AddTask(Task* task);

  /// @brief 线程编号
  int id_;

 private:
  /// @brief 用于激活线程的管道写入端文件描述符
  int notify_send_fd_ = 0;
  /// @brief libevent 事件循环对象
  struct event_base* base_ = nullptr;

  /// @brief 线程任务列表
  std::list<Task*> tasks_;
  /// @brief 线程安全 互斥
  std::mutex tasks_mutex_;
};

_END_NAMESPACE_

#endif  // THREAD_H