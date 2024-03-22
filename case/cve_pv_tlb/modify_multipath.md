# 微调multi_thread程序
主要流程在alloc 初始化obj之后, 将其copy到另一个全局buffer中. 这样
如果因为访问了 stall tlb, 但是write 到 stall tlb 指向的page中的数据
可以保存下来.
```diff
 void *producer(void *arg) {
        ...
        while (true) {
                pthread_mutex_lock(&mtx);
                if (list_empty(&dataQueue)) {
-                       int insernum = rand() % 5 + 1;
+                       int insernum = rand() % TEST_NUM + 1;
                        for (size_t i = 0; i < insernum; i++) {
-                               struct test *data = (struct test *)malloc(sizeof(struct test) + sizeof(char) * (PKGSIZE + 1) );
-                               memcpy(data->data, tmpbuff, PKGSIZE);
+                               struct test *data = (struct test *)malloc(PKGSIZE);
+                               memcpy(data->data, tmpbuff, DATA_SIZE);
                                list_add(&data->node, &dataQueue);
+                               copy_to_data_bak_st(i, data);
                        }
                }
                pthread_mutex_unlock(&mtx);
@@ -51,6 +70,15 @@
        return NULL;
 }
```

# 测试
经多次测试发现, 在vcpu数量比较大的虚拟机, 启动很多个该进程容易复现.
本次测试在64个vcpu的虚拟机启动20个进程,几秒钟出现了大量coredump. 我们随便选择一个进行调试
```
(gdb) bt
#0  0x000000000040131c in __list_del (prev=0x0, next=0x0) at list.h:30
#1  0x0000000000401354 in __list_del_entry (entry=0x7f8a0c030b50) at list.h:38
#2  0x000000000040136f in list_del (entry=0x7f8a0c030b50) at list.h:45
#3  0x0000000000401670 in consumer (arg=0x0) at multi_thread.c:91
#4  0x00007f8a13e10f1b in ?? () from /lib64/libpthread.so.0
#5  0x00007f8a13d481c0 in clone () from /lib64/libc.so.6
(gdb) p dataQueue
$12 = {next = 0x7f8a0c030b50, prev = 0x7f8a0c000b20}
```

可以看到, 再访问 `dataQueue->next` 指向的entry就出现了问题, 我们看备份的数据
```
(gdb) p data_bak_st.data_bak[0].ptr
$15 = 0x7f8a0c000b20 "\340@@"
(gdb) p data_bak_st.data_bak[1].ptr
$16 = 0x7f8a0c010b30 " \v"
(gdb) p data_bak_st.data_bak[2].ptr
$17 = 0x7f8a0c020b40 "0\v\001\f\212\177"
(gdb) p data_bak_st.data_bak[3].ptr
$18 = 0x7f8a0c030b50 ""
(gdb) p data_bak_st.data_bak[4].ptr
$19 = 0x0
```

可以看到最后一个加入的 object 是数组下标为3.也就是上次alloc一共分配了4个object,
其链表为:
```
--dataQueue--3---2---1---0---+
^                            |
|                            |
+--------------------------<-+
```
但是现在访问3发现 `entry = {0, 0}`
```
(gdb) x/10xg 0x7f8a0c030b50
0x7f8a0c030b50: 0x0000000000000000      0x0000000000000000
0x7f8a0c030b60: 0x0000000000000000      0x0000000000000000
0x7f8a0c030b70: 0x0000000000000000      0x0000000000000000
0x7f8a0c030b80: 0x0000000000000000      0x0000000000000000
0x7f8a0c030b90: 0x0000000000000000      0x0000000000000000
```
我们来看下其备份的数据:
```
(gdb) x/10xg data_bak_st.data_bak[3].buffer
0x434138 <data_bak_st+196632>:  0x00007f8a0c020b40[next]    0x00000000004040e0[prev]
0x434148 <data_bak_st+196648>:  0x6867666564636261          0x6665646362616a69
0x434158 <data_bak_st+196664>:  0x646362616a696867          0x62616a6968676665
0x434168 <data_bak_st+196680>:  0x6a69686766656463          0x6867666564636261
0x434178 <data_bak_st+196696>:  0x6665646362616a69          0x646362616a696867
```
发现正好能对上, 所以基本可以确认是访问了stall tlb.
