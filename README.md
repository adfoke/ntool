# ntool

一个用于终端的小型网络工具，用于显示系统中的TCP和UDP连接。

## 功能特点

- 显示TCP连接及其状态
- 显示UDP连接
- 支持IPv4和IPv6地址
- 可选择性地显示进程信息
- 可按地址或端口过滤连接
- 支持数字输出模式（不解析主机名）

## 系统要求

- macOS 或类Unix系统
- CMake 3.10+
- C编译器（支持C11标准）

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
```

## 使用方法

```bash
ntool [选项]
```

### 选项

- `-t`: 仅显示TCP连接
- `-u`: 仅显示UDP连接
- `-a`: 显示所有连接（默认）
- `-n`: 不解析主机名（仅显示数字地址）
- `-p`: 显示进程信息（PID/程序名）
- `-f <addr>`: 按地址过滤
- `-P <port>`: 按端口号过滤
- `-h`: 显示帮助信息
- `-v`: 显示版本信息

### 示例

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

## 许可证

[MIT License](LICENSE)

## 贡献

欢迎提交问题报告和拉取请求！