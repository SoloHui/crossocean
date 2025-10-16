1. docker启动容器

- `-it` 让容器交互式运行并分配伪终端
- `--name crossocean` 给容器命名为 crossocean
- `--network=host` 让容器使用宿主机网络
- `ubuntu:22.04` 使用 Ubuntu 22.04 镜像
- `/bin/bash` 容器启动后运行 bash 终端

```bash
docker run -it --name crossocean --network=host ubuntu:22.04 /bin/bash
```

2. 进入容器

```bash
docker exec -it crossocean /bin/bash
```

3. 解除容器限制

```bash
unminimize
```

4. 容器内安装常用工具

```bash
apt update
apt install -y iputils-ping net-tools

apt upgrade -y

apt install -y vim git wget curl build-essential cmake


```

5. vscode 配置工作空间

快捷键: `Ctrl + Shift + P` -> C/C++:编辑配置(JSON)

```json
{
  "configurations": [
    {
      "name": "Linux",
      "includePath": [
        "${workspaceFolder}/**"
      ],
      "defines": [],
      "compilerPath": "/usr/bin/gcc",
      "cStandard": "c11",
      "cppStandard": "c++17",
      "intelliSenseMode": "linux-gcc-x64"
    }
  ],
  "version": 4
}
```