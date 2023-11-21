# stack
```
#6 [ffffa7f28e14bdb0] page_fault at ffffffffb3e0114e    //这个不会占用栈, 这个为整个的pt_regs + 压栈的 RIP
    [exception RIP: unknown or invalid address]
    RIP: ffff90fb502a0e80  RSP: ffffa7f28e14be60  RFLAGS: 00010246
    RAX: ffff90fb502a0e80  RBX: 00000000000019f7  RCX: 00000000ffffffff
    RDX: 0000000000000000  RSI: 00000000fffffe01  RDI: ffff90dd55711010
    RBP: ffffffffb480b140   R8: 0000000000000000   R9: 00000000000000de
    R10: 0000000000000037  R11: 0000000000000038  R12: ffff90dd55711000
    R13: ffff90dd548d9000  R14: ffff90dd7bd4f000  R15: ffff90dd548d90d8
    ORIG_RAX: ffffffffffffffff  CS: 0010  SS: 0018
    ffffa7f28e14bdb8: ffff90dd548d90d8[R15] ffff90dd7bd4f000
    ffffa7f28e14bdc8: ffff90dk548d9000 ffff90dd55711000
    ffffa7f28e14bdd8: ffffffffb480b140 00000000000019f7
    ffffa7f28e14bde8: 0000000000000038 0000000000000037
    ffffa7f28e14bdf8: 00000000000000de 0000000000000000
    ffffa7f28e14be08: ffff90fb502a0e80 00000000ffffffff
    ffffa7f28e14be18: 0000000000000000 00000000fffffe01
    ffffa7f28e14be28: ffff90dd55711010[RDI] ffffffffffffffff[ERROR CODE / ORIG_RAX]
    ffffa7f28e14be38: ffff90fb502a0e80[EIP] 0000000000000010[CS]
    ffffa7f28e14be48: 0000000000010246[EFLAGS] ffffa7f28e14be60[ESP]
    ffffa7f28e14be58: 0000000000000018[SS] ffffffffb3561d11[RIP : 压栈获得]
 #7 [ffffa7f28e14be60] css_free_rwork_fn at ffffffffb3561d11
    ffffa7f28e14be68: ffff90dd548d90d0 ffff90dd7fae2a00
    ffffa7f28e14be78: ffff90dd7faeae00 0000000000000000
    ffffa7f28e14be88: ffff90dc5bc3d080 ffff90dd548d90d8
    ffffa7f28e14be98: ffffffffb34cd5a7
 #8 [ffffa7f28e14be98] process_one_work at ffffffffb34cd5a7
    ffffa7f28e14bea0: 000000007faeae00 ffff90dd7fae2a00
    ffffa7f28e14beb0: ffff90dd7fae2a00 ffff90dc5bc3d0a8
    ffffa7f28e14bec0: ffff90dd7fae2a20 ffff90dc5bc3d080
    ffffa7f28e14bed0: ffff90dc5bc3d080 ffffffffb34cdcc0
```

* call 指令伪代码:
  ```
  //压栈
  IF OperandSize = 64
  THEN
      tempDEST := SignExtend(DEST); (* DEST is rel32 *)
      tempRIP := RIP + tempDEST;
      IF stack not large enough for a 8-byte return address
          THEN #SS(0); FI;
          Push(RIP);
      IF ShadowStackEnabled(CPL) AND DEST != 0
          ShadowStackPush8B(RIP);
      FI;
      RIP := tempRIP;
  FI;
  
  
  //处理当前RIP的逻辑
  //有点复杂,暂时不分析
  ```
* `css_free_rwork_fn`
  ```
  /usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.x86_64/./arch/x86/include/asm/atomic64_64.h: 76
  0xffffffffb3561cfa <css_free_rwork_fn+730>:     lock subq $0x1,0x10(%r12)
  0xffffffffb3561d01 <css_free_rwork_fn+737>:     jne    0xffffffffb3561aa6 <css_free_rwork_fn+134>
  
  
  /usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.x86_64/./include/linux/percpu-refcount.h: 285
  0xffffffffb3561d07 <css_free_rwork_fn+743>:     mov    0x20(%r12),%rax
  //异常地址
  0xffffffffb3561d0c <css_free_rwork_fn+748>:     call   0xffffffffb40011f0 <__x86_indirect_thunk_rax>
  
  
  /usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.x86_64/./include/linux/rcupdate.h: 738
  //调用函数返回地址
  0xffffffffb3561d11 <css_free_rwork_fn+753>:     jmp    0xffffffffb3561a7e <css_free_rwork_fn+94> 
  /usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.x86_64/kernel/cgroup/cgroup.c: 4852
  0xffffffffb3561d16 <css_free_rwork_fn+758>:     mov    %r12,%rdi
  0xffffffffb3561d19 <css_free_rwork_fn+761>:     call   0xffffffffb3563a90 <cgroup_rstat_exit>
  /usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.x86_64/kernel/cgroup/cgroup.c: 4853
  0xffffffffb3561d1e <css_free_rwork_fn+766>:     jmp    0xffffffffb3561b25 <css_free_rwork_fn+261>
  ```

* `__x86_indirect_thunk_rax`
  ```
  /usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.x86_64/arch/x86/lib/retpoline.S: 32
  0xffffffffb40011f0 <__x86_indirect_thunk_rax>:  jmp    *%rax
  0xffffffffb40011f2 <__x86_indirect_thunk_rax+2>:        nop
  0xffffffffb40011f3 <__x86_indirect_thunk_rax+3>:        nop
  0xffffffffb40011f4 <__x86_indirect_thunk_rax+4>:        nop
  
  .macro THUNK reg
          .section .text.__x86.indirect_thunk
  
  ENTRY(__x86_indirect_thunk_\reg)
          CFI_STARTPROC
          JMP_NOSPEC %\reg
          CFI_ENDPROC
  ENDPROC(__x86_indirect_thunk_\reg)
  .endm
  ```
* `mov    0x20(%r12),%rax`
  ```
  crash> rd ffff90dd55711020        //r12为 ffff90dd55711000
  ffff90dd55711020:  ffff90fb502a0e80                    ..*P....   //异常rip
  ```

  当然, 也可以通过`pt_regs.rax`获取
  ```
  RAX: ffff90fb502a0e80  RBX: 00000000000019f7  RCX: 00000000ffffffff
  ```


# dis -l css_free_rwork_fn (分析)
* part1
  ```
  /usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.x86_64/./include/linux/percpu-refcount.h: 285
  0xffffffffb3561d07 <css_free_rwork_fn+743>:     mov    0x20(%r12),%rax
  0xffffffffb3561d0c <css_free_rwork_fn+748>:     call   0xffffffffb40011f0 <__x86_indirect_thunk_rax>    <---异常 指令
  /usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.x86_64/./include/linux/rcupdate.h: 738
  0xffffffffb3561d11 <css_free_rwork_fn+753>:     jmp    0xffffffffb3561a7e <css_free_rwork_fn+94>        <---call完了之后会跳转到 +94
  ```
* +94
  ```
  /usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.x86_64/kernel/cgroup/cgroup.c: 4833
  0xffffffffb3561a7e <css_free_rwork_fn+94>:      test   %r14,%r14
  0xffffffffb3561a81 <css_free_rwork_fn+97>:      je     0xffffffffb3561a8a <css_free_rwork_fn+106>
  ```
* c code 4883
  ```cpp
  4815 static void css_free_rwork_fn(struct work_struct *work)
  4816 {
  4817         struct cgroup_subsys_state *css = container_of(to_rcu_work(work),
  4818                                 struct cgroup_subsys_state, destroy_rwork);
  4819         struct cgroup_subsys *ss = css->ss;
  4820         struct cgroup *cgrp = css->cgroup;
  4821
  4822         percpu_ref_exit(&css->refcnt);
  4823
  4824         if (ss) {
  4825                 /* css free path */
  4826                 struct cgroup_subsys_state *parent = css->parent;
  4827                 int id = css->id;
  4828
  4829                 ss->css_free(css);
  4830                 cgroup_idr_remove(&ss->css_idr, id);
  4831                 cgroup_put(cgrp);    //怀疑是这里,我们来看下相关汇编代码
  4832
  4833                 if (parent)          //跳转到这里, 那么异常代码是上面的一些代码
  4834                         css_put(parent);
  ...
  ```

# 详细分析 `css_free_rwork_fn`反汇编

## BEGIN
```
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.x86_64/kernel/cgroup/cgroup.c: 4816
0xffffffffb3561a20 <css_free_rwork_fn>: nopl   0x0(%rax,%rax,1) [FTRACE NOP]
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.x86_64/kernel/cgroup/cgroup.c: 4817
0xffffffffb3561a25 <css_free_rwork_fn+5>:       push   %r15
0xffffffffb3561a27 <css_free_rwork_fn+7>:       push   %r14
0xffffffffb3561a29 <css_free_rwork_fn+9>:       push   %r13
0xffffffffb3561a2b <css_free_rwork_fn+11>:      lea    -0xd0(%rdi),%r13
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.x86_64/kernel/cgroup/cgroup.c: 4819
0xffffffffb3561a32 <css_free_rwork_fn+18>:      push   %r12
0xffffffffb3561a34 <css_free_rwork_fn+20>:      push   %rbp
0xffffffffb3561a35 <css_free_rwork_fn+21>:      push   %rbx
0xffffffffb3561a36 <css_free_rwork_fn+22>:      mov    -0xc8(%rdi),%rbp
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.x86_64/kernel/cgroup/cgroup.c: 4820
//3. 这里赋值了rbx, 所以rbx为rdi, 为 work
0xffffffffb3561a3d <css_free_rwork_fn+29>:      mov    %rdi,%rbx
//10. r12 在这里取值, 为(rdi - 0xd0) : 经计算,这个地方为cgroup_subsys_state的首地址, 获取该部分内存
//    实际上是获取了 cgroup_subsys_state.cgroup的指针值
0xffffffffb3561a40 <css_free_rwork_fn+32>:      mov    -0xd0(%rdi),%r12
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.x86_64/kernel/cgroup/cgroup.c: 4822
0xffffffffb3561a47 <css_free_rwork_fn+39>:      lea    0x10(%r13),%rdi
0xffffffffb3561a4b <css_free_rwork_fn+43>:      call   0xffffffffb38170e0 <percpu_ref_exit>
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.x86_64/kernel/cgroup/cgroup.c: 4824
0xffffffffb3561a50 <css_free_rwork_fn+48>:      test   %rbp,%rbp
0xffffffffb3561a53 <css_free_rwork_fn+51>:      je     0xffffffffb3561ac3 <css_free_rwork_fn+163>
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.x86_64/kernel/cgroup/cgroup.c: 4826
//2. 这里赋值r14, 在看下rbx
//4. 经计算这里获取的是 cgroup_subsys_state.parent
0xffffffffb3561a55 <css_free_rwork_fn+53>:      mov    0x58(%rbx),%r14
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.x86_64/kernel/cgroup/cgroup.c: 4827
0xffffffffb3561a59 <css_free_rwork_fn+57>:      mov    -0x58(%rbx),%ebx
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.x86_64/kernel/cgroup/cgroup.c: 4829
0xffffffffb3561a5c <css_free_rwork_fn+60>:      mov    %r13,%rdi
0xffffffffb3561a5f <css_free_rwork_fn+63>:      mov    0x20(%rbp),%rax
0xffffffffb3561a63 <css_free_rwork_fn+67>:      call   0xffffffffb40011f0 <__x86_indirect_thunk_rax>
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.x86_64/kernel/cgroup/cgroup.c: 4830
//8. 这里为最终会跳转到, cgroup_idr_remove, 所以这里是4830行
0xffffffffb3561a68 <css_free_rwork_fn+72>:      lea    0xb0(%rbp),%rdi
0xffffffffb3561a6f <css_free_rwork_fn+79>:      mov    %ebx,%esi
0xffffffffb3561a71 <css_free_rwork_fn+81>:      call   0xffffffffb355adb0 <cgroup_idr_remove>

//9. 那这里是多少行? 我们需要看下 r12 是如何取值的
//11. 经计算 7c(%r12) 是获取cgroup.self.flags, 查看其是否是1
      对应代码应为
      static inline void css_put(struct cgroup_subsys_state *css)
      {
              if (!(css->flags & CSS_NO_REF)) //这里
                      percpu_ref_put(&css->refcnt);
      }
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.x86_64/./include/linux/cgroup.h: 388
0xffffffffb3561a76 <css_free_rwork_fn+86>:      testb  $0x1,0x7c(%r12)
//12. 这里表示 如果 css->flags & CSS_NO_REF 如果为真, 则ZF为0, 则不跳转, 为假, 则跳转
0xffffffffb3561a7c <css_free_rwork_fn+92>:      je     0xffffffffb3561a95 <css_free_rwork_fn+117>
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.x86_64/kernel/cgroup/cgroup.c: 4833




//1. 我们首先看下r14的赋值
//5. 那这里对比的是parent, 如果parent 是0 则跳转到 +106
//7. 经过上面分析,这里是4833行, 没毛病
0xffffffffb3561a7e <css_free_rwork_fn+94>:      test   %r14,%r14
0xffffffffb3561a81 <css_free_rwork_fn+97>:      je     0xffffffffb3561a8a <css_free_rwork_fn+106>
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.x86_64/./include/linux/cgroup.h: 388
0xffffffffb3561a83 <css_free_rwork_fn+99>:      testb  $0x1,0x7c(%r14)
0xffffffffb3561a88 <css_free_rwork_fn+104>:     je     0xffffffffb3561aa8 <css_free_rwork_fn+136>
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.x86_64/kernel/cgroup/cgroup.c: 4863
//6. 跳转到这里直接返回
0xffffffffb3561a8a <css_free_rwork_fn+106>:     pop    %rbx
0xffffffffb3561a8b <css_free_rwork_fn+107>:     pop    %rbp
0xffffffffb3561a8c <css_free_rwork_fn+108>:     pop    %r12
0xffffffffb3561a8e <css_free_rwork_fn+110>:     pop    %r13
0xffffffffb3561a90 <css_free_rwork_fn+112>:     pop    %r14
0xffffffffb3561a92 <css_free_rwork_fn+114>:     pop    %r15
0xffffffffb3561a94 <css_free_rwork_fn+116>:     ret
```

# css_free_rwork_fn+117 之后代码
```
1. 上面说道 r12为cgroup 的指针, 这里经计算为cgroup.self.refcnt.percpu_count_ptr
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.x86_64/./include/linux/compiler.h: 188
0xffffffffb3561a95 <css_free_rwork_fn+117>:     mov    0x18(%r12),%rax
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.x86_64/./include/linux/percpu-refcount.h: 161
2. 这里代码指
   static inline bool __ref_is_percpu(struct percpu_ref *ref,
                                          unsigned long __percpu **percpu_countp)
   {
      //这里是3
      if (unlikely(percpu_ptr & __PERCPU_REF_ATOMIC_DEAD))
         return false;
   }
0xffffffffb3561a9a <css_free_rwork_fn+122>:     test   $0x3,%al
3. 如果上面条件为真, 则跳转
0xffffffffb3561a9c <css_free_rwork_fn+124>:     jne    0xffffffffb3561cf5 <css_free_rwork_fn+725>
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.x86_64/./include/linux/percpu-refcount.h: 283
4. 条件为假, 则不跳转 (可以看到下面在处理 per-cpu相关的)
0xffffffffb3561aa2 <css_free_rwork_fn+130>:     decq   %gs:(%rax)
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.x86_64/./include/linux/rcupdate.h: 738
...
```

继续分析
```
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.x86_64/./include/linux/cgroup.h: 389
1. 为cgroup.self.refcnt.count
0xffffffffb3561cf5 <css_free_rwork_fn+725>:     lea    0x10(%r12),%rdi
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.x86_64/./arch/x86/include/asm/atomic64_64.h: 76
2. lock sub 1,注意sub指令,如果减为0, 则会将 ZF -> 1
0xffffffffb3561cfa <css_free_rwork_fn+730>:     lock subq $0x1,0x10(%r12)
3. 没有减为0
0xffffffffb3561d01 <css_free_rwork_fn+737>:     jne    0xffffffffb3561aa6 <css_free_rwork_fn+134>
4. 减为了0, 0x20(%r12), 为cgroup.self.refcnt.release
   上述代码为
   static inline void percpu_ref_put_many(struct percpu_ref *ref, unsigned long nr)
   {
           unsigned long __percpu *percpu_count;
   
           rcu_read_lock_sched();

           if (__ref_is_percpu(ref, &percpu_count))
                   this_cpu_sub(*percpu_count, nr);
           //该部分
           else if (unlikely(atomic_long_sub_and_test(nr, &ref->count)))
                   //这个为异常代码
                   ref->release(ref);
   
           rcu_read_unlock_sched();
   }
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.x86_64/./include/linux/percpu-refcount.h: 285
0xffffffffb3561d07 <css_free_rwork_fn+743>:     mov    0x20(%r12),%rax
0xffffffffb3561d0c <css_free_rwork_fn+748>:     call   0xffffffffb40011f0 <__x86_indirect_thunk_rax>    <---异常指令
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.x86_64/./include/linux/rcupdate.h: 738
0xffffffffb3561d11 <css_free_rwork_fn+753>:     jmp    0xffffffffb3561a7e <css_free_rwork_fn+94>
```

找到了具体的异常代码, 我们看下各个数据成员的值

# 获取cgroup_subsys_state 结构的值
```
crash> dis process_one_work
0xffffffffb34cd59f <process_one_work+415>:      mov    %rbx,%rdi
0xffffffffb34cd5a2 <process_one_work+418>:      call   0xffffffffb40011f0 <__x86_indirect_thunk_rax> <---在这里跳转, rbx为第一个参数
0xffffffffb34cd5a7 <process_one_work+423>:      nopl   0x0(%rax,%rax,1)

crash> dis css_free_rwork_fn
0xffffffffb3561a20 <css_free_rwork_fn>: nopl   0x0(%rax,%rax,1) [FTRACE NOP]
0xffffffffb3561a25 <css_free_rwork_fn+5>:       push   %r15
0xffffffffb3561a27 <css_free_rwork_fn+7>:       push   %r14
0xffffffffb3561a29 <css_free_rwork_fn+9>:       push   %r13
0xffffffffb3561a2b <css_free_rwork_fn+11>:      lea    -0xd0(%rdi),%r13
0xffffffffb3561a32 <css_free_rwork_fn+18>:      push   %r12
0xffffffffb3561a34 <css_free_rwork_fn+20>:      push   %rbp
0xffffffffb3561a35 <css_free_rwork_fn+21>:      push   %rbx         <-----在这里push rbx


crash> bt -f
 #7 [ffffa7f28e14be60] css_free_rwork_fn at ffffffffb3561d11
    ffffa7f28e14be68: ffff90dd548d90d0[rbx] ffff90dd7fae2a00
    ffffa7f28e14be78: ffff90dd7faeae00 0000000000000000
    ffffa7f28e14be88: ffff90dc5bc3d080 ffff90dd548d90d8 [r15]
    ffffa7f28e14be98: ffffffffb34cd5a7 [RIP]
 #8 [ffffa7f28e14be98] process_one_work at ffffffffb34cd5a7

crash> struct -o  cgroup_subsys_state
struct cgroup_subsys_state {
  ...
  [144] struct work_struct destroy_work;
  [208] struct rcu_work destroy_rwork;          //这个是该work, 为d0
  [296] struct cgroup_subsys_state *parent;
  ...
}

crash> struct rcu_work
struct rcu_work {
    struct work_struct work;
    struct callback_head rcu;
    struct workqueue_struct *wq;
}

crash> p (char *)(0xffff90dd548d90d0-208)
$3 = 0xffff90dd548d9000 ""
crash> struct cgroup_subsys_state 0xffff90dd548d9000
struct cgroup_subsys_state {
  cgroup = 0x0,
  ss = 0xffffffffb480b140 <memory_cgrp_subsys>,
  refcnt = {
    count = {
      counter = 0
    },
    percpu_count_ptr = 3,
    release = 0xffffffffb355a380 <css_release>,
    confirm_switch = 0x0,
    force_atomic = false,
    rcu = {
      next = 0xffff90dd55711b90,
      func = 0x0
    }
  },
  ...
```
可以看到该 `css.cgroup` 为`0x0`, 很是奇怪, 无法通过该路径进行查看, 
那么这里是为什么呢? 

该`css`是`mem_cgroup`的`css`, 是slab的内存,会在下面的流程中执行`kfree()`,
```
css_free_rwork_fn
  ss->css_free(css);  //mem_cgroup_css_free
    mem_cgroup_free
      __mem_cgroup_free
        kfree
```
我们来看下这个slab
```
crash> kmem 0xffff90dd548d9000
CACHE             OBJSIZE  ALLOCATED     TOTAL  SLABS  SSIZE  NAME
ffff90be87c06600     4096      16385     19968   2496    32k  kmalloc-4k
  SLAB              MEMORY            NODE  TOTAL  ALLOCATED  FREE
  fffffa5a7f523600  ffff90dd548d8000     0      8          7     1
  FREE / [ALLOCATED]
   ffff90dd548d9000

      PAGE         PHYSICAL      MAPPING       INDEX CNT FLAGS
fffffa5a7f523640 1fd48d9000 dead000000000400        0  0 17ffffc0000000
```
可见, 该slab 是 `kmalloc-4k`, 并且还可以得到信息, 该slab一共有8个object, 
其中7个已经allocate, 一个free, 而 我们所看的这个`mem_cgroup` 就是那个
free的object

我们在看下该slab的offset
```
crash> struct kmem_cache ffff90be87c06600
struct kmem_cache {
  cpu_slab = 0x281a0,
  flags = 1073741824,
  min_partial = 6,
  size = 4096,
  object_size = 4096,
  offset = 0, ---> offset 为0
```

可以看到offset为0, 那么这个mem_cgroup.css.cgroup的指针, 被freelist占用,
由于没有下一个freeobject, 所以, 这里赋值成0

那么现在只能通过其他寄存器看下:

上面提到r12是cgroup的首地址, 而且到异常发生时, r12都没有改变过, 我们
看下r12的值
```
R10: 0000000000000037  R11: 0000000000000038  R12: ffff90dd55711000
```

那么我们看下该cgroup的数据成员
```
crash> struct cgroup ffff90dd55711000
struct cgroup {
  self = {
    cgroup = 0xffff90dd55711000,
    ss = 0xffff90fb502a0c00,
    refcnt = {
      count = {
        counter = 0
      },
      percpu_count_ptr = 60425687527035,
      release = 0xffff90fb502a0e80,                 //这个是异常地址
      confirm_switch = 0xffff90fb502a0eaa,
      force_atomic = false,
      rcu = {
        next = 0xffff90fb502a0e00,
        func = 0x0
      }
    },
```
其数据已经乱了, kmem 查看下改地址
```
crash> kmem  ffff90dd55711000
CACHE             OBJSIZE  ALLOCATED     TOTAL  SLABS  SSIZE  NAME
ffff90be87c06600     4096      16385     19968   2496    32k  kmalloc-4k
  SLAB              MEMORY            NODE  TOTAL  ALLOCATED  FREE
  fffffa5a7f55c400  ffff90dd55710000     0      8          8     0
  FREE / [ALLOCATED]
  [ffff90dd55711000]

      PAGE         PHYSICAL      MAPPING       INDEX CNT FLAGS
fffffa5a7f55c440 1fd5711000 dead000000000400        0  0 17ffffc0000000
```

可以看到也很奇怪,该page已经free了(cnt = 0), 但是还有8个allocated的object

我们search下这个页

# search -p ffff90dd55711000
根据之前经验, 需要把当前异常栈的物理地址找到, 过滤掉该页
```
crash> kmem ffffa7f28e14bbd0
    PID: 13533
COMMAND: "kworker/11:3"
   TASK: ffff90dd5bf4bd80  [THREAD_INFO: ffff90dd5bf4bd80]
    CPU: 11
  STATE: TASK_RUNNING (PANIC)

   VMAP_AREA         VM_STRUCT                 ADDRESS RANGE                SIZE
ffff90dd5c55b960  ffff90dca481e940  ffffa7f28e148000 - ffffa7f28e14d000    20480

      PAGE         PHYSICAL      MAPPING       INDEX CNT FLAGS
fffffa5a78baba40 1e2eae9000                0        0  1 17ffffc0000000
```
search -p 的结果
```
crash> search -p ffff90dd55711000
2275d1f38: ffff90dd55711000
ba5d02418: ffff90dd55711000
1e2eae9b10: ffff90dd55711000  //为异常栈地址
1e2eae9c48: ffff90dd55711000  //为异常栈地址
1e2eae9dd0: ffff90dd55711000  //为异常栈地址
1e7c75e418: ffff90dd55711000
1fd5711000: ffff90dd55711000  //为当前异常的页 
1fd5711370: ffff90dd55711000  //为当前异常的页
```

剩余3个地址, 我们分别来看下
* 2275d1f38
  ```
  crash> kmem 2275d1f38
        PAGE         PHYSICAL      MAPPING       INDEX CNT FLAGS
  fffffa5a089d7440  2275d1000                0        0  1 17ffffc0000800 reserved
  ```
* ba5d02418
  ```
  crash> kmem ba5d02418
  CACHE             OBJSIZE  ALLOCATED     TOTAL  SLABS  SSIZE  NAME
  ffff90be87c0d400      576     101462    110488   1973    32k  radix_tree_node
    SLAB              MEMORY            NODE  TOTAL  ALLOCATED  FREE
    fffffa5a2e974000  ffff90c925d00000     0     56         46    10
    FREE / [ALLOCATED]
    [ffff90c925d02238]
  
        PAGE         PHYSICAL      MAPPING       INDEX CNT FLAGS
  fffffa5a2e974080  ba5d02000 dead000000000400 ffff90c925d03140  0 17ffffc0000000
  ```
* 1e7c75e418
  ```
  crash> kmem 1e7c75e418
  CACHE             OBJSIZE  ALLOCATED     TOTAL  SLABS  SSIZE  NAME
  ffff90de07c29800      128     115154    163080   2718     8k  kernfs_node_cache
    SLAB              MEMORY            NODE  TOTAL  ALLOCATED  FREE
    fffffa5a79f1d780  ffff90dbfc75e000     0     60          7    53
    FREE / [ALLOCATED]
    [ffff90dbfc75e3b8]
  
        PAGE         PHYSICAL      MAPPING       INDEX CNT FLAGS
  fffffa5a79f1d780 1e7c75e000 ffff90de07c29800 ffff90dbfc75e660  1 17ffffc0008100 slab,head
  ```

```
ffff90bfa75d1f20:  ffff90dd548d90d8 ffff90dd7bd4f000   ...T.......{....
ffff90bfa75d1f30:  ffff90dd548d9000 ffff90dd55711000   ...T......qU....
ffff90bfa75d1f40:  ffffffffb480b140 00000000000019f7   @...............
ffff90bfa75d1f50:  0000000000000038 0000000000000037   8.......7.......
ffff90bfa75d1f60:  00000000000000de 0000000000000000   ................
ffff90bfa75d1f70:  ffff90fb502a0e80 00000000ffffffff   ..*P............
ffff90bfa75d1f80:  0000000000000000 00000000fffffe01   ................
ffff90bfa75d1f90:  ffff90dd55711010 ffffffffffffffff   ..qU............
ffff90bfa75d1fa0:  ffff90fb502a0e80 0000000000000010   ..*P............
ffff90bfa75d1fb0:  0000000000010246 ffffa7f28e14be60   F.......`.......
ffff90bfa75d1fc0:  0000000000000018 0000000100000001   ................
```
这个跟异常栈很类似, 不知道什么原因产生的 (shadow stack ?)
# 1e7c75e418
```
crash> struct kernfs_node ffff90dbfc75e3b8
struct kernfs_node {
  count = {
    counter = 1
  },
  active = {
    counter = -2147483647
  },
  parent = 0xffff90dd7bfbb7e8,
  name = 0xffff90dc451cdb40 "sysstat-collect.service",
  ...
  {
    dir = {
      subdirs = 0,
      children = {
        rb_node = 0x0
      },
      root = 0xffff90bfa6df9a80
    },
    symlink = {
      target_kn = 0x0
    },
    attr = {
      ops = 0x0,
      open = 0x0,
      size = -122322163885440,
      notify_next = 0x0
    }
  },
  ...
  priv = 0xffff90dd55711000,
  ...
}

crash> struct kernfs_root 0xffff90bfa6df9a80
struct kernfs_root {
  kn = 0xffff90bfa7584220,
  ...
}
crash> kernfs_node 0xffff90bfa7584220
struct kernfs_node {
      ...
      children = {
        rb_node = 0xffff90bfa7584348
      },
      ...
}
```

可以看到是一个cgroup的 node, 我们来看下cgroup在
mkdir 时, priv是如何赋值的
```
cgroup_mdir {
  ...
  cgrp = cgroup_create(parent);
  ...
  kn = kernfs_create_dir(parent->kn, name, mode, cgrp);{
    kernfs_create_dir_ns {
    ...
    kn->priv = priv;
    ...
    }
  }
  ...
}
```
这里比较奇怪的是, 为什么priv 引用了一个 已经释放的页, 很明显
这个 kernfs_node还未释放, 现在来看, 很可能就是要删除这个
`kernfs_node`

查看该红黑树
```
crash> tree -t rbtree -o kernfs_node.rb -s kernfs_node.name ffff90bfa7584268
ffff90bfa7584330
  name = 0xffff90be800e38a0 "memory.usage_in_bytes",
ffff90bfa7584110
  name = 0xffff90be800e3ec0 "memory.move_charge_at_immigrate",
ffff90bfa7584b28
  name = 0xffff90be800e3500 "cgroup.sane_behavior",
ffff90bfa7585d38
  name = 0xffff90be800e3fa0 "memory.use_hierarchy",
...
```
可以看到确实是memory 层级

这里无法往下推导, 以为 kn->priv 指向的是错误的cgroup, 所以推测 可能是该cgroup
在rmdir 流程中删除该 css.

# ba5d02418
```
crash> struct xa_node ffff90c925d02238
struct xa_node {
  ...
  slots = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0xffff90dce59a7000, 0x0, 0xffff90dbcb3c5000, 0x0, 
  0xffff90dcc3d77000, 0x0, 0xffff90dce86ec000, 0xffff90dd55711000, 0xffff90dc1abd3000, 
  0xffff90dbae448000(还在用的), 0xffff90dc9b366000, 0xffff90dc5e010000(还在用的), 0xffff90dbd70ef000, 
  0xffff90dc9ff34000, 0xffff90dc01b1a000, 0xffff90dcd2f72000},
  ...
};
```

有两个还在被用

* 0xffff90dbae448000
  ```
  crash> kmem 0xffff90dbae448000
  CACHE             OBJSIZE  ALLOCATED     TOTAL  SLABS  SSIZE  NAME
  ffff90be87c06600     4096      16385     19968   2496    32k  kmalloc-4k
    SLAB              MEMORY            NODE  TOTAL  ALLOCATED  FREE
    fffffa5a78b91200  ffff90dbae448000     0      8          7     1
    FREE / [ALLOCATED]
    [ffff90dbae448000]
  
        PAGE         PHYSICAL      MAPPING       INDEX CNT FLAGS
  fffffa5a78b91200 1e2e448000 ffff90be87c06600 ffff90dbae44f000  1 17ffffc0008100 slab,head
  ```
* 0xffff90dc5e010000
  ```
  crash> kmem 0xffff90dc5e010000
  CACHE             OBJSIZE  ALLOCATED     TOTAL  SLABS  SSIZE  NAME
  ffff90be87c06600     4096      16385     19968   2496    32k  kmalloc-4k
    SLAB              MEMORY            NODE  TOTAL  ALLOCATED  FREE
    fffffa5a7b780400  ffff90dc5e010000     0      8          8     0
    FREE / [ALLOCATED]
    [ffff90dc5e010000]
  
        PAGE         PHYSICAL      MAPPING       INDEX CNT FLAGS
  fffffa5a7b780400 1ede010000 ffff90be87c06600        0  1 17ffffc0008100 slab,head
  ```
但是这依然说明不了什么, 因为 xa_node(`0xffff90c925d02238`) 这个slab 已经释放了,
所以下面的slots 有被使用的object很正常

search ffff90c925d02238
```
crash> search ffff90c925d02238
crash>
```

可以看到也search不到了

# 查看该page的内容
```
ffff90dd55711000:  ffff90dd55711000 ffff90fb502a0c00   ..qU......*P....
ffff90dd55711010:  0000000000000000 000036f4f3a3ee7b   ........{....6..

ffff90dd55711020:  ffff90fb502a0e80 ffff90fb502a0eaa   ..*P......*P....
ffff90dd55711030:  0000000000000000 ffff90fb502a0e00   ..........*P....
ffff90dd55711040:  0000000000000000 ffff90dc0ea4e048   ........H.......

ffff90dd55711050:  ffff90fb502a1080 ffff90fb502a10aa   ..*P......*P....
ffff90dd55711060:  ffff90dd55711058 ffff90fb502a1000   X.qU......*P....
ffff90dd55711070:  0000000000000000 0000000000000000   ................

ffff90dd55711080:  ffff90fb502a1280 ffff90fb502a12aa   ..*P......*P....
ffff90dd55711090:  0000000000000000 ffff90fb502a1200   ..........*P....
ffff90dd557110a0:  0000000000000000 0000000000000000   ................

ffff90dd557110b0:  ffff90fb502a1480 ffff90fb502a14aa   ..*P......*P....
ffff90dd557110c0:  0000000000000000 ffff90fb502a1400   ..........*P....
ffff90dd557110d0:  0000000000000000 0000000000000000   ................

...后面类似
```
可以看到更像是一个头部 + 一个数组, 数组成员是48 bytes

数据也是有规律
```
index 0  -  0x80 = index 3
index 3  +  0xaa = index 1
```

我们随便看其中一个页

[rd_ffff90fb502a0000_1024](rd_ffff90fb502a0000_1024.txt)
```

然后去search 其中的一个有特点的值`b526453a122ac43e`
```
crash> kmem ffff90eec0228000
      PAGE         PHYSICAL      MAPPING       INDEX CNT FLAGS
fffffa5ac5008a00 3140228000                0        0 64 57ffffc0008000 head
``` 
可以看到这个页还是有人用的

我们去`search ffff90eec0228080`, 看看有没有别的数据结构再用,
如果有的话,查看其是一个什么样的page
```
crash> search -p ffff90eec0228080
1fd5711ad0: ffff90eec0228080
crash>
```
可以看到, 仅有一个page在使用
```
crash> kmem 1fd5711ad0
CACHE             OBJSIZE  ALLOCATED     TOTAL  SLABS  SSIZE  NAME
ffff90be87c06600     4096      16385     19968   2496    32k  kmalloc-4k
  SLAB              MEMORY            NODE  TOTAL  ALLOCATED  FREE
  fffffa5a7f55c400  ffff90dd55710000     0      8          8     0
  FREE / [ALLOCATED]
  [ffff90dd55711000]

      PAGE         PHYSICAL      MAPPING       INDEX CNT FLAGS
fffffa5a7f55c440 1fd5711000 dead000000000400        0  0 17ffffc0000000
```
似曾相识, 这不就是异常的 `"cgroup ?"`

```
crash> kmem 32cfba0098
      PAGE         PHYSICAL      MAPPING       INDEX CNT FLAGS
fffffa5acb3ee800 32cfba0000                0        0 64 57ffffc0008000 head

crash> ptov 32cfba0098
VIRTUAL           PHYSICAL
ffff90f04fba0098  32cfba0098

crash> search -p ffff90f04fba1080

crash> search -p ffff90f04fba0280

crash> ???????
```
