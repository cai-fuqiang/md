## csv 大页预留
```
1752142306.832733 : CSV-CMA: Node 0 - reserve size 0x0000001000000000, (expected size 0x0000000ff9024000)
1752142306.833254 : CSV-CMA: Node 1 - reserve size 0x0000001000000000, (expected size 0x0000000fffec0000)
1752142306.833788 : CSV-CMA: Node 2 - reserve size 0x0000001000000000, (expected size 0x0000000fffec0000)
1752142306.834311 : CSV-CMA: Node 3 - reserve size 0x0000001000000000, (expected size 0x0000000fffec0000)
1752142306.834846 : CSV-CMA: Node 4 - reserve size 0x0000001000000000, (expected size 0x0000000fffec0000)
1752142306.835382 : CSV-CMA: Node 5 - reserve size 0x0000001000000000, (expected size 0x0000000fffec0000)
1752142306.835918 : CSV-CMA: Node 6 - reserve size 0x0000001000000000, (expected size 0x0000000fffec0000)
1752142306.836441 : CSV-CMA: Node 7 - reserve size 0x0000001000000000, (expected size 0x0000000fffec0000)
```

## 在node-1上尝试开启大页
```sh
[root@11-241-212-108 13:11:00 hugepages-2048kB]# pwd
/sys/devices/system/node/node1/hugepages/hugepages-2048kB

## 查看node 1 normal ZONE /proc/zoneinfo 查看剩余内存, 
cat /proc/zoneinfo
Node 1, zone   Normal
  ...
  pages free     32816235
        boost    0
        min      7058
        low      40085
        high     73112
        spanned  33553792
        present  33553792
        managed  33027391
        cma      16777216
        protection: (0, 0, 0, 0, 0)
      nr_free_pages 32816235
      nr_zone_inactive_anon 285
      nr_zone_active_anon 57
      nr_zone_inactive_file 82241
      nr_zone_active_file 32953
      nr_zone_unevictable 0
      nr_zone_write_pending 0
      nr_mlock     0
      nr_bounce    0
      nr_zspages   0
      nr_free_cma  16774451
```

`free` 中 `cma`大概占用一半，我们尝试在`Node 1`分配超过`free - cma`大小
的内存, 多分20GB
```
((32816235 - 16777216) * 4096 ) / 2048/ 1024 + 10240
= 41566

[root@11-241-212-108 14:21:17 hugepages-2048kB]# echo $((41566 * 2 /1024))
81
```
大概是`81G`内存

执行相关命令
```
[root@11-241-212-108 13:50:29 hugepages-2048kB]# pwd
/sys/devices/system/node/node1/hugepages/hugepages-2048kB
[root@11-241-212-108 14:18:26 hugepages-2048kB]# echo 41566 > nr_hugepages
[root@11-241-212-108 14:18:37 hugepages-2048kB]# cat nr_hugepages
41566
```

在查看node-1 Normal zone

```
  pages free     11534647
        boost    0
        min      7058
        low      40085
        high     73112
        spanned  33553792
        present  33553792
        managed  33027391
        cma      16777216
        protection: (0, 0, 0, 0, 0)
      nr_free_pages 11534647
      nr_zone_inactive_anon 285
      nr_zone_active_anon 57
      nr_zone_inactive_file 82241
      nr_zone_active_file 32953
      nr_zone_unevictable 0
      nr_zone_write_pending 0
      nr_mlock     0
      nr_bounce    0
      nr_zspages   0
      nr_free_cma  5767535
```

cma 下降了:
```
[root@11-241-212-108 14:27:31 hugepages-2048kB]# echo $(((16774451 - 5767535)*4/1024/1024))
41
```
也就是说，还是对半分的.

使用大页启动虚拟机


## 尝试分配不同size大页

### 尝试分配80G内存虚拟机
```
[root@11-241-212-108 14:38:23 vm]# virsh start wang_sev
error: Failed to start domain 'wang_sev'
error: internal error: QEMU unexpectedly closed the monitor (vm='wang_sev'): configure accelerator pc-q35-6.2 start
2025-07-11T06:39:41.145912Z qemu-system-x86_64: sev_ram_block_added: failed to register region (0x7f75e7e00000+0x1400000000) error 'Cannot allocate memory'
```
dmesg 信息:
```
[73683.965186] ccp 0000:07:00.4: sev command id 0x22 buffer 0x0000001104ce9000 timeout 100s
[73683.965213] (in):  00000000: 0001 0000
[73683.967304] (out): 00000000: 0001 0000
[73683.967311] ccp 0000:07:00.4: sev command id 0x20 buffer 0x0000001104ce9000 timeout 100s
[73683.967317] (in):  00000000: 0001 0000
[73683.967659] (out): 00000000: 0001 0000
[73879.609065] ccp 0000:07:00.4: sev command id 0x4 buffer 0x0000001104ce9000 timeout 100s
[73879.609081] (in):  00000000: 0000 0000 0000 0000 0000 0000
[73879.609275] (out): 00000000: 0000 0000 0000 0000 0000 0000
[73879.609293] ccp 0000:07:00.4: sev command id 0x30 buffer 0x0000001104ce9000 timeout 100s
[73879.609299] (in):  00000000: 0000 0000 0005 0000 0000 0000 0000 0000
[73879.609303] (in):  00000010: 0000 0000 0000 0000 0000 0000 0000 0000
[73879.609305] (in):  00000020: 0000 0000
[73879.609778] (out): 00000000: 0000 0000 0005 0000 0000 0000 0000 0000
[73879.609784] (out): 00000010: 0000 0000 0000 0000 0000 0000 0000 0000
[73879.609786] (out): 00000020: 0000 0000
[73879.609789] ccp 0000:07:00.4: sev command id 0x21 buffer 0x0000001104ce9000 timeout 100s
[73879.609794] (in):  00000000: 0001 0000 000d 0000
[73879.611953] (out): 00000000: 0001 0000 000d 0000
[73954.992487] kvm_amd: SEV: Failure locking 20971520 pages.
[73955.246221] ccp 0000:07:00.4: sev command id 0x22 buffer 0x0000001104ce9000 timeout 100s
[73955.246239] (in):  00000000: 0001 0000
[73955.248326] (out): 00000000: 0001 0000
[73955.248332] ccp 0000:07:00.4: sev command id 0x20 buffer 0x0000001104ce9000 timeout 100s
[73955.248337] (in):  00000000: 0001 0000
[73955.248766] (out): 00000000: 0001 0000
```

## 尝试分配42G内存
cma大概分配了41G，我们分配42G试试:
```
[root@11-241-212-108 14:47:33 vm]# virsh start wang_sev
error: Failed to start domain 'wang_sev'
error: internal error: QEMU unexpectedly closed the monitor (vm='wang_sev'): configure accelerator pc-q35-6.2 start
2025-07-11T06:47:46.889030Z qemu-system-x86_64: sev_ram_block_added: failed to register region (0x7fdde7e00000+0xa80000000) error 'Cannot allocate memory'
```
dmesg 
```
[74430.668606] ccp 0000:07:00.4: sev command id 0x4 buffer 0x0000001104ce9000 timeout 100s
[74430.668620] (in):  00000000: 0000 0000 0000 0000 0000 0000
[74430.668853] (out): 00000000: 0000 0000 0000 0000 0000 0000
[74430.668870] ccp 0000:07:00.4: sev command id 0x30 buffer 0x0000001104ce9000 timeout 100s
[74430.668877] (in):  00000000: 0000 0000 0005 0000 0000 0000 0000 0000
[74430.668881] (in):  00000010: 0000 0000 0000 0000 0000 0000 0000 0000
[74430.668885] (in):  00000020: 0000 0000
[74430.669367] (out): 00000000: 0000 0000 0005 0000 0000 0000 0000 0000
[74430.669371] (out): 00000010: 0000 0000 0000 0000 0000 0000 0000 0000
[74430.669374] (out): 00000020: 0000 0000
[74430.669378] ccp 0000:07:00.4: sev command id 0x21 buffer 0x0000001104ce9000 timeout 100s
[74430.669383] (in):  00000000: 0001 0000 000e 0000
[74430.671523] (out): 00000000: 0001 0000 000e 0000
[74440.736612] kvm_amd: SEV: Failure locking 11010048 pages.
[74440.998859] ccp 0000:07:00.4: sev command id 0x22 buffer 0x0000001104ce9000 timeout 100s
[74440.998873] (in):  00000000: 0001 0000
[74441.000948] (out): 00000000: 0001 0000
[74441.000952] ccp 0000:07:00.4: sev command id 0x20 buffer 0x0000001104ce9000 timeout 100s
[74441.000957] (in):  00000000: 0001 0000
[74441.001401] (out): 00000000: 0001 0000
```

## 尝试分配40G内存

同样会遇到问题:
```
error: Failed to start domain 'wang_sev'
error: internal error: QEMU unexpectedly closed the monitor (vm='wang_sev'): configure accelerator pc-q35-6.2 start
2025-07-11T06:52:25.781847Z qemu-system-x86_64: sev_ram_block_added: failed to register region (0x7fe053e00000+0xa00000000) error 'Cannot allocate memory'
```

## 尝试分配其他内存
* 30G: Success
* 35G: Success
* 39G: Success

## 内核代码流程
```
setup_arch
  early_csv_reserve_mem
    csv_cma_reserve_mem
```
