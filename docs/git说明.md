### Git 配置命令

这些命令通常在安装 Git 后首次设置，或需要修改全局配置时使用。

```bash
# 设置全局用户名
git config --global user.name "你的姓名"

# 设置全局邮箱
git config --global user.email "你的邮箱@example.com"

# 查看当前所有配置
git config --list

# 设置默认分支名为 main（推荐）
git config --global init.defaultBranch main

# 为命令设置别名，例如用 `git co` 代替 `git checkout`
git config --global alias.co checkout
git config --global alias.br branch
git config --global alias.ci commit
git config --global alias.st status
```

---

### 创建仓库与基础操作

```bash
# 在当前目录初始化一个新的 Git 仓库
git init

# 克隆一个远程仓库到本地
git clone <远程仓库URL>

# 检查当前仓库的状态（查看哪些文件被修改、暂存等）
git status

# 将文件的修改添加到暂存区
git add <文件名>
git add .                  # 添加所有修改和新文件
git add *.txt             # 使用通配符添加特定类型文件

# 将暂存区的内容提交到本地仓库
git commit -m "提交说明信息"
```

---

### 查看与比较变更

```bash
# 查看提交历史
git log
git log --oneline          # 简洁模式，一行显示一个提交
git log --graph           # 图形化显示分支合并历史

# 查看工作区与暂存区的差异（尚未 add 的改动）
git diff

# 查看暂存区与最新提交的差异（已经 add 的改动）
git diff --staged
git diff --cached         # --cached 是 --staged 的同义词

# 查看某次提交的具体内容
git show <提交ID>
```

---

### 分支管理

分支是 Git 的核心功能，用于并行开发。

```bash
# 查看所有分支
git branch
git branch -v             # 查看分支及其最新提交

# 创建新分支
git branch <新分支名>

# 切换分支
git checkout <分支名>
git switch <分支名>        # 较新版本 Git 推荐的命令，语义更清晰

# 创建并切换到新分支
git checkout -b <新分支名>
git switch -c <新分支名>   # 较新版本 Git 推荐的命令

# 将指定分支合并到当前分支
git merge <分支名>

# 删除分支（已合并的分支）
git branch -d <分支名>
# 强制删除分支（未合并的分支）
git branch -D <分支名>

# 重命名当前分支
git branch -m <新分支名>
```

---

### 远程仓库协作

```bash
# 查看远程仓库信息
git remote -v

# 添加远程仓库地址，通常命名为 origin
git remote add origin <远程仓库URL>

# 将本地分支推送到远程仓库
git push -u origin <分支名>  # -u 设置上游分支，后续可直接用 `git push`
git push                    # 推送到已关联的上游分支

# 从远程仓库拉取更新并合并到当前分支
git pull
git pull origin <分支名>    # 拉取指定远程分支并合并

# 获取远程仓库的所有更新（但不自动合并）
git fetch

# 将远程分支列表同步到本地
git fetch --prune

# 将本地分支与远程分支关联
git branch --set-upstream-to=origin/<远程分支名> <本地分支名>
```

---

### 撤销与回退

**谨慎使用**，尤其是在已推送到远程仓库后。

```bash
# 撤销工作区的修改（尚未 add），恢复到上次提交的状态
git checkout -- <文件名>
git restore <文件名>        # 较新版本 Git 推荐的命令

# 将文件从暂存区撤出（取消 add），但保留工作区的修改
git reset HEAD <文件名>     # 旧语法
git restore --staged <文件名> # 新语法

# 撤销上一次的提交，并将修改放回暂存区（修改提交信息或追加文件时常用）
git commit --amend

# 回退到某个提交，并丢弃之后的提交历史（危险操作）
git reset --hard <提交ID>

# 回退到某个提交，但保留工作区的修改（将更改放回工作区）
git reset --soft <提交ID>
git reset --mixed <提交ID>  # --mixed 是默认选项

# 创建一个新的提交来撤销某次提交的更改（安全，推荐用于已推送的代码）
git revert <提交ID>
```

---

### 储藏临时工作

当你需要临时切换分支但又不想提交未完成的工作时。

```bash
# 将当前工作区和暂存区的修改储藏起来
git stash
git stash push -m "储藏信息" # 为储藏添加说明

# 查看所有储藏列表
git stash list

# 恢复最新的储藏，并从储藏列表中删除它
git stash pop

# 恢复指定的储藏，但不从列表中删除
git stash apply stash@{0}

# 删除指定的储藏
git stash drop stash@{0}

# 清空所有储藏
git stash clear
```

---

### 标签管理

用于标记重要的版本节点（如 v1.0.0）。

```bash
# 查看所有标签
git tag

# 在当前提交上创建轻量标签
git tag <标签名>

# 创建带注释的标签
git tag -a v1.0.0 -m "版本 1.0.0 发布"

# 将标签推送到远程仓库
git push origin <标签名>
git push origin --tags     # 推送所有本地标签

# 删除本地标签
git tag -d <标签名>
# 删除远程标签
git push origin --delete <标签名>
```

---

### 实用命令与技巧

```bash
# 忽略文件：创建一个名为 `.gitignore` 的文件，在里面列出要忽略的文件/文件夹模式
# 例如：node_modules/、*.log、.DS_Store

# 交互式地将文件的特定更改块（hunk）添加到暂存区，非常有用
git add -p

# 交互式地 rebase，用于整理提交历史（合并、修改、删除提交）
git rebase -i <提交ID>

# 查看命令历史，方便找回丢失的提交ID
git reflog
```

### 总结：最核心的命令流

对于一个典型的日常开发任务，流程如下：

1.  **更新**： `git pull`
2.  **切换/创建分支**： `git switch -c feature/my-new-feature`
3.  ** coding...**
4.  **查看状态**： `git status`
5.  **添加更改**： `git add .`
6.  **提交**： `git commit -m "完成了新功能"`
7.  **推送**： `git push -u origin feature/my-new-feature` (首次)
    后续只需 `git push`