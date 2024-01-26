# 复现问题, 获取问题buffer

## ceph -s 获取出问题的osd
```
[root@node-2 ceph-osd]# ceph -s
  cluster:
    id:     60f601e9-d626-4758-aca0-8ad6ed73cbc8
    health: HEALTH_WARN
            Degraded data redundancy: 43619/130857 objects degraded (33.333%), 381 pgs degraded, 536 pgs undersized
            15 slow ops, oldest one blocked for 49980 sec, daemons [osd.2,osd.8] have slow ops.
               ^^^^^^^                                              ^^^^^^^^^^^       ^^^^^^^^
            clock skew detected on mon.node-2
            1/3 mons down, quorum node-1,node-2

  services:
    mon:        3 daemons, quorum node-1,node-2 (age 19h), out of quorum: node-3
    mgr:        node-1(active, since 19h), standbys: node-2
    osd:        9 osds: 6 up (since 19h), 6 in (since 6d)
                flags nodeep-scrub
    rbd-mirror: 2 daemons active (3239847, 3304125)
    rgw:        2 daemons active (ceph-rgw-84b9f688bc-7n9bw, ceph-rgw-84b9f688bc-7vdbt)

  task status:

  data:
    pools:   11 pools, 536 pgs
    objects: 43.62k objects, 153 GiB
    usage:   472 GiB used, 7.1 TiB / 7.5 TiB avail
    pgs:     43619/130857 objects degraded (33.333%)
             381 active+undersized+degraded
             155 active+undersized

  io:
    client:   851 B/s rd, 365 KiB/s wr, 0 op/s rd, 38 op/s wr
```

* 出现 slow ops 关键字
* `[osd.2, osd.8]` 表示出问题的osd

接下来需要判断哪个 osd是发送方.
```
[root@node-1 ~]# kubectl get pod -n ceph |grep -E 'osd-2|osd-8'
ceph-osd-2-node-1-nt8l9           1/1     Running   20         8d
ceph-osd-8-node-2-vl45z           1/1     Running   2          7d15h

kubectl exec -ti ceph-osd-2-node-1-nt8l9 -n ceph -- ceph daemon osd.2 config set debug_ms 1
kubectl exec -ti ceph-osd-2-node-1-nt8l9 -n ceph -- ceph daemon osd.2 config set debug_ms 0

kubectl exec -it ceph-osd-8-node-2-vl45z -n ceph -- ceph daemon osd.8 config set debug_ms 1
kubectl exec -it ceph-osd-8-node-2-vl45z -n ceph -- ceph daemon osd.8 config set debug_ms 0
```

osd8在node-2, node2在node-1
```
[root@node-1 ~]# cat /var/log/ceph/ceph-osd.2.log |grep 'bad segment' |tail -1
2024-01-03 11:06:02.153 ffff7d00cf50  1 --2- [v2:46.168.40.2:6806/14824,v1:46.168.40.2:6807/14824]
>> [v2:46.168.40.3:6808/31829,v1:46.168.40.3:6809/31829] 
conn(0xaaab8a68a000 0xaaab88ce6a00 crc :-1 s=THROTTLE_DONE pgs=37476779 cs=18738707 l=0 rev1=1 rx=0 tx=0).
_handle_read_frame_epilogue_main bad segment crc calculated=2563439532 expected=1917290664
[root@node-2 ~]# cat /var/log//ceph/ceph-osd.8.log |grep 'bad segment'
[root@node-2 ~]#
```

有`bad segment`字样的是接收方, 而另一个osd(osd-8)是发送方, 记录 expected crc `1917290664`

## 执行陈帆提供的send.stp获取Message地址
```
[root@node-2 ceph-osd]# cat send.stp
probe process("/usr/bin/ceph-osd").statement("write_message@src/msg/async/ProtocolV2.cc:517") {
        printf("message %p \n", $m)
}

probe process("/usr/bin/ceph-osd").statement("asm_crc_rev1@/usr/src/debug/ceph-14.2.22-14/src/msg/async/frames_v2.cc:179") {
        printf("send msgr asm crc rev1 %s \n", $epilogue$$)
        printf("epilogue %s %s %s %s\n", $epilogue->crc_values[0]$, $epilogue->crc_values[1]$,$epilogue->crc_values[2]$,$epilogue->crc_values[3]$)
}

[root@node-2 ceph-osd]# ps aux |grep ceph-osd |grep 'i 8'
ceph     31829 56.0  0.4 3572352 2436864 ?     Ssl  Jan02 779:54 /usr/bin/ceph-osd --cluster ceph -f -i 8 --setuser ceph --setgroup disk

[root@node-2 ceph-osd]# stap -x 31829 send.stp |grep -B 2 1917290664
--
message 0xaaab8c4c2000
send msgr asm crc rev1 {.late_status='\016', .crc_values=[{.v=1234220063}, ...]}
epilogue {.v=1234220063} {.v=4294967295} {.v=1917290664} {.v=0}
--
message 0xaaab8c4c2000
send msgr asm crc rev1 {.late_status='\016', .crc_values=[{.v=1234220063}, ...]}
epilogue {.v=1234220063} {.v=4294967295} {.v=1917290664} {.v=0}
```

获取 Message 地址`0xaaab8c4c2000`

## 执行gcore, 利用gdb 获取出问题地址
```
[root@node-2 ceph-osd]# gcore -o  /usr/bin/ceph-osd 31829
(gdb) p ((Message*)0xaaab8c4c2000)->data->_buffers->_root->next
$5 = (ceph::buffer::v14_2_0::ptr_hook *) 0xaaab8f2ef040
(gdb)  p *((ceph::buffer::v14_2_0::ptr_node *)0xaaab8f2ef040)
$6 = {<ceph::buffer::v14_2_0::ptr_hook> = {next = 0xaaab89ee9240}, <ceph::buffer::v14_2_0::ptr> = {_raw = 0xaaab87838fa0, _off = 0,
    _len = 10}, <No data fields>}
(gdb)  p *((ceph::buffer::v14_2_0::ptr_node *)0xaaab89ee9240)
$7 = {<ceph::buffer::v14_2_0::ptr_hook> = {next = 0xaaab8c6024c0}, <ceph::buffer::v14_2_0::ptr> = {_raw = 0xaaab8f482528, _off = 0,
    _len = 13}, <No data fields>}
(gdb)  p *((ceph::buffer::v14_2_0::ptr_node *)0xaaab8c6024c0)
$8 = {<ceph::buffer::v14_2_0::ptr_hook> = {next = 0xaaab8a9b2420}, <ceph::buffer::v14_2_0::ptr> = {_raw = 0xaaab8fa5f7e8, _off = 0,
    _len = 294}, <No data fields>}
(gdb)  p *((ceph::buffer::v14_2_0::ptr_node *)0xaaab8a9b2420)
$10 = {<ceph::buffer::v14_2_0::ptr_hook> = {next = 0xaaab8f23fbe0}, <ceph::buffer::v14_2_0::ptr> = {_raw = 0xaaab8f482528, _off = 13,
    _len = 15}, <No data fields>}
(gdb)  p *((ceph::buffer::v14_2_0::ptr_node *)0xaaab8f23fbe0)
$11 = {<ceph::buffer::v14_2_0::ptr_hook> = {next = 0xaaab8c076e40}, <ceph::buffer::v14_2_0::ptr> = {_raw = 0xaaab8b3d3e88, _off = 0,
    _len = 35}, <No data fields>}
(gdb)  p *((ceph::buffer::v14_2_0::ptr_node *)0xaaab8c076e40)
$12 = {<ceph::buffer::v14_2_0::ptr_hook> = {next = 0xaaab7235fec0}, <ceph::buffer::v14_2_0::ptr> = {_raw = 0xaaab8f482528, _off = 28,
    _len = 4}, <No data fields>}
(gdb)  p *((ceph::buffer::v14_2_0::ptr_node *)0xaaab7235fec0)
$13 = {<ceph::buffer::v14_2_0::ptr_hook> = {next = 0xaaab8b219140}, <ceph::buffer::v14_2_0::ptr> = {_raw = 0xaaab8c2d6120, _off = 53248,
    _len = 16384}, <No data fields>}
(gdb)  p ((ceph::buffer::v14_2_0::ptr_node *)0xaaab7235fec0)->_raw->data
$20 = 0xaaab976f0000 ""
```

## 查看stap 脚本输出
```

```
