# bio && dio
从字面意思来看，bio: buffer  IO;dio: direct IO; 两者
最主要的区别是是否使用kernel 的文件系统中的buffer, 即
page cache, 通常情况下, pagecache 会加速用户态访问文件
内容，但是在某些情况下, 用户态自己维护了cache, 使用bio
反而会因为数据copy而降低效率（因为需要将用户态buffer copy
到 pagecache, 或者将pagecache copy到用户态buffer)。

那么dio 是如何将用户态数据传递到设备的呢?

# dio处理流程
## 文件系统层
分析下read系统调用，并且操作的是blockdev文件的相关代码
路径如下:
```
SYSCALL_DEFINE3--read
  ksys_read
    vfs_read
      __vfs_read
        new_sync_read
          call_read_iter
            file->f_op->read_iter(blkdev_read_iter)
              generic_file_read_iter
                mapping->a_ops->direct_IO(blkdev_direct_IO)
	          __blkdev_direct_IO_simple/__blkdev_direct_IO
	  	    bio_iov_iter_get_pages
```

### new_sync_read

```cpp
static ssize_t new_sync_read(struct file *filp, char __user *buf, size_t len, loff_t *ppos)
{
        struct iovec iov = { .iov_base = buf, .iov_len = len };
        struct kiocb kiocb;
        struct iov_iter iter;
        ssize_t ret;

        init_sync_kiocb(&kiocb, filp);
        kiocb.ki_pos = *ppos;
        iov_iter_init(&iter, READ, &iov, 1, len);

        ret = call_read_iter(filp, &kiocb, &iter);
        BUG_ON(ret == -EIOCBQUEUED);
        *ppos = kiocb.ki_pos;
        return ret;
}
```

* 该函数会将buf和len 赋值到 struct iovec结构体中，但是不会copy数据

### blkdev_read_iter
```cpp
ssize_t
generic_file_read_iter(struct kiocb *iocb, struct iov_iter *iter)
{
        size_t count = iov_iter_count(iter);
        ssize_t retval = 0;

        if (!count)
                goto out; /* skip atime */

        if (iocb->ki_flags & IOCB_DIRECT) {
		...
		retval = mapping->a_ops->direct_IO(iocb, iter);
		...
	}
	retval = generic_file_buffered_read(iocb, iter, retval);
out:
	return retval;
```
在blkdev_read_iter中会根据ki_flags确定是direct IO ，还是buffer IO,
然后走不同的分支。

### blkdev_direct_IO
```cpp
static ssize_t
blkdev_direct_IO(struct kiocb *iocb, struct iov_iter *iter)
{
        int nr_pages;

        nr_pages = iov_iter_npages(iter, BIO_MAX_PAGES + 1);
        if (!nr_pages)
                return 0;
        if (is_sync_kiocb(iocb) && nr_pages <= BIO_MAX_PAGES)
                return __blkdev_direct_IO_simple(iocb, iter, nr_pages);

        return __blkdev_direct_IO(iocb, iter, min(nr_pages, BIO_MAX_PAGES));
}
```

这里为什么会有两个分支呢，主要是因为`__blkdev_direct_IO_simple`中初始化bio
要用栈中的一些内存，这就需要满足两个条件
* 需要是同步IO， 因为异步IO会在submit_bio之后，io complete返回之前退栈，
而io complete 相关流程还要使用bio数据结构。
* 占用的占内存不能太大，这里主要是指的nr_pages, 对应的`bio->bi_io_vec`,
如果`nr_pages`比较大的话, `bio->bi_io_vec`无法使用栈中的数组，但是
栈中的数组是大小是`DIO_INLINE_BIO_VECS` (4), 并非`BIO_MAX_PAGES`, 
一开始kernel 使用大的是`DIO_INLINE_BIO_VECS`, 后因为下面的patch给
改了，没有具体调查。
commit 72ecad22d9f198aafee64218512e02ffa7818671 

下面只看下`__blkdev_direct_IO_simple`

### __blkdev_direct_IO_simple

```cpp
static ssize_t
__blkdev_direct_IO_simple(struct kiocb *iocb, struct iov_iter *iter, int nr_pages)
{
	struct file *file = iocb->ki_filp;
	struct block_device *bdev = I_BDEV(bdev_file_inode(file));
	struct bio_vec inline_vecs[DIO_INLINE_BIO_VECS], *vecs, *bvec;
	loff_t pos = iocb->ki_pos;
	bool should_dirty = false;
	struct bio bio;
	ssize_t ret;
	blk_qc_t qc;
	int i;
	
	if ((pos | iov_iter_alignment(iter)) &
	¦   (bdev_logical_block_size(bdev) - 1))
	        return -EINVAL;
	
	if (nr_pages <= DIO_INLINE_BIO_VECS)
	        vecs = inline_vecs;
	else {
	        vecs = kmalloc_array(nr_pages, sizeof(struct bio_vec),
	                        ¦    GFP_KERNEL);
	        if (!vecs)
	                return -ENOMEM;
	}

	bio_init(&bio, vecs, nr_pages);
	bio_set_dev(&bio, bdev);
	bio.bi_iter.bi_sector = pos >> 9;
	bio.bi_write_hint = iocb->ki_hint;
	bio.bi_private = current;
	bio.bi_end_io = blkdev_bio_end_io_simple;
	bio.bi_ioprio = iocb->ki_ioprio;
	
	ret = bio_iov_iter_get_pages(&bio, iter);
	if (unlikely(ret))
	        goto out;

	...	
	qc = submit_bio(&bio);
	
	...

}
```

可以看到上面提到的栈中的资源主要有两个，
`struct bio bio`, 和`struct bio_vec inline_vecs[]`

这理会判断`nr_pages <= DIO_INLINE_BIO_VECS`, 如果不是的话，则使用
`kmalloc_array()`分配，这个判断是在上面的patch中加入的。

而`bio_init`函数会初始化`bio->bi_io_vec`, 该成员是一个bio_vec的指针。
而bio_vec数据结构如下:
```cpp
struct bio_vec {
        struct page     *bv_page; 
        unsigned int    bv_len;   
        unsigned int    bv_offset;
};
```
我们知道page主要对应的是文件系统层而和bio是block 层, 他们之间有个对应的关系，
也就是说一个page中，不是所有的数据都会传递到block层，也可能传递多个连续的page到
块设备层，所以定义的bio_vec 中有bv_len, 和bv_offset 划定这个范围。

该成员下面会用到

bio_init定义如下:
```cpp
void bio_init(struct bio *bio, struct bio_vec *table,
        ¦     unsigned short max_vecs)
{
        memset(bio, 0, sizeof(*bio));
        atomic_set(&bio->__bi_remaining, 1);
        atomic_set(&bio->__bi_cnt, 1);

        bio->bi_io_vec = table;
        bio->bi_max_vecs = max_vecs;
}
```
这里将 `bio->bi_max_vecs = max_vesc;`，关于`bio->bi_max_vecs`
成员下面会用到。

### bio_iov_iter_get_pages
```cpp
int bio_iov_iter_get_pages(struct bio *bio, struct iov_iter *iter)
{
        unsigned short orig_vcnt = bio->bi_vcnt;

        do {
                int ret = __bio_iov_iter_get_pages(bio, iter);

                if (unlikely(ret))
                        return bio->bi_vcnt > orig_vcnt ? 0 : ret;

        } while (iov_iter_count(iter) && !bio_full(bio));

        return 0;
}
```
该函数用来将iov_iter中的page, 初始化到bio中。
实际上就是数据从文件系统层传递到block层。在
函数 `__bio_iov_iter_get_pages()`会将iter中的io vector
取下来，并将相应的page初始化到bio结构体中。

```cpp
static int __bio_iov_iter_get_pages(struct bio *bio, struct iov_iter *iter)
{
	//====(1)===BEG
        unsigned short nr_pages = bio->bi_max_vecs - bio->bi_vcnt;
        unsigned short entries_left = bio->bi_max_vecs - bio->bi_vcnt;
        struct bio_vec *bv = bio->bi_io_vec + bio->bi_vcnt;
        struct page **pages = (struct page **)bv;
	//====(1)===END
        ssize_t size, left;
        unsigned len, i;
        size_t offset;

        /*
        ¦* Move page array up in the allocated memory for the bio vecs as far as
        ¦* possible so that we can start filling biovecs from the beginning
        ¦* without overwriting the temporary page array.
        */
	//====(2)===BEG
        BUILD_BUG_ON(PAGE_PTRS_PER_BVEC < 2);
        pages += entries_left * (PAGE_PTRS_PER_BVEC - 1);
	//====(2)===END

        size = iov_iter_get_pages(iter, pages, LONG_MAX, nr_pages, &offset);
        if (unlikely(size <= 0))
                return size ? size : -EFAULT;

        for (left = size, i = 0; left > 0; left -= len, i++) {
                struct page *page = pages[i];

                len = min_t(size_t, PAGE_SIZE - offset, left);

                if (__bio_try_merge_page(bio, page, len, offset)) {
                        put_page(page);
                } else {
                        if (WARN_ON_ONCE(bio_full(bio)))
                	      return -EINVAL;
                        __bio_add_page(bio, page, len, offset);
                }
                offset = 0;
        }

        iov_iter_advance(iter, size);
        return 0;
}
```
前面提到，`bio->bi_io_vec`中存放了, nr_pages个 bio_vec, 理论上
想让每一个page 都占用一个`struct bio_vec`， 但是底层get pages的相关接口
需要传入page指针数组，而不是 bio_vec 数组，而上面我们说到，kernel在处理
bio 中的vecs 这样的临时的数据结构花了很大的功夫（这里提到的临时，是因为
bio起到的作用是在文件系统层和块设备层传递信息，传递完之后，该数据结构就没有
其他用途了，就可以释放掉)，尽量保证性能。那么这里有需要一个这样的临时结构--
page数组，研发人员的做法是，还是利用bio_vec这个数组，在未使用，而且本次
也不会使用的部分，开辟临时数组空间。对应代码(1)(2), 
图示如下:

>> !!!!!!!!!!!!!!之后可以补一个图

```cpp
ssize_t iov_iter_get_pages(struct iov_iter *i,
                ¦  struct page **pages, size_t maxsize, unsigned maxpages,
                ¦  size_t *start)
{
        if (maxsize > i->count)
                maxsize = i->count;

        if (unlikely(i->type & ITER_PIPE))
                return pipe_get_pages(i, pages, maxsize, maxpages, start);
        iterate_all_kinds(i, maxsize, v, ({
		//===========(1)====================
                unsigned long addr = (unsigned long)v.iov_base;
                size_t len = v.iov_len + (*start = addr & (PAGE_SIZE - 1));
                int n;
                int res;

                if (len > maxpages * PAGE_SIZE)
                        len = maxpages * PAGE_SIZE;
                addr &= ~(PAGE_SIZE - 1);
                n = DIV_ROUND_UP(len, PAGE_SIZE);
                res = get_user_pages_fast(addr, n, (i->type & WRITE) != WRITE, pages);
                if (unlikely(res < 0))
                        return res;
                return (res == n ? len : res * PAGE_SIZE) - *start;
        0;}),({
                /* can't be more than PAGE_SIZE */
                *start = v.bv_offset;
                get_page(*pages = v.bv_page);
                return v.bv_len;
        }),({
                return -EFAULT;
        })
        )
        return 0;
}
```
这个函数中的宏，`iterate_all_kinds`比较复杂，该函数主要是走的(1)代码流程，
实际上是为了获取 iov_iter 中 关于`**pages` 数组, 并且进行get_page操作，
主要流程是通过`get_user_pages_fast`函数获取的，这里我们就不展开了。

所以，bio初始化后的page向量，实际上是使用的用户态传递进来的buf。

而之后的流程就和buffer IO相同了。
我们再看下buffer IO

# buffer IO
对应的函数为`generic_file_buffered_read`
```cpp
static ssize_t generic_file_buffered_read(struct kiocb *iocb,
                struct iov_iter *iter, ssize_t written)
{
	...
	for (;;) {
		page = find_get_page(mapping, index);
		if (!page) {
		        if (iocb->ki_flags & IOCB_NOWAIT)
		                goto would_block;
		        page_cache_sync_readahead(mapping,
		                        ra, filp,
		                        index, last_index - index);
		        page = find_get_page(mapping, index);
		        if (unlikely(page == NULL))
		                goto no_cached_page;
		}
		...
		ret = copy_page_to_iter(page, offset, nr, iter);
		...
	}
	...
}
```

对于buffer IO，先去mapping(struct address_space *) 去找pagecache,
如果能找到，就不需要在进行预读操作，如果找不到，则需要去预读，
预读过程，需要分配page ,而不是使用用户态的page, 在后面的流程中，
需要将pagecache中的内容copy到 iter中（用户态buffer)。
