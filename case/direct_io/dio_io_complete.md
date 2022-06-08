# ext4_direct_io
## 代码路径
```
ext4_direct_IO
	ext4_direct_IO_write
		__blockdev_direct_IO
			do_blockdev_direct_IO
				dio_send_cur_page
					dio_new_bio
				dio_bio_submit
```
这里主要来看下`dio_new_bio()`

## 代码分析

### dio_new_bio
```cpp
static inline int dio_new_bio(struct dio *dio, struct dio_submit *sdio,
        sector_t start_sector, struct buffer_head *map_bh)
{
    sector_t sector;
    int ret, nr_pages;

    ret = dio_bio_reap(dio, sdio);
    if (ret)
        goto out;
    sector = start_sector << (sdio->blkbits - 9);
    nr_pages = min(sdio->pages_in_io, BIO_MAX_PAGES);
    BUG_ON(nr_pages <= 0);
    dio_bio_alloc(dio, sdio, map_bh->b_bdev, sector, nr_pages);
    sdio->boundary = 0;
out:
    return ret;
}

```
该函数通过`dio_bio_alloc()`分配sdio->bio
### dio_bio_alloc
```cpp
static inline void
dio_bio_alloc(struct dio *dio, struct dio_submit *sdio,
    ¦   ¦ struct block_device *bdev,
    ¦   ¦ sector_t first_sector, int nr_vecs)
{
    struct bio *bio;

    /*
    ¦* bio_alloc() is guaranteed to return a bio when allowed to sleep and
    ¦* we request a valid number of vectors.
    ¦*/
    bio = bio_alloc(GFP_KERNEL, nr_vecs);

    bio_set_dev(bio, bdev);
    bio->bi_iter.bi_sector = first_sector;
    bio_set_op_attrs(bio, dio->op, dio->op_flags);
    if (dio->is_async)
        bio->bi_end_io = dio_bio_end_aio;
    else
        bio->bi_end_io = dio_bio_end_io;

    bio->bi_write_hint = dio->iocb->ki_hint;

    sdio->bio = bio;
    sdio->logical_offset_in_bio = sdio->cur_page_fs_offset;
}
```

如果是异步io的话, bio->bi_end_io赋值`dio_bio_end_aio`, 
`bio->bi_end_io`, 会在io_complete过程中调用到

### dio_bio_end_aio
```cpp
static void dio_bio_end_aio(struct bio *bio)
{
    struct dio *dio = bio->bi_private;
    unsigned long remaining;
    unsigned long flags;
    bool defer_completion = false;

    /* cleanup the bio */
    dio_bio_complete(dio, bio);

    spin_lock_irqsave(&dio->bio_lock, flags);
    remaining = --dio->refcount;
    if (remaining == 1 && dio->waiter)
        wake_up_process(dio->waiter);
    spin_unlock_irqrestore(&dio->bio_lock, flags);

    if (remaining == 0) {
        /*
        ¦* Defer completion when defer_completion is set or
        ¦* when the inode has pages mapped and this is AIO write.
        ¦* We need to invalidate those pages because there is a
        ¦* chance they contain stale data in the case buffered IO
        ¦* went in between AIO submission and completion into the
        ¦* same region.
        ¦*/
        if (dio->result)
            defer_completion = dio->defer_completion ||
                    ¦  (dio->op == REQ_OP_WRITE &&
                    ¦   dio->inode->i_mapping->nrpages);
        if (defer_completion) {
            INIT_WORK(&dio->complete_work, dio_aio_complete_work);
            queue_work(dio->inode->i_sb->s_dio_done_wq,
                ¦  &dio->complete_work);
        } else {
            dio_complete(dio, 0, DIO_COMPLETE_ASYNC);
        }
    }
}
```
可以看到在该函数最后,会根据`defer_completion`, 确定执行`dio_complete`
的方式, 如果`defer_completion` 为真,则会init , queue一个work,
如果为假,则会执行`dio_complete`.
那么我们看下`dio_aio_complete_work`和`dio_complete`, 
那为什么会有这两个分支呢,`defer_completion`的判定有两个条件
`dio->defer_completion`, 这个可以从用户态决定,感觉像是如果用户态
对这个dio的延迟不要求的话,可以设置下,来提升系统整体的延迟.
第二个判断条件是`dio->io == REQ_OP_WRITE && dio->inode->i_mapping->nrpages`,
这个判断条件表明,该dio是一个WRITE操作,并且有pagecache, 
那么在这种条件下, `dio_complete`函数中就需要 invalidate pagecache
,而invalidate pagecache的操作,可能会加锁, 并且产生调度,这在软中断
执行过程中是不允许的.

### dio_aio_complete_work
```cpp
static void dio_aio_complete_work(struct work_struct *work)
{
    struct dio *dio = container_of(work, struct dio, complete_work);

    dio_complete(dio, 0, DIO_COMPLETE_ASYNC | DIO_COMPLETE_INVALIDATE);
}
```

该函数也是调用dio_complete.

### dio_complete
```cpp
static ssize_t dio_complete(struct dio *dio, ssize_t ret, unsigned int flags)
{
	...
	/*
	¦* Try again to invalidate clean pages which might have been cached by
	¦* non-direct readahead, or faulted in by get_user_pages() if the source
	¦* of the write was an mmap'ed region of the file we're writing.  Either
	¦* one is a pretty crazy thing to do, so we don't support it 100%.  If
	¦* this invalidation fails, tough, the write still worked...
	¦*
	¦* And this page cache invalidation has to be after dio->end_io(), as
	¦* some filesystems convert unwritten extents to real allocations in
	¦* end_io() when necessary, otherwise a racing buffer read would cache
	¦* zeros from unwritten extents.
	¦*/
	if (flags & DIO_COMPLETE_INVALIDATE &&
	¦   ret > 0 && dio->op == REQ_OP_WRITE &&
	¦   dio->inode->i_mapping->nrpages) {
	    err = invalidate_inode_pages2_range(dio->inode->i_mapping,
	                offset >> PAGE_SHIFT,
	                (offset + ret - 1) >> PAGE_SHIFT);
	    if (err)
	        dio_warn_stale_pagecache(dio->iocb->ki_filp);
	}
	...

}
```
可以看到,在该函数执行过程中,会执行`invalidate_inode_page2_ranger`函数,
去invalidate pagecache.

### invalidate_inode_page2_ranger
```cpp
int invalidate_inode_pages2_range(struct address_space *mapping,
                ¦ pgoff_t start, pgoff_t end)
{
	...
	while (index <= end && pagevec_lookup_entries(&pvec, mapping, index,
        min(end - index, (pgoff_t)PAGEVEC_SIZE - 1) + 1,
        indices)) {
    for (i = 0; i < pagevec_count(&pvec); i++) {
        struct page *page = pvec.pages[i];

        /* We rely upon deletion not changing page->index */
        index = indices[i];
        if (index > end)
            break;

        if (xa_is_value(page)) {
            if (!invalidate_exceptional_entry2(mapping,
                            ¦  index, page))
                ret = -EBUSY;
            continue;
        }

        lock_page(page);
        WARN_ON(page_to_index(page) != index);
        if (page->mapping != mapping) {
            unlock_page(page);
            continue;
        }
        wait_on_page_writeback(page);
        if (page_mapped(page)) {
            if (!did_range_unmap) {
                /*
                ¦* Zap the rest of the file in one hit.
                ¦*/
                unmap_mapping_pages(mapping, index,
                    (1 + end - index), false);
                did_range_unmap = 1;
            } else {
                /*
                ¦* Just zap this page
                ¦*/
                unmap_mapping_pages(mapping, index,
                            1, false);
            }
        }
        BUG_ON(page_mapped(page));
        ret2 = do_launder_page(mapping, page);
        if (ret2 == 0) {
            if (!invalidate_complete_page2(mapping, page))
                ret2 = -EBUSY;
        }
        if (ret2 < 0)
            ret = ret2;
        unlock_page(page);
    }
	...
}
```
可以看出该函数的逻辑是,在mapping中找到相应的page, 然后将page
lock, 对page 进行unmap操作,之后在unlock page, 其中lock_page
的操作,可能会睡眠.

调用堆栈如下:

```
#0  dio_complete (dio=0xfd2e0a514c656700, ret=-131390643895624, flags=4294936704) at fs/direct-io.c:256
#1  0xffffffff81528426 in dio_bio_end_aio (bio=0xffff88803dcd9f00) at fs/direct-io.c:383
#2  0xffffffff81858f98 in bio_endio (bio=0xffff88803dcd9f00) at block/bio.c:1793
#3  0xffffffff818627aa in req_bio_endio (rq=0xffff88803d8a6780, bio=0xffff88803dcd9f00, nbytes=4096, error=0 '\000')
    at block/blk-core.c:196
#4  0xffffffff81865b7c in blk_update_request (req=0xffff88803d8a6780, error=0 '\000', nr_bytes=4096)
    at block/blk-core.c:1458
#5  0xffffffff8187a0a2 in blk_mq_end_request (rq=0xffff88803d8a6780, error=0 '\000') at block/blk-mq.c:562
#6  0xffffffff81af44de in virtblk_request_done (req=0xffff88803b580280) at drivers/block/virtio_blk.c:225
#7  0xffffffff81873427 in blk_done_softirq (h=0xffffffff82a05120 <softirq_vec+32>) at block/blk-softirq.c:37
#8  0xffffffff824001e4 in __do_softirq () at kernel/softirq.c:292
#9  0xffffffff810f748c in invoke_softirq () at kernel/softirq.c:373
#10 0xffffffff810f7601 in irq_exit () at kernel/softirq.c:413
--Type <RET> for more, q to quit, c to continue without paging--
#11 0xffffffff8103f5d2 in exiting_irq () at ./arch/x86/include/asm/apic.h:536
#12 0xffffffff82201767 in do_IRQ (regs=0xffffc900001bba68) at arch/x86/kernel/irq.c:258
#13 0xffffffff8220094f in common_interrupt () at arch/x86/entry/entry_64.S:583
#14 0xffffc900001bba68 in ?? ()
#15 0x0000000000000000 in ?? ()

```
