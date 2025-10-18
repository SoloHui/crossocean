#include <arpa/inet.h>     // 提供 inet_pton 等 IPv4 地址转换函数。
#include <event2/event.h>  // libevent 核心事件循环接口。
#include <event2/util.h>   // libevent 提供的套接字与错误工具函数。
#include <signal.h>        // 处理 SIGINT 等信号相关接口。
#include <sys/socket.h>    // UDP 套接字通信相关 API。

#include <array>     // 定长缓冲区容器 std::array。
#include <cerrno>    // 访问 errno 以获取错误码。
#include <cstdlib>   // 提供 std::strtol 等通用函数。
#include <cstring>   // C 风格内存与字符串辅助函数。
#include <iostream>  // 标准输入输出流。
#include <string>    // std::string 处理地址与消息。

namespace {

void SignalCB(evutil_socket_t /*fd*/, short /*events*/,
              void* arg) {  // SIGINT 信号回调，用于停止事件循环。
  auto* base =
      static_cast<event_base*>(arg);  // 取回作为上下文传入的 event_base。
  std::cout << "\nCaught SIGINT, shutting down."
            << std::endl;      // 通知用户开始关闭流程。
  event_base_loopbreak(base);  // 要求事件循环优雅退出。
}

void ReadCB(evutil_socket_t fd, short /*events*/,
            void* /*ctx*/) {                   // UDP 套接字可读时触发的回调。
  std::array<char, 2048> buffer{};             // 存放收到报文的缓冲区。
  sockaddr_storage client_addr{};              // 保存客户端地址信息。
  socklen_t client_len = sizeof(client_addr);  // 地址结构体长度。

  const ssize_t received = recvfrom(fd, buffer.data(), buffer.size(), 0,
                                    reinterpret_cast<sockaddr*>(&client_addr),
                                    &client_len);  // 接收数据并获取来源地址。
  if (received < 0) {                              // 检查接收是否失败。
    const int err = errno;  // 立即读取 errno，避免后续覆盖。
    if (err == EAGAIN || err == EWOULDBLOCK) {  // 非阻塞场景下的正常情况。
      return;                                   // 如果暂时没有数据则直接返回。
    }
    std::cerr << "recvfrom failed: " << evutil_socket_error_to_string(err)
              << std::endl;  // 使用 libevent 工具输出可读错误信息。
    return;                  // 无法恢复的错误，结束本次处理。
  }

  std::cout << "Received UDP packet: " << buffer.data()
            << std::endl;  // 打印收到的报文内容。

  std::string reply = "UDP Echo: ";  // 回显消息的前缀。
  reply.append(buffer.data(),
               static_cast<size_t>(received));  // 追加收到的原始数据长度。

  const ssize_t sent = sendto(fd, reply.data(), reply.size(), 0,
                              reinterpret_cast<sockaddr*>(&client_addr),
                              client_len);  // 将回显数据发送回客户端。
  if (sent < 0) {                           // 检查发送是否失败。
    const int err = errno;                  // 保存 errno 以便打印。
    std::cerr << "sendto failed: " << evutil_socket_error_to_string(err)
              << std::endl;  // 输出发送失败的详细信息。
  }
}

}  // namespace

int main(int argc, char** argv) {      // 程序入口，配置并运行 UDP 回显服务。
  std::string bind_ip = "172.17.0.2";  // 默认绑定 IP，除非命令行覆盖。
  int bind_port = 9340;                // 默认监听的 UDP 端口。

  if (argc > 1) {         // 第一个参数可能是端口或 IP。
    char* end = nullptr;  // strtol 使用的指针，用于判断转换成功。
    const long parsed_port =
        std::strtol(argv[1], &end, 10);  // 尝试按十进制解析端口。
    if (argv[1] && *argv[1] != '\0' && (!end || *end == '\0') &&
        parsed_port > 0 && parsed_port <= 65535) {  // 判断是否为有效端口范围。
      bind_port = static_cast<int>(parsed_port);  // 若合法则使用解析出的端口。
    } else {
      bind_ip = argv[1];  // 否则视为 IP 地址参数。
    }
  }

  if (argc > 2) {         // 第二个参数用于补充未指定的配置。
    char* end = nullptr;  // 重置转换指针。
    const long parsed_port =
        std::strtol(argv[2], &end, 10);  // 再次尝试解析端口。
    if (argv[2] && *argv[2] != '\0' && (!end || *end == '\0') &&
        parsed_port > 0 && parsed_port <= 65535) {  // 检查端口是否符合要求。
      bind_port = static_cast<int>(parsed_port);    // 成功则覆盖端口。
    } else {
      bind_ip = argv[2];  // 否则认为是 IP。
    }
  }

  if (argc > 3) {       // 第三个参数显式指定 IP。
    bind_ip = argv[3];  // 直接覆盖绑定 IP。
  }

  in_addr addr{};  // IPv4 二进制地址结构。
  if (inet_pton(AF_INET, bind_ip.c_str(), &addr) !=
      1) {  // 将文本 IP 转为二进制形式。
    std::cerr << "Invalid IP argument. Using default."
              << std::endl;                      // 输入无效时提示。
    bind_ip = "0.0.0.0";                         // 回退到监听全部网卡。
    inet_pton(AF_INET, bind_ip.c_str(), &addr);  // 将回退 IP 再转换一次。
  }

  evutil_socket_t fd = socket(AF_INET, SOCK_DGRAM, 0);  // 1 创建 UDP 套接字。
  if (fd < 0) {  // 判断套接字是否创建成功。
    std::cerr << "Could not create UDP socket." << std::endl;  // 提示创建失败。
    return 1;  // 无套接字无法继续，返回错误码。
  }

  if (evutil_make_listen_socket_reuseable(fd) <
      0) {  // 2 开启地址复用，便于快速重启。
    std::cerr << "Could not make socket reusable."
              << std::endl;  // 设置失败时输出错误。
    evutil_closesocket(fd);  // 关闭套接字，避免资源泄漏。
    return 1;                // 无法满足需求则退出。
  }

  if (evutil_make_socket_nonblocking(fd) < 0) {  // 3 将套接字设为非阻塞。
    std::cerr << "Could not set socket to non-blocking."
              << std::endl;  // 非阻塞设置失败的提示。
    evutil_closesocket(fd);  // 关闭套接字释放资源。
    return 1;                // 事件循环需要非阻塞，失败则退出。
  }

  if (evutil_make_socket_closeonexec(fd) < 0) {  // 4 禁止子进程继承套接字。
    std::cerr << "Could not set close-on-exec on socket."
              << std::endl;  // 设置失败日志。
    evutil_closesocket(fd);  // 清理套接字。
    return 1;                // 配置不完整，程序退出。
  }

  sockaddr_in sin{};         // 5 IPv4 绑定地址结构。
  sin.sin_family = AF_INET;  // 指定地址族为 IPv4。
  sin.sin_addr = addr;       // 填充二进制 IP 地址。
  sin.sin_port =
      htons(static_cast<uint16_t>(bind_port));  // 将端口转为网络字节序。

  if (bind(fd, reinterpret_cast<sockaddr*>(&sin), sizeof(sin)) <
      0) {                  // 6 绑定到指定地址。
    const int err = errno;  // 保存 errno 以便输出。
    std::cerr << "bind failed: " << evutil_socket_error_to_string(err)
              << std::endl;  // 打印绑定失败的详细原因。
    evutil_closesocket(fd);  // 绑定失败后立即关闭套接字。
    return 1;                // 无法监听则直接退出。
  }

  event_base* base = event_base_new();  // 7 创建 libevent 事件循环基对象。
  if (!base) {                          // 检查事件循环是否创建成功。
    std::cerr << "Could not initialize libevent."
              << std::endl;  // 初始化失败提示。
    evutil_closesocket(fd);  // 释放套接字资源。
    return 1;                // 事件循环不可用则退出。
  }

  // EV_ET(边缘触发-变化触发)
  // 默认是水平触发 EV_LEVEL(只要数据没有被读完就一直触发)
  // event_self_cbarg() 作用: 传递当前创建的 event 作为回调参数
  event* read_event =
      event_new(base, fd, EV_READ | EV_PERSIST, ReadCB,
                event_self_cbarg());  // 8 注册一个持久的读事件处理 UDP 套接字。
  if (!read_event) {                  // 判断事件创建是否成功。
    std::cerr << "Could not create UDP read event."
              << std::endl;  // 创建失败日志。
    event_base_free(base);   // 释放事件循环资源。
    evutil_closesocket(fd);  // 关闭套接字。
    return 1;                // 无法注册读事件则退出。
  }

  if (event_add(read_event, nullptr) < 0) {  // 9 将读事件加入事件循环。
    std::cerr << "Could not add UDP read event."
              << std::endl;  // 添加失败时输出错误。
    event_free(read_event);  // 释放读事件资源。
    event_base_free(base);   // 释放事件循环资源。
    evutil_closesocket(fd);  // 关闭套接字。
    return 1;                // 事件无法监控，则终止。
  }

  event* signal_event = evsignal_new(base, SIGINT, SignalCB,
                                     base);  // 10 创建监听 Ctrl+C 的信号事件。
  if (!signal_event || event_add(signal_event, nullptr) <
                           0) {  // 11 确认信号事件创建并成功注册。
    std::cerr << "Could not create SIGINT handler."
              << std::endl;  // 信号处理器安装失败提示。
    if (signal_event) {
      event_free(signal_event);  // 如果创建成功但添加失败，释放资源。
    }
    event_free(read_event);  // 释放读事件。
    event_base_free(base);   // 释放事件循环。
    evutil_closesocket(fd);  // 关闭套接字。
    return 1;                // 无法优雅退出则终止程序。
  }

  std::cout << "test_event_udp listening on " << bind_ip << ':' << bind_port
            << std::endl;  // 输出当前监听的地址与端口。

  // 事件主循环 监控事件是否发送过来 分发事件到相应的回调。
  // 如果没有事件注册则退出
  event_base_dispatch(base);  // 15 进入 libevent 事件循环。

  event_free(read_event);    // 16 事件循环结束后释放读事件。
  event_free(signal_event);  // 释放信号事件。
  event_base_free(base);     // 销毁事件循环基对象。
  evutil_closesocket(fd);    // 关闭 UDP 套接字。

  std::cout << "test_event_udp stopped." << std::endl;  // 通知服务已停止。
  return 0;                                             // 返回 0 表示正常退出。
}
