# core/com/test/CMakeLists.txt

cmake_minimum_required(VERSION 3.16)

project(test_com LANGUAGES CXX)

include(${CMAKE_SOURCE_DIR}/cmake/common.cmake)

# 查找 libevent
if(WIN32)
  find_package(Libevent CONFIG REQUIRED)
else()
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(LIBEVENT REQUIRED libevent)
endif()

# 编译测试
cpp_test(${PROJECT_NAME})

# 链接 libevent 库
if(WIN32)
  target_link_libraries(${PROJECT_NAME} PRIVATE libevent::core libevent::extra)
else()
  target_include_directories(${PROJECT_NAME} PRIVATE ${LIBEVENT_INCLUDE_DIRS})
  target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBEVENT_LIBRARIES})
  target_link_directories(${PROJECT_NAME} PRIVATE ${LIBEVENT_LIBRARY_DIRS})
endif()
