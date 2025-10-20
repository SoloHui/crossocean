#include <event2/event.h>
#include <event2/listener.h>

#include <cstring>
#include <iostream>

#ifdef _WIN32
#include <signal.h>
#endif

#include "hftp_server_cmd.h"
#include "hthread_pool.h"

using namespace std;

#define PORT 9340

void ListenCB(struct evconnlistener* listener, evutil_socket_t fd,
              struct sockaddr* addr, int socklen, void* ctx) {
  cout << "Main::ListenCB() called!" << endl;
  // 在这里可以创建一个具体的任务对象并分发给线程池
  HTask* task = new HFTPServerCmd();
  task->socket_fd_ = fd;
  HThreadPool* thread_pool = HThreadPool::GetInstance();  // 获取线程池单例对象
  thread_pool->Dispatch(task);
}

int main(int argc, char* argv[]) {
  cout << "Main::Test HThreadPool Start!" << endl;

#ifdef _WIN32
  // 初始化 socket 环境
  WSADATA wsa;
  WSAStartup(MAKEWORD(2, 2), &wsa);
  signal(SIGPIPE, SIG_IGN);
#endif

  // 获取线程池单例对象
  HThreadPool* thread_pool = HThreadPool::GetInstance();

  // 初始化线程池，创建并启动指定数量的线程
  int thread_num = 2;  // 示例线程数量
  thread_pool->Init(thread_num);

  // 创建`libevent`上下文
  event_base* base = event_base_new();
  if (!base) {
    cerr << "Main::Failed to create event base" << endl;
    return -1;
  }
  // 创建监听器
  sockaddr_in sin;
  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = htonl(0);
  sin.sin_port = htons(PORT);

  // 监听指定端口
  evconnlistener* listener = evconnlistener_new_bind(
      base,      // libevent上下文
      ListenCB,  // 连接到来时的回调函数
      nullptr,   // 回调函数参数
      LEV_OPT_REUSEABLE |
          LEV_OPT_CLOSE_ON_FREE,  // 监听器选项(地址重用、释放时关闭socket)
      -1,                         // 连接请求队列长度(-1为默认值)
      (sockaddr*)&sin,            // 监听地址信息
      sizeof(sin)                 // 地址信息长度
  );

  if (!listener) {
    cerr << "Main::Failed to create listener" << endl;
    event_base_free(base);
    return -1;
  }

  // 进入事件循环
  if (base) {
    event_base_dispatch(base);
  }

  // 清理资源
  if (listener) {
    evconnlistener_free(listener);
  }
  if (base) {
    event_base_free(base);
  }

#ifdef _WIN32
  WSACleanup();
#endif

  cout << "Main::Test HThreadPool End!" << endl;
  return 0;
}