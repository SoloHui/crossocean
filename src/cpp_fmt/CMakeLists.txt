cmake_minimum_required(VERSION 3.16)

project(cpp_fmt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(cpp_fmt main.cpp
    test_head1.h test_head1.cpp
    test_head2.h test_head2.cpp
    i_thread.h
)

include(GNUInstallDirs)
install(TARGETS cpp_fmt
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
