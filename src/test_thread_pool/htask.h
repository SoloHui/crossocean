/**
 * @file htask.h
 * @author L.J.H (3414467112@qq.com)
 * @brief 任务类声明
 * @date 2025-10-20
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once
class HTask {
 public:
  struct event_base* base_ = nullptr;
  int socket_fd_ = -1;
  int thread_id_ = -1;

  // 初始化任务
  virtual bool Init() = 0;
};
