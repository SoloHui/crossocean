/**
 * @file hthread_pool.h
 * @author L.J.H (3414467112@qq.com)
 * @brief 线程池类声明(单件模式)
 * @date 2025-10-19
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

#include <vector>

class HThread;
class HTask;

class HThreadPool {
 public:
  //  单件模式(静态函数-无锁-无多线程调用问题)
  static HThreadPool* GetInstance() {
    static HThreadPool instance;
    return &instance;
  }

  // 初始化所有线程并启动
  void Init(int thread_num);

  // 分发线程
  void Dispatch(HTask* task);

 private:
  //  构造函数私有化，防止外部创建对象
  HThreadPool() {};

 private:
  // 线程数量
  int thread_num_;
  // 线程容器
  std::vector<HThread*> threads_;
  // 上一个分发任务的线程索引
  int last_thread_index_ = -1;
};