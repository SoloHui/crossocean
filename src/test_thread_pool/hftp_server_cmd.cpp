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
    ssize_t n = bufferevent_read(bev, buf, sizeof(buf) - 1);
    if (n <= 0) {
      break;
    }
    buf[n] = '\0';  // 确保字符串以 null 结尾
    cout << "HFTPServerCmd::ReadCB() Received data on socket "
         << cmd_task->socket_fd_ << ": " << buf << endl;

    //  测试代码, 测试退出命令
    if (strstr(buf, "exit") != nullptr) {
      cout << "HFTPServerCmd::ReadCB() Exiting on socket "
           << cmd_task->socket_fd_ << endl;
      bufferevent_free(bev);
      delete cmd_task;
      break;
    }
  }
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
  bufferevent_setcb(bev, ReadCB, 0, 0, this);
  bufferevent_enable(bev, EV_READ | EV_WRITE);

  return true;
}