#!/bin/bash

# 循环执行脚本
while true
do
    # 运行查询作业状态的脚本，并将结果通过管道传递给grep命令进行过滤
    /star/u/wangzhen/bin/mysubmit | grep -E 'rcas[0-9]+ ' |
    # 解析管道传递过来的结果，提取每个节点的运行作业数量
    while IFS="| " read -r node jobs completed removed idle running held suspended
    do
        # 检查节点的运行作业数量
        if [[ "$running" =~ ^[0-9]+$ ]] && [ "$running" -gt 0 ]; then
            # 将节点的信息输出到标准输出
            echo "$node $jobs $completed $removed $idle $running $held $suspended"
        fi
    done

    # 每隔一分钟执行一次
    sleep 60
done
