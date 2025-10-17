#include <arpa/inet.h>
#include <event2/event.h>
#include <event2/util.h>
#include <signal.h>
#include <sys/socket.h>

#include <array>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

namespace {

void OnSignal(evutil_socket_t /*fd*/, short /*events*/, void* arg) {
  auto* base = static_cast<event_base*>(arg);
  std::cout << "\nCaught SIGINT, shutting down." << std::endl;
  event_base_loopbreak(base);
}

void OnRead(evutil_socket_t fd, short /*events*/, void* /*ctx*/) {
  std::array<char, 2048> buffer{};
  sockaddr_storage client_addr{};
  socklen_t client_len = sizeof(client_addr);

  const ssize_t received =
      recvfrom(fd, buffer.data(), buffer.size(), 0,
               reinterpret_cast<sockaddr*>(&client_addr), &client_len);
  if (received < 0) {
    const int err = errno;
    if (err == EAGAIN || err == EWOULDBLOCK) {
      return;
    }
    std::cerr << "recvfrom failed: " << evutil_socket_error_to_string(err)
              << std::endl;
    return;
  }

  std::cout << "Received UDP packet: " << buffer.data() << std::endl;

  std::string reply = "UDP Echo: ";
  reply.append(buffer.data(), static_cast<size_t>(received));

  const ssize_t sent =
      sendto(fd, reply.data(), reply.size(), 0,
             reinterpret_cast<sockaddr*>(&client_addr), client_len);
  if (sent < 0) {
    const int err = errno;
    std::cerr << "sendto failed: " << evutil_socket_error_to_string(err)
              << std::endl;
  }
}

}  // namespace

int main(int argc, char** argv) {
  std::string bind_ip = "172.17.0.2";
  int bind_port = 9340;

  if (argc > 1) {
    char* end = nullptr;
    const long parsed_port = std::strtol(argv[1], &end, 10);
    if (argv[1] && *argv[1] != '\0' && (!end || *end == '\0') &&
        parsed_port > 0 && parsed_port <= 65535) {
      bind_port = static_cast<int>(parsed_port);
    } else {
      bind_ip = argv[1];
    }
  }

  if (argc > 2) {
    char* end = nullptr;
    const long parsed_port = std::strtol(argv[2], &end, 10);
    if (argv[2] && *argv[2] != '\0' && (!end || *end == '\0') &&
        parsed_port > 0 && parsed_port <= 65535) {
      bind_port = static_cast<int>(parsed_port);
    } else {
      bind_ip = argv[2];
    }
  }

  if (argc > 3) {
    bind_ip = argv[3];
  }

  in_addr addr{};
  if (inet_pton(AF_INET, bind_ip.c_str(), &addr) != 1) {
    std::cerr << "Invalid IP argument. Using default." << std::endl;
    bind_ip = "0.0.0.0";
    inet_pton(AF_INET, bind_ip.c_str(), &addr);
  }

  evutil_socket_t fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd < 0) {
    std::cerr << "Could not create UDP socket." << std::endl;
    return 1;
  }

  if (evutil_make_listen_socket_reuseable(fd) < 0) {
    std::cerr << "Could not make socket reusable." << std::endl;
    evutil_closesocket(fd);
    return 1;
  }

  if (evutil_make_socket_nonblocking(fd) < 0) {
    std::cerr << "Could not set socket to non-blocking." << std::endl;
    evutil_closesocket(fd);
    return 1;
  }

  if (evutil_make_socket_closeonexec(fd) < 0) {
    std::cerr << "Could not set close-on-exec on socket." << std::endl;
    evutil_closesocket(fd);
    return 1;
  }

  sockaddr_in sin{};
  sin.sin_family = AF_INET;
  sin.sin_addr = addr;
  sin.sin_port = htons(static_cast<uint16_t>(bind_port));

  if (bind(fd, reinterpret_cast<sockaddr*>(&sin), sizeof(sin)) < 0) {
    const int err = errno;
    std::cerr << "bind failed: " << evutil_socket_error_to_string(err)
              << std::endl;
    evutil_closesocket(fd);
    return 1;
  }

  event_base* base = event_base_new();
  if (!base) {
    std::cerr << "Could not initialize libevent." << std::endl;
    evutil_closesocket(fd);
    return 1;
  }

  event* read_event =
      event_new(base, fd, EV_READ | EV_PERSIST, OnRead, nullptr);
  if (!read_event) {
    std::cerr << "Could not create UDP read event." << std::endl;
    event_base_free(base);
    evutil_closesocket(fd);
    return 1;
  }

  if (event_add(read_event, nullptr) < 0) {
    std::cerr << "Could not add UDP read event." << std::endl;
    event_free(read_event);
    event_base_free(base);
    evutil_closesocket(fd);
    return 1;
  }

  event* signal_event = evsignal_new(base, SIGINT, OnSignal, base);
  if (!signal_event || event_add(signal_event, nullptr) < 0) {
    std::cerr << "Could not create SIGINT handler." << std::endl;
    if (signal_event) {
      event_free(signal_event);
    }
    event_free(read_event);
    event_base_free(base);
    evutil_closesocket(fd);
    return 1;
  }

  std::cout << "test_event_udp listening on " << bind_ip << ':' << bind_port
            << std::endl;
  event_base_dispatch(base);

  event_free(read_event);
  event_free(signal_event);
  event_base_free(base);
  evutil_closesocket(fd);

  std::cout << "test_event_udp stopped." << std::endl;
  return 0;
}
