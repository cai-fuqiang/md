## 环境

ovs pmd 线程被隔离到了特定的核心上，一般情况下，该进程都会接近100%运行.

## 目前问题

当前线程所在的核心cpu占用并未接近100%，例如下面sar 日志:
```
[root@A07-R18-I19-165-30082B2 wfq]# ps -o comm,pid,tid,comm,psr -p `pidof ovs-vswitchd` -T |grep pmd11
pmd11             6502   7095 pmd11             5
[root@A07-R18-I19-165-30082B2 wfq]# sar  -P 5
...
05:00:01 PM     CPU     %user     %nice   %system   %iowait    %steal     %idle
05:10:04 PM       5     97.66      0.00      0.06      0.00      0.00      2.28
05:20:01 PM       5     80.05      0.00      0.17      0.00      0.00     19.78
```

说明该cpu在某时刻空闲. 这边想定位出该cpu为什么会进入空闲。

## 编写脚本抓取pmd 线程的运行时间

脚本路径: [get_ovs_sched](./get_ovs_sched.sh)

打印信息示例如下:
```
Timestamp            se.sum_exec_runtime  schedstat_runtime    schedstat_waittime   schedstat_timeslices
2025-12-03 20:25:20  9982.272564          9982275161           94420                28
2025-12-03 20:25:30  9985.252298          9985251615           112058               30
2025-12-03 20:25:40  9985.289329          9984291574           92859                26
2025-12-03 20:25:50  9984.279167          9985277848           93686                28
```

* `se.sum_exec_runtime`, `schedstat_runtime`均可以获取进程在cpu上的
  运行时间，
* `schedstat_waittime` 则表示在  就绪队列的等待时间。
* `schedstat_timeslices` 表示进程调度到cpu上的次数

在问题复现时:
```
Timestamp            se.sum_exec_runtime  schedstat_runtime    schedstat_waittime   schedstat_timeslices
2025-12-03 20:43:11  8974.208490          8972449742           100004               1569
2025-12-03 20:43:21  8891.345313          8892106557           109399               1704
2025-12-03 20:43:31  8915.237655          8915498632           94971                1588
2025-12-03 20:43:41  8878.913124          8878435567           136844               1718
2025-12-03 20:43:51  8760.624251          8760840394           98556                1824
```

可以看到`schedstat_waittime`并没有显著提升，但是`schedstat_runtime`却出现显著下
降，而`schedstat_timeslices`也有显著提升，说明进程因为某些主动让出cpu进入睡眠。


## 通过ftrace event sched_switch 调试
执行下面命令
```
cd /sys/kernel/debug/tracing/events/sched/sched_switch
## 过滤想追踪的线程
echo 'prev_pid == $TID  || next_pid == $TID' > filter
## 打开堆栈记录
echo 1 >  /sys/kernel/debug/tracing/options/stacktrace
echo 0 > enable; echo '' > /sys/kernel/debug/tracing/trace; echo 1 > enable; sleep 10; echo 0 > enable
cat /sys/kernel/debug/tracing/trace > trace.txt
```

([trace.txt](./trace.txt))

查看文件，发现该进程执行大量futex调用导致调度走
```
[root@A07-R18-I19-165-30082B2 wfq]# cat trace.txt  |grep futex_wait_queue |wc -l
608
```
和 
```
[root@A07-R18-I19-165-30082B2 wfq]# stat /sys/kernel/debug/tracing/trace
  File: ‘/sys/kernel/debug/tracing/trace’
  Size: 0               Blocks: 0          IO Block: 4096   regular empty file
Device: 6h/6d   Inode: 1516        Links: 1
Access: (0644/-rw-r--r--)  Uid: (    0/    root)   Gid: (    0/    root)
Access: 2025-12-04 00:49:59.280000000 +0800
Modify: 2025-12-03 20:46:29.088768564 +0800
Change: 2025-12-03 20:46:29.088768564 +0800
 Birth: -

## get_ovs_sched.sh 获取的日志
Timestamp            se.sum_exec_runtime  schedstat_runtime    schedstat_waittime   schedstat_timeslices
2025-12-03 20:46:11  9193.826899          9192827632           191433               715
2025-12-03 20:46:21  9268.392946          9268393633           255705               651
2025-12-03 20:46:31  9301.844067          9302839940           187446               600
```
和这段时间进程调度到cpu的次数接近，说明这段时间进程大部分是因为`futex`系统调用而
调度走.

我们执行下面的命令过滤因`futex`调入的事件，和后续被唤醒的事件:

```sh
cat trace.txt  |grep -B 2 -C 5 futex_wait_queue_me |grep sched > trace_futex.txt
```

[trace_futex](./trace_futex.txt)

日志大概如下:
```sh
[root@A07-R18-I19-165-30082B2 wfq]# cat trace_futex.txt  |head -4
           pmd11-7095  [005] d... 14157.339643: sched_switch: prev_comm=pmd11 prev_pid=7095 prev_prio=110 prev_state=S ==> next_comm=swapper/5 next_pid=0 next_prio=120
          <idle>-0     [005] d... 14157.342149: sched_switch: prev_comm=swapper/5 prev_pid=0 prev_prio=120 prev_state=R ==> next_comm=pmd11 next_pid=7095 next_prio=110
           pmd11-7095  [005] d... 14157.342157: sched_switch: prev_comm=pmd11 prev_pid=7095 prev_prio=110 prev_state=S ==> next_comm=swapper/5 next_pid=0 next_prio=120
          <idle>-0     [005] d... 14157.344081: sched_switch: prev_comm=swapper/5 prev_pid=0 prev_prio=120 prev_state=R ==> next_comm=pmd11 next_pid=7095 next_prio=110
```

编写脚本，获取时间戳一列`偶数行-奇数行`的和

$$
\sum_{i=1}^{\left\lfloor n/2 \right\rfloor} timestamp_{2i} - timestamp_{2i-1}
$$

当文件内容是
```
1.    pmd11 调出
2.    pmd11 调入

...

2i-1. pmd11 调出
2i.   pmd11 调入
```
获取的是 off-cpu的时间.

当文件内容是:
```
1.    pmd11 调入
2.    pmd11 调出

...

2i-1. pmd11 调入
2i.   pmd11 调出
```

获取的是 on-cpu的时间

我们构造出这两个文件:
* on_cpu: [trace_futex_run.txt](./trace_futex_run.txt)
* off_cpu: [trace_futex_sleep.txt](./trace_futex_sleep.txt)

并编写统计脚本:

[on_off_cpu.sh](./on_off_cpu.sh)

对这两个文件执行该脚本:
```
[root@A07-R18-I19-165-30082B2 wfq]# sh on_off_cpu.sh trace_futex_sleep.txt  tail -4
606 : 14167.277734 - 14167.277727 = 0.000007
607 : 14167.280608 - 14167.279962 = 0.000646
608 : 14167.294185 - 14167.292266 = 0.001919
\sum {2i}-{2i-1}: 0.741443

[root@A07-R18-I19-165-30082B2 wfq]# sh on_offcpu.sh trace_futex_run.txt  |tail -4
605 : 14167.277727 - 14167.277720 = 0.000007
606 : 14167.279962 - 14167.277734 = 0.002228
607 : 14167.292266 - 14167.280608 = 0.011658
\sum {2i}-{2i-1}: 9.213099
```

可以发现睡眠时间大部分都是futex造成的

## 结论
该线程主要是由于futex系统调用导致进程让出cpu。
