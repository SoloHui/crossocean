在 Ubuntu 上使用 Doxygen 可以分为以下几个步骤：

## 1. 安装 Doxygen

```bash
# 更新软件包列表
sudo apt update

# 安装 doxygen
sudo apt install doxygen

# 安装图形化工具（可选）
sudo apt install doxygen-gui
```

## 2. 安装 Graphviz（用于生成图表）

```bash
# 安装 graphviz
sudo apt install graphviz
```

## 3. 基本使用方法

### 命令行方式

```bash
# 生成配置文件
doxygen -g

# 生成文档
doxygen Doxyfile
```

### 图形界面方式

```bash
# 启动 GUI 配置工具
doxywizard
```

## 4. 配置 Doxygen

### 生成默认配置文件
```bash
doxygen -g myconfig
```

### 常用配置选项
在 `Doxyfile` 中修改以下参数：

```
# 项目相关
PROJECT_NAME           = "My Project"
PROJECT_NUMBER         = 1.0
OUTPUT_DIRECTORY       = docs/

# 输入文件
INPUT                  = src/
RECURSIVE              = YES

# 输出格式
GENERATE_HTML          = YES
GENERATE_LATEX         = NO

# 其他设置
ALPHABETICAL_INDEX    = NO  # 关闭字母索引

# 图表生成
UML_LOOK               = YES
DOT_UML_DETAILS        = YES
DOT_IMAGE_FORMAT       = svg
HAVE_DOT               = YES
CALL_GRAPH             = YES
CALLER_GRAPH           = YES
```

## 5. 代码注释示例

### C++ 示例
```cpp
/**
 * @brief Calculator class for basic arithmetic operations
 */
class Calculator {
public:
    /**
     * @brief Add two numbers
     * @param a First number
     * @param b Second number
     * @return Sum of a and b
     */
    double add(double a, double b) {
        return a + b;
    }
    
    /**
     * @brief Subtract two numbers
     * @param a First number
     * @param b Second number
     * @return Difference of a and b
     */
    double subtract(double a, double b) {
        return a - b;
    }
};
```

### Python 示例
```python
def calculate_area(radius):
    """
    @brief Calculate the area of a circle
    
    @param radius: Radius of the circle
    @type radius: float
    
    @return: Area of the circle
    @rtype: float
    
    @note This function uses π approximation
    """
    return 3.14159 * radius * radius
```

## 6. 常用命令

```bash
# 清理生成的文档
rm -rf docs/

# 重新生成文档
doxygen Doxyfile

# 从特定配置文件生成
doxygen myconfig
```

## 7. 高级功能

### 包含数学公式
```
USE_MATHJAX = YES
MATHJAX_FORMAT = HTML-CSS
```

### 包含源代码
```
SOURCE_BROWSER = YES
INLINE_SOURCES = YES
```

### 搜索功能
```
SEARCHENGINE = YES
```

## 8. 自动化脚本示例

创建 `generate_docs.sh`：
```bash
#!/bin/bash

# 清理旧文档
rm -rf docs/

# 生成新文档
doxygen Doxyfile

# 打开文档（可选）
# xdg-open docs/html/index.html
```

给脚本执行权限：
```bash
chmod +x generate_docs.sh
```

## 9. 验证安装

```bash
# 检查版本
doxygen --version

# 查看帮助
doxygen --help
```

这样你就可以在 Ubuntu 上成功使用 Doxygen 来生成项目文档了。生成的 HTML 文档通常位于 `docs/html/` 目录下，用浏览器打开 `index.html` 即可查看。