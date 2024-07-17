# 日志打印

## kernel dmesg
```
//===(1)==
Jul  7 18:37:27 node-2 kernel: Booting Linux on physical CPU 0x0000000000 [0x700f8620]
...

//===(2)==
Jul  9 09:54:35 node-2 kernel: lpfc 0005:01:00.0: 163: [141367.366011] 0:(0):0307 Mailbox cmd x12 (x0/x0) Cmpl lpfc_sli_wake_mbox_wait [lpfc] Data: x1200 x20000000 x1 x1 x1 x0 x0 x0 x0 x0 x0 x50
Jul  9 09:54:35 node-2 kernel: lpfc 0005:01:00.0: 164: [141367.366035] 0:(0):0354 Mbox cmd issue - Enqueue Data: xe (x0/x0) x0 x8200 x2
Jul  9 09:54:35 node-2 kernel: lpfc 0005:01:00.0: 165: [141367.366046] 0:(0):0355 Mailbox cmd xe (x0/x0) issue Data: x0 x8300
Jul  9 09:54:35 node-2 kernel: lpfc 0005:01:00.0: 166: [141367.366284] 0:(0):0307 Mailbox cmd xe (x0/x0) Cmpl lpfc_sli_wake_mbox_wait [lpfc] Data: xe00 x80000000 x0 x0 x0 x0 x0 x0 x0 x0 x0 x0
Jul  9 09:54:35 node-2 kernel: lpfc 0005:01:00.0: 167: [141367.366292] 0:(0):0354 Mbox cmd issue - Enqueue Data: x12 (x0/x0) x0 x8200 x2
Jul  9 09:54:35 node-2 kernel: lpfc 0005:01:00.0: 168: [141367.366303] 0:(0):0355 Mailbox cmd x12 (x0/x0) issue Data: x0 x8300
...

//===(3)==
Jul  9 09:54:38 node-2 kernel: lpfc 0005:01:00.0: 226: [141367.545783] 0:(0):0307 Mailbox cmd xe (x0/x0) Cmpl lpfc_sli_wake_mbox_wait [lpfc] Data: xe00 x80000000 x0 x0 x0 x0 x0 x0 x0 x0 x0 x0
Jul  9 09:54:38 node-2 kernel: lpfc 0005:01:00.0: 227: [141367.545811] 0:(0):0354 Mbox cmd issue - Enqueue Data: x12 (x0/x0) x0 x8200 x2
Jul  9 09:54:38 node-2 kernel: lpfc 0005:01:00.0: 228: [141367.545881] 0:(0):0355 Mailbox cmd x12 (x0/x0) issue Data: x0 x8300
Jul  9 09:54:38 node-2 kernel: lpfc 0005:01:00.0: 229: [141367.546103] 0:(0):0307 Mailbox cmd x12 (x0/x0) Cmpl lpfc_sli_wake_mbox_wait [lpfc] Data: x1200 x20000000 x1 x1 x1 x0 x0 x0 x0 x0 x0 x50
Jul  9 09:54:38 node-2 kernel: lpfc 0005:01:00.0: 230: [141401.390821] 0:(0):0354 Mbox cmd issue - Enqueue Data: xe (x0/x0) x0 x8200 x2
Jul  9 09:54:38 node-2 kernel: lpfc 0005:01:00.0: 231: [141401.390845] 0:(0):0355 Mailbox cmd xe (x0/x0) issue Data: x0 x8300
...

//===(4)==
Jul  9 09:54:38 node-2 kernel: lpfc 0005:01:00.0: 253: [141401.522361] 0:(0):0307 Mailbox cmd x12 (x0/x0) Cmpl lpfc_sli_wake_mbox_wait [lpfc] Data: x1200 x20000000 x1 x1 x1 x0 x0 x0 x0 x0 x0 x50
Jul  9 09:54:38 node-2 kernel: lpfc 0005:01:00.0: 254: [141401.522401] 0:(0):0354 Mbox cmd issue - Enqueue Data: xe (x0/x0) x0 x8200 x2
Jul  9 09:54:38 node-2 kernel: lpfc 0005:01:00.0: 255: [141401.522413] 0:(0):0355 Mailbox cmd xe (x0/x0) issue Data: x0 x8300
Jul  9 09:54:38 node-2 kernel: lpfc 0005:01:00.0: 0: [141401.522685] 0:(0):0307 Mailbox cmd xe (x0/x0) Cmpl lpfc_sli_wake_mbox_wait [lpfc] Data: xe00 x80000000 x0 x0 x0 x0 x0 x0 x0 x0 x0 x0
Jul  9 09:54:38 node-2 kernel: lpfc 0005:01:00.0: 1: [141401.522693] 0:(0):0354 Mbox cmd issue - Enqueue Data: x12 (x0/x0) x0 x8200 x2
Jul  9 09:54:38 node-2 kernel: lpfc 0005:01:00.0: 2: [141401.522704] 0:(0):0355 Mailbox cmd x12 (x0/x0) issue Data: x0 x8300
...

//===(5)==
Jul  9 09:54:38 node-2 kernel: lpfc 0005:01:00.0: 157: [141401.690337] 0:(0):0354 Mbox cmd issue - Enqueue Data: x12 (x0/x0) x0 x8200 x2
Jul  9 09:54:38 node-2 kernel: lpfc 0005:01:00.0: 158: [141401.690348] 0:(0):0355 Mailbox cmd x12 (x0/x0) issue Data: x0 x8300
Jul  9 09:54:38 node-2 kernel: lpfc 0005:01:00.0: 159: [141401.690553] 0:(0):0307 Mailbox cmd x12 (x0/x0) Cmpl lpfc_sli_wake_mbox_wait [lpfc] Data: x1200 x20000000 x1 x1 x1 x0 x0 x0 x0 x0 x0 x50
Jul  9 09:54:38 node-2 kernel: lpfc 0005:01:00.0: 160: [141427.264823] 0:(0):0354 Mbox cmd issue - Enqueue Data: xe (x0/x0) x0 x8200 x2
Jul  9 09:54:38 node-2 kernel: lpfc 0005:01:00.0: 161: [141427.264848] 0:(0):0355 Mailbox cmd xe (x0/x0) issue Data: x0 x8300
Jul  9 09:54:38 node-2 kernel: lpfc 0005:01:00.0: 162: [141447.548392] 0:(0):0354 Mbox cmd issue - Enqueue Data: xe (x0/x0) x0 x8300 x2
Jul  9 09:54:38 node-2 kernel: lpfc 0005:01:00.0: 0:0310 Mailbox command xe timeout Data: x0 x8300 xffff30ff97ea6a00
Jul  9 09:54:38 node-2 kernel: lpfc 0005:01:00.0: 0:0345 Resetting board due to mailbox timeout
```
1. kernel启动
2. lpfc driver开始打印 Mailbox相关command, command 类型有两种, `0x12`, `0xe`
   + **0x0e**: `MBX_READ_STATUS`
   + **0x12**: `MBX_READ_LNK_STAT`

3. 看起来comand分个时间段执行, 我们获取下大致时间
   ```
   # 获取启动时间戳
   ➜  ~  date -d "2024-07-07 18:37:27" +%s
   1720348647
   
   # 获取第一轮cmd 时间
   ➜  ~ date -d @$[1720348647+141367]
   2024年 07月 09日 星期二 09:53:34 CST

   # 获取第二轮cmd 时间
   ➜  ~ date -d @$[1720348647+141401]
   2024年 07月 09日 星期二 09:54:08 CST

   # 获取第三轮
   ➜  ~ date -d @$[1720348647+141447]
   2024年 07月 09日 星期二 09:54:54 CST   //WHAT 你是认真的么?
   ```

   可以看到, 第一轮是距离timeout出发大致1分钟左右, 第二轮大概距离timeout大致30s左右, 
   第三轮打印(其实就一条, 竟然比timeout 还要晚16s, 个人感觉这个地方很奇怪. 这里先忽略, 
   对后续分析影响不大

我们来看下, 各类型打印, 触发的流程:


## 打印相关流程

* 0307 Mailbox cmd x12 (x0/x0) Cmpl lpfc_sli_wake_mbox_wait [lpfc] Data:
```

lpfc_do_work
  lpfc_work_done
    lpfc_sli_handle_mb_event {
      lpfc_printf_log(
         0307 Mailbox cmd x12 (x0/x0) Cmpl lpfc_sli_wake_mbox_wait [lpfc] Data: x1200 x20000000 x1 x1 x1 x0 x0 x0 x0 x0 x0 x50
         或
         0307 Mailbox cmd xe (x0/x0) Cmpl lpfc_sli_wake_mbox_wait [lpfc] Data: xe00 x80000000 x0 x0 x0 x0 x0 x0 x0 x0 x0 x0
      }
      lpfc_sli_wake_mbox_wait
    }
```

该流程看起来是要处理 `mailbox completion from firmware`, 也就是说下发的 command已经completion了

* 0354 Mbox cmd issue - Enqueue Data:
```
fc_stat_show
lpfc_get_stats
lpfc_sli_issue_mbox
lpfc_sli_issue_mbox_s4 {
   0354 Mbox cmd issue - Enqueue Data: xe (x0/x0) x0 x8200 x2
}
```
`fc_stat_show` 相关代码
```
#define fc_host_statistic(name)                                         \
static ssize_t show_fcstat_##name(struct device *cd,                    \
                                  struct device_attribute *attr,        \
                                  char *buf)                            \
{                                                                       \
        return fc_stat_show(cd, buf,                                    \
                            offsetof(struct fc_host_statistics, name)); \
}                                                                       \
static FC_DEVICE_ATTR(host, name, S_IRUGO, show_fcstat_##name, NULL)
```
这里看起来是sysfs 的 api. 也就是入口点

* 0355 Mailbox cmd xe (x0/x0) issue Data:
```
lpfc_do_work
lpfc_work_done
lpfc_sli4_post_async_mbox
  0355 Mailbox cmd x%x (x%x/x%x) issue Data:

  mod_timer(&psli->mbox_tmo, (jiffies +
          msecs_to_jiffies(1000 * lpfc_mbox_tmo_val(phba, mboxq))));
```

这里是下发Mailbox cmd 的地方,并且 会启动timer, 超时时间如下:
```cpp
int
lpfc_mbox_tmo_val(struct lpfc_hba *phba, LPFC_MBOXQ_t *mboxq)
{ 
   ...
   return LPFC_MBOX_TMO;
   ...
}
/* Timeout for normal outstanding mbox command (Seconds) */
#define LPFC_MBOX_TMO                           30
```
`lpfc_mbox_tmo_val`前面的条件均不满足, 直接返回`LPFC_MBOX_TMO` -- 30, 也就是超时时间为30


而timer的取消则在下面的流程
```
lpfc_sli4_hba_intr_handler
  lpfc_sli4_process_eq
    lpfc_sli4_hba_handle_eqe
      lpfc_sli4_sp_handle_eqe
        ret = queue_work_on(cq->chann, phba->wq, &cq->spwork);
          lpfc_sli4_sp_process_cq
            __lpfc_sli4_sp_process_cq
              lpfc_sli4_sp_handle_mcqe
                lpfc_sli4_sp_handle_mbox_event
                  del_timer(&phba->sli.mbox_tmo);
```

可以看到正常顺序应该为:
```

 --> 通过 sysfs 触发某个attr的read 从而 issue mailbox cmd -- lpfc_sli_issue_mbox_s4
      + 唤醒worker
 --> 下发 Mailbox cmd -- lpfc_sli4_post_async_mbox (在worker里做)
      + 设置超时时间
 --> 收到中断 -- lpfc_sli4_sp_handle_mbox_event
      + 取消timer
      + 简单处理唤醒worker
 --> completion Mailbox cmd -- lpfc_sli_handle_mb_event
```

> NOTE
>
> 由于没有在博通官网找到spec文档(可能博通没有公开),所以上面的流程大部分是猜测的.

## 超时
```
Jul  9 09:54:38 node-2 kernel: lpfc 0005:01:00.0: 160: [141427.264823] 0:(0):0354 Mbox cmd issue - Enqueue Data: xe (x0/x0) x0 x8200 x2
Jul  9 09:54:38 node-2 kernel: lpfc 0005:01:00.0: 161: [141427.264848] 0:(0):0355 Mailbox cmd xe (x0/x0) issue Data: x0 x8300
Jul  9 09:54:38 node-2 kernel: lpfc 0005:01:00.0: 162: [141447.548392] 0:(0):0354 Mbox cmd issue - Enqueue Data: xe (x0/x0) x0 x8300 x2
Jul  9 09:54:38 node-2 kernel: lpfc 0005:01:00.0: 0:0310 Mailbox command xe timeout Data: x0 x8300 xffff30ff97ea6a00
Jul  9 09:54:38 node-2 kernel: lpfc 0005:01:00.0: 0:0345 Resetting board due to mailbox timeout
```
在160: 处的打印,相当于只是触发了操作`sysfs attr` 的流程, 还未启动worker来下发.

而在161, 162 处打印, 相当于已经下发了`Mailbox cmd`, 但是似乎并没有等到interrupt.
而其距timeout的时间差, 正好是30s, (09:54:08, 09:54:38). 所以这里不知为何没有收到
interrupt.

这种问题, 软硬件可能都有问题.
* 硬件: hang住了, 不回应该cmd
* 软件:
   + 没有正确下发给硬件
   + 错误处理了interrupt.

# LPFC timeout REDHAT SOLUTION

https://access.redhat.com/solutions/31990

原文如下:
There could be many resolution to this issue based on situation:

+ Completely shut down the server, then restart. A simple reboot will not clear these messages.
+ Check the HBA attached to the server and if found faulty than replace it.
+ Upgrade the HBA driver and firmware if required.


大概意思是, 彻底关闭server, 并检查服务器HBA卡

如果有必要, 更新HBA driver和 firmware.

## hostha trigger ipmi watchdog hard reset

目前来看, hostha触发的 ipmi watchdog hard reset和hba卡没有关联(可能和网络有关), 
还请 宏达帮忙看下.

另外, 07.10 日发生的reboot中并未有lpfc相关错误打印.


# 简单结论

* reboot 和lpfc timeout无关, 可能和网络有关.
* 由于没有spec文档, 并且可能涉及硬件问题, 从代码层面调研较困难, 如果之后还有该类报错,
  建议升级driver和firmware

# doc
- Emulex® Command Line Protocol (CLP)
- https://docs.broadcom.com/doc/elx-CLP-SP118-100.pdf
-
- https://docs.broadcom.com/doc/12356069
-
- Fibre Channel Standards
- https://www.broadcom.com/support/fibre-channel-networking/san-standards/fc-standards
-
-
- fc 一篇文档 非官方
- https://www.seagate.com/staticfiles/support/disc/manuals/Interface%20manuals/100293070a.pdf
-
- mailbox command
- https://comp.periphs.scsi.narkive.com/KUxy8Onq/scsi-mailbox-commands
