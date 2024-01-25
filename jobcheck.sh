#!/bin/bash

# 循环执行脚本
while true
do
    # 运行查询作业状态的脚本，并将结果存储到临时文件中
    /star/u/wangzhen/bin/mysubmit | grep -E 'rcas[0-9]+ ' > temp.txt

    # 解析临时文件的内容，提取每个节点的运行作业数量
    while IFS="| " read -r node jobs completed removed idle running held suspended
    do
        echo "$node $jobs $completed $removed $idle $running $held $suspended"
        # 检查节点的运行作业数量
        if [[ "$running" =~ ^[0-9]+$ ]] && [ "$running" -gt 0 ]; then
            # 将节点的信息写入out.txt文件
            echo "$node $jobs $completed $removed $idle $running $held $suspended" >> out.txt
        fi
    done < temp.txt

    # 每隔一分钟执行一次
    sleep 60
done
