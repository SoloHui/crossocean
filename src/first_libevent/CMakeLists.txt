cmake_minimum_required(VERSION 3.16)

project(first_libevent LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找 libevent
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBEVENT REQUIRED libevent)

add_executable(first_libevent main.cpp)

# 链接 libevent 库
target_link_libraries(first_libevent PRIVATE ${LIBEVENT_LIBRARIES})

include(GNUInstallDirs)
install(TARGETS first_libevent
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
