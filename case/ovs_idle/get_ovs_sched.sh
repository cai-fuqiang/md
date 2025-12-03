#!/bin/bash

PID=$1

if [ -z "$PID" ]; then
    echo "Usage: $0 <pid>"
    exit 1
fi

PREV_sum_exec_runtime=""
PREV_schedstat=""

# 打印表头
printf "%-20s %-20s %-20s %-20s %-20s\n" "Timestamp" "se.sum_exec_runtime" "schedstat_runtime" "schedstat_waittime" "schedstat_timeslices"

while true; do
    if [ ! -e "/proc/$PID/sched" ] || [ ! -e "/proc/$PID/schedstat" ]; then
        echo "Process $PID not found."
        exit 1
    fi

    # 当前时间
    now=$(date "+%Y-%m-%d %H:%M:%S")
    # 读取se.sum_exec_runtime
    sum_exec_runtime=$(grep 'se.sum_exec_runtime' /proc/$PID/sched | awk '{print $3}')
    # 读取schedstat
    schedstat=($(cat /proc/$PID/schedstat)) # 得到数组

    if [ -n "$PREV_sum_exec_runtime" ]; then
        # 用 bc 计算浮点数差值
        diff_runtime=$(echo "$sum_exec_runtime - $PREV_sum_exec_runtime" | bc)
        diff_schedstat0=$((schedstat[0] - PREV_schedstat[0]))
        diff_schedstat1=$((schedstat[1] - PREV_schedstat[1]))
        diff_schedstat2=$((schedstat[2] - PREV_schedstat[2]))

        # 打印一行表格，包括时间
        printf "%-20s %-20s %-20d %-20d %-20d\n" "$now" "$diff_runtime" "$diff_schedstat0" "$diff_schedstat1" "$diff_schedstat2"
    fi

    PREV_sum_exec_runtime=$sum_exec_runtime
    PREV_schedstat=("${schedstat[@]}")

    sleep 10
done
