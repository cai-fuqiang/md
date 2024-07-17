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
代码如下:
```cpp
6610 static void
6611 lpfc_nlp_release(struct kref *kref)
6612 {
6613         struct lpfc_nodelist *ndlp = container_of(kref, struct lpfc_nodelist,
6614                                                   kref);
6615         struct lpfc_vport *vport = ndlp->vport;
6616
6617         lpfc_debugfs_disc_trc(ndlp->vport, LPFC_DISC_TRC_NODE,
6618                 "node release:    did:x%x flg:x%x type:x%x",
6619                 ndlp->nlp_DID, ndlp->nlp_flag, ndlp->nlp_type);
6620
6621         lpfc_printf_vlog(vport, KERN_INFO, LOG_NODE,
6622                          "0279 %s: ndlp: x%px did %x refcnt:%d rpi:%x\n",
6623                          __func__, ndlp, ndlp->nlp_DID,
6624                          kref_read(&ndlp->kref), ndlp->nlp_rpi);    //-->打印日志
6625
6626         /* remove ndlp from action. */
6627         lpfc_cancel_retry_delay_tmo(vport, ndlp);
6628         lpfc_cleanup_node(vport, ndlp);
6629
6630         /* Not all ELS transactions have registered the RPI with the port.
6631          * In these cases the rpi usage is temporary and the node is
6632          * released when the WQE is completed.  Catch this case to free the
6633          * RPI to the pool.  Because this node is in the release path, a lock
6634          * is unnecessary.  All references are gone and the node has been
6635          * dequeued.
6636          */
6637         if (ndlp->nlp_flag & NLP_RELEASE_RPI) {
6638                 if (ndlp->nlp_rpi != LPFC_RPI_ALLOC_ERROR &&
6639                     !(ndlp->nlp_flag & (NLP_RPI_REGISTERED | NLP_UNREG_INP))) {
6640                         lpfc_sli4_free_rpi(vport->phba, ndlp->nlp_rpi);
6641                         ndlp->nlp_rpi = LPFC_RPI_ALLOC_ERROR;
6642                 }
6643         }
6644
6645         /* The node is not freed back to memory, it is released to a pool so
6646          * the node fields need to be cleaned up.
6647          */
6648         ndlp->vport = NULL;
```
看起来是一个use-after-free

## 上游patch

查找上游patch, 找到了几个patch, 发现都不太像, 分析最像的一个

```
commit 577a942df3de2666f6947bdd3a5c9e8d30073424
Author: James Smart <jsmart2021@gmail.com>
Date:   Tue Apr 12 15:19:50 2022 -0700

    scsi: lpfc: Fix null pointer dereference after failing to issue FLOGI and PLOGI
```

COMMIT MESSAGE 如下:
```
    If lpfc_issue_els_flogi() fails and returns non-zero status, the node
    reference count is decremented to trigger the release of the nodelist
    structure. However, if there is a prior registration or dev-loss-evt work
    pending, the node may be released prematurely.  When dev-loss-evt
    completes, the released node is referenced causing a use-after-free null
    pointer dereference.

    Similarly, when processing non-zero ELS PLOGI completion status in
    lpfc_cmpl_els_plogi(), the ndlp flags are checked for a transport
    registration before triggering node removal.  If dev-loss-evt work is
    pending, the node may be released prematurely and a subsequent call to
    lpfc_dev_loss_tmo_handler() results in a use after free ndlp dereference.

    Add test for pending dev-loss before decrementing the node reference count
    for FLOGI, PLOGI, PRLI, and ADISC handling.
```

这里面提到 当 `lpfc_issue_els_flogi()` 失败, 并且返回 non-zero, 会减少引用计数
并且出发nodelist release. 但是如果有 dev-loss-evt work pending , 当其complete时,
会造成 use-after-free 访问到空指针.

处理`lpfc_cmpl_els_plogi()`也有同样的问题.

其代码处理大概如下:
```diff
diff --git a/drivers/scsi/lpfc/lpfc_els.c b/drivers/scsi/lpfc/lpfc_els.c
index 59e8c6f9d30e..44ef87324b80 100644
--- a/drivers/scsi/lpfc/lpfc_els.c
+++ b/drivers/scsi/lpfc/lpfc_els.c
@@ -1534,10 +1534,13 @@ lpfc_initial_flogi(struct lpfc_vport *vport)
        /* Reset the Fabric flag, topology change may have happened */
        vport->fc_flag &= ~FC_FABRIC;
        if (lpfc_issue_els_flogi(vport, ndlp, 0)) {
-               /* This decrement of reference count to node shall kick off
-                * the release of the node.
+               /* A node reference should be retained while registered with a
+                * transport or dev-loss-evt work is pending.
+                * Otherwise, decrement node reference to trigger release.
                 */
-               lpfc_nlp_put(ndlp);
+               if (!(ndlp->fc4_xpt_flags & (SCSI_XPT_REGD | NVME_XPT_REGD)) &&
+                   !(ndlp->nlp_flag & NLP_IN_DEV_LOSS))
+                       lpfc_nlp_put(ndlp);
                return 0;
```

`NLP_IN_DEV_LOSS` 会在`lpfc_dev_loss_tmo_callbk` 置位:
```cpp
 150 void
 151 lpfc_dev_loss_tmo_callbk(struct fc_rport *rport)
 152 {
             ...
 201         evtp = &ndlp->dev_loss_evt;
             ...
 210         spin_lock_irqsave(&ndlp->lock, iflags);
 211         ndlp->nlp_flag |= NLP_IN_DEV_LOSS;  //<==== 置位NLP_IN_DEV_LOSS
             ...
 231         evtp->evt_arg1 = lpfc_nlp_get(ndlp);
 232
 233         spin_lock_irqsave(&phba->hbalock, iflags);
 234         if (evtp->evt_arg1) {
 235                 evtp->evt = LPFC_EVT_DEV_LOSS;
 236                 list_add_tail(&evtp->evt_listp, &phba->work_list);   //<===pending dev_loss_evt
 237                 lpfc_worker_wake_up(phba);
 238         }
 239         spin_unlock_irqrestore(&phba->hbalock, iflags);
 240
```
可以看到, 置位`NLP_IN_DEV_LOSS` 是在`lpfc_dev_loss_tmo_callbk()`函数中做的, 所以并不能解决这两个函数
的冲突, 起作用实际上是, 解决pending `dev_loss_evt` work(commit message 中提到的) 和`lpfc_initial_flogi`
之间的冲突问题. 

该patch中提到的函数的相关打印在dmesg 中可以找到. commit message 中提到的另一个`prior registration`
的条件, 现在在vmcore中也获取不到. 因为 `lpfc_nlp_release` 会将`nlp_flag` 置为0. 
```cpp
static void
lpfc_nlp_release(struct kref *kref)
{
        ...
        ndlp->vport = NULL;
        ndlp->nlp_state = NLP_STE_FREED_NODE;
        ndlp->nlp_flag = 0;
        ...
}
```

# REDHAT solution

https://access.redhat.com/solutions/6960815

该链接中提到的情况和咱们现在遇到的十分接近, 但是其bugzilla 是private的, 访问不了.
通过下载其提到的 `kernel-4.18.0-513.5.1.el8_9` 未找到, BUG单号 `2125577` 该版本是
GA 版本, 可能红帽这边不是以patch 合入的.

# 接来的打算
目前来看调试信息太少,无法确定触发`lpfc_nlp_release`和`lpfc_nlp_init`以及
`queue dev_loss_tmo` 的点. 如果继续调查根因, 最好能打开debugfs 然后获取下
异常现场的打印. 目前来看, 每次启动到异常重启时间不算长. 不知道是否有条件
在客户那边打开debugfs重现下
