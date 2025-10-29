# cmake-format安装与使用

## Ubuntu
```bash
sudo apt install cmake-format
```

## Windows
```cmd
pip install cmake-format
```


## CMake 配置示例
编辑`CMakeUserPresets.json`文件，添加 cmake-format 配置：

```json
{
  "version": 3,
  "configurePresets": [
    {
      "name": "default",
      "inherits": "windows_debug",
      "cacheVariables": {
        "CMAKE_FORMAT": "C:/Users/HUI/AppData/Local/Packages/PythonSoftwareFoundation.Python.3.13_qbz5n2kfra8p0/LocalCache/local-packages/Python313/Scripts/cmake-format.exe"
      }
    },
    {
      "name": "default",
      "inherits": "linux_debug",
      "cacheVariables": {
        "CMAKE_FORMAT": "cmake-format"
      }
    }
  ]
}