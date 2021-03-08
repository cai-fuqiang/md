### 入口函数
submit_bio
submit_bio 函数实际上就是将上层的bio数据，传递到block层

### 相关流程
```
submit_bio
    (*)generic_make_request
        (*) q->make_request_fn() -->blk_mq_make_request()
```

### 具体解释

**generic_make_request**<br />
该函数将struct bio 转换为struct request进行提交，其中转换和提交
的函数为q->make_request_fn(), 该函数在初始化request_queue时定义，
nvme驱动定义为 blk_mq_make_request

但是这里面有一个循环，为什么会有这样的处理呢？
因为在将bio转换为request时，会有一种情形是，将bio split成多个
bio，然后再调用blk_mq_make_request进行提交, 函数流程为
```
generic_make_request
    blk_mq_make_request
        blk_queue_split
            generic_make_request
```
这样就形成了递归调用，而假如说该bio需要多次split的话，
那么递归调用会很深，去申请内存，假如说有这样一种情形：
申请内存，但内存不足时，需要进行flush操作，提交bio，而bio
因为需要split而陷入递归需要内存申请内存，这样就陷入了死循环，
所以为了避免这样的操作, generic_make_request设置了一个机制，
使得递归调用不超过两层。

做法是这样的：
设置一个队列指针current->bio_list，每次进入该函数时会检查该队列
指针是否为NULL，如果为NULL，则将bio挂入该队列, 该队列指针在首次
进入，该队列指针为NULL，会将该指针指向一个队列，在函数首次进入
而第二次函数进入时，该队列不为NULL，则直接将bio挂入该队列，立即
返回。待第一次进入该函数返回时，会将current->bio_list置为NULL，
这样就保证了该函数递归调用不超过两层。如下代码
```
submit_bio
    generic_make_request-->首次进入
    {
        {
            !!赋值bio_list:
            !!current->bio_list = someone_list;
            LOOP:
                q->make_request_fn()        -->blk_mq_make_request
                    blk_queue_split
                    blk_queue_split会将bio分成两个，一个是不需要再split的bio(bio1),
                    另一个是剩下的bio(bio2)，bio2可能还需要split，将bio2调用generic
                    _make_request进行递归调用进行处理，而bio1，则进行make_request的
                    进一步处理
                        generic_make_request
                        {
                            if (current->bio_list) {
                                bio_list_add(&current->bio_list[0], bio);   
                                !!直接return
                                !!goto out;
                            }
                        }
                将bio1进行进一步处理
            将bio2进行下一步处理
            goto LOOP;
        }
        !!将bio_list置为NULL
        !!current->bio_list = NULL;
    }
```
