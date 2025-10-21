#include "hftp_server_cmd.h"

#include <event2/bufferevent.h>
#include <event2/event.h>
#include <string.h>

#include <iostream>

using namespace std;

static void ReadCB(struct bufferevent* bev, void* ctx) {
  // 读取数据的回调函数
  HFTPServerCmd* cmd_task = static_cast<HFTPServerCmd*>(ctx);
  char buf[1024] = {0};
  for (;;) {
    int n = bufferevent_read(bev, buf, sizeof(buf) - 1);
    if (n <= 0) {
      break;
    }
    buf[n] = '\0';  // 确保字符串以 null 结尾
    cout << "HFTPServerCmd::ReadCB() Received data on socket "
         << cmd_task->socket_fd_ << ": " << buf << endl;

    //  测试代码, 测试退出命令
    // if (strstr(buf, "exit") != nullptr) {
    //   cout << "HFTPServerCmd::ReadCB() Exiting on socket "
    //        << cmd_task->socket_fd_ << endl;
    //   bufferevent_free(bev);
    //   delete cmd_task;
    //   break;
    // }
  }
}

static void EventCB(struct bufferevent* bev, short what, void* ctx) {
  // 事件回调函数
  HFTPServerCmd* cmd_task = static_cast<HFTPServerCmd*>(ctx);
  // 如果对方网络断掉, 或者机器崩溃, 有可能不会收到 EOF (超时解决)
  if (what & BEV_EVENT_EOF) {
    cout << "HFTPServerCmd::EventCB() Connection closed on socket "
         << cmd_task->socket_fd_ << endl;
  } else if (what & BEV_EVENT_ERROR) {
    cout << "HFTPServerCmd::EventCB() Error on socket " << cmd_task->socket_fd_
         << endl;
  } else if (what & BEV_EVENT_TIMEOUT) {
    cout << "HFTPServerCmd::EventCB() Timeout on socket "
         << cmd_task->socket_fd_ << endl;
  } else {
    cout << "HFTPServerCmd::EventCB() Unknown event on socket "
         << cmd_task->socket_fd_ << endl;
  }
  // 释放资源
  bufferevent_free(bev);
  delete cmd_task;
}

// 初始化任务, 运行在子线程中
bool HFTPServerCmd::Init() {
  cout << "HFTPServerCmd::Init() Initializing FTP server command task." << endl;
  // 监听 socket bufferevent
  // base socket

  bufferevent* bev = bufferevent_socket_new(
      base_,                 // event_base
      socket_fd_,            // 监听的 socket fd
      BEV_OPT_CLOSE_ON_FREE  // 选项：释放 bufferevent 时关闭底层 socket
  );

  if (!bev) {
    cerr << "HFTPServerCmd::Init() Failed to create bufferevent." << endl;
    return false;
  }

  // 设置读写回调
  bufferevent_setcb(bev, ReadCB, nullptr, EventCB, this);
  bufferevent_enable(bev, EV_READ | EV_WRITE);

  // 添加超时设置(读10秒)
  timeval read_timeout = {10, 0};  // 10秒读超时(秒, 微秒)
  timeval write_timeout = {0, 0};  // 不设置写超时
  bufferevent_set_timeouts(bev, &read_timeout, &write_timeout);

  return true;
}