# 现在writeback机制提交记录
```
commit 03ba3782e8dcc5b0e1efe440d33084f066e38cae
Author: Jens Axboe <jens.axboe@oracle.com>
Date:   Wed Sep 9 09:08:54 2009 +0200

    writeback: switch to per-bdi threads for flushing data
```
## I_DIRTY_TIME 增加commit
```
commit 0ae45f63d4ef8d8eeec49c7d8b44a1775fff13e8
Author: Theodore Ts'o <tytso@mit.edu>
Date:   Mon Feb 2 00:37:00 2015 -0500

    vfs: add support for a lazytime mount option
```

# writeback work
## writeback:WB_REASON_LAPTOP_TIMER
```
blk_alloc_queue_node
    timer_setup(&q->backing_dev_info->laptop_mode_wb_timer,     
    /*-----*/laptop_mode_timer_fn, 0);   ==> laptop_mode_timer_fn
        wakeup_flusher_threads_bdi  ==> resion:WB_REASON_LAPTOP_TIMER
           __wakeup_flusher_threads_bdi
                wb_start_writeback(wb, reason);
                    wb_wakeup(wb);
                        mod_delayed_work(bdi_wq, &wb->dwork, 0); ==> wb_workfn
```

# 一般io

# 没有看懂的代码
`struct bdi_writeback->b_ditry, b_io, b_more_io,b_dirty_time的作用`

# 参考链接
http://blog.sina.com.cn/s/blog_a558c25a0102vhv3.html  read()/write()的生命旅程之四——第四章：writeback

