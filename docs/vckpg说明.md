## vcpkg 使用说明总结

### 核心命令：包的安装与管理

这是最常用的一组命令，用于获取、编译和安装库。

1.  **搜索包**
    ```bash
    vcpkg search [包名或部分名]
    ```
    **示例：** `vcpkg search curl`，`vcpkg search zlib`
    *   在安装前，先用这个命令确认包的确切名称和可用的版本。

2.  **安装包**
    ```bash
    vcpkg install [包名]
    ```
    **示例：** `vcpkg install curl`，`vcpkg install sfml`
    *   这是最核心的命令。它会自动下载、编译并安装库到你的 vcpkg 目录下。
    *   默认会编译适合你当前开发环境的版本（Windows 上编译 x86-windows，Linux/macOS 上编译 x64-linux / x64-osx）。

3.  **指定三元组安装**
    ```bash
    vcpkg install [包名]:[三元组]
    ```
    **示例：**
    ```bash
    vcpkg install curl:x64-windows      # 安装 64 位 Windows 版本
    vcpkg install sfml:x86-windows      # 安装 32 位 Windows 版本
    vcpkg install zlib:arm64-windows    # 安装 ARM64 Windows 版本
    vcpkg install sdl2:x64-linux-dynamic # 安装 Linux 动态链接版本
    ```
    *   **三元组** 是 vcpkg 的核心概念，用于指定目标平台、架构和链接方式。
    *   常见的三元组：`x64-windows`，`x86-windows`，`x64-windows-static`，`x64-linux`，`x64-osx`。

4.  **移除已安装的包**
    ```bash
    vcpkg remove [包名]:[三元组]
    ```
    **示例：** `vcpkg remove curl:x64-windows`
    *   从 vcpkg 的安装目录中移除该库。

5.  **移除包及其依赖**
    ```bash
    vcpkg remove --recurse [包名]:[三元组]
    ```
    **示例：** `vcpkg remove --recurse curl:x64-windows`
    *   如果一个库被其他多个库依赖，普通的 `remove` 不会删除它。使用 `--recurse` 会递归删除那些不再被任何已安装库依赖的包。**（推荐使用）**

---

### 项目集成命令

安装完库后，需要让你的 CMake 或 MSBuild 项目能找到它们。

1.  **集成安装**
    ```bash
    vcpkg integrate install
    ```
    *   **全局集成**：执行后，系统范围内的 Visual Studio 和 CMake 项目都能自动找到通过 vcpkg 安装的库。对于新手来说，这是最简单的方式。
    *   它会设置全局的环境变量和 NuGet 源。

2.  **集成移除**
    ```bash
    vcpkg integrate remove
    ```
    *   移除全局集成。

3.  **CMake 工具链文件**
    *   这是**更推荐、更现代**的集成方式，尤其是在团队协作或 CI/CD 环境中。
    *   在你的 CMake 项目中，在 `project()` 命令之后，通过 `-DCMAKE_TOOLCHAIN_FILE` 参数指定 vcpkg 的工具链文件。
    ```bash
    cmake -B [build目录] -S . -DCMAKE_TOOLCHAIN_FILE=[vcpkg根目录]/scripts/buildsystems/vcpkg.cmake
    ```
    **示例：**
    ```bash
    # 假设 vcpkg 在 C:\dev\vcpkg
    cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake
    ```

---

### 信息查看与维护命令

1.  **列出已安装的包**
    ```bash
    vcpkg list
    ```
    *   显示所有已经安装的库及其版本和三元组。

2.  **更新 vcpkg 自身**
    ```bash
    # 如果使用 Git 克隆的（推荐方式）
    git pull
    .\bootstrap-vcpkg.bat  # Windows
    ./bootstrap-vcpkg.sh   # Linux/macOS
    ```

3.  **查看帮助**
    ```bash
    vcpkg help
    vcpkg help [命令]  # 例如 vcpkg help install
    ```

4.  **导出已安装的包**
    ```bash
    vcpkg export [包名] --raw
    ```
    **示例：** `vcpkg export curl zlib sfml --raw --output=d:\my_libs`
    *   将指定的包（及其所有依赖）导出到一个目录中，方便离线分发或部署。
    *   `--raw` 选项表示导出为原始的库文件和头文件。

5. **更新`git`基线**
    ```bash
    vcpkg x-update-baseline
    ```
    *   检查并更新 vcpkg 的包清单和基线到最新版本。

---

### 实用命令与技巧

1.  **安装所有在 `vcpkg.json` 中声明的依赖**
    ```bash
    vcpkg install
    ```
    *   在包含 `vcpkg.json` 清单文件的目录下运行此命令，vcpkg 会自动读取该文件并安装所有列出的依赖。这是现代 CMake 项目的最佳实践。

2.  **查看一个包的依赖信息**
    ```bash
    vcpkg depend-info [包名]
    ```
    **示例：** `vcpkg depend-info sfml`

3.  **清理下载的缓存和临时文件**
    ```bash
    vcpkg clean
    ```
    *   清理 `buildtrees`， `packages` 和其他临时目录，释放磁盘空间。

---

### 工作流总结

一个典型的使用 vcpkg 的工作流如下：

1.  **初始化**：使用 Git 克隆 vcpkg 并运行引导脚本。
2.  **搜索**：`vcpkg search some-library` 找到需要的库。
3.  **安装**：`vcpkg install some-library:x64-windows` 安装指定版本的库。
4.  **集成**：
    *   **简单项目**：运行 `vcpkg integrate install`。
    *   **CMake 项目（推荐）**：在 CMake 配置时传入 `-DCMAKE_TOOLCHAIN_FILE=...`。
5.  **在项目中使用**：在 CMakeLists.txt 中使用 `find_package`，或在 Visual Studio 中直接包含头文件和链接库即可。