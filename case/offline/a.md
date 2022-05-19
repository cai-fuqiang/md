# 问题现象
在ft2500扩容云产品节点对应的云主机中vnc console和dmesg中会有如下报错:

```
[    0.020629] smp: Bringing up secondary CPUs ...
...
[    8.124952] CPU3: failed to come online
[    8.124997] CPU3: failed in unknown state : 0x0
[   14.097343] CPU4: failed to come online
[   14.097356] CPU4: failed in unknown state : 0x0

...
[   61.030924] psci: failed to boot CPU3 (-22)
[   61.032017] CPU3: failed to boot: -22
[   61.032829] CPU3: failed in unknown state : 0x0
[   61.041995] psci: failed to boot CPU4 (-22)
[   61.043151] CPU4: failed to boot: -22
...

```

# 测试分析
在环境https://192.168.4.199/上做了大量对比测试。有使用麒麟OS、CentOS 8.1、es kernel 10
（指的是4.18.0-147.5.1.el8_1.5es.10.aarch64，下面提到的es kernel 11、15等是同样的）、
es kernel 11、es kernel 14、es kernel 15。node-1节点未打上NUMA相关patch，es kernel 11、
es kernel 14、es kernel 15会复现出该case的情况（虚拟机无法启动）以及必现vcpu offline的问题。
es kernel 11极大概率复现该case描述的问题。es kernel 10以及CentOS 8.1、麒麟OS无任何问题。

node-3节点应用了NUMA相关patch，es kernel 11、es kernel 14、es kernel 15小概率复现该case描述
问题（虚拟机未启动），概率性复现vcpu offline的情况。es kernel 11出现vcpu offline的情况较多。
es kernel 10以及CentOS 8.1、麒麟OS无任何问题。

通过查看kernel的代码提交记录，发现es kernel 11及以后的版本应用了飞腾S2500相关patch。目前定位
是该patch导致的问题。

虚拟机中未使用到FTS2500 patch中关于IRQ的代码，但是有使用到memblock相关代码，尝试去除FTS2500 
patch中关于memblock的代码修改，在es kernel 10上应用并重新编译kernel rpm，在虚拟机中安装新的kernel，
并进行重启测试。未复现出cpu offline和虚拟机启动失败的情况。
