#include "htask.h"

class HFTPServerCmd : public HTask {
 public:
  HFTPServerCmd() {}
  ~HFTPServerCmd() {}

  // 初始化任务
  virtual bool Init();
};