# clangformat.cmake 提供代码格式化功能

# 查找 clang-format 可执行文件
if(NOT CLANG_FORMAT)
  find_program(
    CLANG_FORMAT
    NAMES clang-format
    DOC "clang-format executable")
endif()

if(CLANG_FORMAT)
  message(STATUS "Found clang-format: ${CLANG_FORMAT}")
else()
  message(
    WARNING
      "clang-format not found. Code formatting targets will not be available.")
endif()

# 添加代码格式化函数 参数: CODE_PATH - 需要格式化的代码路径(相对于项目根目录)
function(add_clang_format_target CODE_PATH)
  if(NOT CLANG_FORMAT)
    message(
      WARNING "clang-format not found. Skipping format target for ${CODE_PATH}")
    return()
  endif()

  # 获取绝对路径
  get_filename_component(ABSOLUTE_CODE_PATH "${CMAKE_SOURCE_DIR}/${CODE_PATH}"
                         ABSOLUTE)

  if(NOT EXISTS "${ABSOLUTE_CODE_PATH}")
    message(WARNING "Code path does not exist: ${ABSOLUTE_CODE_PATH}")
    return()
  endif()

  # 收集所有 .h 和 .cpp 文件
  file(
    GLOB_RECURSE
    FORMAT_SOURCES
    "${ABSOLUTE_CODE_PATH}/*.h"
    "${ABSOLUTE_CODE_PATH}/*.cpp"
    "${ABSOLUTE_CODE_PATH}/*.hpp"
    "${ABSOLUTE_CODE_PATH}/*.cc"
    "${ABSOLUTE_CODE_PATH}/*.cxx")

  if(NOT FORMAT_SOURCES)
    message(WARNING "No source files found in ${CODE_PATH}")
    return()
  endif()

  # 生成目标名称(将路径中的斜杠替换为下划线)
  string(REPLACE "/" "_" TARGET_SUFFIX "${CODE_PATH}")
  string(REPLACE "\\" "_" TARGET_SUFFIX "${TARGET_SUFFIX}")
  set(FORMAT_TARGET_NAME "format_${TARGET_SUFFIX}")

  # 添加格式化目标
  add_custom_target(
    ${FORMAT_TARGET_NAME}
    COMMAND ${CLANG_FORMAT} -i --style=file ${FORMAT_SOURCES}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running clang-format on ${CODE_PATH}"
    VERBATIM)

  message(
    STATUS "Added clang-format target: ${FORMAT_TARGET_NAME} for ${CODE_PATH}")
  message(STATUS "  Files to format: ${FORMAT_SOURCES}")
  message(
    STATUS
      "  Run with: ${R}cmake --build ${CMAKE_BINARY_DIR} --target ${FORMAT_TARGET_NAME}${E}"
  )

endfunction()

# 添加格式化所有代码的目标
function(add_clang_format_all)
  if(NOT CLANG_FORMAT)
    message(WARNING "clang-format not found. Skipping format-all target")
    return()
  endif()

  # 收集项目中所有 .h 和 .cpp 文件
  file(GLOB_RECURSE ALL_FORMAT_SOURCES "${CMAKE_SOURCE_DIR}/core/*.h"
       "${CMAKE_SOURCE_DIR}/core/*.cpp" "${CMAKE_SOURCE_DIR}/app/*.h"
       "${CMAKE_SOURCE_DIR}/app/*.cpp")

  if(NOT ALL_FORMAT_SOURCES)
    message(WARNING "No source files found in project")
    return()
  endif()

  # 添加格式化所有代码的目标
  add_custom_target(
    clang-format-all
    COMMAND ${CLANG_FORMAT} -i --style=file ${ALL_FORMAT_SOURCES}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running clang-format on all source files"
    VERBATIM)

  message(STATUS "Added clang-format target: clang-format-all")
  message(
    STATUS
      "  Run with: cmake --build ${CMAKE_BINARY_DIR} --target clang-format-all")

endfunction()
