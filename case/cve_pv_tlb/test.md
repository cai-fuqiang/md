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

> NOTE
>
> * cpu host: `host-passthrough` 虚拟机
> * cpu AMD : `host-model` 虚拟机

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

