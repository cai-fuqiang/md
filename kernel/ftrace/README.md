# ftrace

## ftrace简介

### ftrace框架
<div style="text-align: center;">

![image](./ftrace框架.png) <br/>
图1. ftrace框架
</div>

从框架上来看，ftrace很简单，底层是连接了各种各样的tracer，
ftrace将tracer打印的信息，存储到ringbuffer中，然后通过
debugfs和用户态交互，用来控制tracer的行为，以及ringbuffer中
信息的获取。


ftrace实际上就是做了一个对tracer的包装，使其可以
使用ftrace框架，来操作ringbuffer记录数据，使用debugfs和用户态交互。
ftrace其实角色更像是一个中间者。来连接各个tracer和kernel的底层资源。
(很像cgroup)。

其实ftrace并不复杂，复杂的是各个类型的tracer。这里，由于了解有限，
只简单介绍一个tracer实现: **function tracer**。

### ftrace用户态使用

关于ftrace的用户态使用这里不做介绍，可以参考kernel源码中的
doc: <br />
Documentation/trace/ftrace.rst


# function tracer

## 其他知识
### mcount && ftrace
mcount是gcc的一个特性, 通过`gcc -pg`可以在function中加入一个
stub, 我们可以写个小程序测试下:

1. 编写C代码

```C/C++
int main()
{
    return 0;
}
```
2. 将C代码翻译成汇编代码

```
gcc -pg -S  main.c -o main.S
     ^
```

3. 查看`main.S`文件


<details>
<summary>main.S</summary>

```C/C++
        .file   "main.c"
        .text
        .globl  main
        .type   main, @function
main:
.LFB0:
        .cfi_startproc
        pushq   %rbp
        .cfi_def_cfa_offset 16
        .cfi_offset 6, -16
        movq    %rsp, %rbp
        .cfi_def_cfa_register 6
1:      call    mcount				//在这个地方加入了mcount插桩点
        movl    $0, %eax
        popq    %rbp
        .cfi_def_cfa 7, 8
        ret
        .cfi_endproc
.LFE0:
        .size   main, .-main
        .ident  "GCC: (GNU) 8.3.1 20191121 (Red Hat 8.3.1-5)"
        .section        .note.GNU-stack,"",@progbits
```
</details>

这里再函数开始部分执行了`call mcount`指令, 
在用户态程序中（连接了GLIBC），glibc库会实现mcount函数。

而内核中是这么做的:
* 使用`gcc -pg`在编译`.c->.o`在函数开始部分插入`call mcount`，
  （x86使用的是`call __fentry(gcc -pg -mfentry`, 这个会在函数运行时更早执行）
* 在编译阶段, 会调用`recordmount`程序扫描ELF header来找到所有`call mcount`指令的
地址，并将这些地址写入到`__mcount_loc`段, 然后覆盖之前的.o中的该段，在链接阶段，
会将所有的这些段合并到一起。在kernel初始化阶段会将这些`call mcount指令`替换成`nop`
指令
(参考Documentation/trace/ftrace.rst)

<details>
<summary>PS: x86 -mfentry编译后汇编代码:</summary>

```C/C++
        .file   "main.c"
        .text
        .globl  main
        .type   main, @function
main:
.LFB0:
        .cfi_startproc
1:      call    __fentry__
        pushq   %rbp
        .cfi_def_cfa_offset 16
        .cfi_offset 6, -16
        movq    %rsp, %rbp
        .cfi_def_cfa_register 6
        movl    $0, %eax
        popq    %rbp
        .cfi_def_cfa 7, 8
        ret
        .cfi_endproc
.LFE0:
        .size   main, .-main
        .ident  "GCC: (GNU) 8.3.1 20191121 (Red Hat 8.3.1-5)"
        .section        .note.GNU-stack,"",@progbits
```
</details>

## self modify code
其实function tracer是通过修改函数中插入的`nop`指令，使之调用
另一个function`ftrace_caller`，这里实际上涉及一个`self modify code`
问题
以arm64为例:

### icache dcache
CPU和主存之间也存在多级高速缓存，一般分为3级，分别是L1, L2和L3。
另外，我们的代码都是由2部分组成：指令和数据。L1 Cache比较特殊，
每个CPU会有2个L1 Cache。分别为指令高速缓存(Instruction Cache，
简称iCache)和数据高速缓存(Data Cache，简称dCache)。L2和L3一般
不区分指令和数据，可以同时缓存指令和数据。
<div style="text-align: center;">

![image](./basic_cache.svg)
</div>

### 为什么要区分icache dcache
iCache的作用是缓存指令，dCache是缓存数据。为什么我们需要区分数
据和指令呢？原因之一是出于性能的考量。CPU在执行程序时，可以同
时获取指令和数据，做到硬件上的并行，提升性能。另外，指令和数据
有很大的不同。例如，指令一般不会被修改，所以iCache在硬件设计上
是可以是只读的，这在一定程度上降低硬件设计的成本。所以硬件设计
上，系统中一般存在L1 dCache和L1 iCache，L2 Cache和L3 Cache。

### 一致性问题
我们的程序在执行的时候，指令一般是不会修改的。这就不会存在任何
一致性问题。但是，总有些特殊情况。例如`self-modifying code`，
这些代码在执行的时候会修改自己的指令。我们修改指令的步骤如下：

* 将需要修改的指令数据加载到dCache中
* 修改成新指令，写回dCache

实际上当我们`modify code`后, 实际上只是修改了dcache中的数据，而
内存和icache中的指令并没有改变，这样就造成了同步问题。

### 如何解决
一般来说都是由软件来维护cache的一致性, 在arm-A8手册中有详细说明:
`Example G4-1 Cache cleaning operations for self-modifying code`
详细指令为:
```
STR Rt, [Rn]
DCCMVAU Rn ; Clean data cache by MVA to point of unification (PoU)
DSB ; Ensure visibility of the data cleaned from cache
ICIMVAU Rn ; Invalidate instruction cache by MVA to PoU
BPIMVA Rn ; Invalidate branch predictor by MVA to PoU
DSB ; Ensure completion of the invalidations
ISB ; Synchronize the fetched instruction stream
```

1. dc指令clean data cache
2. dsb 确保clean data cache已经完成
3. ic指令invalidate inc cache
4. dsb确保ic指令执行完成
5. isb同步获取的指令流

## function tracer init

### 获取mcount指令

在我们设置function tracer跟踪器时，相当于会修改大部分function code中的
`nop`->`call ftrace_caller`，kernel是如何索引到这些地址的呢？实际上和
刚刚我们提到的`recordmoount`程序有关系

而其实在kernel初始化时，需要将`bl _mcount`替换成`nop`达到最小的性能损耗。
我们来看下代码:

```C/C++
//===========include/asm-generic/vmlinux.lds.h=============
#ifdef CONFIG_FTRACE_MCOUNT_RECORD
#define MCOUNT_REC()    . = ALIGN(8);               \
            __start_mcount_loc = .;         \
            KEEP(*(__mcount_loc))           \
            __stop_mcount_loc = .;

void __init ftrace_init(void)                                    
{                                                                
    extern unsigned long __start_mcount_loc[];                   
    extern unsigned long __stop_mcount_loc[];                    
    unsigned long count, flags;                                  
    int ret;                                                     
                                                                 
    local_irq_save(flags);                                       
    ret = ftrace_dyn_arch_init();                                
    local_irq_restore(flags);                                    
    if (ret)                                                     
        goto failed;                                             
                                                                 
    count = __stop_mcount_loc - __start_mcount_loc;		//这两个地址区间, 存储这所有stub的地址
    if (!count) {                                                
        pr_info("ftrace: No functions to be traced?\n");         
        goto failed;                                             
    }                                                            
                                                                 
    pr_info("ftrace: allocating %ld entries in %ld pages\n",     
        count, count / ENTRIES_PER_PAGE + 1);                    
                                                                 
    last_ftrace_enabled = ftrace_enabled = 1;                    
                                                                 
    ret = ftrace_process_locs(NULL,                              
                  __start_mcount_loc,                            
                  __stop_mcount_loc);                            
                                                                 
    set_ftrace_early_filters();                                  
                                                                 
    return;                                                      
 failed:                                                         
    ftrace_disabled = 1;                                         
}

```
`ftrace_process_locs`
该函数比较长我们来分段看

```C/C++
static int ftrace_process_locs(struct module *mod,     
                   unsigned long *start,               
                   unsigned long *end)                 
{                                                      
    struct ftrace_page *start_pg;                      
    struct ftrace_page *pg;                            
    struct dyn_ftrace *rec;                            
    unsigned long count;                               
    unsigned long *p;                                  
    unsigned long addr;                                
    unsigned long flags = 0; /* Shut up gcc */         
    int ret = -ENOMEM;                                 
                                                       
    count = end - start;                               
                                                       
    if (!count)                                        
        return 0;                                      
	
	//将这些地址做了排序                                                       
    sort(start, count, sizeof(*start),                 
         ftrace_cmp_ips, NULL);                        
 
	//首先要分配并初始化`ftrace_page` 结构体
    start_pg = ftrace_allocate_pages(count);           
    if (!start_pg)                                     
        return -ENOMEM;                                
                                                       
	...

```

我们来看下ftrace_pages的结构

```C/C++
struct ftrace_page {
    struct ftrace_page  *next;		//指向下一个ftrace_page
    struct dyn_ftrace   *records;	//指向dyn_ftrace数组
    int         index;				//当前索引值
    int         size;				//大小
};

struct dyn_ftrace {
    unsigned long       ip; /* address of mcount call-site */ //stub指令地址
    unsigned long       flags;
    struct dyn_arch_ftrace  arch;
};
```

我们再来看下ftrace_allocate_pages

```C/C++
static struct ftrace_page *
ftrace_allocate_pages(unsigned long num_to_init)
{
    struct ftrace_page *start_pg;
    struct ftrace_page *pg;
    int order;
    int cnt;

    if (!num_to_init)
        return 0;
	//分配start_pg内存
    start_pg = pg = kzalloc(sizeof(*pg), GFP_KERNEL);
    if (!pg)
        return NULL;

    /*
     * Try to allocate as much as possible in one continues
     * location that fills in all of the space. We want to
     * waste as little space as possible.
     */
    for (;;) {
        cnt = ftrace_allocate_records(pg, num_to_init);	//分配records，也就是dyn_ftrace
        if (cnt < 0)
            goto free_pages;

        num_to_init -= cnt;								//这里可能分配了一部分，还需要再分配
        if (!num_to_init)
            break;

        pg->next = kzalloc(sizeof(*pg), GFP_KERNEL);	//修改next指向
        if (!pg->next)
            goto free_pages;

        pg = pg->next;
    }

    return start_pg;

 free_pages:											//内存分配失败流程
    pg = start_pg;
    while (pg) {
        order = get_count_order(pg->size / ENTRIES_PER_PAGE);
        free_pages((unsigned long)pg->records, order);
        start_pg = pg->next;
        kfree(pg);
        pg = start_pg;
    }
    pr_info("ftrace: FAILED to allocate memory for functions\n");
    return NULL;
}
```
从上面的流程可以看出`ftrace_allocate_records`可能分配了部分，相当于可能分阶段分配
我们来看下这个函数的流程

```C/C++
static int ftrace_allocate_records(struct ftrace_page *pg, int count)
{
    int order;
    int cnt;

    if (WARN_ON(!count))
        return -EINVAL;
	//get older
    order = get_count_order(DIV_ROUND_UP(count, ENTRIES_PER_PAGE));

    /*
     * We want to fill as much as possible. No more than a page
     * may be empty.
     */
	//这里保证不能有一个以上的整页空着
    while ((PAGE_SIZE << ordee) / ENTRY_SIZE >= count + ENTRIES_PER_PAGE)
        order--;

 again:
    pg->records = (void *)__get_free_pages(GFP_KERNEL | __GFP_ZERO, order);

    if (!pg->records) {		//内存分配失败可能会导致older--
        /* if we can't allocate this size, try something smaller */
        if (!order)
            return -ENOMEM;
        order >>= 1;
        goto again;
    }

    cnt = (PAGE_SIZE << order) / ENTRY_SIZE;
    pg->size = cnt;		//设置size

    if (cnt > count)
        cnt = count;

    return cnt;
}
```

这里导致没有分配count那么多(older--)的原因主要有两个:
* 分配连续页内存时，会造成一页以上的内存空余。
* 内存不够，减小内存分配

我们接着看`ftrace_process_locs`剩下的部分:

```C/C++
static int ftrace_process_locs(struct module *mod,
                   unsigned long *start,
                   unsigned long *end)
{
...
     mutex_lock(&ftrace_lock);

     /*
      * Core and each module needs their own pages, as
      * modules will free them when they are removed.
      * Force a new page to be allocated for modules.
      */
     if (!mod) {
         WARN_ON(ftrace_pages || ftrace_pages_start);
         /* First initialization */
		//如果不是模块的话, 设置ftrace_pages_start和start_pg
         ftrace_pages = ftrace_pages_start = start_pg;	
     } else {
         if (!ftrace_pages)
             goto out;

         if (WARN_ON(ftrace_pages->next)) {
             /* Hmm, we have free pages? */
             while (ftrace_pages->next)
                 ftrace_pages = ftrace_pages->next;
         }

         ftrace_pages->next = start_pg;
     }

     p = start;
     pg = start_pg;
     while (p < end) {
         addr = ftrace_call_adjust(*p++);
         /*
          * Some architecture linkers will pad between
          * the different mcount_loc sections of different
          * object files to satisfy alignments.
          * Skip any NULL pointers.
          */
         if (!addr)
             continue;

         if (pg->index == pg->size) {		//刚刚初始化完，pg->index都是0
             /* We should have allocated enough */
             if (WARN_ON(!pg->next))
                 break;
             pg = pg->next;
         }
		 //根据__mcount_slot段中的内容，初始化dyn_ftrace 中的ip值
         rec = &pg->records[pg->index++];
         rec->ip = addr;
     }

    /* We should have used all pages */
    WARN_ON(pg->next);

    /* Assign the last page to ftrace_pages */
    ftrace_pages = pg;

    /*
     * We only need to disable interrupts on start up
     * because we are modifying code that an interrupt
     * may execute, and the modification is not atomic.
     * But for modules, nothing runs the code we modify
     * until we are finished with it, and there's no
     * reason to cause large interrupt latencies while we do it.
     */
    if (!mod)
        local_irq_save(flags);

	//去modify code, 将call _mcount ==> nop指令
    ftrace_update_code(mod, start_pg);
    if (!mod)
        local_irq_restore(flags);
    ret = 0;
 out:
    mutex_unlock(&ftrace_lock);

    return ret;
}
```

上面主要是做了两个事情:
* 更新ftrace_page->dyn_ftrace[].ip
* ftrace_update_code  将`call _mcount`指令替换为`nop`指令


我们通过上面的流程可以得到下图:
![images](./ftrace_stub.svg)

## init trace debugfs file

调用堆栈
```
tracer_init_tracefs
	ftrace_init_tracefs_toplevel
		ftrace_init_dyn_tracefs
			ftrace_create_filter_files
```

查看`ftrace_create_function_file`代码

```C/C++
int ftrace_create_function_file(struct trace_array *tr,
                 struct dentry *parent)
{
	trace_create_file("set_ftrace_filter", 0644, parent,    
          ops, &ftrace_filter_fops);                    
                                                        
	trace_create_file("set_ftrace_notrace", 0644, parent,   
	          ops, &ftrace_notrace_fops);                   
}
```

`trace_create_file` 就是ftrace为tracer提供的接口，用于
创建debugfs文件

这里就不再展开。


## set function tracer
在用户态执行
`echo function > /sys/kernel/debug/tracing/current_tracer`
会设置当前的跟踪器为`function`跟踪器。
