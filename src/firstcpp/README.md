# XMS 项目代码结构说明

## 项目概述
XMS 是一个基于 CMake 构建系统的 C++ 项目，采用模块化设计，支持多子项目管理。

## 目录结构

```
xms/
├── CMakeLists.txt              # 主项目CMake配置文件
├── CMakeLists.txt.user         # Qt Creator用户配置文件
├── LICENSE                     # 许可证文件
├── bin/                        # 可执行文件输出目录
│   └── firstcpp               # firstcpp子项目的可执行文件
├── build/                      # CMake构建目录
│   ├── cmake_install.cmake    # CMake安装脚本
│   ├── CMakeCache.txt         # CMake缓存文件
│   ├── compile_commands.json  # 编译命令数据库
│   ├── Makefile              # 生成的Makefile
│   └── CMakeFiles/           # CMake生成的临时文件
├── doc/                       # 文档目录
│   └── 代码结构.md            # 本文档
├── include/                   # 头文件目录（预留）
├── lib/                       # 库文件目录（预留）
└── src/                       # 源代码目录
    └── firstcpp/              # firstcpp子项目
        ├── CMakeLists.txt     # firstcpp子项目CMake配置
        └── main.cpp           # firstcpp主程序源文件
```

## CMake 配置说明

### 主项目配置 (`/CMakeLists.txt`)
```cmake
cmake_minimum_required(VERSION 3.16)   # 最低CMake版本要求
project(xms LANGUAGES CXX)             # 项目名称和语言设置
add_subdirectory(src/firstcpp)         # 添加firstcpp子项目
```

### firstcpp 子项目配置 (`/src/firstcpp/CMakeLists.txt`)
```cmake
cmake_minimum_required(VERSION 3.16)                    # 最低CMake版本要求
project(firstcpp LANGUAGES CXX)                         # 子项目名称
set(CMAKE_CXX_STANDARD 17)                              # C++17标准
set(CMAKE_CXX_STANDARD_REQUIRED ON)                     # 强制要求C++17
add_executable(firstcpp main.cpp)                       # 创建可执行文件目标

include(GNUInstallDirs)                                  # 包含GNU安装目录定义
install(TARGETS firstcpp                                 # 安装配置
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}          # 库文件安装目录
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}          # 可执行文件安装目录
)
```

## 子项目说明

### firstcpp
- **功能**: 基础的 "Hello World" C++ 程序
- **源文件**: `src/firstcpp/main.cpp`
- **编译产物**: 可执行文件 `firstcpp`
- **C++ 标准**: C++17
- **依赖**: 仅依赖标准库

### main.cpp 内容
```cpp
#include <iostream>
using namespace std;

int main()
{
    cout << "Hello World!" << endl;
    return 0;
}
```

## 构建说明

### 构建命令
```bash
# 创建并进入构建目录
mkdir -p build && cd build

# 配置项目
cmake ..

# 编译项目
make

# 安装（可选）
make install
```

### 运行程序
```bash
# 从构建目录运行
./src/firstcpp/firstcpp

# 或从bin目录运行（如果已安装）
./bin/firstcpp
```

## 扩展规划

项目结构设计支持未来扩展：
- `include/` 目录可用于存放公共头文件
- `lib/` 目录可用于存放静态/动态库
- `src/` 目录可添加更多子项目
- 采用模块化CMake配置，便于管理多个子项目

## 开发环境

- **CMake**: 3.16 或更高版本
- **编译器**: 支持 C++17 的编译器（如 GCC 7+, Clang 5+, MSVC 2017+）
- **构建系统**: Make（Linux/macOS）或 Visual Studio（Windows）