# tracepoint

我们以`trace/events/block.h`文件为例

```C/C++
//==================FILE:trace/events/block.h========================
#undef TRACE_SYSTEM
#define TRACE_SYSTEM block			//定义TRACE_SYSTEM，一会我们会看到它的作用

#if !defined(_TRACE_BLOCK_H) || defined(TRACE_HEADER_MULTI_READ)	//防止重复定义
#define _TRACE_BLOCK_H												//防止重复定义

#include <linux/blktrace_api.h>		//以下三个是和block相关的头文件
#include <linux/blkdev.h>
#include <linux/buffer_head.h>
#include <linux/tracepoint.h>		//作为宏第一次展开的头文件

DECLARE_EVENT_CLASS(block_buffer,	//下面开始定义各个tracepoint
	...
);
DEFINE_EVENT(block_buffer, block_touch_buffer,
	...
};	

	...

#include <trace/define_trace.h>		//包含trace/define_trace.h，作为该头文件的第二次展开

```

***

所以从上面代码来看，第一次宏定义展开是通过`<linux/tracepoint.h>`文件展开<br />

## linux/tracepint.h

1. `TRACE_EVENT()`相关定义

```C/C++
#define TRACE_EVENT(name, proto, args, struct, assign, print)   \
    DECLARE_TRACE(name, PARAMS(proto), PARAMS(args))

#define DECLARE_TRACE(name, proto, args)                \
    __DECLARE_TRACE(name, PARAMS(proto), PARAMS(args),      \
            cpu_online(raw_smp_processor_id()),     \
            PARAMS(void *__data, proto),            \
            PARAMS(__data, args))
```

2. `__DECLARE_TRACE`相关定义

```C/C++
//=======================FILE:linux/tracepoint.h=======================
#define __DECLARE_TRACE(name, proto, args, cond, data_proto, data_args) \
    extern struct tracepoint __tracepoint_##name;           \
    static inline void trace_##name(proto)              \		//tracepoint func定义
    {                               \
        if (static_key_false(&__tracepoint_##name.key))     \
            __DO_TRACE(&__tracepoint_##name,        \
                TP_PROTO(data_proto),           \
                TP_ARGS(data_args),         \
                TP_CONDITION(cond), 0);         \
        if (IS_ENABLED(CONFIG_LOCKDEP) && (cond)) {     \
            rcu_read_lock_sched_notrace();          \
            rcu_dereference_sched(__tracepoint_##name.funcs);\
            rcu_read_unlock_sched_notrace();        \
        }                           \
    }                               \
	__DECLARE_TRACE_RCU(name, PARAMS(proto), PARAMS(args),      \
	    PARAMS(cond), PARAMS(data_proto), PARAMS(data_args))    \
	static inline int                       \
	register_trace_##name(void (*probe)(data_proto), void *data)    \	//tracepoint 注册回调函数
	{                               \
	    return tracepoint_probe_register(&__tracepoint_##name,  \
	                    (void *)probe, data);   \
	}                               \
	static inline int                       \
	register_trace_prio_##name(void (*probe)(data_proto), void *data,\
	               int prio)                \
	{                               \
	    return tracepoint_probe_register_prio(&__tracepoint_##name, \
	                      (void *)probe, data, prio); \
	}                               \
	static inline int                       \
	unregister_trace_##name(void (*probe)(data_proto), void *data)  \	//tracepoint反注销函数
	{                               \
	    return tracepoint_probe_unregister(&__tracepoint_##name,\
	                    (void *)probe, data);   \
	}                               \
	static inline void                      \
	check_trace_callback_type_##name(void (*cb)(data_proto))    \
	{                               \
	}                               \
	static inline bool                      \
	trace_##name##_enabled(void)                    \
	{                               \
	    return static_key_false(&__tracepoint_##name.key);  \
	}
```

可以看到`trace##name`就是我们的tracepoint点
这个函数的展开我们集中看下

```C/C++
//=======================FILE:linux/tracepoint.h=======================
//这个结构体定义使用了extern, 说明他是一个在别处定义的全局变量[?1]
extern struct tracepoint __tracepoint_##name;           \	
static inline void trace_##name(proto)              \		
{                               \
    if (static_key_false(&__tracepoint_##name.key))     \
        __DO_TRACE(&__tracepoint_##name,        \
            TP_PROTO(data_proto),           \
            TP_ARGS(data_args),         \
            TP_CONDITION(cond), 0);         \
    if (IS_ENABLED(CONFIG_LOCKDEP) && (cond)) {     \
        rcu_read_lock_sched_notrace();          \
        rcu_dereference_sched(__tracepoint_##name.funcs);\
        rcu_read_unlock_sched_notrace();        \
    }                           \
}
                               \
#define __DO_TRACE(tp, proto, args, cond, rcucheck)         \
    do {                                \
        struct tracepoint_func *it_func_ptr;            \
        void *it_func;                      \
        void *__data;                       \
                                    \
        if (!(cond))                        \
            return;                     \
        if (rcucheck)                       \
            rcu_irq_enter_irqson();             \
        rcu_read_lock_sched_notrace();              \
        it_func_ptr = rcu_dereference_sched((tp)->funcs);   \	//这里应该是一个rcu链表
        if (it_func_ptr) {                  \
            do {                        \
                it_func = (it_func_ptr)->func;      \
                __data = (it_func_ptr)->data;       \
                ((void(*)(proto))(it_func))(args);  \			//调用该函数
            } while ((++it_func_ptr)->func);        \
        }                           \
        rcu_read_unlock_sched_notrace();            \
        if (rcucheck)                       \
            rcu_irq_exit_irqson();              \
    } while (0)


//=======================FILE:linux/tracepoint.h=======================
```

# tracepoint 初始化流程
## 代码流程
```
start_kernel
	trace_init
		trace_event_init
```
```C/C++

//=========================FILE:kernel/trace/trace_events.c===============
void __init trace_event_init(void)
{
    event_trace_memsetup();
    init_ftrace_syscalls();
    event_trace_enable();
}

static __init int event_trace_enable(void)
{
    struct trace_array *tr = top_trace_array();
    struct trace_event_call **iter, *call;
    int ret;

    if (!tr)
        return -ENODEV;
	
	//这个和trace event段相关，相关的段请见章节trace event session
    for_each_event(iter, __start_ftrace_events, __stop_ftrace_events) {

        call = *iter;
        ret = event_init(call);
        if (!ret)
            list_add(&call->list, &ftrace_events);
    }

    /*
     * We need the top trace array to have a working set of trace
     * points at early init, before the debug files and directories
     * are created. Create the file entries now, and attach them
     * to the actual file dentries later.
     */
    __trace_early_add_events(tr);

    early_enable_events(tr, false);

    trace_printk_start_comm();

    register_event_cmds();

    register_trigger_cmds();

    return 0;
}

```

## trace event session
for_each_event实际上是从`__start_ftrace_events`-->`__stop_ftrace_events`
之间的内存进行解析，里面存放的是类似与`struct trace_event_call *[]`这样的
数组。我们来看下这两个地址的来龙去脉
相关代码为

<details>
<summary>代码部分</summary>

```C/C++
//=====================include/asm-generic/vmlinux.lds.h===============
#ifdef CONFIG_EVENT_TRACING
/*
 * 从这里可以看到__start_ftrace_events为这个段的起点，
 * __stop_ftrace_events 为这个段的终点
 */
#define FTRACE_EVENTS() . = ALIGN(8);                   \
            __start_ftrace_events = .;          \
            KEEP(*(_ftrace_events))             \
            __stop_ftrace_events = .;           \
            __start_ftrace_eval_maps = .;           \
            KEEP(*(_ftrace_eval_map))           \
            __stop_ftrace_eval_maps = .;
#else
#define FTRACE_EVENTS()
#endif
```

那么这个段是怎么和vmlinux session联系起来的呢
我们看下面的代码:
```C/C++
//================FILE:include/asm-generic/vmlinux.lds.h===============
#define INIT_DATA                           \
    KEEP(*(SORT(___kentry+*)))                  \
    *(.init.data init.data.*)                   \
    MEM_DISCARD(init.data*)                     \
    KERNEL_CTORS()                          \
    MCOUNT_REC()                            \
    *(.init.rodata .init.rodata.*)                  \
    FTRACE_EVENTS()//在这里定义             \
    TRACE_SYSCALLS()                        \
    KPROBE_BLACKLIST()                      \
    ERROR_INJECT_WHITELIST()                    \
    MEM_DISCARD(init.rodata)                    \
    CLK_OF_TABLES()                         \
    RESERVEDMEM_OF_TABLES()                     \
    TIMER_OF_TABLES()                       \
    IOMMU_OF_TABLES()                       \
    CPU_METHOD_OF_TABLES()                      \
    CPUIDLE_METHOD_OF_TABLES()                  \
    KERNEL_DTB()                            \
    IRQCHIP_OF_MATCH_TABLE()                    \
    ACPI_PROBE_TABLE(irqchip)                   \
    ACPI_PROBE_TABLE(timer)                     \
    EARLYCON_TABLE()

#define INIT_DATA_SECTION(initsetup_align)              \
	//该段被定义为.init_data段
    .init.data : AT(ADDR(.init.data) - LOAD_OFFSET) {       \
        INIT_DATA                       \
        INIT_SETUP(initsetup_align)             \
        INIT_CALLS                      \
        CON_INITCALL                        \
        SECURITY_INITCALL                   \
        INIT_RAM_FS                     \
    }


//================FILE:arch/x86/kernel/vmlinux.lds.S===============
INIT_TEXT_SECTION(PAGE_SIZE)			//.init.text
ef CONFIG_X86_64
:init
if

/*
 * Section for code used exclusively before alternatives are run. All
 * references to such code must be patched out by alternatives, normally
 * by using X86_FEATURE_ALWAYS CPU feature bit.
 *
 * See static_cpu_has() for an example.
 */
.altinstr_aux : AT(ADDR(.altinstr_aux) - LOAD_OFFSET) {
    *(.altinstr_aux)
}

INIT_DATA_SECTION(16)					//.init.data

.x86_cpu_dev.init : AT(ADDR(.x86_cpu_dev.init) - LOAD_OFFSET) {	//.x86_cpu_dev.init
    __x86_cpu_dev_start = .;
    *(.x86_cpu_dev.init)
    __x86_cpu_dev_end = .;
}
```

可以看到段的为如下层级
```
.init.data {
    sym:__start_ftrace_events 	
	._ftrace_events {
        data1,
        data2,
        data3
	}
	sym:__start_ftrace_events 	
}

NOTE: 这里的data* 为struct trace_event_call **
```
那么我们这里来简单验证下
```
# readelf -S vmlinux
...
  [17] .init.text        PROGBITS         ffffffff8270a000  01b0a000
       000000000009204f  0000000000000000  AX       0     0     16
  [18] .init.data        PROGBITS         ffffffff8279d000  01b9d000
       00000000001e8530  0000000000000000  WA       0     0     4096
  [19] .x86_cpu_dev.init PROGBITS         ffffffff82985530  01d85530
...
```

可以看到.init.data段在.init.text段和.x86_cpu_dev.init段之间<br />
我们再看下`__start_ftrace_events`,` __stop_ftrace_events`这两个符号在哪

```
# readelf -s vmlinux|grep ftrace_events
126286: ffffffff8281fa70     0 NOTYPE  GLOBAL DEFAULT   18 __start_ftrace_events
127512: ffffffff82643650    16 OBJECT  GLOBAL DEFAULT   14 ftrace_events
138772: ffffffff82821b30     0 NOTYPE  GLOBAL DEFAULT   18 __stop_ftrace_events
```
可以看到这两个符号都在.init.data段的范围之间
</details>

## __trace_early_add_events
在执行`__trace_early_add_events`之前，实际上已经将所有的`struct trace_event_call`
挂入全局链表`ftrace_events`中.

而`__trace_early_add_events`要做的是，将`ftrace_events`中的所有的`trace_event_call`
以某种方式注册到系统中，
<!-- 有待确认
并且创建`"event_dir"`, 如下图所有的目录:
```
[root@localhost linux-4.18.0-147]# ls /sys/kernel/debug/tracing/events/
alarmtimer    context_tracking  fib           header_page  iommu        kyber    napi            page_pool  raw_syscalls  sched   sunrpc    tlb       workqueue
block         cpuhp             fib6          huge_memory  irq          libata   neigh           percpu     rcu           scsi    swiotlb   ucsi      writeback
bpf_test_run  devlink           filelock      hyperv       irq_matrix   mce      net             power      regmap        signal  syscalls  udp       x86_fpu
bridge        dma_fence         filemap       i2c          irq_vectors  mdio     nmi             printk     resctrl       skb     task      vmscan    xdp
cgroup        drm               fs_dax        initcall     kmem         migrate  oom             qdisc      rpm           smbus   tcp       vsock     xen
clk           enable            ftrace        intel_iommu  kvm          module   page_isolation  random     rseq          sock    thermal   vsyscall  xfs
compaction    exceptions        header_event  iomap        kvmmmu       msr      pagemap         ras        rtc           spi     timer     wbt       xhci-hcd
```
-->

我们来展开下代码:
<!--__trace_early_add_events BEGIN -->
<details>
<summary><code>__trace_early_add_events</code></summary>

```C/C++
/*
 * For early boot up, the top trace array requires to have
 * a list of events that can be enabled. This must be done before
 * the filesystem is set up in order to allow events to be traced
 * early.
 */
/*
 * 在启动早期，顶层trace array需要有一个events list去使能。这个需要
 * 在文件系统建立之前被做完，以便允许events能够早期被跟踪
 * 
 * NOTE: 所以从上面的注释来看，执行完该部分，实际上内核就可以使用ftrace
 * 功能了
 */
static __init void
__trace_early_add_events(struct trace_array *tr)
{
    struct trace_event_call *call;
    int ret;
	//遍历ftrace_events
    list_for_each_entry(call, &ftrace_events, list) {
        /* Early boot up should not have any modules loaded */
        if (WARN_ON_ONCE(call->mod))
            continue;
		//往tr中去注册一个trace_events
        ret = __trace_early_add_new_event(call, tr);
        if (ret < 0)
            pr_warn("Could not create early event %s\n",
                trace_event_name(call));
    }
}
```

<!--__trace_early_add_new_event BEGIN -->
<details>
<summary><code>__trace_early_add_new_event</code></summary>

```C/C++
/*
 * Just create a decriptor for early init. A descriptor is required
 * for enabling events at boot. We want to enable events before
 * the filesystem is initialized.
 */
//注释和上面函数的注释意思一致
static __init int
__trace_early_add_new_event(struct trace_event_call *call,
                struct trace_array *tr)
{
    struct trace_event_file *file;

    file = trace_create_new_event(call, tr);
    if (!file)
        return -ENOMEM;

    return 0;
}

```
这里可以看出实际上在`trace_event_call`之上还有一个数据结构，名为
`trace_event_file`

<!-- trace_event_new_event BEGIN-->
<details>
<summary><code>trace_create_new_event</code></summary>

```C/C++
static struct trace_event_file *                                  
trace_create_new_event(struct trace_event_call *call,             
               struct trace_array *tr)                            
{                                                                 
    struct trace_event_file *file;                                
                                                                  
    file = kmem_cache_alloc(file_cachep, GFP_TRACE);              
    if (!file)                                                    
        return NULL;                                              
                                                                  
    file->event_call = call;				//赋值event_call
    file->tr = tr;             				//赋值tr
    atomic_set(&file->sm_ref, 0);                                 
    atomic_set(&file->tm_ref, 0);                                 
    INIT_LIST_HEAD(&file->triggers);		//这个不知道干嘛
	//加入到tr->events中实际上这个就算是注册成功了, 因为使能
	//该events时，实际上是从tr->events中找的
    list_add(&file->list, &tr->events);
                                                                  
    return file;                                                  
}
```
</details>
<!-- trace_event_new_event END-->

</details>
<!--__trace_early_add_new_event END-->

</details>
<!--__trace_early_add_events END-->

## early_enable_events
` early_enable_events` 函数的作用是, 根据cmdline中的内容，去使能一些trace_event
我们来看下相关代码

首先我们看下和trace_event cmdline相关代码:

<!--trace_event cmdline BEGIN-->
<details>
<summary><code>trace_event cmdline</code></summary>

```C/C++
//============FILE:kernel/trace/trace_events=============
//在x86中COMMAND_LINE_SIZE长度为2048
static char bootup_event_buf[COMMAND_LINE_SIZE] __initdata;

static __init int setup_trace_event(char *str)
{
    strlcpy(bootup_event_buf, str, COMMAND_LINE_SIZE);
    ring_buffer_expanded = true;
    tracing_selftest_disabled = true;

    return 1;
}
__setup("trace_event=", setup_trace_event);
```

</details>
<!--trace_event cmdline END-->
从代码中可以看到, 在cmdline中是以`trace_event=`作为前缀
<!--early_enable_events BEGIN-->
<details>
<summary><code>early_enable_events</code></summary>

```C/C++
static __init void
early_enable_events(struct trace_array *tr, bool disable_first)
{
    char *buf = bootup_event_buf;
    char *token;
    int ret;

    while (true) {
        //在cmdline中，各个event_call是通过','分隔的
        token = strsep(&buf, ",");

        if (!token)
            break;

        if (*token) {
            /* Restarting syscalls requires that we stop them first */
            if (disable_first)          //如果是disable的话，先取消使能
                ftrace_set_clr_event(tr, token, 0);

            ret = ftrace_set_clr_event(tr, token, 1);  //使能该trace_event
            if (ret)
                pr_warn("Failed to enable trace event: %s\n", token);
        }

        /* Put back the comma to allow this to be called again */
        if (buf)
            *(buf - 1) = ',';
    }
}
```
</details>
<!--early_enable_events BEGIN-->

上面最主要的函数为`ftrace_set_clr_event`，该函数的原型是:<br/>
```C/C++
static int ftrace_set_clr_event(struct trace_array *tr, char *buf, int set)
```
作用为:使能buf字符串代表的trace events

后面的流程暂不介绍了
