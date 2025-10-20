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

class HThread {
 public:
  HThread();
  ~HThread();

  // 启动线程
  void Start();

  // 线程入口函数
  void Main();

  // 线程编号
  int id_;

 private:
  /* data */
};