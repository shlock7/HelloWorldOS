#!/bin/bash

# 检查参数数量
if [ "$#" -ne 2 ]; then
  echo "Usage: $0 <commit_message> <branch_name>"
  exit 1
fi

# 提取参数
commit_message="$1"
branch_name=$2

# 执行Git命令
git add .
git commit -m "$commit_message"
git push origin $branch_name

echo "Git commands executed successfully."
