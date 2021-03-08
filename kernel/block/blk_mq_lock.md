# 处理锁相关流程
## ctx锁相关流程

### 函数调用流程
```
schedule
	sched_submit_work
		blk_schedule_flush_plug
			blk_flush_plug_list
				blk_mq_flush_plug_list
					blk_mq_sched_insert_requests
						blk_mq_insert_requests
```

### 函数流程
```C/C++
void blk_mq_insert_requests(struct blk_mq_hw_ctx *hctx, struct blk_mq_ctx *ctx,
                struct list_head *list)

{
    struct request *rq;
    enum hctx_type type = hctx->type;

    /*
     * preemption doesn't flush plug list, so it's possible ctx->cpu is
     * offline now
     */
    list_for_each_entry(rq, list, queuelist) {
        BUG_ON(rq->mq_ctx != ctx);
        trace_block_rq_insert(hctx->queue, rq);
    }

    spin_lock(&ctx->lock);							//加锁
    list_splice_tail_init(list, &ctx->rq_lists[type]);
    blk_mq_hctx_mark_pending(hctx, ctx);            //标记该硬件队列，该软件队列有request处于pending的状态
    spin_unlock(&ctx->lock);						//释放锁
}

```

## hctx队列锁相关流程 实际上，主要的锁点为dispatch队列的锁

### 函数调用流程
```
blk_mq_sched_insert_requests
	blk_mq_run_hw_queue
		__blk_mq_delay_run_hw_queue
			__blk_mq_run_hw_queue
				blk_mq_sched_dispatch_requests
```

### 函数流程
1. **__blk_mq_delay_run_hw_queue**

```C/C++
static void __blk_mq_delay_run_hw_queue(struct blk_mq_hw_ctx *hctx, bool async,
                    unsigned long msecs)
{
    if (unlikely(blk_mq_hctx_stopped(hctx)))
        return;

    if (!async && !(hctx->flags & BLK_MQ_F_BLOCKING)) {
        int cpu = get_cpu();
        if (cpumask_test_cpu(cpu, hctx->cpumask)) {      	//这里可能schedule到了别的cpu
            __blk_mq_run_hw_queue(hctx);		
            put_cpu();
            return;
        }

        put_cpu();
    }

	//假如说schedule到了别的cpu, 则会将等待那个cpu的kblockd去完成
    kblockd_mod_delayed_work_on(blk_mq_hctx_next_cpu(hctx), &hctx->run_work,
                    msecs_to_jiffies(msecs));
}
```

2. **blk_mq_sched_dispatch_requests**

```C/C++
void blk_mq_sched_dispatch_requests(struct blk_mq_hw_ctx *hctx)
{
    struct request_queue *q = hctx->queue;
    struct elevator_queue *e = q->elevator;
    const bool has_sched_dispatch = e && e->type->ops.dispatch_request;
    LIST_HEAD(rq_list);

    /* RCU or SRCU read lock is needed before checking quiesced flag */
    if (unlikely(blk_mq_hctx_stopped(hctx) || blk_queue_quiesced(q)))
        return;

    hctx->run++;
    /*//如果在这个dispatch list 上有更早的条目，把他们抓取出来获取更公平的派发
     * If we have previous entries on our dispatch list, grab them first for
     * more fair dispatch.
     */
    if (!list_empty_careful(&hctx->dispatch)) {             //如果不是空
        spin_lock(&hctx->lock);
        if (!list_empty(&hctx->dispatch))
            list_splice_init(&hctx->dispatch, &rq_list);    //将他们移动到rq_list中
        spin_unlock(&hctx->lock);
    }
	...

	if (!list_empty(list)) {
        bool needs_restart;

        /*
         * If we didn't flush the entire list, we could have told
         * the driver there was more coming, but that turned out to
         * be a lie.
         */     //告诉驱动可能会有更多的请求来?
        if (q->mq_ops->commit_rqs)
            q->mq_ops->commit_rqs(hctx);

        spin_lock(&hctx->lock);
        list_splice_init(list, &hctx->dispatch);        //放入dispatch
        spin_unlock(&hctx->lock);
	...
}
```

硬件队列锁也是一个自旋锁，这个锁的颗粒很小，锁争用来自于ctx 的 dispatch
操作，而这几个ctx 在nvme驱动中，都是根据cpu亲和性映射的, **?很可能来自于
同一个NUMA**
