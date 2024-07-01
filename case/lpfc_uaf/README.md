# vmcore 分析

## fc_rport 数据结构
```
crash> bt 
...
#10 [ffff80003b5afd20] lpfc_dev_loss_tmo_callbk at ffffc2f503ac54a0 [lpfc]
#11 [ffff80003b5afd70] fc_rport_final_delete at ffffc2f501ca71c8 [scsi_transport_fc]
...

crash> dis fc_rport_final_delete
...
0xffffc2f501ca71c0 <fc_rport_final_delete+328>: mov     x0, x20
0xffffc2f501ca71c4 <fc_rport_final_delete+332>: ldr     x1, [x1,#128]
0xffffc2f501ca71c8 <fc_rport_final_delete+336>: blr     x1
...

crash> dis lpfc_dev_loss_tmo_callbk
0xffffc2f503ac5458 <lpfc_dev_loss_tmo_callbk>:  sub     sp, sp, #0x60
0xffffc2f503ac545c <lpfc_dev_loss_tmo_callbk+4>:        stp     x29, x30, [sp,#16] --> 实际上栈顶到栈底只有0x50大小
0xffffc2f503ac5460 <lpfc_dev_loss_tmo_callbk+8>:        add     x29, sp, #0x10
0xffffc2f503ac5464 <lpfc_dev_loss_tmo_callbk+12>:       stp     x19, x20, [sp,#32]

crash> bt -f
#10 [ffff80003b5afd20] lpfc_dev_loss_tmo_callbk at ffffc2f503ac54a0 [lpfc]
    ffff80003b5afd20: ffff80003b5afd70 ffffc2f501ca71cc
    ffff80003b5afd30: ffff44a351d1a000 ffff44a3ab505800 [x20]
    ffff80003b5afd40: ffff44a3ab5056c0 ffff44a3ab505900
    ffff80003b5afd50: 0000000000000000 0000000000000000
    ffff80003b5afd60: ffff44a351d1a000 ffff44a3ab505800
...
```
x20 为 fc_rport
```
crash> struct fc_rport ffff44a3ab505800
  ...
  dev_loss_tmo = 30,
  ...
  port_state = FC_PORTSTATE_DELETED,
  ...
  dev = {
  parent = 0xffff44a351d1a278,
  p = 0xffff44a34e902100,
  kobj = {
    name = 0xffff24a44750f980 "rport-13:0-0",
  ...
}
```
可以看到 port_state 为`FC_PORTSTATE_DELETED`, timeout设置为`30`


## 分析问题现场

```
-- 获取 ndlp -- lpfc_nodelist
  struct fc_rport {
     [216] void *dd_data;
  }
  crash> struct lpfc_rport_data
  struct lpfc_rport_data {
      struct lpfc_nodelist *pnode;
  }
...
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_10.aarch64/drivers/scsi/lpfc/lpfc_hbadisc.c: 159
0xffffc2f503ac5474 <lpfc_dev_loss_tmo_callbk+28>:       ldr     x0, [x20,#216]
0xffffc2f503ac5478 <lpfc_dev_loss_tmo_callbk+32>:       ldr     x19, [x0]


-- 获取 phba -- lpfc_hba 
--- 获取 vport -- lpfc_vport
  crash> struct lpfc_nodelist.vport -o
  struct lpfc_nodelist {
    [280] struct lpfc_vport *vport;
  }

0xffffc2f503ac549c <lpfc_dev_loss_tmo_callbk+68>:       ldr     x21, [x19,#280]
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_10.aarch64/drivers/scsi/lpfc/lpfc_hbadisc.c: 164

0xffffc2f503ac54a0 <lpfc_dev_loss_tmo_callbk+72>:       mov     x0, x21


--- 获取 lpfc_vport.phba
  crash> struct lpfc_vport.phba -o
  struct lpfc_vport {
       [0] struct lpfc_hba *phba;
  }
0xffffc2f503ac54a4 <lpfc_dev_loss_tmo_callbk+76>:       ldr     x23, [x21] --> 异常指令
```

可以看到 
* `x19` : `ndlp(struct lpfc_nodelist)`
* `x21` : `vport(struct lpfc_vport)`,

异常栈寄存器列表:
```
     PC: ffffc2f503ac54a4  [lpfc_dev_loss_tmo_callbk+76]
     LR: ffffc2f501ca71cc  [fc_rport_final_delete+340]
     SP: ffff80003b5afd10  PSTATE: 60c00009
    X29: ffff80003b5afd20  X28: 0000000000000000  X27: 0000000000000000
    X26: 0000000000000000  X25: ffff24a44d030000  X24: 0000000000000000
    X23: 0000000000000000  X22: ffff44a3ab505900  X21: 0000000000000000
    X20: ffff44a3ab505800  X19: ffff44a3ab4f8000  X18: 0000000000000010
    X17: 0000fffd627ce7e0  X16: ffffc2f5fd676a60  X15: 0000000000aaaaaa
    X14: 0e200e200e200e20  X13: 0000000000000001  X12: 00000000ffffffff
    X11: ffffc2f5029d0090  X10: 0000000000000001   X9: 0000000000000001
     X8: 0000000000000020   X7: 0000000000000000   X6: 0000000000000001
     X5: 0000000000000000   X4: 0000000000fffffe   X3: 000000000000ffff
     X2: ffffc2f503b4da08   X1: 0000000000000080   X0: 0000000000000000
```

查看`lpfc_nodelist`数据结构:
```
crash> struct lpfc_nodelist ffff44a3ab4f8000
struct lpfc_nodelist {
  ...
  vport = 0x0,
  ...
}
```

可以看到 为0x0,( 当然x21 也是0x0, 也就是该问题的触发点)

在 4.18.0-372 kernel 中的代码为:
```cpp
 150 void
 151 lpfc_dev_loss_tmo_callbk(struct fc_rport *rport)
 152 {
 153         struct lpfc_nodelist *ndlp;
 154         struct lpfc_vport *vport;
 155         struct lpfc_hba   *phba;
 156         struct lpfc_work_evt *evtp;
 157         unsigned long iflags;
 158
 159         ndlp = ((struct lpfc_rport_data *)rport->dd_data)->pnode;
 160         if (!ndlp)
 161                 return;
 162
 163         vport = ndlp->vport;
 164         phba  = vport->phba;    //<==== NULL HERE
```

### 分析 lpfc_nodelist 状态

通过kmem , 查看 phba 所在的内存

```
crash> kmem 0xffff44a3ab4f8000
CACHE             OBJSIZE  ALLOCATED     TOTAL  SLABS  SSIZE  NAME
ffff24a440810800      512     175605    205824   1608    64k  kmalloc-512
  SLAB              MEMORY            NODE  TOTAL  ALLOCATED  FREE
  fffffff128cad3c0  ffff44a3ab4f0000     2    128        127     1
  FREE / [ALLOCATED]
   ffff44a3ab4f8000

      PAGE         PHYSICAL      MAPPING       INDEX CNT FLAGS
fffffff128cad3c0 20206b4f0000 ffff24a440810800 ffff44a3ab4f8000  1 4fffff8000000100 slab
```

可以看到该内存已经被释放

通过dmesg 能看出来, 释放的调用点
```
[ 1381.972463] lpfc 0000:85:00.0: 130: [ 1380.412370] 4:(0):0279 lpfc_nlp_release: ndlp: xffff44a3ab4f8000 did fffffe refcnt:0 rpi:0
```

看起来是一个use-after-free

> NOTE
> ```
> [ 1381.972463] lpfc 0000:85:00.0: 130: [ 1380.412370] 4:(0):0279 lpfc_nlp_release: ndlp: xffff44a3ab4f8000 did fffffe refcnt:0 rpi:0
> [ 1403.211994] **** lpfc_rport_invalid: Null vport on ndlp xffff44a3ab4f8000, DID xfffffe rport xffff44a3ab505800 SID xffffffff
> ```
上面打印中, 在`lpfc_nlp_release`之后, 似乎
# patch
* scsi: lpfc: Early return after marking final NLP_DROPPED flag in dev_loss_tmo (Paul Ely) [RHEL-2602]

  https://github.com/torvalds/linux/commit/9c3034968ed0feeaf72e5b549b19c7767a1a04f2?diff=unified&w=0
* 

