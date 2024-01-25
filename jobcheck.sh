#!/bin/bash

# 循环执行脚本
while true
do
    # 执行命令并将输出保存在数组中
    output=()
    while IFS="| " read -r node jobs completed removed idle running held suspended
    do
        output+=("$node $jobs $completed $removed $idle $running $held $suspended")
        echo "$node $jobs $completed $removed $idle $running $held $suspended"
    done < <(/star/u/wangzhen/bin/mysubmit | grep -E 'rcas[0-9]+ ')

    # 逐行读取数组中的内容
    for line in "${output[@]}"
    do
        # 解析每行的字段
        read -r node jobs completed removed idle running held suspended <<< "$line"

        # 检查节点的运行作业数量
        if [[ "$running" =~ ^[0-9]+$ ]] && [ "$running" -gt 0 ]; then
            # 将节点的信息输出到标准输出
            echo "$node $jobs $completed $removed $idle $running $held $suspended"
        fi
    done

    # 每隔一分钟执行一次
    sleep 60
done
