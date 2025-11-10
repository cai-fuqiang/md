## 测试aliyun快照时文件系统一致性
### 未开启应用一致性

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

### 开启应用一致性

> NOTE
>
> 在centos7 上测试，使用其他镜像可能会失败


使用如下systemtap脚本获取:
```
probe kprobe.function("freeze_super") {
    printf("freeze_super called at %s (pid=%d)\n", execname(), pid())
    print_backtrace()
}
probe kprobe.function("thaw_super") {
    printf("thaw_super called at %s (pid=%d)\n", execname(), pid())
    print_backtrace()
}
```

输出信息:

```
Pass 1: parsed user script and 473 library scripts using 272056virt/69208res/3512shr/65836data kb, in 440usr/30sys/746real ms.
Pass 2: analyzed script: 2 probes, 3 functions, 3 embeds, 0 globals using 286700virt/84528res/6264shr/78380data kb, in 180usr/620sys/3445real ms.
Missing separate debuginfos, use: debuginfo-install kernel-3.10.0-1160.119.1.el7.x86_64 
Pass 3: using cached /root/.systemtap/cache/1c/stap_1cc6b922ca858ccb5c6104fc4651a9c5_2594.c
Pass 4: using cached /root/.systemtap/cache/1c/stap_1cc6b922ca858ccb5c6104fc4651a9c5_2594.ko
Pass 5: starting run.
WARNING: Missing unwind data for a module, rerun with 'stap -d kernel'
freeze_super called at fsfreezer (pid=1967)
 0xffffffff9aa5f090
 0xffffffff9aa71c71
 0xffffffff9afc539a
freeze_super called at fsfreezer (pid=1967)
 0xffffffff9aa5f090
 0xffffffff9aa71c71
 0xffffffff9afc539a
thaw_super called at fsfreezer (pid=1967)
 0xffffffff9aa5f060
 0xffffffff9aa71c71
 0xffffffff9afc539a
thaw_super called at fsfreezer (pid=1967)
 0xffffffff9aa5f060
 0xffffffff9aa71c71
 0xffffffff9afc539a
```

可以看到是由一个叫`fsfreezer`的进程执行的filesystem frozen

可执行文件位置:
```
[root@iZ2zej48nvay43y5f66hlfZ usr]# find ./ -name "fsfreezer"
./local/share/aliyun-assist/plugin/app-snapshot-plugin/1.8/fsfreezer

[root@iZ2zej48nvay43y5f66hlfZ usr]# rpm -qf ./local/share/aliyun-assist/plugin/app-snapshot-plugin/1.8/fsfreezer
file /usr/local/share/aliyun-assist/plugin/app-snapshot-plugin/1.8/fsfreezer is not owned by any package
```

不确定是如何安装上的, aliyun似乎并未公开该程序源码

### 再次mount
```
[    1.147337] EXT4-fs (nvme0n1p1): mounted filesystem with ordered data mode. Opts: (null)
[    1.670240] EXT4-fs (nvme0n1p1): re-mounted. Opts: (null)
[    9.168285]  nvme1n1: p1
[    9.179122]  nvme1n1: p1
[   10.720093]  nvme1n1: p1
[   10.850036]  nvme1n1: p1
[   54.609040] EXT4-fs (nvme1n1p1): mounted filesystem with ordered data mode. Opts: (null)
```

再次`mount`没有输出`recovery`的信息，所以aliyun通过这种方式保证了文件系统一致性

## aliyun内部日志
```
[root@iZ2zej48nvay43y5f66hlfZ log]# cat app_snapshot_plugin.log.20251027.5 
time="2025-10-27 13:47:31.584201122" level=info msg="Runtime Args: [--RamRoleName=snap -EnableFsFreeze=true -TimeoutInSeconds=30 -PreScriptPath=/tmp/prescript.sh -PostScriptPath=/tmp/postscript.sh -Name=Created_At_202510271347]"
time="2025-10-27 13:47:31.584390988" level=error msg="Failded to read local default configure file" error="open /usr/local/share/aliyun-assist/plugin/app-snapshot-plugin/1.8/app_snapshot_plugin.json: no such file or directory"
time="2025-10-27 13:47:31.603080569" level=info msg="http://100.100.100.200/latest/meta-data/instance-id i-2zej48nvay43y5f66hlf <nil>"
time="2025-10-27 13:47:31.617991815" level=info msg="http://100.100.100.200/latest/meta-data/region-id cn-beijing <nil>"
time="2025-10-27 13:47:31.659075547" level=info msg="=========BEGIN===========" Type=lsb_release
time="2025-10-27 13:47:31.659118349" level=info msg="LSB Version:\t:core-4.1-amd64:core-4.1-noarch"
time="2025-10-27 13:47:31.659127461" level=info msg="Distributor ID:\tCentOS"
time="2025-10-27 13:47:31.659135034" level=info msg="Description:\tCentOS Linux release 7.9.2009 (Core)"
time="2025-10-27 13:47:31.6591418" level=info msg="Release:\t7.9.2009"
time="2025-10-27 13:47:31.659148683" level=info msg="Codename:\tCore"
time="2025-10-27 13:47:31.659156585" level=info msg="==========End===========" Type=lsb_release
time="2025-10-27 13:47:31.65919145" level=error msg="File /tmp/prescript.sh not found , error=stat /tmp/prescript.sh: no such file or directory"
time="2025-10-27 13:47:31.65920855" level=error msg="File /tmp/postscript.sh not found , error=stat /tmp/postscript.sh: no such file or directory"
time="2025-10-27 13:47:31.65921966" level=error msg="Pre/Post-Script have no Expected Permission"
time="2025-10-27 13:47:31.659231645" level=info msg="Pre/Post-Script files were not found!"
time="2025-10-27 13:47:31.659243095" level=info msg="Dump SnapshotContext Options:"
time="2025-10-27 13:47:31.659250138" level=info msg="InstanceID: i-2zej48nvay43y5f66hlf"
time="2025-10-27 13:47:31.659257204" level=info msg="RegionID: cn-beijing"
time="2025-10-27 13:47:31.659263495" level=info msg="RamRoleName: snap"
time="2025-10-27 13:47:31.65927293" level=info msg="ExcludeDiskId: []"
time="2025-10-27 13:47:31.65927963" level=info msg="ProductNetwork: vpc"
time="2025-10-27 13:47:31.659286672" level=info msg="PreScriptPath: /tmp/prescript.sh"
time="2025-10-27 13:47:31.659293473" level=info msg="PostScriptPath: /tmp/postscript.sh"
time="2025-10-27 13:47:31.659300671" level=info msg="EnableFsFreeze: true"
time="2025-10-27 13:47:31.659309099" level=info msg="TimeoutInSeconds: 30"
time="2025-10-27 13:47:31.659315904" level=info msg="ScriptTimeoutInSeconds: 1800"
time="2025-10-27 13:47:31.787152472" level=info msg="Network connection RequestId= 57A0B508-56C3-590D-BD21-BC35F987A050, timeCost=127.727092ms"
time="2025-10-27 13:47:31.787204086" level=info msg="Attached DiskId=d-2zej48nvay43y5ezy63u, Device=/dev/xvdb, Category=cloud_auto"
time="2025-10-27 13:47:31.787216747" level=info msg="Attached DiskId=d-2zej48nvay43y5ezy63t, Device=/dev/xvda, Category=cloud_essd"
time="2025-10-27 13:47:31.921288861" level=info msg="Start to check Pre/Post-Script"
time="2025-10-27 13:47:31.921347255" level=error msg="lstat /tmp/prescript.sh: no such file or directory"
time="2025-10-27 13:47:31.921358029" level=error msg="lstat /tmp/prescript.sh: no such file or directory"
time="2025-10-27 13:47:31.921371697" level=error msg="No such Pre/Post-Script files or have no \"execute\" Permission"
time="2025-10-27 13:47:31.921394627" level=info msg="Discovering Mounted Volumes ..."
time="2025-10-27 13:47:31.924304584" level=info msg="=========BEGIN===========" Type=lsblk
time="2025-10-27 13:47:31.924338057" level=info msg="NAME=\"nvme0n1\" TYPE=\"disk\" FSTYPE=\"\" MOUNTPOINT=\"\" LABEL=\"\" UUID=\"\" MODEL=\"Alibaba Cloud Elastic Block Storage     \" SIZE=\"21474836480\""
time="2025-10-27 13:47:31.924352576" level=info msg="NAME=\"nvme0n1p1\" TYPE=\"part\" FSTYPE=\"ext4\" MOUNTPOINT=\"/\" LABEL=\"\" UUID=\"c8b5b2da-5565-4dc1-b002-2a8b07573e22\" MODEL=\"\" SIZE=\"21473787904\""
time="2025-10-27 13:47:31.924365022" level=info msg="NAME=\"nvme1n1\" TYPE=\"disk\" FSTYPE=\"\" MOUNTPOINT=\"\" LABEL=\"\" UUID=\"\" MODEL=\"Alibaba Cloud Elastic Block Storage     \" SIZE=\"10737418240\""
time="2025-10-27 13:47:31.924373965" level=info msg="NAME=\"nvme1n1p1\" TYPE=\"part\" FSTYPE=\"ext4\" MOUNTPOINT=\"/mnt\" LABEL=\"\" UUID=\"e2daa09e-73eb-4914-900d-c4dd131a7804\" MODEL=\"\" SIZE=\"10736352768\""
time="2025-10-27 13:47:31.924383704" level=info msg="==========End===========" Type=lsblk
time="2025-10-27 13:47:31.92441921" level=info msg="Empty lsblk device MountPoint, skipped: lsblk MOUNTPOINT=, NAME=nvme0n1, TYPE=disk, FSTYPE=, LABEL=, UUID=, MODEL=Alibaba, Size=21474836480"
time="2025-10-27 13:47:31.924430379" level=info msg="Empty lsblk device MountPoint, skipped: lsblk MOUNTPOINT=, NAME=nvme1n1, TYPE=disk, FSTYPE=, LABEL=, UUID=, MODEL=Alibaba, Size=10737418240"
time="2025-10-27 13:47:31.925771709" level=info msg="=========BEGIN===========" Type=/bin/mount
time="2025-10-27 13:47:31.925817028" level=info msg="sysfs on /sys type sysfs (rw,nosuid,nodev,noexec,relatime)"
time="2025-10-27 13:47:31.925826278" level=info msg="proc on /proc type proc (rw,nosuid,nodev,noexec,relatime)"
time="2025-10-27 13:47:31.925833859" level=info msg="devtmpfs on /dev type devtmpfs (rw,nosuid,size=1856112k,nr_inodes=464028,mode=755)"
time="2025-10-27 13:47:31.925841577" level=info msg="securityfs on /sys/kernel/security type securityfs (rw,nosuid,nodev,noexec,relatime)"
time="2025-10-27 13:47:31.92584893" level=info msg="tmpfs on /dev/shm type tmpfs (rw,nosuid,nodev)"
time="2025-10-27 13:47:31.92585643" level=info msg="devpts on /dev/pts type devpts (rw,nosuid,noexec,relatime,gid=5,mode=620,ptmxmode=000)"
time="2025-10-27 13:47:31.925863833" level=info msg="tmpfs on /run type tmpfs (rw,nosuid,nodev,mode=755)"
time="2025-10-27 13:47:31.925871048" level=info msg="tmpfs on /sys/fs/cgroup type tmpfs (ro,nosuid,nodev,noexec,mode=755)"
time="2025-10-27 13:47:31.925880767" level=info msg="cgroup on /sys/fs/cgroup/systemd type cgroup (rw,nosuid,nodev,noexec,relatime,xattr,release_agent=/usr/lib/systemd/systemd-cgroups-agent,name=systemd)"
time="2025-10-27 13:47:31.925889375" level=info msg="pstore on /sys/fs/pstore type pstore (rw,nosuid,nodev,noexec,relatime)"
time="2025-10-27 13:47:31.925896813" level=info msg="cgroup on /sys/fs/cgroup/devices type cgroup (rw,nosuid,nodev,noexec,relatime,devices)"
time="2025-10-27 13:47:31.925903915" level=info msg="cgroup on /sys/fs/cgroup/net_cls,net_prio type cgroup (rw,nosuid,nodev,noexec,relatime,net_prio,net_cls)"
time="2025-10-27 13:47:31.92591183" level=info msg="cgroup on /sys/fs/cgroup/freezer type cgroup (rw,nosuid,nodev,noexec,relatime,freezer)"
time="2025-10-27 13:47:31.925918739" level=info msg="cgroup on /sys/fs/cgroup/cpu,cpuacct type cgroup (rw,nosuid,nodev,noexec,relatime,cpuacct,cpu)"
time="2025-10-27 13:47:31.925928607" level=info msg="cgroup on /sys/fs/cgroup/pids type cgroup (rw,nosuid,nodev,noexec,relatime,pids)"
time="2025-10-27 13:47:31.92593605" level=info msg="cgroup on /sys/fs/cgroup/cpuset type cgroup (rw,nosuid,nodev,noexec,relatime,cpuset)"
time="2025-10-27 13:47:31.925943588" level=info msg="cgroup on /sys/fs/cgroup/blkio type cgroup (rw,nosuid,nodev,noexec,relatime,blkio)"
time="2025-10-27 13:47:31.925951473" level=info msg="cgroup on /sys/fs/cgroup/hugetlb type cgroup (rw,nosuid,nodev,noexec,relatime,hugetlb)"
time="2025-10-27 13:47:31.925958547" level=info msg="cgroup on /sys/fs/cgroup/memory type cgroup (rw,nosuid,nodev,noexec,relatime,memory)"
time="2025-10-27 13:47:31.925966066" level=info msg="cgroup on /sys/fs/cgroup/perf_event type cgroup (rw,nosuid,nodev,noexec,relatime,perf_event)"
time="2025-10-27 13:47:31.925973451" level=info msg="configfs on /sys/kernel/config type configfs (rw,relatime)"
time="2025-10-27 13:47:31.925984685" level=info msg="/dev/nvme0n1p1 on / type ext4 (rw,relatime,data=ordered)"
time="2025-10-27 13:47:31.925991878" level=info msg="systemd-1 on /proc/sys/fs/binfmt_misc type autofs (rw,relatime,fd=26,pgrp=1,timeout=0,minproto=5,maxproto=5,direct,pipe_ino=10314)"
time="2025-10-27 13:47:31.926002664" level=info msg="debugfs on /sys/kernel/debug type debugfs (rw,relatime)"
time="2025-10-27 13:47:31.926009586" level=info msg="hugetlbfs on /dev/hugepages type hugetlbfs (rw,relatime)"
time="2025-10-27 13:47:31.926016797" level=info msg="mqueue on /dev/mqueue type mqueue (rw,relatime)"
time="2025-10-27 13:47:31.926023749" level=info msg="sunrpc on /var/lib/nfs/rpc_pipefs type rpc_pipefs (rw,relatime)"
time="2025-10-27 13:47:31.926031892" level=info msg="tmpfs on /run/user/0 type tmpfs (rw,nosuid,nodev,relatime,size=373340k,mode=700)"
time="2025-10-27 13:47:31.926039617" level=info msg="/dev/nvme1n1p1 on /mnt type ext4 (rw,relatime,data=ordered)"
time="2025-10-27 13:47:31.926046655" level=info msg="binfmt_misc on /proc/sys/fs/binfmt_misc type binfmt_misc (rw,relatime)"
time="2025-10-27 13:47:31.926055539" level=info msg="==========End===========" Type=/bin/mount
time="2025-10-27 13:47:31.926106766" level=info msg="MountPoint: /, FsType: ext4 Added"
time="2025-10-27 13:47:31.926120149" level=info msg="Add MountPoint mountpoint:/_deviceName:nvme0n1p1_fsType:ext4_uniqueName:/_nvme0n1p1"
time="2025-10-27 13:47:31.926129402" level=info msg="MountPoint: /mnt, FsType: ext4 Added"
time="2025-10-27 13:47:31.926139213" level=info msg="Add MountPoint mountpoint:/mnt_deviceName:nvme1n1p1_fsType:ext4_uniqueName:/mnt_nvme1n1p1"
time="2025-10-27 13:47:31.926237597" level=info msg="###########1. Starting FsFreeze"
time="2025-10-27 13:47:32.925106618" level=info msg="FsFreeze Finished successfully"
time="2025-10-27 13:47:32.925151817" level=info msg="FsFreeze Finished"
time="2025-10-27 13:47:32.925165015" level=info msg="=========BEGIN===========" Type=FsFreeze
time="2025-10-27 13:47:32.925176577" level=info msg="user defined signal 12025-10-27 13:47:31 ############2. Filesystem Freeze Started"
time="2025-10-27 13:47:32.925185013" level=info msg="2025-10-27 13:47:31 freezing: /"
time="2025-10-27 13:47:32.925194923" level=info msg="2025-10-27 13:47:31 freezing: /mnt"
time="2025-10-27 13:47:32.925202706" level=info msg="2025-10-27 13:47:31 ############ 3. Filesystem Freeze Completed "
time="2025-10-27 13:47:32.925212501" level=info msg="2025-10-27 13:47:32 quiescing 1 second "
time="2025-10-27 13:47:32.925219571" level=info msg="2025-10-27 13:47:32 ############ 5. Filesystem Thaw Received "
time="2025-10-27 13:47:32.925233361" level=info msg="2025-10-27 13:47:32 Thawing: /mnt"
time="2025-10-27 13:47:32.925240869" level=info msg="2025-10-27 13:47:32 Thawing: /"
time="2025-10-27 13:47:32.925248994" level=info msg="==========End===========" Type=FsFreeze
time="2025-10-27 13:47:32.925258464" level=info msg="=========BEGIN===========" Type=CreateSnapshot
time="2025-10-27 13:47:32.925268584" level=info msg="2025-10-27 13:47:32.25470979 SnapshotGroup, requestId=27F2F825-776E-5C3D-839B-813B36123F90, GroupId=ssg-2ze7j78cztap073v1xba, Cost:318.727822ms"
time="2025-10-27 13:47:32.925280698" level=info msg="2025-10-27 13:47:32.333255308 Error:Unexpected length of the snapshot group%!(EXTRA int=30)"
time="2025-10-27 13:47:32.925288885" level=info msg="2025-10-27 13:47:32.395244228 Error:Unexpected length of the snapshot group%!(EXTRA int=30)"
time="2025-10-27 13:47:32.925296783" level=info msg="2025-10-27 13:47:32.466278714 Error:Unexpected length of the snapshot group%!(EXTRA int=30)"
time="2025-10-27 13:47:32.925304679" level=info msg="2025-10-27 13:47:32.668963454 Quering SnapshotGroup Progress[0], RequestId=B92CE5D7-4C03-542D-B3BE-1C6A492B3A36, GroupId=ssg-2ze7j78cztap073v1xba, Cost=202.666366ms, TotalCost=414.230578ms"
time="2025-10-27 13:47:32.925313612" level=info msg="2025-10-27 13:47:32.92417632 Quering SnapshotGroup Progress[1], RequestId=C089B9DC-230A-53CC-83E1-0EF20C20DBBE, GroupId=ssg-2ze7j78cztap073v1xba, Cost=205.137166ms, TotalCost=669.44581ms"
time="2025-10-27 13:47:32.925321247" level=info msg="2025-10-27 13:47:32.924177784 Finished Quering SnapshotGroup Progress"
time="2025-10-27 13:47:32.925328954" level=info msg="2025-10-27 13:47:32.924191678 Finish SnapshotGroup=ssg-2ze7j78cztap073v1xba Creation, TotalCost=988.217821ms, QueryCost=669.468498ms"
time="2025-10-27 13:47:32.925341399" level=info msg="==========End===========" Type=CreateSnapshot
time="2025-10-27 13:47:33.069961139" level=info msg="Take FsConsistent snapshots successfully"
time="2025-10-27 13:47:33.070003669" level=info msg="Prescript cost=, Postscript cost="
time="2025-10-27 13:47:33.070019742" level=info msg="Prepare Freeze=14.586567ms, Thaw cost=1.059262ms, Snapshot Creation=988.224506ms, Tag Resources=144.6089ms"
time="2025-10-27 13:47:33.070031507" level=info msg="Finish whole Processes of Snapshot successfully"
```

### 约束

#### 不能将同一硬盘挂载到两个目录

当我们将`/dev/nvme1n1p1`挂载到两个路径时，
```
[root@iZ2zej48nvay43y5f66hlfZ /]# mount /dev/nvme1n1p1 /mnt2/
[root@iZ2zej48nvay43y5f66hlfZ /]# mount |grep nvme1n1p1
/dev/nvme1n1p1 on /mnt type ext4 (rw,relatime,data=ordered)
/dev/nvme1n1p1 on /mnt2 type ext4 (rw,relatime,data=ordered)
```
任务会执行失败，查看日志:

```
[root@iZ2zej48nvay43y5f66hlfZ log]# cat app_snapshot_plugin.log.20251027.8 
time="2025-10-27 14:09:44.275743555" level=info msg="Runtime Args: [--RamRoleName=snap -EnableFsFreeze=true -TimeoutInSeconds=30 -PreScriptPath=/tmp/prescript.sh -PostScriptPath=/tmp/postscript.sh -ExcludeDiskId=d-2zej48nvay43y5ezy63t -Name=Created_At_202510271409]"
time="2025-10-27 14:09:44.27592087" level=error msg="Failded to read local default configure file" error="open /usr/local/share/aliyun-assist/plugin/app-snapshot-plugin/1.8/app_snapshot_plugin.json: no such file or directory"
time="2025-10-27 14:09:44.28521262" level=info msg="http://100.100.100.200/latest/meta-data/instance-id i-2zej48nvay43y5f66hlf <nil>"
time="2025-10-27 14:09:44.293287091" level=info msg="http://100.100.100.200/latest/meta-data/region-id cn-beijing <nil>"
time="2025-10-27 14:09:44.309703922" level=info msg="=========BEGIN===========" Type=lsb_release
time="2025-10-27 14:09:44.309738811" level=info msg="LSB Version:\t:core-4.1-amd64:core-4.1-noarch"
time="2025-10-27 14:09:44.309746008" level=info msg="Distributor ID:\tCentOS"
time="2025-10-27 14:09:44.30975265" level=info msg="Description:\tCentOS Linux release 7.9.2009 (Core)"
time="2025-10-27 14:09:44.309758934" level=info msg="Release:\t7.9.2009"
time="2025-10-27 14:09:44.309763635" level=info msg="Codename:\tCore"
time="2025-10-27 14:09:44.30976907" level=info msg="==========End===========" Type=lsb_release
time="2025-10-27 14:09:44.309791344" level=error msg="File /tmp/prescript.sh not found , error=stat /tmp/prescript.sh: no such file or directory"
time="2025-10-27 14:09:44.309804145" level=error msg="File /tmp/postscript.sh not found , error=stat /tmp/postscript.sh: no such file or directory"
time="2025-10-27 14:09:44.30981302" level=error msg="Pre/Post-Script have no Expected Permission"
time="2025-10-27 14:09:44.309823206" level=info msg="Pre/Post-Script files were not found!"
time="2025-10-27 14:09:44.309840256" level=info msg="Dump SnapshotContext Options:"
time="2025-10-27 14:09:44.309847057" level=info msg="InstanceID: i-2zej48nvay43y5f66hlf"
time="2025-10-27 14:09:44.309851925" level=info msg="RegionID: cn-beijing"
time="2025-10-27 14:09:44.309856296" level=info msg="RamRoleName: snap"
time="2025-10-27 14:09:44.30987237" level=info msg="ExcludeDiskId: [d-2zej48nvay43y5ezy63t]"
time="2025-10-27 14:09:44.30987858" level=info msg="ProductNetwork: vpc"
time="2025-10-27 14:09:44.309883148" level=info msg="PreScriptPath: /tmp/prescript.sh"
time="2025-10-27 14:09:44.309887726" level=info msg="PostScriptPath: /tmp/postscript.sh"
time="2025-10-27 14:09:44.309892357" level=info msg="EnableFsFreeze: true"
time="2025-10-27 14:09:44.309897748" level=info msg="TimeoutInSeconds: 30"
time="2025-10-27 14:09:44.309902276" level=info msg="ScriptTimeoutInSeconds: 1800"
time="2025-10-27 14:09:44.426058149" level=info msg="Network connection RequestId= CF1E3059-EF37-5DDE-922F-313CCF7F850E, timeCost=116.078595ms"
time="2025-10-27 14:09:44.426103238" level=info msg="Attached DiskId=d-2zej48nvay43y5ezy63u, Device=/dev/xvdb, Category=cloud_auto"
time="2025-10-27 14:09:44.426114021" level=info msg="Excluded DiskId=d-2zej48nvay43y5ezy63t, Device=/dev/xvda, Category=cloud_essd"
time="2025-10-27 14:09:44.426122386" level=error msg="All attached ESSD Disks were excluded"
```

并未有什么有价值打印. 怀疑和多挂载点有关。但是日志没有做好。

#### 当选择某几个硬盘做快照时，frozen所有硬盘

创建一个实例，有三个盘，两个数据盘，一个系统盘.给两个数据盘创建快照:

![aliyun_snapshot_two_data_disk](pic/aliyun_snapshot_two_data_disk.png)

结果三个盘都frozen了:
```
[root@iZ2zeaod5gslt1xw4j7nfoZ 1.8]# cat log/app_snapshot_plugin.log.20251027.5 |grep -E 'freezing|Thaw'
time="2025-10-27 15:32:30.514272608" level=info msg="2025-10-27 15:32:29 freezing: /"
time="2025-10-27 15:32:30.514282724" level=info msg="2025-10-27 15:32:29 freezing: /mnt2"
time="2025-10-27 15:32:30.514292697" level=info msg="2025-10-27 15:32:29 freezing: /mnt"
time="2025-10-27 15:32:30.51432347" level=info msg="2025-10-27 15:32:30 ############ 5. Filesystem Thaw Received "
time="2025-10-27 15:32:30.514333752" level=info msg="2025-10-27 15:32:30 Thawing: /mnt"
time="2025-10-27 15:32:30.51434341" level=info msg="2025-10-27 15:32:30 Thawing: /mnt2"
time="2025-10-27 15:32:30.514353054" level=info msg="2025-10-27 15:32:30 Thawing: /"
time="2025-10-27 15:32:30.659631972" level=info msg="Prepare Freeze=16.606892ms, Thaw cost=977.311µs, Snapshot Creation=998.907861ms, Tag Resources=145.081685ms"
```

但是阿里这边能够找到，哪个盘没有标记制作快照:
```
[root@iZ2zeaod5gslt1xw4j7nfoZ 1.8]# cat log/app_snapshot_plugin.log.20251027.5  |grep Exclude
time="2025-10-27 15:32:29.137528309" level=info msg="Runtime Args: [--RamRoleName=consistent-snapshot -EnableFsFreeze=true -TimeoutInSeconds=30 -PreScriptPath=/tmp/prescript.sh -PostScriptPath=/tmp/postscript.sh -ExcludeDiskId=d-2zeaod5gslt1xw4noek8 -Name=Created_At_202510271532]"
time="2025-10-27 15:32:29.17208288" level=info msg="ExcludeDiskId: [d-2zeaod5gslt1xw4noek8]"
time="2025-10-27 15:32:29.28167274" level=info msg="Excluded DiskId=d-2zeaod5gslt1xw4noek8, Device=/dev/xvda, Category=cloud_essd_entry"
```

但是在暂停的时候，却选择让所有盘暂停，不知道是不是BUG .

## 测试超时
编写kprobe程序制造延迟:
```
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/delay.h>    // 用于msleep/udelay

static struct kprobe kp;

static int handler_pre(struct kprobe *p, struct pt_regs *regs)
{
    printk(KERN_INFO "freeze_super called! pid=%d, delaying 2s\n", current->pid);
    msleep(20000); // 延迟20秒
    return 0;
}

static int __init kprobe_init(void)
{
    kp.symbol_name = "freeze_super";
    kp.pre_handler = handler_pre;

    if (register_kprobe(&kp) < 0) {
        printk(KERN_INFO "register_kprobe failed\n");
        return -1;
    }
    printk(KERN_INFO "kprobe registered for freeze_super\n");
    return 0;
}

static void __exit kprobe_exit(void)
{
    unregister_kprobe(&kp);
    printk(KERN_INFO "kprobe unregistered\n");
}

module_init(kprobe_init)
module_exit(kprobe_exit)
MODULE_LICENSE("GPL");
```
在界面设置10s延迟:

![aliyun_timeout_10](pic/aliyun_timeout_10.png)

触发执行快照, 在操作日志中发现了timeout事件
```
[level="info"][time="2025-10-31 17:01:18.444005807"][message="Version 1.0.0.8"]
[level="info"][time="2025-10-31 17:01:18.462900139"][message="InstanceID= i-2zec6tjwyxo894ijbqbz RegionID= cn-beijing ProductNetwork= vpc"]
[level="error"][time="2025-10-31 17:01:18.478344729"][message="File /tmp/prescript.sh not found , error=stat /tmp/prescript.sh: no such file or directory"]
[level="error"][time="2025-10-31 17:01:18.478361112"][message="File /tmp/postscript.sh not found , error=stat /tmp/postscript.sh: no such file or directory"]
[level="error"][time="2025-10-31 17:01:18.478370009"][message="Pre/Post-Script have no Expected Permission"]
[level="info"][time="2025-10-31 17:01:18.478379977"][message="Pre/Post-Script files were not found!"]
[level="info"][time="2025-10-31 17:01:18.478434233"][message="Begin to Create Application Consistent Snapshots ..."]
[level="info"][time="2025-10-31 17:01:18.596047673"][message="Network connection RequestId= F5892C1D-9F40-597F-B71F-D211F33241CD, timeCost=117.529694ms"]
[level="info"][time="2025-10-31 17:01:18.596061242"][message="Attached DiskId=d-2zec6tjwyxo894iew1g3, Device=/dev/xvda, Category=cloud_essd"]
[level="info"][time="2025-10-31 17:01:18.806493047"][message="Start to check Pre/Post-Script"]
[level="error"][time="2025-10-31 17:01:18.806515172"][message="lstat /tmp/prescript.sh: no such file or directory"]
[level="error"][time="2025-10-31 17:01:18.806528902"][message="No such Pre/Post-Script files or have no "execute" Permission"]
[level="info"][time="2025-10-31 17:01:18.806543915"][message="Discovering Mounted Volumes ..."]
[level="info"][time="2025-10-31 17:01:18.809154824"][message="MountPoint: /, FsType: ext4 Added"]
[level="info"][time="2025-10-31 17:01:18.809222201"][message="###########1. Starting FsFreeze"]
FsFreeze was interrupted by cancel or timeout context deadline exceeded
[level="info"][time="2025-10-31 17:01:38.80925727"][message="###########4. Thawing FsFreeze"]
```
但是云助手一直卡住:

![cloud_man_block](pic/cloud_man_block.png)

另外也登录不上了。似乎是bug.

**结论**

aliyun 是在冻结文件系统之前就做了超时处理, 似乎是一个异步事件。 (阿里这边做的不
是很准，例如我将延迟搞成了8s，设置超时5s还是会执行成功).

## 控制面流程

### who initiated this function
阿里云这边控制面发起创建快照是由云助手命令发起整个云助手命令. 通过
下面方式抓取。

1. 由于打快照的动作比较快 **并且execsnoop抓不到云助手命令！这个比较奇怪** , 使用
   上面的kprobe搞一些延迟
2. 打快照通过下面的方式抓取进程cmdline

   ```sh
   [root@iZbp1592ehjxikrnuq6dpqZ kprobe]# ps -ef |grep fsfr
   root      1834  1801  0 11:30 ?        00:00:00 /usr/local/share/aliyun-assist/plugin/app-snapshot-plugin/1.8/fsfreezer 30 /
   root      1864  1358  0 11:30 pts/0    00:00:00 grep --color=auto fsfr
   [root@iZbp1592ehjxikrnuq6dpqZ kprobe]# ps aux |grep 1801
   root      1801  0.0  0.4 724136 17876 ?        Sl   11:30   0:00 /usr/local/share/aliyun-assist/plugin/app-snapshot-plugin/1.8/app-snapshot-plugin --RamRoleName=snap -EnableFsFreeze=true -TimeoutInSeconds=30 -PreScriptPath=/tmp/prescript.sh -PostScriptPath=/tmp/postscript.sh -Name=Created_At_202511101129
   root      1887  0.0  0.0 112812   980 pts/0    S+   11:30   0:00 grep --color=auto 1801 
   ```

可以发现有两个命令:
* app-snapshot-plugin
* fsfreezer

第一个命令为云助手执行的命令，在guest侧发起创建快照.

第二个命令是冻结文件系统的命令，命令参数
```
[root@iZbp1592ehjxikrnuq6dpqZ kprobe]# /usr/local/share/aliyun-assist/plugin/app-snapshot-plugin/1.8/fsfreezer --help
Version: 1.0.0.8
Usage: Timeoutinseconds Mountpoint1 [Mountpoint2 ...]
```

> NOTE
>
> 直接在guest中执行该命令，也可以触发保存快照流程。

### 这些命令在何时安装

执行快照之前:
```
[root@iZbp1gqjwuuwwdi53tf6ipZ ~]# find / -name "app-snapshot-plugin"
[root@iZbp1gqjwuuwwdi53tf6ipZ ~]# 
```
发起快照之后:
```
[root@iZbp1gqjwuuwwdi53tf6ipZ ~]# find / -name "app-snapshot-plugin"
/usr/local/share/aliyun-assist/plugin/app-snapshot-plugin
/usr/local/share/aliyun-assist/plugin/app-snapshot-plugin/1.8/app-snapshot-plugin
```

可以发现镜像里面没有，在云助手命令执行之后才安装。

### 这两个命令如何交互

`fsfreezer` 提供超时功能，如果`fsfreezer`未超时，`app-snapshot-plugin` 理应通知
`fsfreezer` 解冻文件系统。他们两个如何交互呢?

我们用strace追踪fsfreezer. 日志如下:
```
open("/", O_RDONLY|O_NONBLOCK)          = 3
ioctl(3, FIFREEZE)
kill(2654, SIGUSR1User defined signal 1

## 等待30s
2025-11-10 11:39:21 quiescing 1 second
2025-11-10 11:39:22 quiescing 1 second
2025-11-10 11:39:50 quiescing 1 second
2025-11-10 11:39:50 Timedout to receive Thaw from parent process
2025-11-10 11:39:50 Thawing: /
ioctl(3, FITHAW)                        = 0
...
```

可以发现，其会在冻结文件系统之后，发`SIGUSR1`信号给其父进程，然后
循环等待, 直到超时, 恢复文件系统。


我们再来看下未超时的流程, 我们直接使用`killsnoop`工具抓取:
```

[root@iZbp1592ehjxikrnuq6dpqZ tools]# ps -ef |grep fsfre
root      2934  2893  0 13:10 pts/1    00:00:00 /usr/local/share/aliyun-assist/plugin/app-snapshot-plugin/1.8/fsfreezer 30 /
root      2967  1296  0 13:10 pts/0    00:00:00 grep --color=auto fsfre
[root@iZbp1592ehjxikrnuq6dpqZ tools]# strace -fp 2934
strace: Process 2934 attached
stat("/etc/localtime", {st_mode=S_IFREG|0644, st_size=556, ...}) = 0
getppid()                               = 2893
kill(2893, SIGUSR1)                     = 0
rt_sigprocmask(SIG_BLOCK, [CHLD], [], 8) = 0
rt_sigaction(SIGCHLD, NULL, {sa_handler=SIG_DFL, sa_mask=[], sa_flags=0}, 8) = 0
rt_sigprocmask(SIG_SETMASK, [], NULL, 8) = 0
nanosleep({tv_sec=1, tv_nsec=0}, {tv_sec=0, tv_nsec=891700083}) = ? ERESTART_RESTARTBLOCK (Interrupted by signal)
--- SIGUSR1 {si_signo=SIGUSR1, si_code=SI_USER, si_pid=2893, si_uid=0} ---
rt_sigreturn({mask=[]})                 = -1 EINTR (Interrupted system call)
stat("/etc/localtime", {st_mode=S_IFREG|0644, st_size=556, ...}) = 0
stat("/etc/localtime", {st_mode=S_IFREG|0644, st_size=556, ...}) = 0
stat("/etc/localtime", {st_mode=S_IFREG|0644, st_size=556, ...}) = 0
ioctl(3, FITHAW)                        = 0
close(3)                                = 0
write(1, "2025-11-10 13:10:44 ############"..., 291) = 291
exit_group(0)                           = ?
```

可以看到fsfreezer 在给`app-snapshot-plugin`发送给父进程`SIGUSR1`信号后，其最终等
到父进程`SIGUSR1`信号，等待到信号后，其就解冻文件系统。所以整个的流程为:
```
fsfreezer                        app-snapshot-plugin
====================================================
                                 发起快照前，等待文件系统冻结
冻结文件系统，冻结
  成功后，像父进程
  app-snapshot-plugin发送
  SIGUSR1 信号

睡眠，等待SIGUSR1信号
                                 收到SIGUSR1信号，执行应用一致性冻结脚本

                                 调用控制面保存快照接口

                                 执行结束，发送SIGUSR1信号给子进程

等待到SIGUSR1信号, 确认没有
超时，结束进程
                                 获取fsfreezer 命令返回值, 判断是否正常返回
```
## TODO
* 测试fsfroze 能否保证xfs事务完整
