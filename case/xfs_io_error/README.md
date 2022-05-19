# dmesg报错
```
============
=====(1)====
============
[26646.482469] XFS (dm-0): writeback error on sector 61411008    
[26651.601098] print_req_error: I/O error, dev vda, sector 1859136 flags 100001
[26651.601961] XFS (dm-0): writeback error on sector 884288
[26651.602561] print_req_error: I/O error, dev vda, sector 1859240 flags 100001
[26651.603199] XFS (dm-0): writeback error on sector 884392
[26656.721406] print_req_error: I/O error, dev vda, sector 1926312 flags 100001
[26656.725975] XFS (dm-0): writeback error on sector 951464
[26661.841433] print_req_error: I/O error, dev vda, sector 1859152 flags 100001
[26661.842508] XFS (dm-0): writeback error on sector 884304
[26663.198259] print_req_error: I/O error, dev vda, sector 1860328 flags 1
[26663.199047] XFS (dm-0): writeback error on sector 885480
[26664.752941] print_req_error: I/O error, dev vda, sector 124946784 flags 801
[26664.753786] XFS (dm-0): writeback error on sector 123971936
[26665.936973] print_req_error: I/O error, dev vda, sector 122293912 flags 1801
[26665.937836] print_req_error: I/O error, dev vda, sector 122293912 flags 1801
============
=====(2)====
============
[26665.938623] XFS (dm-0): metadata I/O error in "xlog_iodone" at daddr 0x73b2e98 len 64 error 5
============
=====(3)====
============
[26665.939897] XFS (dm-0): xfs_do_force_shutdown(0x2) called from line 1271 of file fs/xfs/xfs_log.c. Return address = 00000000638fc19f
[26665.940975] XFS (dm-0): Log I/O Error Detected. Shutting down filesystem
============
=====(4)====
============
[26665.941579] XFS (dm-0): Please unmount the filesystem and rectify the problem(s)
[26665.942568] print_req_error: I/O error, dev vda, sector 184375104 flags 1001
[26672.080806] print_req_error: I/O error, dev vda, sector 1865160 flags 100001
[26672.082130] XFS (dm-0): writeback error on sector 890312
[26679.694944] XFS (dm-0): writeback error on sector 181873480
[26688.686976] XFS (dm-0): writeback error on sector 60736608
[26698.458977] XFS (dm-0): writeback error on sector 183401272
[26730.888782] print_req_error: I/O error, dev vda, sector 338659624 flags 801
[26730.889013] print_req_error: I/O error, dev vda, sector 1860736 flags 1
[26730.889071] print_req_error: I/O error, dev vda, sector 1926320 flags 100001
[26730.889158] XFS (dm-0): writeback error on sector 951472
[26730.890430] XFS (dm-1): writeback error on sector 86679848
[26730.891205] XFS (dm-0): writeback error on sector 885888
[26731.305933] print_req_error: I/O error, dev vda, sector 1862144 flags 1
[26731.306964] XFS (dm-0): writeback error on sector 887296
[26731.539883] print_req_error: I/O error, dev vda, sector 299396096 flags 801
[26731.541890] XFS (dm-1): writeback error on sector 47416320
[26736.081424] print_req_error: I/O error, dev vda, sector 184374928 flags 100001
[26736.083929] print_req_error: I/O error, dev vda, sector 1859240 flags 100001
[26736.084714] XFS (dm-0): writeback error on sector 884392
[26736.085362] print_req_error: I/O error, dev vda, sector 1859136 flags 100001
[26736.086130] XFS (dm-0): writeback error on sector 884288
[26736.086817] print_req_error: I/O error, dev vda, sector 1913896 flags 100001
[26736.088433] print_req_error: I/O error, dev vda, sector 1865112 flags 100001
[26736.089127] print_req_error: I/O error, dev vda, sector 1926328 flags 100001
[26736.089883] print_req_error: I/O error, dev vda, sector 1926312 flags 100001
[26736.090398] print_req_error: I/O error, dev vda, sector 1926168 flags 100001
[26736.091039] print_req_error: I/O error, dev vda, sector 1922792 flags 100001
[26736.091564] print_req_error: I/O error, dev vda, sector 62386032 flags 100001
[26736.092184] XFS (dm-0): writeback error on sector 183400080
[26736.092746] XFS (dm-0): writeback error on sector 939048
[26736.093240] XFS (dm-0): writeback error on sector 890264
[26736.093780] XFS (dm-0): writeback error on sector 951480
[26736.094258] XFS (dm-0): writeback error on sector 951464
[26736.094797] XFS (dm-0): writeback error on sector 951320
[26736.095280] XFS (dm-0): writeback error on sector 947944
[26736.095909] XFS (dm-0): writeback error on sector 61411184
============
=====(5)====
============
[26788.567742] INFO: task xfsaild/dm-1:38039 blocked for more than 120 seconds.
[26788.568567]       Tainted: G           OE    --------- -  - 4.18.0-147.5.1.el8_1.5es.14.x86_64 #1
[26788.569600] "echo 0 > /proc/sys/kernel/hung_task_timeout_secs" disables this message.
[26788.570574] xfsaild/dm-1    D    0 38039      2 0x80000080
[26788.571179] Call Trace:
[26788.571967]  ? __schedule+0x253/0x830
[26788.572546]  schedule+0x28/0x70
[26788.573157]  xfs_log_force+0x160/0x2e0 [xfs]
[26788.573773]  ? wake_up_q+0x70/0x70
[26788.574339]  xfsaild+0x17b/0x7f0 [xfs]
[26788.575077]  ? xfs_trans_ail_cursor_first+0x80/0x80 [xfs]
[26788.575851]  kthread+0x112/0x130
[26788.576405]  ? kthread_flush_work_fn+0x10/0x10
[26788.576988]  ret_from_fork+0x22/0x40
[26805.889488] print_req_error: 9 callbacks suppressed
[26805.890381] print_req_error: I/O error, dev vda, sector 335721545 flags 1801
[26805.891135] print_req_error: I/O error, dev vda, sector 335721545 flags 1801
[26805.891869] XFS (dm-1): metadata I/O error in "xlog_iodone" at daddr 0x4fdcc49 len 64 error 5
[26805.893042] print_req_error: I/O error, dev vda, sector 338659640 flags 801
[26805.893141] XFS (dm-1): xfs_do_force_shutdown(0x2) called from line 1271 of file fs/xfs/xfs_log.c. Return address = 00000000638fc19f
[26805.893873] xfs_destroy_ioend: 11 callbacks suppressed
[26805.893877] XFS (dm-1): writeback error on sector 86679864
[26805.895011] XFS (dm-1): Log I/O Error Detected. Shutting down filesystem
[26805.895013] XFS (dm-1): Please unmount the filesystem and rectify the problem(s)
[26806.545049] print_req_error: I/O error, dev vda, sector 299396128 flags 801
[26806.545940] XFS (dm-1): writeback error on sector 47416352
[26811.088414] print_req_error: I/O error, dev vda, sector 335721554 flags 1801
[26811.089347] print_req_error: I/O error, dev vda, sector 335721554 flags 1801
[26811.090045] XFS (dm-1): metadata I/O error in "xlog_iodone" at daddr 0x4fdcc52 len 64 error 5
[26811.091045] print_req_error: I/O error, dev vda, sector 335721561 flags 1801
[26811.091695] print_req_error: I/O error, dev vda, sector 335721561 flags 1801
[26811.092384] XFS (dm-1): metadata I/O error in "xlog_iodone" at daddr 0x4fdcc59 len 64 error 5
[26811.465223] overlayfs: failed to get metacopy (-5)
[26811.466049] overlayfs: failed to get metacopy (-5)
[26824.445328] overlayfs: failed to get metacopy (-5)
[26824.446004] overlayfs: failed to get metacopy (-5)
[48099.393044] hrtimer: interrupt took 3037049 ns
```
（1）开始发生`I/O error`的报错
（2）始打印metadata error
（3）发现文件系统问题比较严重，触发filesystem shutdown
（4）感觉该文件系统并未shutdown，仍然有一些IO触发
（5）xfsaild schedule超时打印calltrace

# 分析dmesg报错
发现dmesg报错中并未有hbd和libceph的报错打印，直接发生I/O报错，所以感觉该问题
可能是存储后端存在问题或者也可能存在多虚机实例。

但是后来采集50环境上关于虚机`instance-00000298`, 只在node-13上运行过，不存在
多实例虚机的可能。

# 结合代码分析

##  分析print_req_error相关堆栈

### print_req_error定义

```C/C++
static void print_req_error(struct request *req, blk_status_t status)
{
    int idx = (__force int)status;

    if (WARN_ON_ONCE(idx >= ARRAY_SIZE(blk_errors)))
        return;

    printk_ratelimited(KERN_ERR "%s: %s error, dev %s, sector %llu flags %x\n",
                __func__, blk_errors[idx].name,
                req->rq_disk ?  req->rq_disk->disk_name : "?",
                (unsigned long long)blk_rq_pos(req),
                req->cmd_flags);
}
```
这里面比较关键的是打印中的`%s error`, 所依据的是`blk_errors[idx].name`, 所以其值和
传进来的status相关。
`blk_errors[]`定义如下:

```C/C++
static const struct {
    int     errno;
    const char  *name;
} blk_errors[] = {
    [BLK_STS_OK]        = { 0,      "" },
    [BLK_STS_NOTSUPP]   = { -EOPNOTSUPP, "operation not supported" },
    [BLK_STS_TIMEOUT]   = { -ETIMEDOUT, "timeout" },
    [BLK_STS_NOSPC]     = { -ENOSPC,    "critical space allocation" },
    [BLK_STS_TRANSPORT] = { -ENOLINK,   "recoverable transport" },
    [BLK_STS_TARGET]    = { -EREMOTEIO, "critical target" },
    [BLK_STS_NEXUS]     = { -EBADE, "critical nexus" },
    [BLK_STS_MEDIUM]    = { -ENODATA,   "critical medium" },
    [BLK_STS_PROTECTION]    = { -EILSEQ,    "protection" },
    [BLK_STS_RESOURCE]  = { -ENOMEM,    "kernel resource" },
    [BLK_STS_DEV_RESOURCE]  = { -EBUSY, "device resource" },
    [BLK_STS_AGAIN]     = { -EAGAIN,    "nonblocking retry" },

    /* device mapper special case, should not leak out: */
    [BLK_STS_DM_REQUEUE]    = { -EREMCHG, "dm internal retry" },

    /* everything else not covered above: */
    [BLK_STS_IOERR]     = { -EIO,   "I/O" },
};
```
所以status传进来的值为`BLK_STS_IOERR`


### print_req_error调用堆栈
对于一般的`scsi io`来说, 调用堆栈如下所示:

```
scsi_io_completion
        scsi_end_request
                blk_update_request
                        print_req_error
```

`scsi_io_completion`相关代码:
```C/C++
void scsi_io_completion(struct scsi_cmnd *cmd, unsigned int good_bytes) 
{        
		int result = cmd->result;
        ...
        blk_status_t blk_stat = BLK_STS_OK;

        if (unlikely(result))   /* a nz result may or may not be an error */    
            result = scsi_io_completion_nz_result(cmd, result, &blk_stat);

		...
		if (likely(blk_rq_bytes(req) > 0 || blk_stat == BLK_STS_OK)) {      
    		if (likely(!scsi_end_request(req, blk_stat, good_bytes, 0)))    
    	    	return; /* no bytes remaining */                            
		}
		...
}                                                                   
```
可以看出`blk_stat`的赋值，通过`scsi_io_completion_nz_result()`,
```C/C++
tatic int scsi_io_completion_nz_result(struct scsi_cmnd *cmd, int result, 
                    blk_status_t *blk_statp)                               
{    
	...
	//下面代码是可能将blk_statp变量赋值为除BLK_STS_OK其他值的流程                                                                      
    if (blk_rq_is_passthrough(req)) {                                      
        if (sense_valid) {                                                 
            /*                                                             
             * SG_IO wants current and deferred errors                     
             */                                                            
            scsi_req(req)->sense_len =                                     
                min(8 + cmd->sense_buffer[7],                              
                ¦   SCSI_SENSE_BUFFERSIZE);                                
        }                                                                  
        if (sense_current)                                                 
            *blk_statp = scsi_result_to_blk_status(cmd, result);           
    } else if (blk_rq_bytes(req) == 0 && sense_current) {                  
        /*                                                                 
        ¦* Flush commands do not transfers any data, and thus cannot use   
        ¦* good_bytes != blk_rq_bytes(req) as the signal for an error.     
        ¦* This sets *blk_statp explicitly for the problem case.           
        ¦*/                                                                
        *blk_statp = scsi_result_to_blk_status(cmd, result);               
    }                                                                      
	...
}
```
通过上面的代码可以看出，实际上时调用`scsi_result_to_blk_status()`接口
查看 ` scsi_result_to_blk_status()`代码

```
static blk_status_t scsi_result_to_blk_status(struct scsi_cmnd *cmd, int result)
{                                                                               
    switch (host_byte(result)) {                                                
    case DID_OK:                                                                
        /*                                                                      
        ¦* Also check the other bytes than the status byte in result            
        ¦* to handle the case when a SCSI LLD sets result to                    
        ¦* DRIVER_SENSE << 24 without setting SAM_STAT_CHECK_CONDITION.         
        ¦*/                                                                     
        if (scsi_status_is_good(result) && (result & ~0xff) == 0)               
            return BLK_STS_OK;                                                  
        return BLK_STS_IOERR;                                                   
    case DID_TRANSPORT_FAILFAST:                                                
        return BLK_STS_TRANSPORT;                                               
    case DID_TARGET_FAILURE:                                                    
        set_host_byte(cmd, DID_OK);                                             
        return BLK_STS_TARGET;                                                  
    case DID_NEXUS_FAILURE:                                                     
        set_host_byte(cmd, DID_OK);                                             
        return BLK_STS_NEXUS;                                                   
    case DID_ALLOC_FAILURE:                                                     
        set_host_byte(cmd, DID_OK);                                             
        return BLK_STS_NOSPC;                                                   
    case DID_MEDIUM_ERROR:                                                      
        set_host_byte(cmd, DID_OK);                                             
        return BLK_STS_MEDIUM;                                                  
    default:                                                                    
        return BLK_STS_IOERR;                                                   
    }                                                                           
}
```
实际上是通过传进来的result将其转换为错误值, 这个值是通过
`scsi_cmnd->result`获取的，实际上是底层驱动赋值该值。
所以，感觉很可能是后端存储的问题。


# print_req_error打印其他分析

```
[26661.841433] print_req_error: I/O error, dev vda, sector 1859152 flags 100001
[26664.752941] print_req_error: I/O error, dev vda, sector 124946784 flags 801
[26665.936973] print_req_error: I/O error, dev vda, sector 122293912 flags 1801
[26665.938623] XFS (dm-0): metadata I/O error in "xlog_iodone" at daddr 0x73b2e98 len 64 error 5
```
这里的flags实际上是`scsi_cmnd->cmd_flags`
值定义在`enum req_opf和enmu req_flags_bits`, 分别是解释该值前8位，和其他位,定义不在展开
|值|解释|
|----|----|
|100001|REQ_OP_WRITE,__REQ_BACKGROUND|
|801|REQ_OP_WRITE, __REQ_SYNC|
|1801|REQ_OP_WRITE,__REQ_SYNC, __REQ_META|
所以metadata I/O error发生在同步meta数据时，I/O出错后报的错，
找到改行报错打印点，位于`xlog_iodone` 函数：

```
static void
xlog_iodone(xfs_buf_t *bp)
{
    struct xlog_in_core *iclog = bp->b_log_item;
    struct xlog     *l = iclog->ic_log;
    int         aborted = 0;

    /*
     * Race to shutdown the filesystem if we see an error or the iclog is in
     * IOABORT state. The IOABORT state is only set in DEBUG mode to inject
     * CRC errors into log recovery.
     */
    if (XFS_TEST_ERROR(bp->b_error, l->l_mp, XFS_ERRTAG_IODONE_IOERR) ||
        iclog->ic_state & XLOG_STATE_IOABORT) {
        if (iclog->ic_state & XLOG_STATE_IOABORT)
            iclog->ic_state &= ~XLOG_STATE_IOABORT;

        xfs_buf_ioerror_alert(bp, __func__);
		xfs_buf_stale(bp);                                     
		xfs_force_shutdown(l->l_mp, SHUTDOWN_LOG_IO_ERROR);    
	...
```
根据注释来看，如果发现了一个error，就会在这个地方报错，并且会shutdown filesystem。
所以，感觉这个文件系统并未损坏，可能时记录文件系统日志元数据的时候，发生io错误了，导致
该打印. 
可以将该盘复制一份 ，挂载到其他地方，使用`xfs_repair`工具修复看看。
