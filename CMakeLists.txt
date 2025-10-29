cmake_minimum_required(VERSION 3.16)

project(crossocean LANGUAGES CXX)

# 构建选项
option(BUILD_FORMAT "Build format" OFF)
option(BUILD_TEST "Build tests" OFF)

# 核心库
add_subdirectory(core/com)

# 应用程序
add_subdirectory(app/disk_server)

# 启用测试
if(BUILD_TEST)
  message(STATUS "Build tests enabled")
  enable_testing()
  add_subdirectory(core/com/test)
endif()

# 启用代码格式化
if(BUILD_FORMAT)
  message(STATUS "Build format enabled")
  # 引入`clang-format`模块
  include(cmake/clangformat.cmake)
  # 添加格式化所有代码的目标
  add_clang_format_all()

  # 引入`cmake-format`模块
  include(cmake/cmakeformat.cmake)
  # 添加格式化所有`CMake`文件的目标
  add_cmake_format_all()
endif()
