# COM 模块单元测试

本目录包含 COM 通信模块的单元测试代码。

## 文件结构

- `main.cpp` - 测试主入口，初始化 libevent 多线程支持
- `thread_test.cpp` - Thread 类的单元测试
- `thread_pool_test.cpp` - ThreadPool 类的单元测试
- `task_test.cpp` - Task 类的单元测试
- `server_task_test.cpp` - ServerTask 类的单元测试
- `integration_test.cpp` - 集成测试

## 测试内容

### 1. Thread 测试 (ThreadTest)
- **CreateAndSetup**: 测试线程对象的创建和基本功能
- **AddTask**: 测试向线程添加任务
- **ActivateThread**: 测试线程激活功能
- **MultipleTasksProcessing**: 测试多个任务的处理

### 2. ThreadPool 测试 (ThreadPoolTest)
- **Initialization**: 测试线程池的初始化
- **DispatchTask**: 测试任务分发
- **RoundRobinDispatch**: 测试多任务轮询分发策略

### 3. Task 测试 (TaskTest)
- **GettersAndSetters**: 测试 Task 基本属性的 getter 和 setter
- **Initialization**: 测试 Task 的初始化
- **FailedInitialization**: 测试失败任务的处理

### 4. ServerTask 测试 (ServerTaskTest)
- **PortConfiguration**: 测试 ServerTask 端口设置
- **InvalidPortInitialization**: 测试无效端口初始化失败
- **ValidPortInitialization**: 测试有效端口初始化
- **CustomCallback**: 测试自定义回调函数
- **MultipleServerTasks**: 测试多个 ServerTask 使用不同端口

### 5. 集成测试 (IntegrationTest)
- **ThreadPoolWithServerTask**: 线程池与 ServerTask 集成测试
- **ConcurrentTaskProcessing**: 并发任务处理测试

## 编译和运行

### Windows 系统

```bash
# 配置项目
cmake --preset windows_debug

# 编译测试
cmake --build build/windows_debug --target test_com

# 运行测试
.\bin\test_comd.exe

# 或使用 CTest 运行
cd build/windows_debug
ctest -C Debug -V
```

### Linux 系统

```bash
# 配置项目
cmake --preset linux_debug

# 编译测试
cmake --build build/linux_debug --target test_com

# 运行测试
./bin/test_com

# 或使用 CTest 运行
cd build
ctest -V
```

## 测试输出示例

```
[==========] Running 18 tests from 5 test suites.
[----------] Global test environment set-up.
[----------] 4 tests from ThreadTest
[ RUN      ] ThreadTest.CreateAndSetup
[       OK ] ThreadTest.CreateAndSetup
[ RUN      ] ThreadTest.AddTask
[       OK ] ThreadTest.AddTask
...
[==========] 18 tests from 5 test suites ran. (XXX ms total)
[  PASSED  ] 18 tests.
```

## 依赖项

- Google Test (GTest)
- libevent
- C++17 或更高版本

## 注意事项

1. 测试使用高端口号 (18080-18083) 以避免权限问题
2. 某些测试包含睡眠等待，以确保异步操作完成
3. 测试前会自动初始化 libevent 的多线程支持

## 测试覆盖率

当前测试覆盖了以下功能：
- ✅ Thread 的创建、设置和任务处理
- ✅ ThreadPool 的初始化和任务分发
- ✅ Task 的属性管理和初始化
- ✅ ServerTask 的端口配置和监听
- ✅ 线程池与任务的集成
- ✅ 并发任务处理

## 扩展测试

如需添加新的测试用例，请遵循以下模式：

```cpp
TEST(TestSuiteName, TestName) {
  // Arrange - 准备测试数据和环境
  
  // Act - 执行测试操作
  
  // Assert - 验证结果
  EXPECT_TRUE(condition);
}
```
