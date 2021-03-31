# Linux内存回收

## 标记位
### PG_active PG_referenced
PG_active标志位决定page在哪个链表，也就是说active list
中的pages的PG_active都为1，而inactive list中的pages的
PG_active都为0。PG_referenced标志位则是表明page最近是
否被使用过。当一个page被访问，mark_page_accessed()会检
测该page的PG_referenced位，如果PG_referenced为0，则将其
置为1。

不管是active list还是inactive list，都是采用FIFO(First 
In First Out)的形式，新的元素从链表头部加入，中间的元素
逐渐向尾端移动。在需要进行内存回收时，内核总是选择inactive 
list尾端的页面进行回收。

```
inactive, unreferenced -> inactive, referenced
active, unreferenced -> active, referenced
```

nactive list上PG_referenced为1的page在回收之前被再次访问到
，也就是说它在inactive list中时被访问了2次，mark_page_accessed()
就会调用activate_page()将其置换到active list的头部，同时将其
PG_active位置1，PG_referenced位清0（可以理解为两个PG_referenced
才换来一个PG_active），这个过程叫做promotion（逆袭）
```
inactive, referenced -> active, unreferenced
```
当active list中的一个page在到达链表尾端时，如果其PG_referenced位为
1，则被放回链表头部，但同时其PG_referenced会被清0。如果其PG_referenced
位为0，那么就会被放入inactive list的头部，这个过程叫做demotion。可见，
```
+                * 2) Global reclaim encounters a page, memcg encounters a
+                *    page that is not marked for immediate reclaim or
+                *    the caller does not have __GFP_IO. In this case mark
+                *    the page for immediate reclaim and continue scanning.
+                *
+                *    __GFP_IO is checked  because a loop driver thread might
+                *    enter reclaim, and deadlock if it waits on a page for
+                *    which it is needed to do the write (loop masks off
+                *    __GFP_IO|__GFP_FS for this reason); but more thought
+                *    would probably show more reasons.
+                *
+                *    Don't require __GFP_FS, since we're not going into the
+                *    FS, just waiting on its writeback completion. Worryingly,
+                *    ext4 gfs2 and xfs allocate pages with
+                *    grab_cache_page_write_begin(,,AOP_FLAG_NOFS), so testing
+                *    may_enter_fs here is liable to OOM on them.
+                *
+                * 3) memcg encounters a page that is not already marked
+                *    PageReclaim. memcg does not have any dirty pages
+                *    throttling so we could easily OOM just because too many
+                *    pages are in writeback and there is nothing else to
+                *    reclaim. Wait for the writeback to complete.
+                */
 
                if (PageWriteback(page)) {
-                       /*
-                        * memcg doesn't have any dirty pages throttling so we
-                        * could easily OOM just because too many pages are in
-                        * writeback and there is nothing else to reclaim.
-                        *
-                        * Check __GFP_IO, certainly because a loop driver
-                        * thread might enter reclaim, and deadlock if it waits
-                        * on a page for which it is needed to do the write
-                        * (loop masks off __GFP_IO|__GFP_FS for this reason);
-                        * but more thought would probably show more reasons.
-                        *
-                        * Don't require __GFP_FS, since we're not going into
-                        * the FS, just waiting on its writeback completion.
-                        * Worryingly, ext4 gfs2 and xfs allocate pages with
-                        * grab_cache_page_write_begin(,,AOP_FLAG_NOFS), so
-                        * testing may_enter_fs here is liable to OOM on them.
-                        */
-                       if (global_reclaim(sc) ||
+                       /* Case 1 above */
+                       if (current_is_kswapd() &&
+                           PageReclaim(page) &&
+                           zone_is_reclaim_writeback(zone)) {
+                               unlock_page(page);
+                               congestion_wait(BLK_RW_ASYNC, HZ/10);
+                               zone_clear_flag(zone, ZONE_WRITEBACK);
+                               goto keep;
+
+                       /* Case 2 above */
+                       } else if (global_reclaim(sc) ||
                            !PageReclaim(page) || !(sc->gfp_mask & __GFP_IO)) {
                                /*
                                 * This is slightly racy - end_page_writeback()
@@ -757,9 +787,13 @@ static unsigned long shrink_page_list(struct list_head *page_list,
                                 */
                                SetPageReclaim(page);
                                nr_writeback++;
+
                                goto keep_locked;
+
+                       /* Case 3 above */
+                       } else {
+                               wait_on_page_writeback(page);
                        }
-                       wait_on_page_writeback(page);
                }

                if (!force_reclaim)

@@ -1374,8 +1408,10 @@ shrink_inactive_list(unsigned long nr_to_scan, struct lruvec *lruvec,
         *                     isolated page is PageWriteback
         */
        if (nr_writeback && nr_writeback >=
-                       (nr_taken >> (DEF_PRIORITY - sc->priority)))
+                       (nr_taken >> (DEF_PRIORITY - sc->priority))) {
                wait_iff_congested(zone, BLK_RW_ASYNC, HZ/10);
+               zone_set_flag(zone, ZONE_WRITEBACK);
+       }

        /*
         * Similarly, if many dirty pages are encountered that are not
@@ -2669,8 +2705,8 @@ static bool prepare_kswapd_sleep(pg_data_t *pgdat, int order, long remaining,
  * the high watermark.
  *
  * Returns true if kswapd scanned at least the requested number of pages to
- * reclaim. This is used to determine if the scanning priority needs to be
- * raised.
+ * reclaim or if the lack of progress was due to pages under writeback.
+ * This is used to determine if the scanning priority needs to be raised.
  */
 static bool kswapd_shrink_zone(struct zone *zone,
                               struct scan_control *sc,
@@ -2697,6 +2733,8 @@ static bool kswapd_shrink_zone(struct zone *zone,
        if (nr_slab == 0 && !zone_reclaimable(zone))
                zone->all_unreclaimable = 1;

+       zone_clear_flag(zone, ZONE_WRITEBACK);
+
        return sc->nr_scanned >= sc->nr_to_reclaim;
 }



```


```
t e62e384e9da8d9a0c599795464a7e76fd490931c
Author: Michal Hocko <mhocko@suse.cz>
Date:   Tue Jul 31 16:45:55 2012 -0700

    memcg: prevent OOM with too many dirty pages

    The current implementation of dirty pages throttling is not memcg aware
    which makes it easy to have memcg LRUs full of dirty pages.  Without
    throttling, these LRUs can be scanned faster than the rate of writeback,
    leading to memcg OOM conditions when the hard limit is small.
    
    当前脏页限制的实现不支持memcg，这使得使memcg LRU充满脏页变得容易。 如果没
    有限制，这些LRU的扫描速度可以快于回写速率，从而在硬限制较小时导致出现memcg 
    OOM情况。

    This patch fixes the problem by throttling the allocating process
    (possibly a writer) during the hard limit reclaim by waiting on
    PageReclaim pages.  We are waiting only for PageReclaim pages because
    those are the pages that made one full round over LRU and that means that
    the writeback is much slower than scanning.
    
    这个补丁通过在硬限制回收期间等待PageReclaim页面来限制分配过程（可能是写者(writer)）
    来解决此问题。 我们只在等待PageReclaim页面，因为这些页面在LRU上进行了整整一轮
    的处理，这意味着写回比扫描慢得多。
    
    The solution is far from being ideal - long term solution is memcg aware
    dirty throttling - but it is meant to be a band aid until we have a real
    fix.  We are seeing this happening during nightly backups which are placed
    into containers to prevent from eviction of the real working set.

    该解决方案远非理想的解决方案-长期解决方案是可识别memcg脏页控制机制-
    但在我们确定一个真正的解决方案之前，它是一个帮手。 我们看到这种情况
    是在每晚备份中发生的，这些备份被放入容器中以防止实际工作集被逐出。

    The change affects only memcg reclaim and only when we encounter
    PageReclaim pages which is a signal that the reclaim doesn't catch up on
    with the writers so somebody should be throttled.  This could be
    potentially unfair because it could be somebody else from the group who
    gets throttled on behalf of the writer but as writers need to allocate as
    well and they allocate in higher rate the probability that only innocent
    processes would be penalized is not that high.
    
    该更改仅影响memcg回收，并且仅当我们遇到PageReclaim页面时，这是一个信号，
    表明该回收不会赶上writer，因此应限制某人。 这可能是不公平的，因为可能是来自该
    组的其他人代表writer受到了限制，但是由于也writer需要分配，并且他们以更高的比
    率分配只有无辜的过程才会受到惩罚的可能性并不高。

    I have tested this change by a simple dd copying /dev/zero to tmpfs or
    ext3 running under small memcg (1G copy under 5M, 60M, 300M and 2G
    containers) and dd got killed by OOM killer every time.  With the patch I
    could run the dd with the same size under 5M controller without any OOM.
    The issue is more visible with slower devices for output.

    我已经通过在小型memcg（在5M，60M，300M和2G容器下的1G副本）下运行的简单dd复
    制/ dev / zero到tmpfs或ext3来测试了此更改，并且dd每次都被OOM杀手杀死。 有
    了补丁，我可以在没有任何OOM的情况下在5M控制器下以相同大小运行dd。 使用较慢
    的设备进行输出时，该问题更加明显。

    ...
	@@ -720,9 +720,26 @@ static unsigned long shrink_page_list(struct list_head *page_list,
                        (PageSwapCache(page) && (sc->gfp_mask & __GFP_IO));

                if (PageWriteback(page)) {
-                       nr_writeback++;
-                       unlock_page(page);
-                       goto keep;
+                       /*
+                        * memcg doesn't have any dirty pages throttling so we
+                        * could easily OOM just because too many pages are in
+                        * writeback from reclaim and there is nothing else to
+                        * reclaim.
+                        *
+                        * Check may_enter_fs, certainly because a loop driver
+                        * thread might enter reclaim, and deadlock if it waits
+                        * on a page for which it is needed to do the write
+                        * (loop masks off __GFP_IO|__GFP_FS for this reason);
+                        * but more thought would probably show more reasons.
+                        */
+                       if (!global_reclaim(sc) && PageReclaim(page) &&
+                                       may_enter_fs)
+                               wait_on_page_writeback(page);
+                       else {
+                               nr_writeback++;
+                               unlock_page(page);
+                               goto keep;
+                       }
                }


```


# __GFP_*

* __GFP_IO
内存分配的过程中可进行IO操作，也就是说分配过程中如果需要换出页，必须设置该标志，才能将换出的页写入磁盘

* __GFP_FS
内存分配过程中可执行VFS操作，也就是可以调用VFS的接口

* __GFP_RECLAIM
 请求分配可回收的page

# 参考资料
https://zhuanlan.zhihu.com/p/70964195
https://blog.csdn.net/zsj100213/article/details/89238114
http://blog.chinaunix.net/uid-29158138-id-5204947.html
