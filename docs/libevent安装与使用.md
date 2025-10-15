### libevent安装与使用

1. 安装libevent

```bash
sudo apt update

sudo apt install libevent-dev

```

2. 使用libevent

> `cmake`中使用`pkg-config`查找并链接`libevent`库

```bash
sudo apt install pkg-config

```

```cmake
# 查找 libevent
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBEVENT REQUIRED libevent)

add_executable(first_libevent main.cpp)

# 链接 libevent 库
target_link_libraries(first_libevent PRIVATE ${LIBEVENT_LIBRARIES})

```
