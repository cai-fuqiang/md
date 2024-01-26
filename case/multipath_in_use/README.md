# 编写stap脚本让qemu block住,测试
```
function wait_a_long_time()
%{
        int i = 0;
        u64 start_time;
        start_time = get_jiffies_64();
        timeout_time = start_time;
        printk("[BEG]the start time is %llu\n", start_time);
        for (i = 0; i < 100; i++) {
                start_time = get_jiffies_64();
                printk("[LOOP] the start time is %llu\n", start_time);
                msleep(1000);
        }
        printk("the wait end  \n");
        set_current_state(TASK_RUNNING);
        return;
%}

probe process("/usr/libexec/qemu-kvm").function("main").return
{
        printf("wait a long time beg\n");
        wait_a_long_time();
        printf("wait a long time end\n");
}
```
1. 运行stap脚本
```
stap  -v wait_on_do_vm_stop.stap  -g --suppress-time-limits
```

2.启动关闭虚拟机
```
virsh start  fuqiang
virsh destroy fuqiang
```

3. 等待虚机shutoff, 并执行`multipath -f`
```
lt-info1 qemu+ssh://172.36.0.3/system fuqiang ; multipath -f /dev/mapper/mpatha
```
> NOTE
>
> 由于qemu阻塞住，lt-info1会卡一段时间(可能是libvirt 会交互qemu),
> 但是过一段时间后，lt-info1 会执行完成，但是此时qemu-kvm进程还在kernel侧
> 阻塞(可能是libvirt 断定qemu已经 shutoff, 不再交互qemu)

输出为:
```
Domain fuqiang: 1 CPUs  state(5)
Aug 24 17:29:12 | /dev/mapper/mpatha: map in use
```
可以看到 即使在 shutoff的状态下，qemu进程还在，此时md设备还被占用

# 停止target服务
1. 启动虚拟机
```
virsh start fuqiang
```
2.  将iscsi server端的target服务停掉
```
systemctl stop target
```
3. 关闭虚拟机
```
virsh stop fuqiang
```
4. 等待虚拟机shutoff, 并执行`multipath -f `
```
lt-info1 qemu+ssh://172.36.0.3/system fuqiang ; multipath -f /dev/mapper/mpatha
```
也出现了和上面一样的打印, lt-info1 命令会卡住，同时等该命令返回时，已经是shutoff的状态,
`multipath -f`命令，也是打印`map in use`的错误。

(但是打印没有截取，再次复现target服务出现问题)

# 补充上面复现现象
> NOTE
>
> 在虚拟机中做fio 测试容易复现问题，复现步骤如上

等待虚拟机shutoff, 并执行`multipath -f`
```
[root@node-1 wangfuqiang]# lt-info1 qemu+ssh://172.36.0.3/system fuqiang
 multipath -f /dev/mapper/mpatha
Domain fuqiang: 1 CPUs  state(5)
[root@node-1 wangfuqiang]#
[root@node-1 wangfuqiang]#
[root@node-1 wangfuqiang]#  multipath -f /dev/mapper/mpatha
Aug 25 14:53:27 | /dev/mapper/mpatha: map in use
```

查看 multipath 状态:
```
[root@node-1 qemu]# multipath -l
mpatha (360014058b4204584a424fc984ec54b05) dm-2 LIO-ORG,disk
size=50G features='1 queue_if_no_path' hwhandler='1 alua' wp=rw
`-+- policy='round-robin 0' prio=0 status=enabled
  |- 10:0:0:0 sdm 8:192 failed faulty running
  |- 11:0:0:0 sdk 8:160 failed faulty running
  `- 12:0:0:0 sdn 8:208 failed faulty running
```
查看qemu进程状态
```
[root@node-1 ~]# ps aux |grep qemu
qemu      997339 64.5  0.0      0     0 ?        D    14:49   2:23 [qemu-kvm]
[root@node-1 ~]# cat /proc/997339/stack
[<0>] __switch_to+0x6c/0x90
[<0>] exit_aio+0x104/0x120
[<0>] mmput+0x54/0x168
[<0>] vhost_dev_cleanup+0x290/0x2c0 [vhost]
[<0>] vhost_net_release+0x5c/0xd0 [vhost_net]
[<0>] __fput+0x9c/0x1d8
[<0>] ____fput+0x20/0x30
[<0>] task_work_run+0xb0/0xe0
[<0>] do_exit+0x3c0/0xa78
[<0>] do_group_exit+0x3c/0xd0
[<0>] get_signal+0x160/0x818
[<0>] do_notify_resume+0x15c/0x2f8
[<0>] work_pending+0x8/0x10
[<0>] 0xffffffffffffffff
```
可以发现，流程为 qemu 进程接收到信号，执行`do_exit`流程中`D`住

# 查看kernel 相关代码
```cpp
void exit_aio(struct mm_struct *mm)
{
        struct kioctx_table *table = rcu_dereference_raw(mm->ioctx_table);
        struct ctx_rq_wait wait;
        int i, skipped;

        if (!table)
                return;

        atomic_set(&wait.count, table->nr);
        //==========(1)=================
        init_completion(&wait.comp);

        skipped = 0;
        for (i = 0; i < table->nr; ++i) {
        //==========(2)=================
                struct kioctx *ctx =
                        rcu_dereference_protected(table->table[i], true);

                if (!ctx) {
                        skipped++;
                        continue;
                }

                /*
                ¦* We don't need to bother with munmap() here - exit_mmap(mm)
                ¦* is coming and it'll unmap everything. And we simply can't,
                ¦* this is not necessarily our ->mm.
                ¦* Since kill_ioctx() uses non-zero ->mmap_size as indicator
                ¦* that it needs to unmap the area, just set it to 0.
                ¦*/
                ctx->mmap_size = 0;
                //==========(3)=================
                kill_ioctx(mm, ctx, &wait);
        }                                                                            

        //==========(4)=================
        if (!atomic_sub_and_test(skipped, &wait.count)) {
                /* Wait until all IO for the context are done. */
                wait_for_completion(&wait.comp);
        }

        RCU_INIT_POINTER(mm->ioctx_table, NULL);
        kfree(table);
}
```

1. init completion
2. 遍历table中的每一个 kioctx
3. kill it ! kill_ioctx()的主要作用, `wake_up_all(&ctx->wait)`, 
该wait发生在`io_getevents` 流程中。同时将`wait`赋值给`ctx->rq_wait`
代码如下:
```cpp
static int kill_ioctx(struct mm_struct *mm, struct kioctx *ctx,
                ¦     struct ctx_rq_wait *wait)
{
    ...
    wake_up_all(&ctx->wait);
    ...
    ctx->rq_wait = wait;
    ...
}
```
4. 等待completion
那么该wait会在什么时候completion呢? 
```cpp
static void free_ioctx_reqs(struct percpu_ref *ref)
{
        struct kioctx *ctx = container_of(ref, struct kioctx, reqs);

        /* At this point we know that there are no any in-flight requests */
        if (ctx->rq_wait && atomic_dec_and_test(&ctx->rq_wait->count))
                complete(&ctx->rq_wait->comp);

        /* Synchronize against RCU protected table->table[] dereferences */
        INIT_RCU_WORK(&ctx->free_rwork, free_ioctx);
        queue_rcu_work(system_wq, &ctx->free_rwork);
}
```
可以看到, 在`free_ioctx_req()`的流程中，会调用到`complete`
```cpp
static struct kioctx *ioctx_alloc(unsigned nr_events)
{
    ...
    if (percpu_ref_init(&ctx->reqs, free_ioctx_reqs, 0, GFP_KERNEL))
            goto err;
    ...
}
```
在alloc时，会指定 该对象的销毁函数。那这里我们需要看下，该对象的引用计数
在何时sub.
```cpp
static void aio_complete(struct aio_kiocb *iocb, long res, long res2)
{
    ...
    //该流程最后
    percpu_ref_put(&ctx->reqs);
}
```

*综上所述，在qemu exit() 时会去等待aio返回*
