# src/hdisk_server/CMakeLists.txt

cmake_minimum_required(VERSION 3.16)

project(hdisk_server LANGUAGES CXX)

include(${CMAKE_SOURCE_DIR}/cmake/common.cmake)

# 获取动态库
list(PREPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/out")

cpp_execute(${PROJECT_NAME} hcom)

# Winsock networking support
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)
endif()

# 配置动态库的头文件路径
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/out/include/hcom
)