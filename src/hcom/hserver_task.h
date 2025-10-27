#ifndef HSERVER_TASK_H
#define HSERVER_TASK_H

#include "crossocean.h"
#include "htask.h"

typedef void (*ListenCBFunc)(int socket_fd, struct sockaddr* addr, int socklen,
                             void* user_arg);

_CROSSOCEAN_NAMESPACE_

class HServerTask : public HTask {
 public:
  HServerTask() {}
  ~HServerTask() {}

  virtual bool Init() override;

  // 封装回调函数(函数指针)
  ListenCBFunc ListenCB = nullptr;

 public:
  int server_port() const { return server_port_; }
  void set_server_port(int port) { server_port_ = port; }

 private:
  int server_port_;
};

_END_NAMESPACE_

#endif  // HSERVER_TASK_H