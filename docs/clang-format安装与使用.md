## clang-format 安装与使用

### Ubuntu

1. 编辑 APT 源  

```bash
sudo vim /etc/apt/sources.list
```

添加：
```bash
deb http://apt.llvm.org/jammy/ llvm-toolchain-jammy-19 main
# deb-src http://apt.llvm.org/jammy/ llvm-toolchain-jammy main
```

2. 更新与安装  

```bash
sudo apt update
sudo apt install clang-format-19
```

### Windows
- 在 Microsoft Visual Studio 安装目录中查找并使用自带的 clang-format，可将其所在路径加入系统 PATH 环境变量，或者作为 CMake 变量直接导入。

### CMake 配置示例

编辑`CMakeUserPresets.json`文件，添加 clang-format 配置：

```json
{
  "version": 3,
  "configurePresets": [
    {
      "name": "default",
      "inherits": "windows_debug",
      "cacheVariables": {
        "CLANG_FORMAT": "D:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/Llvm/bin/clang-format.exe"
      }
    },
    {
      "name": "default",
      "inherits": "linux_debug",
      "cacheVariables": {
        "CLANG_FORMAT": "clang-format-19"
      }
    }
  ]
}
```

### 基本命令
```bash
clang-format-19 -style=google -i your_file.cpp
```

注意: clang-format --style=file 会优先查找当前目录或上级目录中的 .clang-format 文件以获取格式化风格配置。