cmake_minimum_required(VERSION 3.16)

project(test_thread_pool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 根据系统类型查找 libevent 库
if(WIN32)
    # Windows 系统：使用 find_package
    find_package(Libevent CONFIG REQUIRED)
else()
    # Linux 系统：使用 pkg-config
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBEVENT REQUIRED libevent)
endif()

add_executable(test_thread_pool
    main.cpp
    hftp_server_cmd.cpp
    hthread_pool.cpp
    hthread.cpp
)

# 链接 libevent 库
if(WIN32)
    target_link_libraries(test_thread_pool PRIVATE libevent::core libevent::extra)
else()
    target_include_directories(test_thread_pool PRIVATE ${LIBEVENT_INCLUDE_DIRS})
    target_link_libraries(test_thread_pool PRIVATE ${LIBEVENT_LIBRARIES})
    target_link_directories(test_thread_pool PRIVATE ${LIBEVENT_LIBRARY_DIRS})
endif()

include(GNUInstallDirs)
install(TARGETS test_thread_pool
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
