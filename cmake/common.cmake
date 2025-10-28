# cmake/common.cmake

# 公司名称缩写
set(COMPANY_NAME "co")
# 统一的命名空间
set(CROSSOCEAN_NAMESPACE "namespace co {")
set(USING_CROSSOCEAN_NAMESPACE "using namespace co;")

# CMAKE_SOURCE_DIR是项目根目录
set(RUNTIME_DIR ${CMAKE_SOURCE_DIR}/bin)
set(LIBRARY_DIR ${CMAKE_SOURCE_DIR}/lib)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 获取当前目录下源码和头文件
macro(get_src_include)
    # CMAKE_CURRENT_LIST_DIR 当前CMakeLists.txt文件所在路径
    aux_source_directory(${CMAKE_CURRENT_LIST_DIR} SRC)
    
    # 加入.h文件到编译依赖
    FILE(GLOB H_FILE ${CMAKE_CURRENT_LIST_DIR}/*.h)
    
    # 对外接口的头文件
    FILE(GLOB H_FILE_I ${CMAKE_CURRENT_LIST_DIR}/include/*.h)
endmacro()


#配置编译参数
macro(set_cpp name)
    #############################################################
    # 设置头文件查找路径
    target_include_directories(${name} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/include/
        ${CMAKE_CURRENT_LIST_DIR}
    )

    #############################################################
    # 配置vs属性 bigobj
    if(MSVC)
        set_target_properties(${name} PROPERTIES
            COMPILE_FLAGS "-bigobj"
        )
    endif()

    if(NOT WIN32)
        target_link_libraries(${name} PRIVATE pthread)
    endif()
    
    #############################################################
    # 输出路径配置 Debug和Release配置同一个路径
    # 默认为Debug
    if(CMAKE_BUILD_TYPE STREQUAL "")
        set(CMAKE_BUILD_TYPE Debug)
    endif()
    
    # 配置输出路径 Debug Release版本输出路径一致
    set(CONF_TYPES Debug Release RelWithDebInfo MinSizeRel)
    list(APPEND CONF_TYPES "")
    foreach(type IN LISTS CONF_TYPES)
        set(conf "")
        if(type)
            string(TOUPPER _${type} conf)
            if("${type}" STREQUAL "${CMAKE_BUILD_TYPE}")
                message("CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
            endif()
        endif()
        set_target_properties(${name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY${conf} ${RUNTIME_DIR}  #dll  exe 执行程序
        LIBRARY_OUTPUT_DIRECTORY${conf} ${LIBRARY_DIR}  # .so .dylib 
        ARCHIVE_OUTPUT_DIRECTORY${conf} ${LIBRARY_DIR}  # .lib .a
        PDB_OUTPUT_DIRECTORY${conf} ${RUNTIME_DIR} #pdb
        )
    endforeach()
    
    # 设置debug添加d后缀
    set_target_properties(${name} PROPERTIES
        DEBUG_POSTFIX "d"
    )

    # 配置导入路径路径
    target_link_directories(${name} PRIVATE ${LIBRARY_DIR})

    set(debug_postfix "")
    # 拿到配置属性
    if(WIN32)
        get_target_property(debug_postfix ${name} DEBUG_POSTFIX)
    endif()

    if(NOT WIN32)
        target_link_libraries(${name} PRIVATE pthread)
    endif()

endmacro()


function (cpp_test name)
    message(STATUS "Building test: ${name}")
    message("==========================================================================================")

    # 获取当前目录下源码和头文件
    get_src_include()

    # 添加执行程序
    add_executable(${name} ${SRC})
    set_cpp(${name})

    set_target_properties(${name} PROPERTIES
        MSVC_RUNTIME_LIBRARY MultiThreadedDebug
    )

    # 设置 find_package 查找路径
    find_package(GTest)
    message("GTest = ${GTest_FOUND}")
    
    # gtest 库查找路径
    target_link_directories(${name} PRIVATE ${GTEST_PATH}/lib)
    
    # gtest 头文件路径
    target_include_directories(${name} PRIVATE ${GTEST_PATH}/include)
    if(WIN32)
        target_link_libraries(${name}
            GTest::gtest_main
        )
    else()
        target_link_libraries(${name}
            gtest_main
            gtest
        )
        target_link_libraries(${name} pthread)
    endif()

    # 联合ctest和gtest
    include(GoogleTest)
    gtest_discover_tests(${name})

    # 打开才能运行ctest
    enable_testing() 

    message("==========================================================================================\n")
endfunction()



############################
### 编译执行程序cpp_execute(<name> [lib1] [lib2...])
function (cpp_execute name)

    message(STATUS "Building executable: ${name}")
    message("==========================================================================================")

    # 获取当前目录下源码和头文件
    get_src_include()

    # 添加执行程序
    add_executable(${name} ${SRC} ${H_FILE} ${H_FILE_I})
    
    # 设置配置信息
    set_cpp(${name})

    # 链接依赖库
    message("ARGC = ${ARGC} ${ARGV0} ${ARGV1}")
    math(EXPR size "${ARGC}-1")
    if(size GREATER 0)
        foreach(i RANGE 1 ${size} )
            message("target_link_libraries ${ARGV${i}}")
            set(lib_name ${ARGV${i}})
            target_link_libraries(${name} PRIVATE ${lib_name}${debug_postfix})
        endforeach()
    endif()
    message("==========================================================================================\n")
endfunction()


function(cpp_library name)

    message(STATUS "Building library: ${name}")
    message("==========================================================================================")
    
    #############################################################
    # 配置项目是否是动态库
    # 用户指定xlog是动态库还是静态库
    string(TOUPPER ${name} NAME)
    option(${NAME}_SHARED "OFF is static cpp_library" ON)
    message("${NAME}_SHARED = ${${NAME}_SHARED}")
    set(TYPE STATIC)
    if(${NAME}_SHARED)
        set(TYPE SHARED)
    endif()

    #############################################################
    # 生成c++代码，传递命名空间
    configure_file(${CMAKE_SOURCE_DIR}/cmake/crossocean.h.in ${CMAKE_CURRENT_LIST_DIR}/include/crossocean.h)

    # 查找项目的源码和头文件
    get_src_include()

    #############################################################
    #生成库
    add_library(${name} ${TYPE} ${SRC} ${H_FILE} ${H_FILE_I})
    
    set_cpp(${name})

    #############################################################
    # 静态库和动态库传递不同的宏变量给c++
    # XLOG_STATIC XLOG_EXPORTS
    if(${NAME}_SHARED)
        target_compile_definitions(${name} PUBLIC ${NAME}_EXPORTS)
    else()
        target_compile_definitions(${name} PUBLIC ${NAME}_STATIC)
    endif()
    
    #############################################################
    # 库的安装配置
    # ${CMAKE_INSTALL_PREFIX}  
    # cmake -S . -B build -D CMAKE_INSTALL_PREFIX=./out
    # cmake --install build --prefix=out
    set(version 1.0)
    #设置安装的头文件
    set_target_properties(${name} PROPERTIES
        PUBLIC_HEADER "${H_FILE_I}"
    )
    install(TARGETS ${name}
        EXPORT ${name}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include/${name}
    )
    
    # 支持find_package
    # 生成并安装配置文件 xlogConfig.cmake
    install(EXPORT ${name} FILE ${name}Config.cmake
        DESTINATION lib/config/${name}-${version}
    )

    # 版本文件
    set(CONF_VER_FILE 
        ${CMAKE_CURRENT_LIST_DIR}/../../lib/conf/${name}-${version}/${name}ConfigVersion.cmake)
    
    message("CONF_VER_FILE = ${CONF_VER_FILE}")
    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
        ${CONF_VER_FILE}
        VERSION ${version}
        COMPATIBILITY SameMajorVersion  # 版本兼容问题
    )
    install(FILES ${CONF_VER_FILE}
        DESTINATION lib/config/${name}-${version}
    )
    message("==========================================================================================\n")
endfunction()
