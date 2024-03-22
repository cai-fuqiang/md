# 测试情况

## 在37环境测试

### 测试时间
03-21 18:29:00

### 测试虚拟机
`host-model` 和 `host-passthrough`各启动了6台
```
[root@node-25 ~]# ps aux |grep qemu |grep   '\-cpu EPYC-IBPB' |wc -l
6
[root@node-25 ~]# ps aux |grep qemu |grep   '\-cpu host' |wc -l
6
```

### 使用systemd 服务管理该程序
```
[Unit]
Description=CVE test app
Documentation=man:sshd(8) man:sshd_config(5)

[Service]
Type=forking
ExecStart=/root/wangfuqiang/cve/multi_thread
PIDFile=/var/run/cve_test.pid
ExecReload=/bin/kill -HUP $MAINPID
TimeoutStopSec=0
Restart=always
```

### 编写脚本获取进程状态
```
for i in `seq 1 6`
do
        echo cpu host $i
        sshpass -p 'W2751129.' ssh wang-test-0${i} 'ps aux |grep multi_thread|grep -v grep'
        echo cpu AMD $i
        sshpass -p 'W2751129.' ssh wang0${i} 'ps aux |grep multi_thread|grep -v grep'
done
```

### 输出如下
```
cpu host 1

Authorized users only. All activities may be monitored and reported.
root        7909  153  0.0 297312   300 ?        Ssl  18:14  21:28 /root/wangfuqiang/cve/multi_thread
cpu AMD 1

Authorized users only. All activities may be monitored and reported.
root        3284  154  0.0 297312   100 ?        Ssl  18:13  23:58 /root/wangfuqiang/cve/multi_thread
cpu host 2

Authorized users only. All activities may be monitored and reported.
root        7840  155  0.0 297312   296 ?        Ssl  18:14  21:36 /root/wangfuqiang/cve/multi_thread
cpu AMD 2

Authorized users only. All activities may be monitored and reported.
root        8517  152  0.0 297312   132 ?        Ssl  18:14  22:37 /root/wangfuqiang/cve/multi_thread
cpu host 3

Authorized users only. All activities may be monitored and reported.
root        7841  156  0.0 297312   136 ?        Ssl  18:14  21:36 /root/wangfuqiang/cve/multi_thread
cpu AMD 3

Authorized users only. All activities may be monitored and reported.
root        8244  150  0.0 297312   172 ?        Ssl  18:14  21:56 /root/wangfuqiang/cve/multi_thread
cpu host 4

Authorized users only. All activities may be monitored and reported.
root        7840  149  0.0 297312   116 ?        Ssl  18:15  20:42 /root/wangfuqiang/cve/multi_thread
cpu AMD 4

Authorized users only. All activities may be monitored and reported.
root        8639  160  0.0 297312   128 ?        Ssl  18:28   0:12 /root/wangfuqiang/cve/multi_thread
cpu host 5

Authorized users only. All activities may be monitored and reported.
root        8473  149  0.0 297312   144 ?        Ssl  18:16  18:47 /root/wangfuqiang/cve/multi_thread
cpu AMD 5

Authorized users only. All activities may be monitored and reported.
root        8473  149  0.0 297312   248 ?        Ssl  18:16  18:47 /root/wangfuqiang/cve/multi_thread
cpu host 6

Authorized users only. All activities may be monitored and reported.
root        7866  150  0.0 297312   156 ?        Ssl  18:17  17:42 /root/wangfuqiang/cve/multi_thread
cpu AMD 6

Authorized users only. All activities may be monitored and reported.
root        8069  152  0.0 297312   256 ?        Ssl  18:14  21:32 /root/wangfuqiang/cve/multi_thread
```

# 经过一晚测试, 测试结果

## 查看进程是否存在
```
[root@wang01 wangfuqiang]# sh get_all_node_pid.sh 2>&1 |grep multi_thread |wc -l               
12
```
查看是否有`coredump`
```
[root@wang01 wangfuqiang]# cat get_all_coredump.sh                                             
for i in `seq 1 6`
do
        echo cpu host $i
        sshpass -p 'W2751129.' ssh wang-test-0${i} 'ls /var/lib/systemd/coredump'
        echo cpu AMD $i
        sshpass -p 'W2751129.' ssh wang0${i} 'ls /var/lib/systemd/coredump'
done

[root@wang01 wangfuqiang]# sh get_all_coredump.sh
cpu host 1

Authorized users only. All activities may be monitored and reported.
cpu AMD 1

Authorized users only. All activities may be monitored and reported.
cpu host 2

Authorized users only. All activities may be monitored and reported.
cpu AMD 2

Authorized users only. All activities may be monitored and reported.
cpu host 3

Authorized users only. All activities may be monitored and reported.
cpu AMD 3

Authorized users only. All activities may be monitored and reported.
cpu host 4

Authorized users only. All activities may be monitored and reported.
cpu AMD 4

Authorized users only. All activities may be monitored and reported.
cpu host 5

Authorized users only. All activities may be monitored and reported.
cpu AMD 5

Authorized users only. All activities may be monitored and reported.
cpu host 6

Authorized users only. All activities may be monitored and reported.
cpu AMD 6

Authorized users only. All activities may be monitored and reported.
```
并没有`coredump`, 看似cve已经修复. 打算把环境切回 147_24 kernel 在进
行测试.

# kernel 切回147_24 测试
## 测试时间
```
2024 03-22 10:40-13:30
```
## 测试结果统计
```
[root@wang01 wangfuqiang]# sh get_all_coredump.sh  2>&1 |grep -v Auth                                                                                                                                        
cpu host 1

core.multi_thread.0.5cf4bb07ed0a4d26953d1b95e6a34fd2.1404.1711080706000000000000.lz4
cpu AMD 1

cpu host 2

cpu AMD 2

cpu host 3

core.multi_thread.0.52f682785d384be09a46123194635e78.1359.1711075517000000000000.lz4
core.multi_thread.0.52f682785d384be09a46123194635e78.1649.1711077732000000000000.lz4
core.multi_thread.0.52f682785d384be09a46123194635e78.4342.1711077818000000000000.lz4
core.multi_thread.0.52f682785d384be09a46123194635e78.4360.1711081242000000000000.lz4
core.multi_thread.0.52f682785d384be09a46123194635e78.4805.1711081368000000000000.lz4
core.multi_thread.0.52f682785d384be09a46123194635e78.4827.1711081958000000000000.lz4
cpu AMD 3

cpu host 4

core.multi_thread.0.ce89f08bacb64eb9a29edb6a6c891ba0.1357.1711084458000000000000.lz4
cpu AMD 4

cpu host 5

core.multi_thread.0.388db183248341ba8b8c654221253b4b.2116.1711080942000000000000.lz4
core.multi_thread.0.388db183248341ba8b8c654221253b4b.4275.1711084975000000000000.lz4
cpu AMD 5

cpu host 6

core.multi_thread.0.1f774ea81cda4dddb551641e56fd9c10.1356.1711084711000000000000.lz4
cpu AMD 6

[root@wang01 wangfuqiang]# date
Fri Mar 22 13:28:44 CST 2024
```

其中`cpu host 2`虚拟机未复现,其余5台 `host-passthrough`的虚拟机均复现. 另外 `host-model`的虚拟机
均未复现, 符合预期.

# 修复CVE 的相关rpm包信息以及部署方法
## 相关包
|包名|链接|
|---|---|
|kernel|http://koji.easystack.io/kojifiles/packages/kernel/4.18.0/147.5.1.es8_25/x86_64/kernel-4.18.0-147.5.1.es8_25.x86_64.rpm|
|kernel-core|http://koji.easystack.io/kojifiles/packages/kernel/4.18.0/147.5.1.es8_25/x86_64/kernel-core-4.18.0-147.5.1.es8_25.x86_64.rpm|
|kernel-modules|http://koji.easystack.io/kojifiles/packages/kernel/4.18.0/147.5.1.es8_25/x86_64/kernel-modules-4.18.0-147.5.1.es8_25.x86_64.rpm|
|kernel-modules-extra|http://koji.easystack.io/kojifiles/packages/kernel/4.18.0/147.5.1.es8_25/x86_64/kernel-modules-extra-4.18.0-147.5.1.es8_25.x86_64.rpm|
|kernel-modules-internal|http://koji.easystack.io/kojifiles/packages/kernel/4.18.0/147.5.1.es8_25/x86_64/kernel-modules-internal-4.18.0-147.5.1.es8_25.x86_64.rpm|
|kernel-tools|http://koji.easystack.io/kojifiles/packages/kernel/4.18.0/147.5.1.es8_25/x86_64/kernel-tools-4.18.0-147.5.1.es8_25.x86_64.rpm|
|kernel-tools-libs|http://koji.easystack.io/kojifiles/packages/kernel/4.18.0/147.5.1.es8_25/x86_64/kernel-tools-libs-4.18.0-147.5.1.es8_25.x86_64.rpm|
|kernel-headers|http://koji.easystack.io/kojifiles/packages/kernel/4.18.0/147.5.1.es8_25/x86_64/kernel-headers-4.18.0-147.5.1.es8_25.x86_64.rpm|
|kernel-devel|http://koji.easystack.io/kojifiles/packages/kernel/4.18.0/147.5.1.es8_25/x86_64/kernel-devel-4.18.0-147.5.1.es8_25.x86_64.rpm|
|python3-perf|http://koji.easystack.io/kojifiles/packages/kernel/4.18.0/147.5.1.es8_25/x86_64/python3-perf-4.18.0-147.5.1.es8_25.x86_64.rpm|
|perf|http://koji.easystack.io/kojifiles/packages/kernel/4.18.0/147.5.1.es8_25/x86_64/perf-4.18.0-147.5.1.es8_25.x86_64.rpm|

## md5
```
[root@node-25 147_25_all]# md5sum kernel-*
af4ca8a4187d7deb342e121771b8bb6a  kernel-4.18.0-147.5.1.es8_25.x86_64.rpm
85d81eea7e290491c2bda890ca8e0745  kernel-core-4.18.0-147.5.1.es8_25.x86_64.rpm
15694bbee523a662be2ae7c59cfa9924  kernel-devel-4.18.0-147.5.1.es8_25.x86_64.rpm
99eb9fc53fdc14821a96a49d1065bdc5  kernel-headers-4.18.0-147.5.1.es8_25.x86_64.rpm
d5c840982b0012974778644d864a844a  kernel-modules-4.18.0-147.5.1.es8_25.x86_64.rpm
3764792d11cbe333673e8f3596666790  kernel-modules-extra-4.18.0-147.5.1.es8_25.x86_64.rpm
868c6bfd406b88f96fc16389431d1c94  kernel-modules-internal-4.18.0-147.5.1.es8_25.x86_64.rpm
16829e0671008aa85816e831befc8756  kernel-tools-4.18.0-147.5.1.es8_25.x86_64.rpm
02d92e4752d662009b42adcd51818b76  kernel-tools-libs-4.18.0-147.5.1.es8_25.x86_64.rpm
```

## 安装方法
1. 为了确保/boot分区空间足够, 最好执行
   + 执行`rpm -qa |grep kernel-core |grep -v "kernel-core-4.18.0-147.5.1.es8_24.x86_64"`
     查看是否有其他版本的 kernel-core包, 如果有执行 `yum remove` 将其删除.
   + `rm -f /boot/initramfs-4.18.0-147.5.1.es8_24.x86_64kdump.img`
2. yum install + ALL_RPM_PATH
2. 重启后, 执行`uname -r`, 查看kernel版本是否更新

> NOTE
>
> 上面变更步骤仅限v611
