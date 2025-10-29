1. docker启动容器

- `-it` 让容器交互式运行并分配伪终端
- `--name crossocean` 给容器命名为 crossocean
- `-p 192.168.3.209:9340:9340/tcp` 映射宿主机的 9340/tcp 端口到容器的 9340/tcp 端口
- `-p 192.168.3.209:9340:9340/udp` 映射宿主机的 9340/udp 端口到容器的 9340/udp 端口
- `ubuntu:22.04` 使用 Ubuntu 22.04 镜像
- `/bin/bash` 容器启动后运行 bash 终端

```bash
docker run -it --name crossocean -p 192.168.3.209:9340:9340/tcp -p 192.168.3.209:9340:9340/udp ubuntu:22.04 /bin/bash
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
sudo apt update

sudo apt upgrade -y

sudo apt install -y vim git wget curl build-essential cmake

sudo apt install -y iputils-ping net-tools

sudo apt install -y ninja-build

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
