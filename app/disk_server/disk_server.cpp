#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#else
#include <signal.h>
#endif

#include "crossocean.h"
#include "thread_pool.h"

using namespace std;
USING_CROSSOCEAN_NAMESPACE

int main(int argc, char const* argv[]) {
#ifdef _WIN32
  // 初始化`socket`
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);
#else
  // 忽略`SIGPIPE`信号, 防止程序因向已关闭的socket写数据而终止
  if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
    cerr << "main(): signal SIGPIPE error" << endl;
    return -1;
  }
#endif

  // 服务器端口号
  int server_port = 9340;
  // 线程池大小
  int thread_num = 10;

  if (argc > 1) {
    server_port = atoi(argv[1]);
  }
  if (argc > 2) {
    thread_num = atoi(argv[2]);
  }
  if (argc == 1) {
    cout << "Usage: hdisk_server [server_port] [thread_num]" << endl;
  }
  cout << "Starting hdisk_server on port " << server_port << "..." << endl;
  cout << "Using thread pool size: " << thread_num << endl;

  // 初始化线程池
  ThreadPool::GetInstance()->Init(thread_num);

  return 0;
}
