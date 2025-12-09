# GameServer - 游戏服务器

基于Zinx框架开发的游戏服务器，支持多人在线游戏、AOI（Area of Interest）算法、随机姓名生成和随机位置生成。

## 功能特性

- ✅ 基于Zinx框架的网络服务器
- ✅ AOI（兴趣区域）算法实现
- ✅ 通过Redis缓存随机姓名生成
- ✅ 玩家随机位置生成
- ✅ 多人在线游戏支持
- ✅ Protobuf消息协议

## 项目结构

```
GameServer-new-one/
├── GameServer/          # 服务器核心代码
│   ├── main.cpp        # 主程序入口
│   ├── GameRole.cpp    # 游戏角色逻辑
│   ├── GameChannel.cpp # 网络通道处理
│   ├── GameProtocol.cpp # 协议处理
│   ├── GameMsg.cpp     # 消息处理
│   ├── AOI.cpp         # AOI算法实现
│   ├── NameGenerator.cpp # 随机姓名生成器
│   └── msg.pb.cc       # Protobuf生成代码
├── config/             # 配置文件
│   ├── first_name.txt  # 姓氏列表
│   └── last_name.txt   # 名字列表
├── start_server.sh     # 启动脚本
├── stop_server.sh      # 停止脚本
└── README.md           # 本文件
```

## 依赖要求

- C++11或更高版本
- Zinx框架库
- Protobuf库
- Redis (hiredis库)
- pthread库

## 安装依赖

### Ubuntu/Debian

```bash
# 安装编译工具
sudo apt-get install g++ make

# 安装Redis和hiredis
sudo apt-get install redis-server libhiredis-dev

# 安装protobuf
sudo apt-get install libprotobuf-dev protobuf-compiler

# 安装Zinx框架（需要单独安装）
```

## 编译和运行

### 快速启动

```bash
# 启动服务器（会自动编译和启动Redis）
./start_server.sh start

# 停止服务器
./stop_server.sh stop

# 重启服务器
./start_server.sh restart

# 查看服务器状态
./start_server.sh status

# 查看日志
./start_server.sh logs
```

### 手动编译

```bash
cd GameServer
g++ -std=c++11 -pthread -o GameServer \
    main.cpp AOI.cpp GameChannel.cpp GameMsg.cpp \
    GameProtocol.cpp GameRole.cpp NameGenerator.cpp msg.pb.cc \
    -lzinx -lprotobuf -lpthread -lhiredis
```

## 配置说明

### 服务器端口

默认端口：`8899`

可以在 `GameServer/main.cpp` 中修改：

```cpp
ZinxKernel::Zinx_Add_Channel(*(new ZinxTCPListen(8899,new GameChannelFactory())));
```

### AOI世界范围

在 `GameServer/GameRole.cpp` 中配置：

```cpp
static AOIWORLD w(0, 400, 0, 400, 20, 20);
// 参数：x起始, x结束, y起始, y结束, x网格数, y网格数
```

### Redis配置

默认连接：`127.0.0.1:6379`

可以在 `GameServer/NameGenerator.cpp` 中修改。

## 功能说明

### 随机姓名生成

- 从 `config/first_name.txt` 和 `config/last_name.txt` 读取姓名数据
- 优先从Redis缓存读取，提高性能
- 如果Redis不可用，自动从文件读取
- 每次新玩家连接时自动生成随机中文姓名

### 随机位置生成

- 每个新玩家在AOI世界范围内（0-400）随机生成初始位置
- 包括x、y、z三个坐标
- 初始速度v为0

## 客户端连接

使用客户端连接到服务器：

```bash
client.exe <服务器IP> <端口>
# 例如：client.exe 192.168.195.128 8899
```

## 日志

服务器日志保存在 `logs/server.log`

查看实时日志：

```bash
tail -f logs/server.log
```

## 开发

### 添加新功能

1. 在 `GameServer/` 目录下添加新的源文件
2. 更新 `start_server.sh` 中的编译文件列表
3. 重新编译和测试

## 许可证

[添加你的许可证信息]

## 作者

[添加你的信息]

## 贡献

欢迎提交Issue和Pull Request！

