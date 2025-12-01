## vmcore 堆栈分析
打开`softlockup_panic` 后，堆栈分析如下:

```
crash> bt
PID: 3257   TASK: ffff9229cb6e8000  CPU: 59  COMMAND: "modprobe"
 #0 [ffffb6c14d25ce50] panic at ffffffffbb81ec73
 #1 [ffffb6c14d25ced0] watchdog_timer_fn.cold at ffffffffbb828334
 #2 [ffffb6c14d25cf00] __run_hrtimer at ffffffffbaf82b2c
 #3 [ffffb6c14d25cf38] __hrtimer_run_queues at ffffffffbaf82ce1
 #4 [ffffb6c14d25cf78] hrtimer_interrupt at ffffffffbaf830b0
 #5 [ffffb6c14d25cfd8] __sysvec_apic_timer_interrupt at ffffffffbae5d60c
 #6 [ffffb6c14d25cff0] asm_call_sysvec_on_stack at ffffffffbba010ff
--- <IRQ stack> ---
 #7 [ffffb6c161777ac8] asm_call_sysvec_on_stack at ffffffffbba010ff
    [exception RIP: no symbolic reference]
    RIP: ffffeefdad684f80  RSP: ffff926b1a212b10  RFLAGS: ffff926b1a212b10
    RAX: ffffffffbca3b6e0  RBX: ffffb6c161777b18  RCX: ffff922b7d428e80
    RDX: ffffffffbbe5db40  RSI: ffff926b1a13fd88  RDI: ffff926b1a2104f0
    RBP: ffffb6c161777b18   R8: ffffffffbba00d02   R9: 0000000000000000
    R10: 0000000000000000  R11: ffffffffbb865882  R12: 0000000000000000
    R13: ffffffffbaf1e156  R14: ffff9229cb6e8000  R15: ffff927efeda03f0
    ORIG_RAX: ffff92648e87d698  CS: 0000  SS: 0000
bt: WARNING: possibly bogus exception frame
 #8 [ffffb6c161777b98] __audit_inode_child at ffffffffbafca5cb
 #9 [ffffb6c161777c20] __debugfs_create_file at ffffffffbb22f271
#10 [ffffb6c161777c60] blk_mq_debugfs_register_hctx at ffffffffbb31976b
#11 [ffffb6c161777cb8] blk_mq_debugfs_register at ffffffffbb319b20
#12 [ffffb6c161777ce0] blk_register_queue at ffffffffbb2dfac9
#13 [ffffb6c161777d48] __device_add_disk at ffffffffbb2f4a91
#14 [ffffb6c161777d90] nbd_dev_add at ffffffffc0572530 [nbd]
#15 [ffffb6c161777dc8] _MODULE_INIT_START_nbd at ffffffffc0445114 [nbd]
#16 [ffffb6c161777de0] do_one_initcall at ffffffffbae02961
#17 [ffffb6c161777e48] do_init_module at ffffffffbafa0bbc
#18 [ffffb6c161777e68] __se_sys_init_module at ffffffffbafa2203
#19 [ffffb6c161777f38] do_syscall_64 at ffffffffbb862d0d
#20 [ffffb6c161777f50] entry_SYSCALL_64_after_hwframe at ffffffffbba00099
    RIP: 00007f91010b264e  RSP: 00007ffeef3970b8  RFLAGS: 00000246
    RAX: ffffffffffffffda  RBX: 000055ef1cefcc90  RCX: 00007f91010b264e
    RDX: 000055ef1cefced0  RSI: 0000000000141fb0  RDI: 00007f9100434010
    RBP: 00007f9100434010   R8: 000055ef1cefcf40   R9: 00007f9100434010
    R10: 0000000000000005  R11: 0000000000000246  R12: 000055ef1cefced0
    R13: 0000000000000000  R14: 000055ef1cefcdc0  R15: 000055ef1cefcc90
    ORIG_RAX: 00000000000000af  CS: 0033  SS: 002b
```

可以看到ip 已经不是 符号地址，更像是一个page_alloc的内存地址，堆栈已经乱套了,
需要手动回溯堆栈, 我们来看下在转到中断栈之前的代码，有没有触发中断后的`pt_regs`
保存下来。

```cpp
DEFINE_IDTENTRY_SYSVEC(sysvec_apic_timer_interrupt)
{
    struct pt_regs *old_regs = set_irq_regs(regs);

    ack_APIC_irq();
    trace_local_timer_entry(LOCAL_TIMER_VECTOR);
    local_apic_timer_interrupt();
    trace_local_timer_exit(LOCAL_TIMER_VECTOR);

    set_irq_regs(old_regs);
}

static inline struct pt_regs *set_irq_regs(struct pt_regs *new_regs)
{
    struct pt_regs *old_regs;

    old_regs = __this_cpu_read(__irq_regs);
    __this_cpu_write(__irq_regs, new_regs);
    return old_regs;
}
```
中断可以嵌套，所以通过调用`set_irq_regs`设置当前正在处理的中断现场的
`pt_regs`, 并且在处理完成后，将`old_regs`设置回 `per-cpu`变量。


`old_regs`从`per-cpu`变量中获取，而`new_regs`从参数中获取，我们来看下
整个的函数调用.


```rust
#define DEFINE_IDTENTRY_SYSVEC(func)                    \
static void __##func(struct pt_regs *regs);             \
                                    \
__visible noinstr void func(struct pt_regs *regs)           \
{                                   \
    //需要注意的是，这里的regs参数，指向的原来栈的pt_regs  \
    //新的中断栈不保存pt_regs                              \
    irqentry_state_t state = irqentry_enter(regs);          \
                                    \
    instrumentation_begin();                    \
    irq_enter_rcu();                        \
    kvm_set_cpu_l1tf_flush_l1d();                   \
    run_sysvec_on_irqstack_cond(__##func, regs);            \
    irq_exit_rcu();                         \
    instrumentation_end();                      \
    irqentry_exit(regs, state);                 \
}                                   \
static __always_inline void
run_sysvec_on_irqstack_cond(void (*func)(struct pt_regs *regs),
                struct pt_regs *regs)
{
    lockdep_assert_irqs_disabled();

    if (irq_needs_irq_stack(regs))
        __run_sysvec_on_irqstack(func, regs);
    else
        func(regs);
}
static __always_inline void
__run_sysvec_on_irqstack(void (*func)(struct pt_regs *regs),
             struct pt_regs *regs)
{
    // 通过per-cpu变量获取 中断栈指针
    void *tos = __this_cpu_read(hardirq_stack_ptr);

    __this_cpu_add(irq_count, 1);
    // 栈从高向低增长，tos - 8 表示 栈底地址
    // regs: pt_regs, 这里为什么要传递pt_regs呢, 这里会将pt_regs
    // 作为参数传递给func 回调。fucntion回调可以对之前的现场
    // 做一些处理:
    //   apic_timer_interrupt 比较简单，只是调用了 set_irq_regs

    asm_call_sysvec_on_stack(tos - 8, func, regs);
    __this_cpu_sub(irq_count, 1);
}

/*
 * rdi: New stack pointer points to the top word of the stack
 *    : rdi 新的堆栈地址(中断栈)
 * rsi: Function pointer
 *    : callback
 * rdx: Function argument (can be NULL if none)
 *    : 函数参数
 */
SYM_FUNC_START(asm_call_on_stack)
SYM_INNER_LABEL(asm_call_sysvec_on_stack, SYM_L_GLOBAL)
SYM_INNER_LABEL(asm_call_irq_on_stack, SYM_L_GLOBAL)
    /*
     * Save the frame pointer unconditionally. This allows the ORC
     * unwinder to handle the stack switch.
     */
    //这里是为了构造栈帧, 通过这个栈帧，可以回溯到原来的栈
    pushq       %rbp
    mov     %rsp, %rbp

    /*
     * The unwinder relies on the word at the top of the new stack
     * page linking back to the previous RSP.
     */
    //这里将rsp保存到了新的栈中
    mov     %rsp, (%rdi)
    // 将rdi指向的地址（新栈）赋值给rsp
    mov     %rdi, %rsp
    /* Move the argument to the right place */
    // 将 rdx 作为第一个参数传递给callback
    mov     %rdx, %rdi

1:
    .pushsection .discard.instr_begin
    .long 1b - .
    .popsection

    //调用callbak
    CALL_NOSPEC rsi

2:
    .pushsection .discard.instr_end
    .long 2b - .
    .popsection

    /* Restore the previous stack pointer from RBP. */
    leaveq
    RET
SYM_FUNC_END(asm_call_on_stack)
```

根据上面的代码，我们手动回溯下堆栈:

中断栈的地址(从`hardirq_stack_ptr` 中断栈中读取):
```sh
crash> p hardirq_stack_ptr |grep '\[59'
  [59]: ffff927efed9f008

crash> rd ffff927efed9f008
ffff927efed9f008:  ffffb6c14d25d000                    ..%M....

crash> rd ffffb6c14d25cff8
ffffb6c14d25cff8:  ffffb6c161777af0 ## 保存的原来kernel 栈的 rsp的地址)
```

我们来看下发生异常时, `x86_64`硬件是如何保存现场的:

![intel_exp_stack](pic/intel_exp_stack.png)

软件保存现场则在:`PUSH_AND_CLEAR_REGS`中保存:
```cpp
DECLARE_IDTENTRY_SYSVEC(LOCAL_TIMER_VECTOR,     sysvec_apic_timer_interrupt);
|-> DECLARE_IDTENTRY
    |-> idtentry vector asm_##func func has_error_code=0
        |-> idtentry_body \cfunc \has_error_code
            |-> call    error_entry
                |-> PUSH_AND_CLEAR_REGS
```

具体代码:

<details>
<summary>PUSH_AND_CLEAR_REGS</summary>

```cpp
.macro PUSH_AND_CLEAR_REGS rdx=%rdx rax=%rax save_ret=0
    .if \save_ret
    pushq   %rsi        /* pt_regs->si */
    movq    8(%rsp), %rsi   /* temporarily store the return address in %rsi */
    movq    %rdi, 8(%rsp)   /* pt_regs->di (overwriting original return address) */
    .else
    pushq   %rdi        /* pt_regs->di */
    pushq   %rsi        /* pt_regs->si */
    .endif
    pushq   \rdx        /* pt_regs->dx */
    pushq   %rcx        /* pt_regs->cx */
    pushq   \rax        /* pt_regs->ax */
    pushq   %r8     /* pt_regs->r8 */
    pushq   %r9     /* pt_regs->r9 */
    pushq   %r10        /* pt_regs->r10 */
    pushq   %r11        /* pt_regs->r11 */
    pushq   %rbx        /* pt_regs->rbx */
    pushq   %rbp        /* pt_regs->rbp */
    pushq   %r12        /* pt_regs->r12 */
    pushq   %r13        /* pt_regs->r13 */
    pushq   %r14        /* pt_regs->r14 */
    pushq   %r15        /* pt_regs->r15 */
    UNWIND_HINT_REGS

    .if \save_ret
    pushq   %rsi        /* return address on top of stack */
    .endif

    /*
     * Sanitize registers of values that a speculation attack might
     * otherwise want to exploit. The lower registers are likely clobbered
     * well before they could be put to use in a speculative execution
     * gadget.
     */
    xorl    %edx,  %edx /* nospec dx  */
    xorl    %ecx,  %ecx /* nospec cx  */
    xorl    %r8d,  %r8d /* nospec r8  */
    xorl    %r9d,  %r9d /* nospec r9  */
    xorl    %r10d, %r10d    /* nospec r10 */
    xorl    %r11d, %r11d    /* nospec r11 */
    xorl    %ebx,  %ebx /* nospec rbx */
    xorl    %ebp,  %ebp /* nospec rbp */
    xorl    %r12d, %r12d    /* nospec r12 */
    xorl    %r13d, %r13d    /* nospec r13 */
    xorl    %r14d, %r14d    /* nospec r14 */
    xorl    %r15d, %r15d    /* nospec r15 */

.endm
```
</details>


我们手动回溯下堆栈
```
crash> rd ffffb6c161777af0 40
ffffb6c161777af0:  ffffb6c161777b18 ffffffffbb865882   .{wa.....X......
ffffb6c161777b00:  0000000000000000 0000000000000000   ................
ffffb6c161777b10:  ffffffffbba00d02 ffffffffbca3b6e0   ................
ffffb6c161777b20:  ffff922b7d428e80 ffffffffbbe5db40   ..B}+...@.......
ffffb6c161777b30:  ffff926b1a13fd88 ffff926b1a2104f0   ....k.....!.k...
ffffb6c161777b40:  ffff92648e87d698 ffffeefdad684f80   ....d....Oh.....
ffffb6c161777b50:  0000000000000000 ffff926b1a212b10   .........+!.k...
ffffb6c161777b60:  ffff926b1a212b10 0000000000000000   .+!.k...........
ffffb6c161777b70:  0000000000000001 0000000000000004   ................
ffffb6c161777b80:  ffff926b1a13fd88 ffff926b1a2104f0   ....k.....!.k...
ffffb6c161777b90:  ffffffffffffffff[error_code] ffffffffbafca5cb[eip]   ................
ffffb6c161777ba0:  0000000000000010[cs] 0000000000000297[rflags]   ................
ffffb6c161777bb0:  ffffb6c161777bc8[esp] 0000000000000018[ss]   .{wa............
ffffb6c161777bc0:  ffffffffbafca57b ffffffffbb1847a7   {........G......
ffffb6c161777bd0:  ffff926b1a13fda8 ffff92648e87d400   ....k.......d...
ffffb6c161777be0:  0000000404ea1081 ffff926b1a216f18   .........o!.k...
ffffb6c161777bf0:  ffffffffbbea1081 ffffffffbc1804b6   ................
ffffb6c161777c00:  ffff926b1a13fd88 ffffffffbbe5db40   ....k...@.......
ffffb6c161777c10:  ffffffffbbea0cd0 0000000000008100   ................
ffffb6c161777c20:  ffffffffbb22f271 ffff926b1a2104f0   q.".......!.k...

crash> struct pt_regs ffffb6c161777b18 -x
struct pt_regs {
  r15 = 0xffffffffbca3b6e0,
  r14 = 0xffff922b7d428e80,
  r13 = 0xffffffffbbe5db40,
  r12 = 0xffff926b1a13fd88,
  bp = 0xffff926b1a2104f0,
  bx = 0xffff92648e87d698,
  r11 = 0xffffeefdad684f80,
  r10 = 0x0,
  r9 = 0xffff926b1a212b10,
  r8 = 0xffff926b1a212b10,
  ax = 0x0,
  cx = 0x1,
  dx = 0x4,
  si = 0xffff926b1a13fd88,
  di = 0xffff926b1a2104f0,
  orig_ax = 0xffffffffffffffff,
  ip = 0xffffffffbafca5cb,
  cs = 0x10,
  flags = 0x297,
  sp = 0xffffb6c161777bc8,
  ss = 0x18
}
```
查看代码:
```
crash> dis -l ffffffffbafca5cb
/usr/src/debug/kernel-5.10.0-60.70.0.94.jdcloud.1.x86_64/linux-5.10.0-60.70.0.94.jdcloud.1.x86_64/kernel/auditsc.c: 2191
0xffffffffbafca5cb <__audit_inode_child+283>:   test   %rax,%rax
```

查看相关代码:
```cpp
void __audit_inode_child(struct inode *parent,
             const struct dentry *dentry,
             const unsigned char type)
{
2190     list_for_each_entry(n, &context->names_list, list) {
2191         if (!n->name ||
2192             (n->type != AUDIT_TYPE_PARENT &&
2193              n->type != AUDIT_TYPE_UNKNOWN))
2194             continue;
2195
}
```
似乎是卡在了循环里，这里怀疑是 `context->names_list`太长了.

查看该变量:
```sh
crash> bt |grep TASK
PID: 3257   TASK: ffff9229cb6e8000  CPU: 59  COMMAND: "modprobe"
crash> task_struct.audit_context ffff9228cb6e8000
  audit_context = 0xffff92648e87d400
crash> struct audit_context.names_list 0xffff92648e87d400 -o
struct audit_context {
  [ffff92648e87d698] struct list_head names_list;
}
crash> list -H ffff92648e87d698 |wc -l
348744
crash> struct audit_context.name_count 0xffff92648e87d400
  name_count = 348744
```

链表中，竟然有34w个成员!

简单查看其父进程:
```
crash> ps |grep 3257
>  3257   3167  59  ffff9229cb6e8000  RU   0.0    9088   4904  modprobe
crash> bt 3167 |grep TASK
PID: 3167   TASK: ffff926a81e7cd40  CPU: 57  COMMAND: "zbs-agent"
crash> task_struct.audit_context ffff926a81e7cd40
  audit_context = 0xffff926ffb4fcc00
crash> struct audit_context.names_list 0xffff926ffb4fcc00 -o
struct audit_context {
  [ffff926ffb4fce98] struct list_head names_list;
}
crash> list -H ffff926ffb4fce98
(empty)
```

可以发现其父进程`zbs-agent`空的.

在调用`audit_inode_child`时会判断该context是否是dummy:
```sh
audit_inode_child
|-> unlikely(!audit_dummy_context()))
      |-> void *p = audit_context();
          ## 获取 audit_context 的第一个成员dummy
          |-> return !p || *(int *)p;
    |->  __audit_inode_child
```

如果 `audit_context.dummy` 为1, 不继续执行`__audit_inode_child`

该值赋值和全局变量`audit_n_rules`有关系:
```
__audit_syscall_entry
|-> context->dummy = !audit_n_rules;
```

该值和表示当前环境中规则的数量:
```sh
audit_add_rule
|-> if (!dont_count)
    |-> audit_n_rules++;

audit_del_rule
|-> if (!dont_count)
    |-> audit_signals--;
```



而 audit赋值和 audit_n_rules

## 代码分析(ING)

part1:

```cpp
/**
 * __audit_inode_child - collect inode info for created/removed objects
 * @parent: inode of dentry parent
 * @dentry: dentry being audited
 * @type:   AUDIT_TYPE_* value that we're looking for
 *
 * For syscalls that create or remove filesystem objects, audit_inode
 * can only collect information for the filesystem object's parent.
 * This call updates the audit context with the child's information.
 * Syscalls that create a new filesystem object must be hooked after
 * the object is created.  Syscalls that remove a filesystem object
 * must be hooked prior, in order to capture the target inode during
 * unsuccessful attempts.
 *
 * 
 */
void __audit_inode_child(struct inode *parent,
             const struct dentry *dentry,
             const unsigned char type)
{
    struct audit_context *context = audit_context();
    struct inode *inode = d_backing_inode(dentry);
    const struct qstr *dname = &dentry->d_name;
    struct audit_names *n, *found_parent = NULL, *found_child = NULL;
    struct audit_entry *e;
    struct list_head *list = &audit_filter_list[AUDIT_FILTER_FS];
    int i;

    if (!context->in_syscall)
        return;

    rcu_read_lock();
    list_for_each_entry_rcu(e, list, list) {
        for (i = 0; i < e->rule.field_count; i++) {
            struct audit_field *f = &e->rule.fields[i];

            if (f->type == AUDIT_FSTYPE
                && audit_comparator(parent->i_sb->s_magic,
                        f->op, f->val)
                && e->rule.action == AUDIT_NEVER) {
                rcu_read_unlock();
                return;
            }
        }
    }
    rcu_read_unlock();

    ...
```


## 相关链接
1. [Linux内核之内核通知文件系统创建的路径：fsnotify_create用法实例(五十六)](https://blog.csdn.net/u010164190/article/details/137953091)
2. [深入 Linux 审计机制](https://nettingsisyphus.tech/2020/11/17/linux-audit-guide/)
3. [audit-documentation](https://github.com/linux-audit/audit-documentation/wiki)
4. ` [PATCH] Collect more inode information during syscall processing.`
   + 73241ccca0f7786933f1d31b3d86f2456549953a

