# Git配置和提交指南

## 步骤1: 配置Git用户信息

在提交代码之前，需要先配置Git用户信息。你可以选择以下两种方式之一：

### 方式1: 全局配置（推荐，适用于所有仓库）

```bash
git config --global user.name "你的GitHub用户名"
git config --global user.email "你的GitHub邮箱"
```

### 方式2: 仅为此仓库配置

```bash
git config user.name "你的GitHub用户名"
git config user.email "你的GitHub邮箱"
```

## 步骤2: 创建提交

配置完成后，运行：

```bash
# 确保所有文件都已添加
git add .

# 创建提交
git commit -m "Initial commit: GameServer with random name and position generation

- 基于Zinx框架的游戏服务器
- 实现AOI（兴趣区域）算法
- 通过Redis缓存随机姓名生成
- 玩家随机位置生成功能
- 支持多人在线游戏
- 使用Protobuf消息协议"

# 重命名分支为main
git branch -M main
```

## 步骤3: 在GitHub上创建仓库并推送

1. 登录GitHub，创建新仓库（不要初始化README）
2. 运行以下命令（替换YOUR_USERNAME和YOUR_REPO_NAME）：

```bash
# 添加远程仓库
git remote add origin https://github.com/YOUR_USERNAME/YOUR_REPO_NAME.git

# 推送到GitHub
git push -u origin main
```

## 快速命令（配置完成后）

```bash
# 1. 配置用户信息（只需执行一次）
git config --global user.name "你的名字"
git config --global user.email "你的邮箱"

# 2. 提交代码
git add .
git commit -m "Initial commit: GameServer with random name and position generation"

# 3. 重命名为main分支
git branch -M main

# 4. 添加远程仓库并推送（替换为你的仓库地址）
git remote add origin https://github.com/YOUR_USERNAME/YOUR_REPO_NAME.git
git push -u origin main
```

