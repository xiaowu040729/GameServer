# GitHub上传指南

## 步骤1: 配置Git用户信息（如果还没有配置）

```bash
git config --global user.name "你的GitHub用户名"
git config --global user.email "你的GitHub邮箱"
```

## 步骤2: 创建初始提交

```bash
git add .
git commit -m "备注"
```

## 步骤3: 在GitHub上创建新仓库

1. 登录GitHub
2. 点击右上角的 "+" 号，选择 "New repository"
3. 填写仓库名称（例如：`GameServer`）
4. 选择 Public 或 Private
5. **不要**勾选 "Initialize this repository with a README"（因为我们已经有了）
6. 点击 "Create repository"

## 步骤4: 添加远程仓库并推送

GitHub会显示类似以下的命令，复制并执行：

```bash
# 添加远程仓库（将 YOUR_USERNAME 和 YOUR_REPO_NAME 替换为你的实际值）
git remote add origin https://github.com/YOUR_USERNAME/YOUR_REPO_NAME.git

# 或者使用SSH（如果你配置了SSH密钥）
# git remote add origin git@github.com:YOUR_USERNAME/YOUR_REPO_NAME.git

# 推送代码到GitHub
git branch -M main
git push -u origin main
```

## 步骤5: 验证

访问你的GitHub仓库页面，确认代码已经成功上传。

## 常见问题

### 如果推送时要求输入用户名和密码

1. 使用Personal Access Token代替密码
   - GitHub Settings → Developer settings → Personal access tokens → Tokens (classic)
   - 生成新token，勾选 `repo` 权限
   - 推送时使用token作为密码

2. 或者配置SSH密钥（推荐）
   ```bash
   # 生成SSH密钥
   ssh-keygen -t ed25519 -C "your_email@example.com"
   
   # 复制公钥
   cat ~/.ssh/id_ed25519.pub
   
   # 在GitHub Settings → SSH and GPG keys 中添加公钥
   ```

### 如果遇到权限错误

确保你有仓库的写入权限，或者使用你自己的GitHub账户。

## 后续更新代码

```bash
# 修改代码后
git add .
git commit -m "描述你的更改"
git push
```

