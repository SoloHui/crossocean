/**
 * @file htask.h
 * @author L.J.H (3414467112@qq.com)
 * @brief `HTask`类声明
 * @date 2025-10-24
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef HTASK_H
#define HTASK_H
class HTask {
 public:
  /**
   * @brief 任务初始化函数 (纯虚函数)
   *
   * @return true 初始化成功
   * @return false 初始化失败
   */
  virtual bool Init() = 0;

  /**
   * @brief 获取线程ID
   *
   * @return int 线程ID
   */
  int thread_id() { return thread_id_; }
  /**
   * @brief 设置线程ID
   *
   * @param thread_id 线程ID
   */
  void set_thread_id(int thread_id) { thread_id_ = thread_id; }

  /**
   * @brief 获取关联的socket
   *
   * @return int 关联的socket
   */
  int sock() { return sock_; }
  /**
   * @brief 设置关联的socket
   *
   * @param sock 关联的socket
   */
  void set_sock(int sock) { this->sock_ = sock; }

  /**
   * @brief 获取关联的event_base
   *
   * @return struct event_base* 关联的event_base
   */
  struct event_base* base() { return base_; }
  /**
   * @brief 设置关联的event_base
   *
   * @param base 关联的event_base
   */
  void set_base(struct event_base* base) { this->base_ = base; }

 private:
  /// @brief 关联的 event_base
  struct event_base* base_ = 0;
  /// @brief 关联的 socket
  int sock_ = 0;
  /// @brief 任务所属线程ID
  int thread_id_ = 0;
};

#endif
