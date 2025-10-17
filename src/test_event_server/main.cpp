#include <arpa/inet.h>           // 提供IPv4地址与网络字节序转换函数
#include <event2/buffer.h>       // 提供libevent缓冲区读写接口
#include <event2/bufferevent.h>  // 提供基于缓冲区的事件驱动套接字封装
#include <event2/event.h>        // 提供事件循环的核心API
#include <event2/listener.h>     // 提供监听器工具以简化TCP服务器
#include <signal.h>              // 提供信号处理相关API
#include <unistd.h>              // 提供POSIX系统调用如close

#include <cstdlib>   // 提供数值转换函数如strtol
#include <cstring>   // 提供内存操作函数
#include <iostream>  // 提供标准输入输出流
#include <string>    // 提供std::string字符串容器

namespace {  // 匿名命名空间用于限制内部符号作用域

void OnSignal(evutil_socket_t /*fd*/, short /*events*/,
              void* arg) {  // SIGINT信号回调，负责终止事件循环
  auto* base = static_cast<event_base*>(arg);  // 获取事件循环基础对象
  std::cout << "\nCaught SIGINT, shutting down." << std::endl;  // 输出提示信息
  event_base_loopbreak(base);  // 中断事件循环以实现优雅退出
}

void OnRead(bufferevent* bev,
            void* /*ctx*/) {  // 读取回调，处理客户端发来的数据
  evbuffer* input = bufferevent_get_input(bev);    // 获取输入缓冲区
  evbuffer* output = bufferevent_get_output(bev);  // 获取输出缓冲区

  const size_t len = evbuffer_get_length(input);  // 获取输入缓冲区当前数据长度
  if (len == 0) {                                 // 若没有可读数据则直接返回
    return;                                       // 无操作返回等待下一次回调
  }

  std::string message(len, '\0');               // 根据数据长度创建字符串容器
  evbuffer_remove(input, message.data(), len);  // 从输入缓冲区读取数据到字符串

  const std::string response = "Echo: " + message;  // 构造回显响应内容
  evbuffer_add(output, response.data(),
               response.size());  // 将响应写入输出缓冲区等待发送
}

void OnEvent(bufferevent* bev, short events,
             void* /*ctx*/) {    // 状态事件回调，处理连接关闭与错误
  if (events & BEV_EVENT_EOF) {  // 检测到对端正常关闭连接
    std::cout << "Client disconnected." << std::endl;  // 输出客户端断开提示
  } else if (events & BEV_EVENT_ERROR) {               // 检测到连接错误
    std::cerr << "Connection error: "                  // 输出错误提示前缀
              << evutil_socket_error_to_string(
                     EVUTIL_SOCKET_ERROR())  // 转换错误码为可读字符串
              << std::endl;                  // 换行结束输出
  }

  bufferevent_free(bev);  // 释放bufferevent资源
}

void OnAccept(evconnlistener* listener,
              evutil_socket_t fd,  // 新连接回调，封装接受到的套接字
              sockaddr* address, int socklen,
              void* ctx) {                     // 提供客户端地址信息
  auto* base = static_cast<event_base*>(ctx);  // 取出事件循环对象
  (void)listener;                              // 标记未使用的监听器参数
  (void)address;                               // 标记未使用的地址参数
  (void)socklen;                               // 标记未使用的地址长度参数

  bufferevent* bev = bufferevent_socket_new(
      base, fd, BEV_OPT_CLOSE_ON_FREE);  // 为新连接创建bufferevent
  if (!bev) {                            // 检查创建是否成功
    std::cerr << "Failed to create bufferevent." << std::endl;  // 输出失败提示
    close(fd);  // 关闭套接字以避免资源泄漏
    return;     // 返回等待下一次连接
  }

  bufferevent_setcb(bev, OnRead, nullptr, OnEvent,
                    nullptr);                     // 注册读回调和事件回调
  bufferevent_enable(bev, EV_READ | EV_WRITE);    // 启用读写事件监听
  std::cout << "Client connected." << std::endl;  // 输出客户端连接成功
}

void OnAcceptError(evconnlistener* listener, void* /*ctx*/) {  // 监听器错误回调
  const int err = EVUTIL_SOCKET_ERROR();         // 获取最近一次socket错误码
  std::cerr << "Listener error " << err << ": "  // 输出错误码
            << evutil_socket_error_to_string(err) << std::endl;  // 输出错误描述
  evconnlistener_disable(listener);  // 禁用监听器以防止持续错误
}

}  // namespace

int main(int argc, char** argv) {  // 程序入口函数

  std::string server_ip = "172.17.0.2";  // 服务监听IP
  int server_port = 9340;                // 服务监听端口
  bool port_overridden = false;          // 标记端口是否已被命令行覆盖

  if (argc > 1) {         // 检查是否提供第一个参数
    char* end = nullptr;  // 保存数值转换结束位置
    const long parsed_first =
        std::strtol(argv[1], &end, 10);  // 尝试将第一个参数解析为端口
    if (argv[1] && *argv[1] != '\0' && (!end || *end == '\0') &&
        parsed_first > 0 &&
        parsed_first <= 65535) {  // 判断第一个参数是否为合法端口
      server_port = static_cast<int>(parsed_first);  // 更新端口为用户指定值
      port_overridden = true;                        // 标记端口已被覆盖
    } else {                                         // 否则将其视作IP字符串
      server_ip = argv[1];                           // 使用第一个参数作为IP地址
    }
  }

  if (argc > 2) {           // 检查是否提供第二个参数
    if (port_overridden) {  // 若端口已通过第一个参数设置
      server_ip = argv[2];  // 则第二个参数作为IP地址使用
    } else {                // 否则尝试将第二个参数解析为端口
      char* end = nullptr;  // 重置转换结束指针
      const long parsed_second =
          std::strtol(argv[2], &end, 10);  // 解析第二个参数
      if (argv[2] && *argv[2] != '\0' && (!end || *end == '\0') &&
          parsed_second > 0 &&
          parsed_second <= 65535) {  // 判断第二个参数是否为合法端口
        server_port =
            static_cast<int>(parsed_second);  // 使用第二个参数覆盖端口
        port_overridden = true;               // 更新覆盖标记
      } else {                                // 否则视作IP地址
        server_ip = argv[2];                  // 使用第二个参数作为IP
      }
    }
  }

  if (argc > 3) {         // 若提供了第三个或更多参数
    server_ip = argv[3];  // 以第三个参数为最终IP地址
  }

  in_addr addr{};  // 保存转换后的IPv4地址
  if (inet_pton(AF_INET, server_ip.c_str(), &addr) !=
      1) {  // 尝试将IP字符串转换为网络字节序
    std::cerr << "Invalid IP argument. Using default."
              << std::endl;                        // 提示IP格式错误
    server_ip = "0.0.0.0";                         // 回退到默认IP
    inet_pton(AF_INET, server_ip.c_str(), &addr);  // 将默认IP转换成网络字节序
  }

  event_base* base = event_base_new();  // 创建事件循环基础对象
  if (!base) {                          // 检查创建是否成功
    std::cerr << "Could not initialize libevent."
              << std::endl;  // 输出初始化失败提示
    return 1;                // 返回非零以指示错误
  }

  sockaddr_in sin{};         // 初始化IPv4地址结构体
  sin.sin_family = AF_INET;  // 设置地址族为IPv4
  sin.sin_addr = addr;       // 将转换后的IP写入结构体
  sin.sin_port =
      htons(static_cast<uint16_t>(server_port));  // 设置监听端口为server_port

  evconnlistener* listener = evconnlistener_new_bind(  // 创建监听器并绑定地址
      base, OnAccept, base, LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
      -1,                                               // 配置监听器选项和回调
      reinterpret_cast<sockaddr*>(&sin), sizeof(sin));  // 传入绑定地址与长度

  if (!listener) {  // 检查监听器是否创建成功
    std::cerr << "Could not create a listener." << std::endl;  // 输出失败提示
    event_base_free(base);  // 释放事件循环资源
    return 1;               // 返回非零错误码
  }

  evconnlistener_set_error_cb(listener, OnAcceptError);  // 注册监听器错误回调

  event* signal_event =
      evsignal_new(base, SIGINT, OnSignal, base);  // 创建SIGINT信号事件
  if (!signal_event ||
      event_add(signal_event, nullptr) < 0) {  // 检查信号事件是否创建并成功添加
    std::cerr << "Could not create SIGINT event handler."
              << std::endl;         // 输出失败提示
    evconnlistener_free(listener);  // 释放监听器
    event_base_free(base);          // 释放事件循环
    return 1;                       // 返回非零错误码
  }

  std::cout << "test_event_server listening on " << server_ip << ":"
            << server_port << std::endl;  // 输出启动成功信息并展示监听地址
  event_base_dispatch(base);              // 启动事件循环并进入阻塞处理

  evconnlistener_free(listener);                           // 释放监听器资源
  event_free(signal_event);                                // 释放信号事件资源
  event_base_free(base);                                   // 释放事件循环资源
  std::cout << "test_event_server stopped." << std::endl;  // 输出停止信息
  return 0;                                                // 正常结束程序
}
