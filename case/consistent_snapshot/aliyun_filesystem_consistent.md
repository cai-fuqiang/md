## 测试aliyun快照时文件系统一致性

首先将数据盘挂载到mnt下，创建下面python脚本
用于频繁修改文件系统元数据:
```python
import os
import time

while True:
    temp_files = []
    for i in range(1000):
        filename = f"tempfile_{i}.tmp"
        with open(filename, "w") as f:
            f.write("tmpfile...\n")
        temp_files.append(filename)

    for file_path in temp_files:
        try:
            os.remove(file_path)
        except Exception as e:
            print(f"delete {file_path} failed: {e}")
```

使用下面 bcc-tools `execsnoop`抓取是否有进程在该过程被拉起:
然后在页面执行创建快照组,

![execsnoop_print](pic/execsnoop_print.png)

可以发现`execsnoop`并未显示有特殊的进程拉起


使用bpftrace 抓取 `filesystem freeze` 流程:
```
[root@iZ2ze34bqeifu0i50tzhv3Z ~]# cat bpftrace
kprobe:freeze_super { printf("freeze_super called! sb=%p\n", arg0); }
kprobe:thaw_super { printf("thaw_super called! sb=%p\n", arg0); }
```

结果发现，在整个快照过程中也没有输出。

所以，怀疑 **其根本未做文件系统一致性的动作** !


使用之前快照回滚盘, 在第一次挂载数据盘时，会打印 `recovery`:
```
## 第一次
[   68.752727] XFS (nvme1n1): Mounting V5 Filesystem 7c7b5119-64ce-4b26-a913-48f7dd05285a
[   69.171908] XFS (nvme1n1): Starting recovery (logdev: internal)
[   69.979664] XFS (nvme1n1): Ending recovery (logdev: internal)
[  110.650693] XFS (nvme1n1): Unmounting Filesystem 7c7b5119-64ce-4b26-a913-48f7dd05285a

## 第二次
[  138.815070] XFS (nvme1n1): Mounting V5 Filesystem 7c7b5119-64ce-4b26-a913-48f7dd05285a
[  138.822890] XFS (nvme1n1): Ending clean mount
[  141.977432] XFS (nvme1n1): Unmounting Filesystem 7c7b5119-64ce-4b26-a913-48f7dd05285a

## 第三次
[  144.785305] XFS (nvme1n1): Mounting V5 Filesystem 7c7b5119-64ce-4b26-a913-48f7dd05285a
[  144.794335] XFS (nvme1n1): Ending clean mount
```

说明第一次挂载时，xfs 发现日志中的事务是不完整的，所以！aliyun似乎未做文件系统一
致性的动作。

## TODO
* 测试fsfroze 能否保证xfs事务完整
