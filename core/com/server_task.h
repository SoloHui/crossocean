#ifndef SERVER_TASK_H
#define SERVER_TASK_H

#include "crossocean.h"
#include "task.h"

typedef void (*ListenCBFunc)(int socket_fd, struct sockaddr* addr, int socklen,
                             void* user_arg);

CROSSOCEAN_NAMESPACE

class ServerTask : public Task {
 public:
  ServerTask() {}
  ~ServerTask() {}

  virtual bool Init() override;

  // 封装回调函数(函数指针)
  ListenCBFunc ListenCB = nullptr;

 public:
  int server_port() const { return server_port_; }
  void set_server_port(int port) { server_port_ = port; }

 private:
  int server_port_;
};

END_NAMESPACE

#endif  // SERVER_TASK_H
