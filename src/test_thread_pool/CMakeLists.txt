cmake_minimum_required(VERSION 3.16)

project(test_thread_pool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBEVENT REQUIRED libevent)

add_executable(test_thread_pool
    main.cpp
    hftp_server_cmd.cpp
    hthread_pool.cpp
    hthread.cpp
)

if(LIBEVENT_INCLUDE_DIRS)
    target_include_directories(test_thread_pool PRIVATE ${LIBEVENT_INCLUDE_DIRS})
endif()
if(LIBEVENT_CFLAGS_OTHER)
    target_compile_options(test_thread_pool PRIVATE ${LIBEVENT_CFLAGS_OTHER})
endif()

target_link_libraries(test_thread_pool PRIVATE ${LIBEVENT_LIBRARIES})

include(GNUInstallDirs)
install(TARGETS test_thread_pool
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
