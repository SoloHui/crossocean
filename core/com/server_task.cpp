#include "server_task.h"

#include <event2/event.h>
#include <event2/listener.h>

#include <cstring>
#include <iostream>

using namespace std;
USING_CROSSOCEAN_NAMESPACE

static void SListenCB(struct evconnlistener* listener, evutil_socket_t fd,
                      struct sockaddr* addr, int socklen, void* user_arg) {
  // 这里可以处理新的连接请求
  cout << "ServerTask::ListenCB(): New connection received." << endl;
  // 获取任务对象指针
  auto server_task = static_cast<ServerTask*>(user_arg);
  // 调用用户定义的回调函数（如果有的话）
  if (server_task->ListenCB) {
    server_task->ListenCB(fd, addr, socklen, user_arg);
  } else {
    // 默认处理（如果没有用户定义的回调函数）
    cout << "ServerTask::ListenCB(): No user-defined callback provided."
         << endl;
  }
}

bool ServerTask::Init() {
  // 处理客户端请求的连接

  if (server_port_ <= 0) {
    cerr << "ServerTask::Init(): Invalid server port" << endl;
    return false;
  }

  // 监听端口
  sockaddr_in client_addr;
  memset(&client_addr, 0, sizeof(client_addr));

  client_addr.sin_family = AF_INET;
  client_addr.sin_addr.s_addr = INADDR_ANY;
  client_addr.sin_port = htons(server_port_);

  // 设置对应的回调函数和参数
  auto listener =
      evconnlistener_new_bind(base(),     // event_base
                              SListenCB,  // 连接回调函数(这里不处理连接)
                              this,       // 回调函数参数
                              LEV_OPT_REUSEABLE |         // 端口可重用
                                  LEV_OPT_CLOSE_ON_FREE,  // 释放时关闭套接字
                              -1,  // 连接队列长度(-1为默认值)
                              (sockaddr*)&client_addr,  // 监听地址
                              sizeof(client_addr));     // 地址长度

  if (!listener) {
    cerr << "ServerTask::Init(): Failed to create listener" << endl;
    return false;
  }
  cout << "ServerTask::Init(): Server listening on port " << server_port_
       << endl;
  return true;
}