# cmakeformat.cmake 提供 CMake 代码格式化功能

# 查找 cmake-format 可执行文件
if(NOT CMAKE_FORMAT)
  find_program(
    CMAKE_FORMAT
    NAMES cmake-format
    DOC "cmake-format executable")
endif()

if(CMAKE_FORMAT)
  message(STATUS "Found cmake-format: ${CMAKE_FORMAT}")
else()
  message(
    WARNING
      "cmake-format not found. CMake formatting targets will not be available.")
endif()

# 添加 CMake 文件格式化函数 参数: CMAKE_PATH - 需要格式化的 CMake 文件路径(相对于项目根目录)
function(add_cmake_format_target CMAKE_PATH)
  if(NOT CMAKE_FORMAT)
    message(
      WARNING
        "cmake-format not found. Skipping cmake format target for ${CMAKE_PATH}"
    )
    return()
  endif()

  # 获取绝对路径
  get_filename_component(ABSOLUTE_CMAKE_PATH
                         "${CMAKE_SOURCE_DIR}/${CMAKE_PATH}" ABSOLUTE)

  if(NOT EXISTS "${ABSOLUTE_CMAKE_PATH}")
    message(WARNING "CMake path does not exist: ${ABSOLUTE_CMAKE_PATH}")
    return()
  endif()

  # 收集所有 CMakeLists.txt 和 .cmake 文件
  if(IS_DIRECTORY "${ABSOLUTE_CMAKE_PATH}")
    file(GLOB_RECURSE FORMAT_CMAKE_FILES
         "${ABSOLUTE_CMAKE_PATH}/CMakeLists.txt"
         "${ABSOLUTE_CMAKE_PATH}/*.cmake")
  else()
    # 如果是单个文件
    set(FORMAT_CMAKE_FILES "${ABSOLUTE_CMAKE_PATH}")
  endif()

  if(NOT FORMAT_CMAKE_FILES)
    message(WARNING "No CMake files found in ${CMAKE_PATH}")
    return()
  endif()

  # 生成目标名称(将路径中的斜杠替换为下划线)
  string(REPLACE "/" "_" TARGET_SUFFIX "${CMAKE_PATH}")
  string(REPLACE "\\" "_" TARGET_SUFFIX "${TARGET_SUFFIX}")
  string(REPLACE "." "_" TARGET_SUFFIX "${TARGET_SUFFIX}")
  set(FORMAT_TARGET_NAME "cmake_format_${TARGET_SUFFIX}")

  # 添加格式化目标
  add_custom_target(
    ${FORMAT_TARGET_NAME}
    COMMAND ${CMAKE_FORMAT} -i ${FORMAT_CMAKE_FILES}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running cmake-format on ${CMAKE_PATH}"
    VERBATIM)

  message(
    STATUS "Added cmake-format target: ${FORMAT_TARGET_NAME} for ${CMAKE_PATH}")
  message(STATUS "  Files to format: ${FORMAT_CMAKE_FILES}")
  message(
    STATUS
      "  Run with: cmake --build ${CMAKE_BINARY_DIR} --target ${FORMAT_TARGET_NAME}"
  )

endfunction()

# 添加格式化所有 CMake 文件的目标
function(add_cmake_format_all)
  if(NOT CMAKE_FORMAT)
    message(WARNING "cmake-format not found. Skipping cmake-format-all target")
    return()
  endif()

  # 收集项目中所有 CMakeLists.txt 和 .cmake 文件
  file(
    GLOB_RECURSE
    ALL_CMAKE_FILES
    "${CMAKE_SOURCE_DIR}/CMakeLists.txt"
    "${CMAKE_SOURCE_DIR}/cmake/*.cmake"
    "${CMAKE_SOURCE_DIR}/core/**/CMakeLists.txt"
    "${CMAKE_SOURCE_DIR}/app/**/CMakeLists.txt")

  if(NOT ALL_CMAKE_FILES)
    message(WARNING "No CMake files found in project")
    return()
  endif()

  # 添加格式化所有 CMake 文件的目标
  add_custom_target(
    cmake-format-all
    COMMAND ${CMAKE_FORMAT} -i ${ALL_CMAKE_FILES}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running cmake-format on all CMake files"
    VERBATIM)

  message(STATUS "Added cmake-format target: cmake-format-all")
  message(
    STATUS
      "  Run with: cmake --build ${CMAKE_BINARY_DIR} --target cmake-format-all")

endfunction()
