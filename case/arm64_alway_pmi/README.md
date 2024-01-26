# 分析vmcore
## 获取异常进程
```
crash> bt -a
...
PID: 19899  TASK: ffff146a41f98700  CPU: 13  COMMAND: "ovsdb-server"
 #0 [ffff8000101af680] armv8pmu_start at ffffb1e29d5b826c
 #1 [ffff8000101af6a0] armv8pmu_handle_irq at ffffb1e29d5b88a4
 #2 [ffff8000101af840] armpmu_dispatch_irq at ffffb1e29dd88d50
 #3 [ffff8000101af860] handle_percpu_devid_irq at ffffb1e29d69b664
 #4 [ffff8000101af8a0] generic_handle_irq at ffffb1e29d693908
 #5 [ffff8000101af8c0] __handle_domain_irq at ffffb1e29d6942a0
 #6 [ffff8000101af900] gic_handle_irq at ffffb1e29d5a1770
 #7 [ffff8000101afa60] el1_irq at ffffb1e29d5a31b4
```

## 查看files
```
crash> files 19899
PID: 19899  TASK: ffff146a41f98700  CPU: 13  COMMAND: "ovsdb-server"
ROOT: /    CWD: /
 FD       FILE            DENTRY           INODE       TYPE PATH
  0 ffff14699600ca00 ffff1469a00b0c00 ffff146988daad40 CHR  /dev/null
  1 ffff14699600ca00 ffff1469a00b0c00 ffff146988daad40 CHR  /dev/null
  2 ffff14699600ca00 ffff1469a00b0c00 ffff146988daad40 CHR  /dev/null
  3 ffff146998af8000 ffff1469a3112e80 ffff1469a0974010 FIFO
  4 ffff146998af4f00 ffff1469a3112e80 ffff1469a0974010 FIFO
  5 ffff146998afa400 ffff1469a311b040 ffff1469a2e5bc30 REG  /var/log/openvswitch/ovsdb-server.log
  6 ffff146998af9400 ffff1469a3115f40 ffff1469a127d6b0 SOCK UNIX
  7 ffff146998aff300 ffff1469a3115280 ffff1469a2e5d5b0 REG  /tmp/#68079737
  8 ffff146996002100 ffff1469a2b5af80 ffff1469997830f8 REG  /run/openvswitch/ovsdb-server.pid
  9 ffff146998af5200 ffff1469a311cfc0 ffff1469a097def0 FIFO
 10 ffff146996002800 ffff1469a2b5b4c0 ffff1469a208e410 FIFO
 11 ffff146996000700 ffff1469a2b5b4c0 ffff1469a208e410 FIFO
 12 ffff146996004d00 ffff1469a2b5d080 ffff1469a07e9c50 UNKN [perf_event]
                                                            ^^^^^^^^^^^^
 13 ffff14699600c600 ffff1469a2b50240 ffff1469a2ed2330 REG  /etc/openvswitch/.conf.db.~lock~
 14 ffff14699600be00 ffff1469a311cd80 ffff1469a2edf7b0 REG  /etc/openvswitch/conf.db
 15 ffff14699600b500 ffff1469a2b58100 ffff1469a0d5cbb0 SOCK UNIX
 16 ffff146996004c00 ffff1469a2b5c480 ffff1469a0d5fd30 SOCK UNIX
 17 ffff14699600ca00 ffff1469a00b0c00 ffff146988daad40 CHR  /dev/null
 18 ffff1469904a6900 ffff1469a6b57a40 ffff1469a0d59a30 SOCK UNIX
 19 ffff146cd7ab4200 ffff146c22427080 ffff146c280bdef0 SOCK UNIX
 20 ffff14699ec52000 ffff146c3b2f8280 ffff146c3b3bd130 SOCK UNIX
 21 ffff14699ee27f00 ffff146c3b258100 ffff146c3e4dfa70 SOCK TCP
 22 ffff146a41a3d900 ffff1469af128640 ffff146c3e214a70 SOCK UNIX
```
可以发现 `perf_event` 匿名文件

## 获取 perf_event

* 获取地址
  ```
  crash> struct file.private_data ffff146996004d00
    private_data = 0xffff14699f02cbe8
  ```
* perf_event.attr.type
  ```
  crash> struct perf_event.attr.type 0xffff14699f02cbe8
  attr.type = 0,
  ```
  可以发现为 hw perf event.
* perf_event.hw.idx
  ```
  crash> struct perf_event.hw.idx 0xffff14699f02cbe8
    hw.idx = 1,
  ```
  说明event counter idx 为1
* perf_event.hw.config_base --- 决定event type 
  ```
  crash> struct perf_event.hw.config_base 0xffff14699f02cbe8
  hw.config_base = 18446744071562067976,
  ```
  evtCount[15:0] : 为3
  ```
  0x0003 L1D_CACHE_REFILL   Level 1 data cache refill.
  ```

## struct arm64_pmu
* 地址
  ```
  crash> struct perf_event.pmu 0xffff14699f02cbe8
    pmu = 0xffff146988f1a800
  ```
* `armpmu->get_event_idx`
  ```
  crash> struct arm_pmu.get_event_idx 0xffff146988f1a800
  get_event_idx = 0xffffb1e29d5b89c0
  crash> dis 0xffffb1e29d5b89c0 |head -1
  0xffffb1e29d5b89c0 <armv8pmu_get_event_idx>:    stp     x29, x30, [sp,#-48]!
  ```
* pmu_hw_events
  ```
  crash> struct arm_pmu.hw_events 0xffff146988f1a800
  hw_events = 0xffffb1e29ecb5b00
  crash> p (0xffffb1e29ecb5b00+__per_cpu_offset[13])
  $5 = 18446485107497261824
  crash> p (char *)18446485107497261824
  $6 = 0xffff1478bae85b00 ""
  crash> pmu_hw_events 0xffff1478bae85b00
  struct pmu_hw_events {
    events = {0x0, 0xffff14699f02cbe8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
    used_mask = {2},
    pmu_lock = {
      raw_lock = {
        {
          val = {
            counter = 1
          },
          {
            locked = 1 '\001',
            pending = 0 '\000'
          },
          {
            locked_pending = 1,
            tail = 0
          }
        }
      }
    },
    percpu_pmu = 0xffff146988f1a800,
    irq = 42
  }

  ```
  可以发现该cpu上只有一个 perf event, 就是 ovsdb-server所拥有的那个, 并且idx为1

