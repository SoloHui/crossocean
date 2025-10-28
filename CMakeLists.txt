cmake_minimum_required(VERSION 3.16)

project(xms LANGUAGES CXX)

# 添加子目录
# add_subdirectory(src/cpp_fmt)
# add_subdirectory(src/firstcpp)
# add_subdirectory(src/untitled)
# add_subdirectory(src/testdox)

# add_subdirectory(src/first_libevent)
# add_subdirectory(src/test_event_server)
# add_subdirectory(src/test_event_udp)
# add_subdirectory(src/test_thread_pool)

# 核心库
add_subdirectory(core/com)

# 应用程序
add_subdirectory(app/disk_server)