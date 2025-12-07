#!/bin/bash

# Git提交准备脚本

echo "=========================================="
echo "准备提交代码到main分支"
echo "=========================================="
echo ""

# 检查是否已配置用户信息
if [ -z "$(git config user.name)" ] && [ -z "$(git config --global user.name)" ]; then
    echo "⚠️  需要先配置Git用户信息"
    echo ""
    read -p "请输入你的GitHub用户名: " git_username
    read -p "请输入你的GitHub邮箱: " git_email
    
    if [ -n "$git_username" ] && [ -n "$git_email" ]; then
        git config user.name "$git_username"
        git config user.email "$git_email"
        echo "✅ Git用户信息已配置"
    else
        echo "❌ 用户信息不能为空，请手动配置后重新运行此脚本"
        exit 1
    fi
else
    echo "✅ Git用户信息已配置"
    echo "   用户名: $(git config user.name)"
    echo "   邮箱: $(git config user.email)"
fi

echo ""
echo "=========================================="
echo "创建提交..."
echo "=========================================="

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

if [ $? -eq 0 ]; then
    echo "✅ 提交创建成功"
else
    echo "❌ 提交失败"
    exit 1
fi

echo ""
echo "=========================================="
echo "重命名分支为main..."
echo "=========================================="

# 重命名分支为main
git branch -M main

if [ $? -eq 0 ]; then
    echo "✅ 分支已重命名为main"
else
    echo "⚠️  分支重命名失败（可能已经是main分支）"
fi

echo ""
echo "=========================================="
echo "✅ 准备完成！"
echo "=========================================="
echo ""
echo "下一步："
echo "1. 在GitHub上创建新仓库"
echo "2. 运行以下命令添加远程仓库并推送："
echo ""
echo "   git remote add origin https://github.com/YOUR_USERNAME/YOUR_REPO_NAME.git"
echo "   git push -u origin main"
echo ""
echo "或者使用SSH："
echo "   git remote add origin git@github.com:YOUR_USERNAME/YOUR_REPO_NAME.git"
echo "   git push -u origin main"
echo ""

