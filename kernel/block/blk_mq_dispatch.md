## blk_mq_make_request 流程
### 整体流程
```mermaid
flowchart TD
plug[merge 或 insert plug list] --> ctx[flush plug into ctx]
ctx --> hctx[dispatch ctx to hctx]
hctx --> driver[driver -- queue_rq]
driver --> send_cmd
```

### merge 或 insert plug list
```mermaid
flowchart TD
subgraph blk_mq_make_request
blk_queue_split --> id1{blk_attempt_plug_merge<br />该函数会在plug<br />队列进行merge操作}
id1 --执行成功--> return[return]
id1 --执行失败--> blk_mq_get_request[申请一个request]
blk_mq_get_request --> id2[blk_mq_bio_to_request 将bio转换为request]
id2 --> jg2{q->nr_hw_queue == 1 <br />-- 单队列 <br /> q->mq_ops->commit_rqs <br />-- 有commit_rqs方法 <br /> !blk_queue_norot <br />-- 不是SSD}
jg2 --Y--> jg3{判断requst count<br />是否超过最大值}
jg3 --Y--> flush_in_sw[blk_flush_plug_list]
jg3 --N--> blk_add_rq_to_plug
flush_in_sw -->  blk_add_rq_to_plug
jg2-. N .-注释same_rq[/这种情况肯定时merge失败了, <br /> 所以如果是多硬件队列的话,<br />并且不是SSD话,可以考虑直接<br />bypass plug and ctx,直接dispatch到硬件队列 <br /> 来降低延迟/]
blk_add_rq_to_plug-->return
jg2 --N plug new rq--> plug_new_rq[blk_add_rq_to_plug]
plug_new_rq --直接派送dispatch rq--> blk_mq_try_issue_directly
blk_mq_try_issue_directly-->return
end
```

### flush plug into ctx
```mermaid
flowchart TD
subgraph blk_flush_plug_list
end

subgraph blk_mq_flush_plug_list
splice_plug[splice plug <br />队列到临时队列list] --> insert_ctx[分类进行insert ctx]
insert_ctx-->sche_insert[blk_mq_sched_insert_requests]
insert_ctx-. 注释 .- 注释分类进行insertctx[将相同mq_hctx和相同ctx的 <br /> request放在一起insert到ctx队列]
end

subgraph blk_mq_sched_insert_requests
insert_request[blk_mq_insert_requests] --> blk_mq_run_hw_queue
end

subgraph blk_mq_insert_requests
lock[spin_lock ctx] --> list_splice_tail_init 
list_splice_tail_init --> blk_mq_hctx_mark_pending
blk_mq_hctx_mark_pending --> unlock[spin_unlock ctx]
end

blk_flush_plug_list --call--> blk_mq_flush_plug_list
insert_request --展开--> blk_mq_insert_requests
sche_insert --展开--> blk_mq_sched_insert_requests
```

***

### dispatch ctx to hctx

<div style="text-align: center;">

![image](./dispatch_hctx.png)
图6. 流程图
</div>

## 参考链接
https://blog.csdn.net/hu1610552336/article/details/111464548
