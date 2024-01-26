# 问题现象
在某些时候, 环境中会出现高负载现象,通过top查看,发现环境负载很高,
另外, cpu占用率很高, 很大部分是运行在 sys 空间.

通过 perf top 查看,发现热点函数是在 shrink_slab函数.

![问题复现截图](pic/问题复现截图.png)

#  查看代码
```cpp
static unsigned long shrink_slab(gfp_t gfp_mask, int nid,
                                 struct mem_cgroup *memcg,
                                 int priority)
{
        unsigned long ret, freed = 0;
        struct shrinker *shrinker;

        /*
         * The root memcg might be allocated even though memcg is disabled
         * via "cgroup_disable=memory" boot parameter.  This could make
         * mem_cgroup_is_root() return false, then just run memcg slab
         * shrink, but skip global shrink.  This may result in premature
         * oom.
         */
        if (!mem_cgroup_disabled() && !mem_cgroup_is_root(memcg))
                return shrink_slab_memcg(gfp_mask, nid, memcg, priority);

        ...
```
因为出问题的进程是 k8s 相关进程, 在`shrink_slab` 中, 会走到 `shrink_slab_memcg`
```cpp
static unsigned long shrink_slab_memcg(gfp_t gfp_mask, int nid,
                        struct mem_cgroup *memcg, int priority)
{
        struct shrinker_info *info;
        unsigned long ret, freed = 0;
        int i;
        //该mem_cgroup不是online的
        if (!mem_cgroup_online(memcg))
                return 0;
        //获取读者信号量, trylock, 获取不到直接返回
        if (!down_read_trylock(&shrinker_rwsem))
                return 0;

        info = shrinker_info_protected(memcg, nid);
        if (unlikely(!info))
                goto unlock;
        // shrink /ʃrɪŋk/ 收缩
        // 在map中找各个shrinker, 主要是找到id, 然后通过idr找到该shrinker
        for_each_set_bit(i, info->map, shrinker_nr_max) {
                struct shrink_control sc = {
                        .gfp_mask = gfp_mask,
                        .nid = nid,
                        .memcg = memcg,
                };
                struct shrinker *shrinker;

                shrinker = idr_find(&shrinker_idr, i);
                if (unlikely(!shrinker || !(shrinker->flags & SHRINKER_REGISTERED))) {
                        if (!shrinker)
                                clear_bit(i, info->map);
                        continue;
                }

                //因为环境cmdline设置为 cgroup.memory=nokmem, 所以memcg_kmem_enabled
                //返回假, 所以这里
                //只有 只有shrinker->flags & SHRINKER_NONSLAB 为真的时候,才不会continue
                /* Call non-slab shrinkers even though kmem is disabled */
                if (!memcg_kmem_enabled() &&
                    !(shrinker->flags & SHRINKER_NONSLAB))
                        continue;

                ret = do_shrink_slab(&sc, shrinker, priority);
```

查看代码, 发现代码中只有
```cpp
static struct shrinker deferred_split_shrinker = {
        .count_objects = deferred_split_count,
        .scan_objects = deferred_split_scan,
        .seeks = DEFAULT_SEEKS,
        .flags = SHRINKER_NUMA_AWARE | SHRINKER_MEMCG_AWARE |
                 SHRINKER_NONSLAB,
};
```
才会设置`SHRINKER_NONSLAB`

通过systemtap 调试了下,确实能抓到这个shrinker的执行, 不再展示.

# 通过self modify code的方式进行验证
我们想通过self modify code的方式, 让某些流程提前返回,以便验证那些流程出了问题.

举个例子:
```
func_a()
{
    inst1

    inst_no_effect       <===modify this to GOTO END

    inst2

end:

}
```
我们这里假设, inst_no_effect 不会有性能上的影响.
如果做了上述的改动,
改动之前能复现问题, 改动之后,不能复现问题, 就说明是 inst2 指令有问题.

## 测试module
```cpp
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/mm_types.h>

#include <linux/mm.h>
#include <asm/tlbflush.h>
#include <linux/kallsyms.h>
#include <asm/pgtable-hwdef.h>

extern struct pv_mmu_ops pv_mmu_ops;
static bool debug;

#define my_pr_info(...)                                 \
        do {                                            \
                if (debug)                              \
                        pr_info( __VA_ARGS__);  \
        } while(0)
struct page_info {
        unsigned long virt;
        pgd_t *pgd;
        p4d_t *p4d;
        pud_t *pud;
        pmd_t *pmd;
        pte_t *pte;
        pte_t org_pte;
};
static struct page_info g_page_info = {
        .virt = 0
};

static void *symbol_modify = NULL;
static struct mm_struct *init_mm_struct = NULL;
static unsigned long org_val;

pte_t *my_pte_offset_map(pmd_t *pmd, unsigned long addr)
{
        return pte_offset_kernel(pmd, addr);
}
#define get_vaild_ptr_val(ptr)  ((ptr) ? (*(unsigned long *)ptr) : 0)
static void init_page_info(unsigned long virt, struct page_info *pg_info)
{
        pg_info->virt = virt;
        pg_info->pgd = pgd_offset(init_mm_struct,  pg_info->virt);
        pg_info->p4d = p4d_offset(pg_info->pgd,           pg_info->virt);
        pg_info->pud = pud_offset(pg_info->p4d,           pg_info->virt);
        pg_info->pmd = pmd_offset(pg_info->pud,           pg_info->virt);

        pg_info->pmd = pud_trans_huge(*pg_info->pud) ? (pmd_t *)pg_info->pud :
                          pmd_offset(pg_info->pud, pg_info->virt);
        pg_info->pte = pmd_trans_huge(*pg_info->pmd) ? (pte_t *)pg_info->pmd :
                         my_pte_offset_map(pg_info->pmd,    pg_info->virt);

        pg_info->org_pte = *pg_info->pte;

        return ;
}

static void print_page_info(struct page_info *pg_info)
{
        my_pr_info("virt (%lx) \n"
                "pgd : %lx ==> %lx\n"
                "p4d : %lx ==> %lx\n"
                "pud : %lx ==> %lx\n"
                "pmd : %lx ==> %lx\n"
                "pte : %lx ==> %lx\n",
                pg_info->virt,
                (unsigned long )pg_info->pgd, get_vaild_ptr_val(pg_info->pgd),
                (unsigned long )pg_info->p4d, get_vaild_ptr_val(pg_info->p4d),
                (unsigned long )pg_info->pud, get_vaild_ptr_val(pg_info->pud),
                (unsigned long )pg_info->pmd, get_vaild_ptr_val(pg_info->pmd),
                (unsigned long )pg_info->pte, get_vaild_ptr_val(pg_info->pte));
}

#define SET_PAGE_FLAGS(pte, cond, flag)  \
        (                                \
         (pte) = cond ? (pte) | (flag) : \
                (pte) & (~(flag))        \
        )

static void modify_pgtable_flags(struct page_info *pg_info, unsigned long flags, bool enable)
{
        pte_t pte;

        pte.pte = get_vaild_ptr_val(pg_info->pte);

        SET_PAGE_FLAGS(pte.pte, enable, flags);

        set_pte(pg_info->pte, pte);
}

static void revert_pgtable_flags(struct page_info *pg_info)
{
        set_pte(pg_info->pte, pg_info->org_pte);
}


static void modify_val(void *dst, void *src, void *org, size_t size)
{
        if (!g_page_info.virt) {
                init_page_info((unsigned long )symbol_modify, &g_page_info);
        }

        my_pr_info("modify page table prev, print \n");
        print_page_info(&g_page_info);

        modify_pgtable_flags(&g_page_info, PTE_RDONLY, 0);
        my_pr_info("modify page table post, print \n");
        print_page_info(&g_page_info);
        /*
         * avoid org tlb has not RW bit, may cause kernel panic
         *
         * see
         * https://github.com/cai-fuqiang/kernel_test/tree/master/tlb_test
         */
        flush_tlb_all();

        //WRITE_ONCE(pv_mmu_ops.shrink_slab, ptr);
        smp_mb();

        memcpy(org, dst, size);
        memcpy(dst, src, size);

        revert_pgtable_flags(&g_page_info);
        flush_tlb_all();
        my_pr_info("revert page table post, print \n");
        print_page_info(&g_page_info);
}

static int __init my_init(void)
{
        //d2800000
        //unsigned long ret_inst_val = 0xd65f03c0d2800000; mov x0, #0; ret
        //unsigned long ret_inst_val = 0xd2800000d65f03c0;
        //unsigned long ret_inst_val = 0xd2800000;      //mov x0, #0 idr_find
        unsigned long ret_inst_val = 0xd280001c;        //mov x28, #0 down_read_try_lock
        //symbol_modify = (void *)kallsyms_lookup_name("shrink_slab");
        //symbol_modify = (void *)kallsyms_lookup_name("do_shrink_slab");
        //symbol_modify = (void *)kallsyms_lookup_name("do_shrink_slab");
        init_mm_struct = (void *)kallsyms_lookup_name("init_mm");
        //symbol_modify = (void *)0xffff8000102d0a10;           //idr_find
        symbol_modify = (void *)0xffff8000102d0970; //down_read_try_lock
        my_pr_info("shrink_slab(%lx)\n",
                        (unsigned long )symbol_modify);

        my_pr_info("the symbol_modify is %lx\n", (unsigned long )symbol_modify);
        my_pr_info("the init_mm is %lx\n", (unsigned long )init_mm_struct);
        modify_val(symbol_modify, &ret_inst_val, &org_val, sizeof(unsigned int));
        my_pr_info("org val (%lx)\n", org_val);

        return 0;
}

static void __exit my_exit(void)
{
        unsigned long modifyed_val = 0;
        if (!symbol_modify) {
                return;
        }

        modify_val(symbol_modify, &org_val, &modifyed_val, sizeof(unsigned int));

        my_pr_info("modify val(%lx)\n", modifyed_val);
        return;
}
module_param(debug, bool, 0);
MODULE_PARM_DESC(debug, "print debug");

module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("GPL");
```

函数比较长, 这里我们说下主要的代码流程, 因为我们要修改代码,而代码所在的段是只读的,所以
我们要修改pte, 让其不再只读.所以大致流程如下:
```
module_init()
{
    修改页表, 屏蔽只读属性
    修改指令
    修改页表, 还原只读属性
}

module_exit()
{
    修改页表, 屏蔽只读属性
    恢复指令为原本的
    修改页表, 还原只读属性
}
`
```

## 做测试

### 修改 shrink_slab, 让其在开始就返回.

> NOTE
>
> 注意, 返回的之前, 我们需要处理返回值, 而且最好在开始处,返回,避免处理堆栈.

修改后的指令如下:
```
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/mm/vmscan.c: 855
0xffff8000102d0820 <shrink_slab>:       mov     x0, #0x0                        // #0
0xffff8000102d0824 <shrink_slab+4>:     ret
```

修改后, 仍然可能复现
```

```

### 修改shrink_slab_node, 让其在开始就返回

修改指令如下:
```
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/mm/vmscan.c: 655
0xffff8000102cf460 <do_shrink_slab>:    mov     x0, #0x0                        // #0
0xffff8000102cf464 <do_shrink_slab+4>:  ret
```

修改后, 仍然复现.

说明问题代码在 `shrink_slab`->`do_shrink_slab`中间.

### down_read_trylock

在`down_read_trylock`之前没有什么执行流程, 我们直接在 `down_read_trylock`之后, 让其直接return
```
#ifdef CONFIG_MEMCG
static unsigned long shrink_slab_memcg(gfp_t gfp_mask, int nid,
                        struct mem_cgroup *memcg, int priority)
{
        struct shrinker_info *info;
        unsigned long ret, freed = 0;
        int i;

        if (!mem_cgroup_online(memcg))
                return 0;

        if (!down_read_trylock(&shrinker_rwsem))        //让其执行完
                return 0;

        info = shrinker_info_protected(memcg, nid);
        if (unlikely(!info))                            //可以修改info的值, 让其==0
                goto unlock;

```

查看汇编代码
```
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/./include/linux/memcontrol.h: 923
0xffff8000102d0948 <shrink_slab+296>:   ldr     w0, [x21,#124]
0xffff8000102d094c <shrink_slab+300>:   tbz     w0, #1, 0xffff8000102d0a9c <shrink_slab+636>
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/mm/vmscan.c: 761
0xffff8000102d0950 <shrink_slab+304>:   adrp    x20, 0xffff800011f74000 <event_oom_score_adj_update+128>
0xffff8000102d0954 <shrink_slab+308>:   add     x20, x20, #0x618
0xffff8000102d0958 <shrink_slab+312>:   mov     x0, x20         
0xffff8000102d095c <shrink_slab+316>:   bl      0xffff800010160d98 <down_read_trylock>      //调用down_read_trylock
0xffff8000102d0960 <shrink_slab+320>:   cbz     w0, 0xffff8000102d0a9c <shrink_slab+636>
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/mm/vmscan.c: 764
0xffff8000102d0964 <shrink_slab+324>:   add     x0, x21, w23, sxtw #3
0xffff8000102d0968 <shrink_slab+328>:   stp     x27, x28, [sp,#80]
0xffff8000102d096c <shrink_slab+332>:   ldr     x0, [x0,#4120]
0xffff8000102d0970 <shrink_slab+336>:   ldr     x28, [x0,#840]                              //这条指令是获取info的值
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/mm/vmscan.c: 765
0xffff8000102d0974 <shrink_slab+340>:   cbz     x28, 0xffff8000102d0aac <shrink_slab+652>   //跳转到unlock
```

我们将`shrink_slab+336`修改为`mov x28, #0`

```
0xffff8000102d096c <shrink_slab+332>:   ldr     x0, [x0,#4120]
0xffff8000102d0970 <shrink_slab+336>:   mov     x28, #0x0                       // #0       //===>修改代码
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/mm/vmscan.c: 765
0xffff8000102d0974 <shrink_slab+340>:   cbz     x28, 0xffff8000102d0aac <shrink_slab+652>
```

修改后, 问题仍然复现
```
  36.34%  [kernel]                                           [k] down_read_trylock
   6.59%  [kernel]                                           [k] shrink_node
   5.27%  [kernel]                                           [k] up_read
   3.32%  [kernel]                                           [k] finish_task_switch
   3.18%  kubelet                                            [.] 0x000000000002886c
   3.00%  containerd                                         [.] _start
   2.69%  kubelet                                            [.] 0x00000000000213e8
   2.68%  [kernel]                                           [k] mem_cgroup_iter
   2.49%  [kernel]                                           [k] shrink_lruvec
   2.03%  perf                                               [.] 0x0000000000204468
```

### 进一步验证, 我们在

```
#ifdef CONFIG_MEMCG
static unsigned long shrink_slab_memcg(gfp_t gfp_mask, int nid,
                        struct mem_cgroup *memcg, int priority)
{
        struct shrinker_info *info;
        unsigned long ret, freed = 0;
        int i;

        if (!mem_cgroup_online(memcg))      <===让这个返回值始终为0
                return 0;

        if (!down_read_trylock(&shrinker_rwsem))
                return 0;
```

查看汇编指令
```
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/./include/linux/memcontrol.h: 923
0xffff8000102d0948 <shrink_slab+296>:   ldr     w0, [x21,#124]          //===(1)===
0xffff8000102d094c <shrink_slab+300>:   tbz     w0, #1, 0xffff8000102d0a9c <shrink_slab+636>
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/mm/vmscan.c: 761
0xffff8000102d0950 <shrink_slab+304>:   adrp    x20, 0xffff800011f74000 <event_oom_score_adj_update+128>
0xffff8000102d0954 <shrink_slab+308>:   add     x20, x20, #0x618
0xffff8000102d0958 <shrink_slab+312>:   mov     x0, x20
0xffff8000102d095c <shrink_slab+316>:   bl      0xffff800010160d98 <down_read_trylock>
```
将(1)处指令修改为`mov w0, #0`即可

修改后指令如下:
```
0xffff8000102d0948 <shrink_slab+296>:   mov     w0, #0x0                        // #0
0xffff8000102d094c <shrink_slab+300>:   tbz     w0, #1, 0xffff8000102d0a9c <shrink_slab+636>
0xffff8000102d0950 <shrink_slab+304>:   adrp    x20, 0xffff800011f74000 <event_oom_score_adj_update+128>
0xffff8000102d0954 <shrink_slab+308>:   add     x20, x20, #0x618
```

也会复现问题.(15s)
