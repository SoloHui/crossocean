# C++编程规范

规范的代码可以促进团队合作，规范的代码有助于提升代码的可读性，注释规范自动生成文档。
> 示例代码：`src/cpp_fmt`

## 1. 头文件规则
通常每一个.cpp 文件对应一个.h 文件，当然也有特例，main 函数入口cpp 文件没有对应的.h，只有纯虚函数的接口类只有.h 文件没有.cpp。

### 1.1 #define 防止多重包含
- 所有头文件都应该使用#define 防止头文件被多重包含，这里的多重包含指的是一个cpp 编译过程中不被多次包含，若有多个cpp 都调用该头文件，那这个文件还是会被包含多次，因此所有头文件中不要做定义。
- 命名格式：
  - 若为普通的业务逻辑类的代码，如x_msg_task.h 的文件，直接定义为X_MSG_TASK_H。
  - 若用于类库或者公用的库，需把项目名称加在前面。
- 实例（在x_msg_task.h 头文件中）：
```cpp
#ifndef X_MSG_TASK_H
#define X_MSG_TASK_H
// 你的声明代码
#endif
```

### 1.2 减少头文件依赖
- 使用前置声明（forward declarations）尽量减少.h 文件中#include 的数量。
- 当一个头文件被包含时，会引入一项新的依赖，只要该头文件被修改，相关代码就要重新编译。若头文件包含了其他头文件，这些被包含头文件的任何改变，也将导致包含了该头文件的代码重新编译。所以，应尽量少包含头文件，尤其是那些包含在其他头文件中的。
- 示例：用到XThread 类的指针时，在头文件中可不#include 该类文件，直接用`class XThread;`声明；但如果类中定义的是实体对象，则必须引入头文件。
- 建议：在.h 中尽量不引用头文件，在.cpp 中引用。

### 1.3 头文件中不导入命名空间
- 头文件中不调用using 导入命名空间，调用时直接写命名空间，例如`std::string str;`。
- 命名空间导入应在.cpp 中进行，因为无法确认.h 文件会被什么样的文件调用，以及是否会产生参数命名冲突。

## 2. 类规则

### 2.1 构造函数（Constructor）的职责
构造函数中只进行无实际意义的初始化（即非业务逻辑的初始化），尽量在Init()方法中集中初始化业务逻辑数据。

### 2.2 结构体和类（Structs vs. Classes）
- 仅当只有数据时使用struct，其他情况一概使用class。
- 类和结构体的成员变量使用不同的命名规则。

### 2.3 继承（Inheritance）
优先使用组合，若必须使用继承，只使用public 继承。

### 2.4 多重继承（Multiple Inheritance）
- 尽量不使用多重继承。
- 若必须使用，多重继承中只有一个基类是有实现的，其他的都只能是接口类，否则内部对象空间分配容易产生问题。

### 2.5 接口（Interface）

- 接口是指满足特定条件的类，这些类以I 开头（非必需）。
- 接口类中只有纯虚函数（"=0"）和静态函数。

### 2.6 声明次序（Declaration Order）
- 在类中使用声明次序如下：public:、protected:、private:。
- 每一块中，声明次序一般如下：
  1. 构造函数；
  2. 析构函数；
  3. 成员函数，含静态成员函数；
  4. 数据成员，含静态数据成员。

## 3. 格式规则

### 3.1 空格还是制表位（Spaces vs. Tabs）
只使用空格，每次缩进`2`个空格。

### 3.2 括号格式
左大括号总在最后一个参数同一行末尾处,右括号单独起一行，示例：
```cpp
if (true) {
    // 代码内容
}
```

## 4. 命名约定

### 4.1 通用命名规则（General Naming Rules）
- 函数命名、变量命名、文件命名应具有描述性，不要过度缩写。
- 类型和变量应该是名词，函数名可以用“命令性”动词。

### 4.2 文件命名（File Names）
文件名要全部小写，可以包含下划线（_），例如my_useful.cpp。

### 4.3 Class 类型命名（Type Names）
- 类型命名每个单词以大写字母开头，不包含下划线，例如MyExcitingClass、MyExcitingEnum。
- 所有类型命名（类、结构体、类型定义（typedef）、枚举）使用相同约定。

### 4.4 变量命名（Variable Names）
变量名一律小写，单词间以下划线相连，类的成员变量以下划线结尾，例如my_exciting_local_variable、my_exciting_member_variable_。

### 4.5 常量命名（Constant Names）
声明为constexpr 或const 的常量命名时以`k`开头，大小写混合，单词间无下划线，例如`kMaxEntries`。

### 4.6 函数命名（Function Names）
#### 普通函数
函数名以大写字母开头，每个单词首字母大写，没有下划线，例如AddTableEntry()。

#### 成员存取函数
示例：
```cpp
class MyClass {
public:
    int num_entries() const { return num_entries_; }
    void set_num_entries(int num_entries) { num_entries_ = num_entries; }
private:
    int num_entries_;
};
```

### 4.7 宏、枚举命名
- 宏命名使用全部大写+下划线，例如`#define PI_ROUNDED 3.0`。
- 枚举名称属于类型，因此大小写混合，示例：
```cpp
enum UrlTableErrors
{
    OK = 0,
    ERROR_OUT_OF_MEMORY,
    ERROR_MALFORMED_INPUT,
};
```

## 5. Doxygen 注释规则

### 5.1 文件注释
- 文件注释通常放在整个文件开头。
- 项目注释：
```cpp
///////////////////////////////////////////////////////////////////////////
/// @mainpage 项目注释
/// @author 作者
/// @version 版本
/// @date 2019 年07 月10 日
///////////////////////////////////////////////////////////////////////////
```
- 文件注释：
```cpp
///////////////////////////////////////////////////////////////////////////
/// @file 文件名
/// @brief 简介
/// @details 细节
```

### 5.2 类定义注释
```cpp
/// @brief 类的简单概述
/// @details 类的详细概述
```

### 5.3 常量/变量的注释
- 代码前注释：`/// 常量/变量说明 常量/变量`
- 代码后注释：`常量/变量 ///< 说明`（一般用于变量数量较多且名字短的情况）

### 5.4 函数注释
```cpp
/// @brief 函数简介
/// @param 形参1 参数说明
/// @param 形参2 参数说明
/// @return 返回说明
```

### 5.5 Doxygen 设置使用
- 设定项目名称和项目语言。
- 设定编码方式（INPUT_ENCODING）：vs 的项目一般设置为GBK。
- 其他设置可在Doxygen GUI frontend 中操作，步骤如下：
  1. Step 1：指定Doxygen 运行的工作目录（Specify the working directory from which doxygen will run），点击“Select...”选择目录。
  2. Step 2：通过Tizard 和/或Expert 标签配置Doxygen，然后切换到Run 标签生成文档（Configure doxygen using the Tizard and/or Expert tab, then switch to the Run tab to generate the documentation）。
  3. 相关配置项示例：INPUT_ENCODING 可设为UTF-8，设置FILE_PATTERS、RECURSIVE 等参数。