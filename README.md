# ntool

一个用于终端的小型网络工具，用于显示系统中的TCP和UDP连接。类似于netstat或ss命令，但更加轻量级和易于使用。

## 功能特点

- 显示TCP连接及其状态（ESTABLISHED, LISTEN, CLOSE_WAIT等）
- 显示UDP连接
- 完整支持IPv4和IPv6地址
- 显示进程信息（PID和程序名）
- 按地址或端口过滤连接
- 支持数字输出模式（不解析主机名）
- 详细的错误处理和用户友好的错误信息
- 模块化设计，易于扩展

## 系统要求

- macOS 或类Unix系统
- CMake 3.10+
- C编译器（支持C11标准）
- libproc库（用于进程信息，通常在macOS系统中已包含）

## 安装

### 从源代码构建

```bash
# 克隆仓库
git clone https://github.com/yourusername/ntool.git
cd ntool

# 创建构建目录
mkdir build && cd build

# 配置和构建
cmake ..
make

# 安装（可选）
sudo make install

# 卸载（如果需要）
sudo make uninstall
```

## 使用方法

```bash
ntool [选项]
```

### 命令行选项

| 选项 | 描述 |
|------|------|
| `-t` | 仅显示TCP连接 |
| `-u` | 仅显示UDP连接 |
| `-a` | 显示所有连接（默认） |
| `-n` | 不解析主机名（仅显示数字地址） |
| `-p` | 显示进程信息（PID/程序名） |
| `-f <addr>` | 按地址过滤 |
| `-P <port>` | 按端口号过滤 |
| `-h` | 显示帮助信息 |
| `-v` | 显示版本信息 |

### 使用示例

```bash
# 显示所有连接
ntool

# 仅显示TCP连接
ntool -t

# 仅显示UDP连接
ntool -u

# 显示所有连接，不解析主机名
ntool -a -n

# 显示所有连接及其进程信息
ntool -p

# 过滤特定端口的连接
ntool -P 80

# 过滤特定地址的连接
ntool -f 192.168.1.1

# 组合多个选项
ntool -t -p -f 127.0.0.1
```

## 输出示例

### TCP连接

```
Active Internet connections (TCP)
Local Address          Foreign Address        State          PID/Program
127.0.0.1:8080         127.0.0.1:52134        ESTABLISHED    1234/nginx
0.0.0.0:22             *.*                    LISTEN         5678/sshd
[::1]:80               [::1]:52135            ESTABLISHED    1234/nginx
```

### UDP连接

```
Active Internet connections (UDP)
Local Address          Foreign Address        PID/Program
0.0.0.0:53             *.*                    9012/named
127.0.0.1:323          *.*                    3456/chronyd
```

## 开发

### 项目结构

```
ntool/
├── include/         # 头文件
│   ├── common.h     # 通用定义和函数
│   ├── tcp.h        # TCP相关函数
│   └── udp.h        # UDP相关函数
├── src/             # 源代码
│   ├── common.c     # 通用函数实现
│   ├── main.c       # 主程序
│   ├── tcp/         # TCP相关代码
│   │   └── tcp.c
│   └── udp/         # UDP相关代码
│       └── udp.c
├── cmake/           # CMake辅助文件
├── CMakeLists.txt   # CMake构建配置
└── README.md        # 项目文档
```

### 代码文档

项目使用Doxygen风格的注释，可以使用Doxygen工具生成HTML文档：

```bash
# 安装Doxygen（如果尚未安装）
brew install doxygen  # macOS
sudo apt install doxygen  # Ubuntu/Debian

# 生成文档
doxygen Doxyfile
```

### 扩展指南

如果您想扩展ntool的功能，可以按照以下步骤进行：

1. 在`include/`目录中添加新的头文件
2. 在`src/`目录中添加新的实现文件
3. 更新`CMakeLists.txt`以包含新文件
4. 在`main.c`中添加新的命令行选项和功能调用

## 故障排除

### 常见问题

- **权限错误**: 某些网络信息可能需要管理员权限。尝试使用`sudo ntool`运行。
- **编译错误**: 确保您的系统满足所有依赖要求，并且CMake版本至少为3.10。
- **IPv6支持问题**: 不同系统的IPv6支持可能有所不同。如果遇到问题，请提交issue。

## 许可证

[MIT License](LICENSE)

## 贡献

欢迎提交问题报告和拉取请求！如果您想贡献代码，请遵循以下步骤：

1. Fork项目
2. 创建您的特性分支 (`git checkout -b feature/amazing-feature`)
3. 提交您的更改 (`git commit -m 'Add some amazing feature'`)
4. 推送到分支 (`git push origin feature/amazing-feature`)
5. 打开Pull Request

## 致谢

- 感谢所有开源社区的贡献者
- 特别感谢提供网络编程API的Unix/Linux/macOS开发者