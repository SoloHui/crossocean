/**
 * @file hthread_pool.h
 * @author L.J.H (3414467112@qq.com)
 * @brief `ThreadPool`类声明
 * @date 2025-10-24
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef HTHREAD_POOL_H
#define HTHREAD_POOL_H

#include <vector>

#include "crossocean.h"

_CROSSOCEAN_NAMESPACE_

class Thread;
class Task;

class CROSSOCEAN_API ThreadPool {
 public:
  /**
   * @brief 获取`ThreadPool`的静态对象 （静态函数）
   *
   * @return ThreadPool* `ThreadPool`静态对象指针
   */
  static ThreadPool* GetInstance() {
    static ThreadPool instance;
    return &instance;
  }

  /**
   * @brief 初始化线程池
   *
   * @details
   * 初始化`thread_count`个线程, 并启动线程池中所有线程, 创建`event_base`,
   * 并在线程中开始接收消息
   *
   * @param thread_num 线程数量
   */
  void Init(int thread_num);

  /**
   * @brief 分发任务到线程
   *
   * @details
   * 分发任务到线程中执行, 会调用`task`的`Init`函数进行任务初始化,
   * 任务会轮询分发到线程池中的各个线程
   *
   * @param task 任务指针
   */
  void Dispatch(Task* task);

 private:
  ThreadPool() {};

 private:
  /// @brief 线程数量
  int thread_num_ = 0;

  /// @brief 上次分发的线程, 用于轮询调度
  int last_thread_index_ = -1;

  /// @brief 线程池列表
  std::vector<Thread*> threads_;
};

_END_NAMESPACE_

#endif