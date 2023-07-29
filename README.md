1. git remote add origin git@ssh链接
2. git add . #添加所有文件到缓存区
3. git commit -m '提交内容说明' #提交代码到本地仓库
4. git push origin master #将本地仓库中的提交推送到名为 origin 的远程仓库的 master 分支中
5. git push -u #是第一次推送时将本地库与远程库相关联，后续就可以直接使用git push不用加（origin master）
6. git push -f #使用本地库强制覆盖远程库
7. git clone #是第一次从远程仓库获取代码
8. git pull #适用于从远程仓库更新本地仓库的代码，git pull 实际上是 git fetch 和 git merge 两个命令的组合，它首先从远程仓库拉取最新的更新，然后将这些更新与本地仓库合并
9. git diff #是 比较暂存区和工作目录之间的差异，即查看已经被修改但还没有被提交的文件
10. git status #比较暂存区和工作目录之间的差异，即查看已经被修改但还没有被提交的文件
11. git reset #是将git add的文件从暂存区中移除
12. git commit -a #将所有已修改的文件添加到暂存区并提交到本地仓库中，注意的是该指令只会将已经被 git 管理的文件添加到暂存区中，如果有新的文件需要被添加到仓库中，仍然需要使用 git add 命令进行添加。
13. git branch -r #会显示所有远程分支的列表
14. git branch -d 分支名 #删除对应分支
15. git checkout 分支名 #切换对应分支
16. git reset --oneline #查看commit的前面代码
17. git reset --soft #将当前分支指向指定的 commit，但是不会修改暂存区和工作区的内容。这意味着之后可以直接使用 git commit 命令提交修改，而不需要重新添加文件到暂存区中。
18. git reset --mixed #将当前分支指向指定的 commit，并将之后的提交从当前分支历史记录中移除。此外，该模式还会将暂存区的内容恢复到指定的 commit，但是不会修改工作区的内容。这意味着之后需要使用 git add 命令重新将修改添加到暂存区中，然后再使用 git commit 命令提交修改。
19. git reset --hard #将当前分支指向指定的 commit，并将之后的提交从当前分支历史记录中移除。此外，该模式还会将暂存区和工作区的内容都恢复到指定的 commit，即完全丢弃之后的修改。这意味着之后需要重新修改文件，并使用 git add 和 git commit 命令重新提交修改。
20. git config --global  --list #检查一下用户名和邮箱是否配置
21. git config --global  user.name "这里换上你的用户名"
22. git config --global user.email "这里换上你的邮箱"
23. ssh-keygen -t rsa -c "这是你的邮箱" 生成密钥
