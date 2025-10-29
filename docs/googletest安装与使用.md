# GoogleTest 安装与使用

## 1. Ubuntu（源码编译）

```bash
sudo apt update
git clone https://github.com/google/googletest.git
cd googletest
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
sudo cmake --install build
```

`CMakeLists.txt` 片段：

```cmake
cmake_minimum_required(VERSION 3.16)
project(demo_tests LANGUAGES CXX)

find_package(GTest REQUIRED)
enable_testing()

add_executable(sample_test tests/sample_test.cpp)
target_link_libraries(sample_test PRIVATE GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(sample_test)
```

执行测试：

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

## 2. Windows（vcpkg）

1. 安装 vcpkg 并添加到 `CMAKE_TOOLCHAIN_FILE`.
2. 在项目根目录创建 `vcpkg.json`：

```json
{
  "dependencies": [
    {
      "name": "gtest",
      "features": []
    }
  ]
}
```

3. 配置 CMake：

```powershell
cmake -S . -B build -G "Ninja" `
  -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build
ctest --test-dir build
```

## 3. 基本用例

`tests/sample_test.cpp`：

```cpp
#include <gtest/gtest.h>

int Add(int a, int b) { return a + b; }

TEST(MathTest, Add) {
  EXPECT_EQ(Add(1, 2), 3);
  EXPECT_NE(Add(4, 5), 12);
}
```

常见命令：

```bash
ctest --test-dir build --output-on-failure
```