/**
 * @file hftp_server_cmd.h
 * @author L.J.H (3414467112@qq.com)
 * @brief HFTPServerCmd类声明
 * @date 2025-10-20
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "htask.h"

class HFTPServerCmd : public HTask {
 public:
  HFTPServerCmd() {}
  ~HFTPServerCmd() {}

  // 初始化任务
  virtual bool Init();
};