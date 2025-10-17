cmake_minimum_required(VERSION 3.16)

project(test_event_udp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBEVENT REQUIRED libevent)

add_executable(test_event_udp main.cpp)

if(LIBEVENT_INCLUDE_DIRS)
    target_include_directories(test_event_udp PRIVATE ${LIBEVENT_INCLUDE_DIRS})
endif()
if(LIBEVENT_CFLAGS_OTHER)
    target_compile_options(test_event_udp PRIVATE ${LIBEVENT_CFLAGS_OTHER})
endif()

target_link_libraries(test_event_udp PRIVATE ${LIBEVENT_LIBRARIES})

include(GNUInstallDirs)
install(TARGETS test_event_udp
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
