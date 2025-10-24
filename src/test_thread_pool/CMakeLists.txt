cmake_minimum_required(VERSION 3.16)

project(test_thread_pool LANGUAGES CXX)

include(${CMAKE_SOURCE_DIR}/cmake/common.cmake)

# 根据系统类型查找 libevent 库
if(WIN32)
    # Windows 系统：使用 find_package
    find_package(Libevent CONFIG REQUIRED)
else()
    # Linux 系统：使用 pkg-config
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBEVENT REQUIRED libevent)
endif()

cpp_execute(${PROJECT_NAME})

# 链接 libevent 库
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE libevent::core libevent::extra)
else()
    target_include_directories(${PROJECT_NAME} PRIVATE ${LIBEVENT_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBEVENT_LIBRARIES})
    target_link_directories(${PROJECT_NAME} PRIVATE ${LIBEVENT_LIBRARY_DIRS})
endif()
