## 初始化tag_set流程
```
nvme_dev_add
    blk_mq_alloc_tag_set
        blk_mq_alloc_rq_maps
            __blk_mq_alloc_rq_maps
                __blk_mq_alloc_rq_map
                    blk_mq_alloc_rq_map     分配并初始化tags，并且申请request指针数组和static_rqs指针数组
                    blk_mq_alloc_rqs        分配request，赋值给static_rqs指针数组成员
```

### blk_mq_alloc_tag_set
&emsp;&emsp;
    分配并初始化tag_set函数
1) 该函数首先会根据实际情况修改下面成员
```
set->nr_hw_queues           硬件队列数量
set->nr_maps                队列类型个数(eg: HCTX_TYPE_DEFAULT, HCTX_TYPE_READ, HCTX_TYPE_POLL)
set->queue_depth            队列深度(和tag_set->tag->request个数对应)
```

2) 申请struct blk_mq_tags \*数组
3) 申请blk_mq_queue_map数组
4) 调用`blk_mq_update_queue_map`建立软件队列和硬件队列之间的映射关系
5) 调用`blk_mq_alloc_rq_maps`申请tags和tags中的request

### blk_mq_update_queue_map
建立软件队列到硬件队列之间的映射关系<br />
首先map是一个软件队列到硬件队列的映射表，代表着软件队列到硬件队列的映射关系,
但是硬件队列也是分种类的，所以根据上层业务的不同需求，会进入不同的硬件队列，
这里的软件队列也是需要去根据request不同进入不同的硬件队列，所以相当于一个
软件队列映射RH_HCTX_MAX_TYPES个硬件队列。从struct blk_mq_ctx->hctxs可以看出。

其中分配映射关系的函数为
set->ops->map_queues(set)
set->ops为各个块设备驱动定义，如果没有set->ops->map_queues函数，则使用
blk_mq_map_queues()函数进行分配

上层业务选取硬件队列类型的代码为:
```
blk_mq_get_request
    blk_mq_map_queue
代码逻辑为：
    flag & REQ_HIPRI                    HCTX_TYPE_POLL
    flag & REQ_OP_MASK == REQ_OP_READ   HCTX_TYPE_READ
    default                             HCTX_TYPE_DEFAULT

|---                        struct blk_mq_tags                          ---|
|--  ... --|--        struct blk_mq_queue_map map[RH_HCTX_MAX_TYPES]     --|
           |-- map[0] --|-- map[1] --|-- ... --|--    map[nr_cpus]--     --|
          /              \
         /                \
        /                  \
       /                    \
      /                      \
     |cpu1|cpu2|cpu3|..cpumax|
        |
        |
     |硬件队列index|
```
### blk_mq_map_queues
通用的建立软件队列到硬件队列的映射关系<br />
分配要满足的一个条件是:<br />
让所有的软件队列（一个软件队列中需要映射nr_maps硬件队列）都有相应的硬件队列进行映射

首先是遍历所有的present_cpu，给他找到相应的硬件队列进行映射，当硬件队列个数不足时，
退出循环。其次再遍历所有的possible_cpu，这里可以看出，优先分配present_cpu,
而possible_cpu代表这个机器上可以容纳的最多的cpu，其中包括可以热插拔的cpu，但是cpu
并没有装上，而present_cpu则表示当前机器上的cpu个数，所以这个分配原则是，优先给不同
的present_cpu分配不同的硬件队列。遍历possible_cpu时，如果硬件队列数量够，也就是还有
未分配的硬件队列，则继续分配，如果数量不足，则表示所有的硬件队列都有软件队列映射了，
所以这时候，必须有两个软件队列映射到一个硬件队列上，所以这里的一个做法时，优先选取和当前
cpu同一个core 的超线程cpu : first sibling（可能会共享很多资源，例如: 缓存，提升一定效率。
当然，如果first sibling == cpu，则表示之前并没有同一个core的cpu分配到硬件队列资源)。

### blk_mq_alloc_rqs
分配request函数:<br />
该函数实际上是申请连续的内存页，并且在这些内存页中申请request，类似与slab,
函数的流程大概是，看看是否需要2 ^ max_order(4) 这么大的内存页，如果不需要
将order--, 一次申请2 ^ max_order(4)个内存页，如果申请失败，则将order--,
继续申请，如果申请成功，则将该page->lru 挂入tags->page_list，这样的内存页，
就脱离了内存管理系统的lru管理，之后再从内存页中申请request(大小为rq_size),
tags->static_rqs[i],指向它。
```
|--          rq_size                   --|
|-- struct request      --|-- cmd_size --|
```

## 初始化request_queue
```
nvme_alloc_ns
    blk_mq_init_queue
        blk_alloc_queue_node            分配简单初始化request_queue
        blk_mq_init_allocated_queue     主要初始化ctx hctx
            blk_mq_alloc_ctxs           分配ctxs和ctx
            blk_mq_realloc_hw_ctxs      分配hctx
            INIT_DELAYED_WORK(&q->requeue_work, blk_mq_requeue_work)    初始化requeue_work
            blk_queue_make_request(q, blk_mq_make_request)              初始化q->make_request_fn
            blk_mq_map_swqueue          建立起hctx和ctx之间的映射关系
```

### blk_mq_map_swqueue

建立起软件队列和硬件队列之间的映射关系
该函数的逻辑是，将ctx->hctxs[]数组中的成员根据`blk_mq_map_queue_type`
获取的结果赋值, 并且在硬件队列中进行标记( `cpumask_set_cpu(i, hctx->cpumask)` ),
表示该软件队列占用该硬件队列，而ctx->index_hw则表示，该软件队列在hctx中的index,
(其实就是表示了另一层的映射关系，hctx-->ctx的映射)，这个index_hw主要是用于dispatch ctx使用

### blk_mq_map_queue_type
    `q->queue_hw_ctx[q->tag_set->map[type].mq_map[cpu]];`
通过type和cpu获取ctx
