# 使用systemtap 脚本抓测试程序
## 在陈帆提供的模拟ceph程序上做如下改动
```diff
[root@node-1 git]# diff -Naru version1.c version2.c
--- version1.c  2024-01-02 16:30:15.534877762 +0800
+++ version2.c  2024-01-02 16:30:51.093663241 +0800
@@ -5,13 +5,14 @@
 #include <string.h>
 #include <stdint.h>
 #include <stdlib.h>
-
+#include <sys/types.h>
+#include <unistd.h>

 #define PAGE_SIZE getpagesize()

 volatile int sync_step = 0;
 volatile char *p;
-
+volatile int occur = 0;
 static inline unsigned long rdtsc()
 {
         uint64_t cntvct;
@@ -25,20 +26,49 @@

         while (rdtsc() - tsc < cycles);
 }
+void is_occurd(volatile char *ptr)
+{
+       return;
+}

 void *big_read_thread(void *ign)
 {
+       char value = 8;
         while (sync_step != 3);
-        printf("data: %d (%s)\n", *p, (*p == 0 ? "stale, broken" : "cleared, fine"));
+       value = *p;
+        printf("data: %d (%s) pid (%d)\n", value, (value == 0 ? "stale, broken" : "cleared, fine"), getpid());
+       if (value == 0) {
+               occur = 1;
+       }
+       is_occurd(p);
+       sync_step=4;
+       while(1);
+}
+void free_begin(volatile char *p)
+{
+       return;
+}
+void free_end(volatile char *p)
+{
+       return;
 }
-
 void *big_madvise_thread(void *ign)
 {
         sync_step = 1;
         while (sync_step != 2);
+       free_begin(p);
         free((void*)p);
+       free_end(p);
+       while(1);
+}
+void write_begin()
+{
+       return;
+}
+void write_end()
+{
+       return;
 }
-
 int main(void)
 {
         pthread_t aux_thread;
@@ -55,10 +85,11 @@
         sync_step = 2;
         wait_rdtsc(100000);
         *p = posix_memalign((void**)(void*)&p, PAGE_SIZE, PAGE_SIZE);
-
+       //write_begin();
         *p = 8;         // Cache in TLB
+       //write_end();
         sync_step = 3;
         wait_rdtsc(100000);

-        return 0;
+       return occur;
 }
```

## 编写stap脚本
```
function is_probe_task()
{
        return execname() == "tcmalloc_test";
}

function get_active_mm(_task_struct:long)
{
        active_mm = @cast(_task_struct, "task_struct", "kernel<linux/sched.h>")->active_mm;
        return active_mm
}

function print_probe_info()
{
        printf("%d ppfunc(%s) cpu(%d) pid(%d) tid(%d) active_mm(%lx)\t\t",
                gettimeofday_ns(),
                ppfunc(), cpu(), pid(), tid(), get_active_mm(task_current()))
}

function print_func_info(message:string)
{
        print_probe_info();
        printf("%s\n", message);
}

probe print_func_enter.no_sub_str = process("/root/wangfuqiang/tcmalloc/tcmalloc_test").function("free_begin"),
                process("/root/wangfuqiang/tcmalloc/tcmalloc_test").function("write_begin"),
                process("/root/wangfuqiang/tcmalloc/tcmalloc_test").function("read_begin"),
                process("/root/wangfuqiang/tcmalloc/tcmalloc_test").function("main"),
                process("/root/wangfuqiang/tcmalloc/tcmalloc_test").function("free_end"),
                process("/root/wangfuqiang/tcmalloc/tcmalloc_test").function("write_end")
{
        sub_string = ""
}

#probe print_func_enter.no_sub_str = kernel.function("do_rmdir")
#{
#       sub_string = ""
#}
probe print_func_exit.main = process("/root/wangfuqiang/tcmalloc/tcmalloc_test").function("main").return
{
        sub_string = sprintf("return(%d)", $return);
}

probe print_func_exit = print_func_exit.*
{
        print_string = sprintf("%s exit %s", ppfunc(), sub_string);
}

probe print_func_enter = print_func_enter.*
{
        print_string = sprintf("%s enter %s", ppfunc(), sub_string);
}

probe print_func_define_self.is_ocurrd = process("/root/wangfuqiang/tcmalloc/tcmalloc_test").function("is_occurd")
{
        print_string = sprintf("%s enter, ptr(%lx)", ppfunc(), $ptr);
}

probe print_func_define_self.kernel_context_switch = kernel.function("context_switch")
{
        prev_comm = kernel_string($prev->comm);
        next_comm = kernel_string($next->comm);
        #print_string = sprintf("enter context_switch %s->%s", prev_comm, next_comm);
        #print_func_info(print_string);

        if (prev_comm != "tcmalloc_test" && next_comm != "tcmalloc_test") {
                next;
        }
        print_string = sprintf("enter context_switch %s(%d)->%s(%d)",
                        prev_comm,
                        $prev->tgid,
                        next_comm,
                        $next->tgid);
}

probe print_func_specify_task.check_and_switch_context = kernel.statement("check_and_switch_context@arch/arm64/mm/context.c:253")
{
        print_string = "enter check_and_switch_context, and flush tlb"
}

probe print_func_specify_task.do_madvise = kernel.function("do_madvise")
{
        print_string = sprintf("enter start (%lx) len(%u) behavior(%d) ", $start, $len_in, $behavior);
}

probe print_func_specify_task.tlb_flush_check = kernel.function("tlb_flush_check")
{
        print_string = sprintf("BEG:BEG: nr_active_mm(%d) mm_user(%d)", $mm->context->nr_active_mm->counter,
                                $mm->mm_users->counter);
}
probe  print_func_specify_task.tlb_flush_remote = kernel.statement("tlb_flush_check@arch/arm64/mm/context.c:302")
{

        print_string = "FLUSH BROADCAST";
}

probe  print_func_specify_task.tlb_flush_local = kernel.statement("tlb_flush_check@arch/arm64/mm/context.c:296")
{
        print_string = "flush local";
}
probe  print_func_specify_task.tlb_flush_local2 = kernel.statement("tlb_flush_check@arch/arm64/mm/context.c:293")
{
        print_string = "flush local2";
}

probe print_func_specify_task.tlb_flush_print_type = kernel.statement("__flush_tlb_range@./arch/arm64/include/asm/tlbflush.h:349")
{
        print_string = sprintf("flush is %d", $flush);
}
probe print_func_specify_task.exit = kernel.function("tlb_flush_check").return
{
        print_string = sprintf("%s exit", ppfunc());
}

probe print_func_specify_task = print_func_specify_task.*
{}

probe print_func_specify_task
{
        if (!is_probe_task()) {
                next;
        }
        print_func_info(print_string);
}

probe print_func = print_func_exit,
                        print_func_enter,
                        print_func_define_self.*
{}
probe print_func
{
        print_func_info(print_string);
}
```

## 测试方法
并行运行40多个, 测试程序进行测试.并从日志中获取复现问题的
stap日志:
```

[root@node-1 tcmalloc]# cat stap.log   |grep -a 'return(1)'
1704185233865495362 ppfunc(main) cpu(85) pid(39532) tid(39532) active_mm(ffff50000ddb5d80)              main exit return(1)
1704185234228399219 ppfunc(main) cpu(37) pid(40491) tid(40491) active_mm(ffff200003d93c80)              main exit return(1)

[root@node-1 tcmalloc]# cat stap.log |grep -a -E 'ffff50000ddb5d80|->tcmalloc(39532)' > unsort.txt
[root@node-1 tcmalloc]# sort unsort.txt > sort.txt
```

截取`sort.txt` //省略timestamp列, 和 active_mm列
```
ppfunc(main) cpu(84) pid(39532) tid(39532)              main enter
ppfunc(do_madvise) cpu(84) pid(39532) tid(39532)                enter start (1cd50000) len(786432) behavior(8)
//==(1.1)==
ppfunc(free_begin) cpu(87) pid(39532) tid(39670)                free_begin enter
//==(1.2)==
ppfunc(do_madvise) cpu(87) pid(39532) tid(39670)                enter start (1cd40000) len(851968) behavior(8)
//==(2.1)==
ppfunc(tlb_flush_check) cpu(87) pid(39532) tid(39670)            BEG:BEG: nr_active_mm(3) mm_user(3)
ppfunc(tlb_flush_check) cpu(87) pid(39532) tid(39670)            FLUSH BROADCAST
ppfunc(context_switch) cpu(84) pid(39532) tid(39532)            enter context_switch tcmalloc_test(39532)->swapper/84(0)
ppfunc(context_switch) cpu(84) pid(0) tid(0)                    enter context_switch swapper/84(0)->tcmalloc_test(39532)
ppfunc(context_switch) cpu(84) pid(39532) tid(39532)            enter context_switch tcmalloc_test(39532)->stapio(36536)
ppfunc(context_switch) cpu(89) pid(39532) tid(39665)            enter context_switch tcmalloc_test(39532)->stapio(36536)
ppfunc(tlb_flush_check) cpu(87) pid(39532) tid(39670)           tlb_flush_check exit
//==(2.2)==
ppfunc(__flush_tlb_range) cpu(87) pid(39532) tid(39670)          flush is 2
ppfunc(context_switch) cpu(87) pid(39532) tid(39670)            enter context_switch tcmalloc_test(39532)->stapio(36536)
ppfunc(context_switch) cpu(89) pid(39532) tid(39665)            enter context_switch tcmalloc_test(39532)->stapio(36536)
//==(1.3)==
ppfunc(free_end) cpu(87) pid(39532) tid(39670)              free_end enter
ppfunc(do_madvise) cpu(85) pid(39532) tid(39532)                 enter start (1cd50000) len(786432) behavior(8)
//==(3)==
ppfunc(write_begin) cpu(85) pid(39532) tid(39532)                write_begin enter
ppfunc(is_occurd) cpu(89) pid(39532) tid(39665)          is_occurd enter, ptr(1cd40000)
ppfunc(main) cpu(85) pid(39532) tid(39532)               main exit return(1)
```
1. 可以发现, 在 `free_begin`->`free_end`中间, 其所在的cpu `87`发生过一次 `do_madvise`行为
2. 而该`do_madvise`会调用`__flush_tlb_range` flush tlb, 但是其flush行为是 `FLUSH BROADCAST`, 
3. 而在free_end之后(1.3), 再发生 `write_begin()`, 所以说写操作一定是发生在flush tlb broadcast之后,
   写的时候,使用的是 freesh tlb, 所以不应该会复现问题

# 考虑是乱序执行, 修改代码在做测试
经多次调试发现, 如下添加内存屏障
```cpp
int main(void)
{
        pthread_t aux_thread;

        pthread_create(&aux_thread, NULL, big_read_thread, NULL);

        int r = posix_memalign((void**)(void*)&p, PAGE_SIZE, PAGE_SIZE);
        if (r < 0) exit(0);
        memset((void*)p, 8, PAGE_SIZE);
        pthread_create(&aux_thread, NULL, big_madvise_thread, NULL);

        while (sync_step != 1);

        sync_step = 2;
        wait_rdtsc(100000);
        *p = posix_memalign((void**)(void*)&p, PAGE_SIZE, PAGE_SIZE);
        *p = 8;         // Cache in TLB
        
        smp_mb();      //内存屏障处
        
        sync_step = 3;
        wait_rdtsc(100000);

        return occur;
}
```

定义不同的内存屏障, 有如下测试结果:
```
#define smp_mb()   __asm__ __volatile__ ("" ::: "memory")      //编译类型的内存屏障,  问题复现
#define dmb(option) __asm__ __volatile__ ("dmb " #option : : : "memory")
#define smp_mb()        dmb(ish)                               //load,store内存屏障, 问题不复现
#define smp_rmb()       dmb(ishld)                             //load内存屏障, 问题不复现
#define smp_wmb()       dmb(ishst)                             //store内存屏障, 问题不复现
```

所以该测试程序复现是由于乱序导致.
