## 问题
QA人员发现在当前的环境中，有些pod的blkio cgroup中，发现有部分文件
仅输出`total 0`

eg:
```
[root@node-13 ~]# cat /sys/fs/cgroup/blkio//kubepods/burstable/pod189ced8a-7229-41e3-a3bb-5ee422e4eb72/kata_e860ffae4c2a804a5a4b2133898c6e7305ef2dda9c6c59d38c8fb29478ca9b8b/blkio.throttle.io_serviced
Total 0
```

## 自己搭建环境测试该问题

### 环境说明
硬件说明:qemu x86模拟器, 4核心, 带一块nvme硬盘
qemu启动参数:`qemu-system-x86_64 -smp 4 -kernel arch/x86/boot/bzImage -initrd initrd.gz -drive file=./nvme.raw,if=none,id=D22 -device nvme,drive=D22,serial=1234  -append "rdinit=/linuxrc console=ttyS0" -nographic -s -S`

### 执行命令情况
1. 查看磁盘挂载情况
```
# mount
rootfs on / type rootfs (rw,size=47072k,nr_inodes=11768)
proc on /proc type proc (rw,relatime)
sysfs on /sys type sysfs (rw,relatime)
devpts on /dev/pts type devpts (rw,relatime,gid=5,mode=620)
tmpfs on /run type tmpfs (rw,nosuid,nodev,mode=755)
huge on /mnt/huge type hugetlbfs (rw,relatime)
```
2. 挂载cgroup blkio
```
# mount -t cgroup -o blkio cgroup /sys/fs/cgroup/
```

3. 创建子层级
```
# mkdir test
```

4. 查看子层级中的`blkio.throttle.io_serviced`文件

```
# cd test/
# cat blkio.throttle.io_serviced
Total 0
```

5. 执行一些bio操作, 再次查看test子层级中的`blkio.throttle.io_serviced`文件
```
# mount /dev/nvme0n1p1 /mnt/
main-loop: WARNING: I/O thread spun for 1000 iterations
[  309.917195] EXT2-fs (nvme0n1p1): warning: mounting unchecked fs, running e2fsck is recommended

# cat blkio.throttle.io_serviced
Total 0
```
6. 将当前shell进程加入到test子层级中
```
# ps aux |grep sh
root      1112  0.2  2.7  11640  2896 ttyS0    Ss   10:06   0:00 -sh
root      1134  0.0  0.2   8072   308 ttyS0    S+   10:12   0:00 grep sh

# echo 1112 > tasks

# cat blkio.throttle.io_serviced
Total 0 
```
发现还是没有相关打印

7. 在当前shell中进行bio操作, 再次查看
```
# ls /mnt
# cat blkio.throttle.io_serviced
259:0 Read 2
259:0 Write 0
259:0 Sync 0
259:0 Async 2
259:0 Total 2
Total 2

# lsblk |grep 259:0
nvme0n1     259:0    0  500M  0 disk
```
发现为刚刚打印的设备号，为刚刚正在bio的设备

Note: nvme0n1p1为nvme0n1的一个分区设备

## 结论
在创建新的命名空间时，查看该文件不会有打印, 只有在该命名空间中加入
task，并且该task对某一设备发出bio请求后，查看该文件才会有和该设备相关
的打印
