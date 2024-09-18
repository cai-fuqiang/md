
## x86 ASID

hi~ 

之前跟您面试时候, 聊过 lazy tlb, 当时您提到过, 在x86 下, 一般是
把 asid 关闭, 但是我这边看代码, 发现kernel 并没有在x86下控制asid
功能的开关. 


个人认为, x86 tlb shootdown 动作虽然代价比较高, 但是 x86 在引入asid,
之前, 先引入了lazy tlb, 而asid的实现, 也是基于lazy tlb实现的, 所以,
可以很好的减少因为引入asid, 而导致 flush remote tlb 所造成的性能损失.

举个例子:

|CPU A| CPU B|进程状态|
|---|---|---|
|进程A, 线程A1|进程B|current|
||进程A, 线程A2|not current|

如果线程A1发起flush remote tlb时, 因为lazy tlb的实现, 其不会对 CPU B,
发起tlb shootdown. 等待 线程A2调度回来时, 在flush local tlb.

具体代码如下:

## context_switch 代码 -- lazy flush local tlb

```cpp
context_switch()
{
        ...
        if (!next->mm) {

        } else {                //用户线程
                ...
                switch_mm_irqs_off(prev->active_mm, next->mm, next);
                ...
        }
        ...
}

switch_mm_irqs_off()
{
        ...
        if (prev == next) {
                ...
        } else {                //不同地址空间

                if (prev != &init_mm) {
                        VM_WARN_ON_ONCE(!cpumask_test_cpu(cpu,
                                                mm_cpumask(prev)));
                        //===(1)===
                        cpumask_clear_cpu(cpu, mm_cpumask(prev));
                }
                if (next != &init_mm)       //设置next 进程的cpumask
                        cpumask_set_cpu(cpu, mm_cpumask(next));
                //===(2)===
                next_tlb_gen = atomic64_read(&next->context.tlb_gen);
                //===(3)===
                choose_new_asid(next, next_tlb_gen, &new_asid, &need_flush);
        }
        ...
```
1. 即便是在开启了asid的情况下, 如果发生了`context_switch()`, 也还是会clear 掉
   `mm_cpumask`, 这样在`flush_tlb_multi()`时, 则不会发送给该cpu

   ```cpp
   flush_tlb_mm_range()
   {
           ...
           if (cpumask_any_but(mm_cpumask(mm), cpu) < nr_cpu_ids) {
                   flush_tlb_multi(mm_cpumask(mm), info);
           ...
   
   }
   ```
2. x86 的pcid数量是很少的(4096), 很难为每一个进程分配一个独立的asid. 目前x86 这边的做法,
   每个只使用了6个asid ( 下面说下关于为什么只用6个(自己的看法) ), 然后由[ctx_id, tlb_gen] 
   去标记每一个percpu asid.
   ```cpp
   //per cpu 变量
   __visible DEFINE_PER_CPU_ALIGNED(struct tlb_state, cpu_tlbstate) = {
           .loaded_mm = &init_mm,
           .next_asid = 1,
           .cr4 = ~0UL,    /* fail hard if we screw up cr4 shadow initialization */
   };
   //每个cpu 有6个ctx
   struct tlb_state {
           ...
           struct tlb_context ctxs[TLB_NR_DYN_ASIDS];
           ...
   };
   struct tlb_context {
           u64 ctx_id;
           u64 tlb_gen;
   };

   typedef struct {
           ...
           u64 ctx_id;
           atomic64_t tlb_gen;
           ...
   }mm_context_t;
   ```
   其中`ctx_id`用来标识每个进程, 而`tlb_gen`则用来标识"tlb version"

   其中`tlb_context`表示该cpu上的tlb 的该asid的信息. 

   而`mm_context_t`则表示当前进程实际的状态.

   举个例子:

   在tlb_flush, 在tlb flush 的时候会判断, 如果当前cpu asid的 `tlb_gen` 
   小于当前进程的`tlb version`, 而说明 cpu 上的tlb 是old的, 需要flush. 

3. 该地方会得到一个asid(`new_asid`, 并且会判断需不需要对这个asid 做tlb 
   flush(`need_flush`)
   ```cpp
   static void choose_new_asid(struct mm_struct *next, u64 next_tlb_gen,
                               u16 *new_asid, bool *need_flush)
   {
           ...
           //===(1)===
           for (asid = 0; asid < TLB_NR_DYN_ASIDS; asid++) {
                   if (this_cpu_read(cpu_tlbstate.ctxs[asid].ctx_id) !=
                       next->context.ctx_id)
                           continue;
   
                   *new_asid = asid;
           //===(1.1)===
                   *need_flush = (this_cpu_read(cpu_tlbstate.ctxs[asid].tlb_gen) <
                                  next_tlb_gen);
                   return;
           }
   
           /*
            * We don't currently own an ASID slot on this CPU.
            * Allocate a slot.
            */
           //===(2)===
           *new_asid = this_cpu_add_return(cpu_tlbstate.next_asid, 1) - 1;
           if (*new_asid >= TLB_NR_DYN_ASIDS) {
                   *new_asid = 0;
                   this_cpu_write(cpu_tlbstate.next_asid, 1);
           }
           //===(2.1)===
           *need_flush = true;
   }
   ```
   1. 这里有一个循环, 会查找next进程的`ctx_id`, 在当前cpu tlb中有没有, 如果有
      则说明, 当前cpu上有该进程的tlb, 但是由于lazy tlb的特性, 该tlb可能会是old的,
      需要在(1.1) 处, 判断`tlb_gen`, 其中`next_tlb_gen` 是当前进程的最新的`tlb 
      version`, 如果 `<`条件成立, 则说明需要flush
   2. 如果这6个asid中没有找到该进程的`ctx_id`, 则需要分配一个新的asid(占用别的进程的),
      所以也需要flush被占用进程的tlb(2.1).


上面提到, 在发起flush tlb remote 时, 似乎会 inc `mm->context->tlb_gen`, 下面是具体代码:

## inc mm->context->tlb_gen
```cpp
flush_tlb_mm_range()
{
        ...
        //在这里inc mm->context->tlb_gen
        new_tlb_gen = inc_mm_tlb_gen(mm);
        ...
}
```

## 关于为啥只用6个asid

[x86 引入 asid patch][x86 引入 asid patch] 中作者commit message 中提到:

> This is an unorthodox approach to using PCID.  x86's PCID is far too
> short to uniquely identify a process, and we can't even really
> uniquely identify a running process because there are monster
> systems with over 4096 CPUs.  To make matters worse, past attempts
> to use all 12 PCID bits have resulted in slowdowns instead of
> speedups.


以及注释中提到:

```cpp
+/*
+ * 6 because 6 should be plenty and struct tlb_state will fit in
+ * two cache lines.
+ */
+#define TLB_NR_DYN_ASIDS 6
```

选择6正好导致, `tlb_state`大小是128byte, 填满两个 cache line
```
crash> p sizeof(struct tlb_state)
$2 = 128
```

作者似乎发现了, 将`tlb_state->tlb_context_ctxs`变大, 似乎性能会降低,
那似乎只有 walk 该 数组的情况下, 才会踩到该问题. 我这边能找到walk
array的点 是在 `choose_new_asid()` 函数中. 其中 
[Marco Bonelli在stackoverflow上的回答感觉也是这意思][why use 6 asid].

> This was implemented using a global per-CPU array 
> (cpu_tlbstate.ctxs) that is linearly scanned on each mm-switch. 
> Even small values of TLB_NR_DYN_ASIDS can easily trash 
> performance instead of improving it. 


## 总结

个人感觉, x86 asid 在基于lazy tlb的实现下, 不会带来明显的性能损失, 反而
因为asid的存在, 进程切换后, tlb miss 概率会降低. 

另外, 每个cpu上asid 设置为6, 也是一种性能权衡. 会在tlb miss 减少和 
`choose_new_asid()`对于数组遍历带来的性能损失之前做平衡, 但是这块不太清楚
是否满足各种场景. 感觉不太灵活.

## PS: fedora 系统关于x86 是否使用asid 的验证
```
crash> p cpu_tlbstate:0
per_cpu(cpu_tlbstate, 0) = $3 = {
  loaded_mm = 0xffffffffab3cd980 <init_mm>,
  {
    last_user_mm = 0xffff90b0cb1e2680,
    last_user_mm_spec = 18446621687729235584
  },
  loaded_mm_asid = 1,
  next_asid = 6,
  invalidate_other = false,
  user_pcid_flush_mask = 6,
  cr4 = 3614448,
  ctxs = {{
      ctx_id = 3501,
      tlb_gen = 102078
    }, {
      ctx_id = 1,
      tlb_gen = 0
    }, {
      ctx_id = 3094,
      tlb_gen = 5585
    }, {
      ctx_id = 1006,
      tlb_gen = 23305
    }, {
      ctx_id = 211638,
      tlb_gen = 11680
    }, {
      ctx_id = 3182,
      tlb_gen = 62340
    }}
}
➜  ~ uname -r
6.5.8-200.fc38.x86_64
```
可以发现在该系统上, 是使用了 asid

> NOTE
>
> hi~ 上面仅是自己的观点, 如果有错误, 希望您帮忙指正.

[x86 引入 asid patch]: https://lore.kernel.org/all/cover.1500957502.git.luto@kernel.org/
[why use 6 asid]: https://stackoverflow.com/questions/73753466/how-does-linux-use-values-for-pcids
