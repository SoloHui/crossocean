## pkg-config 是什么,如何工作的

`pkg-config` 是一个在编译 C/C++ 等程序时用于 **查询库的信息** 的工具，它帮助开发者获取：

- **库的编译器参数**（如头文件的路径）
- **链接器参数**（如链接哪个 `.so` 或 `.a` 文件）
- **版本号等元信息**

它的作用是 **简化构建过程中的依赖管理**。

安装
```bash
sudo apt update
sudo apt install pkg-config
```

---

### 一、pkg-config 是干什么的？

比如你要用 `glib-2.0` 这个库，传统做法是：

```bash
gcc -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -lglib-2.0 mycode.c -o myapp
```

但路径可能因系统不同而不同，非常不方便。

用 `pkg-config` 之后，你只需：

```bash
gcc mycode.c $(pkg-config --cflags --libs glib-2.0) -o myapp
```

这样系统就自动为你填入了正确的 `-I` 和 `-l` 参数。

---

### 二、pkg-config 如何工作？

它基于 `.pc` 文件（**package config 文件**），这些文件一般位于：

- `/usr/lib/pkgconfig/`
- `/usr/lib/x86_64-linux-gnu/pkgconfig/`
- `/usr/local/lib/pkgconfig/`
- `/usr/local/lib64/pkgconfig/`
- 环境变量指定的路径中（如 `PKG_CONFIG_PATH`）

一个 `.pc` 文件示例如下（`glib-2.0.pc`）：

```bash
prefix=/usr
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include

Name: GLib
Description: C Utility Library
Version: 2.56.4
Libs: -L${libdir} -lglib-2.0
Cflags: -I${includedir}/glib-2.0 -I${libdir}/glib-2.0/include
ini12345678910
```

当你运行：

```bash
pkg-config --cflags glib-2.0
```

它会解析 `Cflags:` 这一行，返回：

```bash
-I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include
```

而：

```bash
pkg-config --libs glib-2.0
```

会返回：

```bash
-L/usr/lib -lglib-2.0
```

---

### 三、常用命令

```bash
pkg-config --cflags <lib>       # 获取编译参数
pkg-config --libs <lib>         # 获取链接参数
pkg-config --modversion <lib>   # 获取库的版本
pkg-config --list-all           # 列出所有已知的库
```

---

### 四、注意事项

- `.pc` 文件通常由库的开发者或包管理器提供；
- 有些库不会默认安装 `.pc` 文件（多数会安装，可以在 install 日志中搜索 `.pc` 找到）；
- 如果刚安装的库执行 `pkg-config --list-all` 没有列出来，需要确认下 安装目录是否在 `PKG_CONFIG_PATH` 中包含，没有的话加上就行了
- 交叉编译或自定义安装路径时需设置 `PKG_CONFIG_PATH` 环境变量：

  ```bash
  export PKG_CONFIG_PATH=/custom/lib/pkgconfig
  ```

