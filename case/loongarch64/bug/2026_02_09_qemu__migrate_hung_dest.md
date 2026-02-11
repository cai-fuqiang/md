### 目的端堆栈

查看目的端堆栈:

```
Thread 17 (Thread 0x7ff7a6862940 (LWP 2463588) "vnc_worker"):
#0  0x00007ffff17e0bb8 in ?? () from target:/usr/lib64/libc.so.6
#1  0x00007ffff17e4054 in pthread_cond_wait () from target:/usr/lib64/libc.so.6
#2  0x000055555964fc34 in qemu_cond_wait_impl (cond=0x55557becbc00, mutex=0x55557becbc38, file=0x5555596d4f50 "../ui/vnc-jobs.c", line=<optimized out>) at ../util/qemu-thread-posix.c:225
#3  0x00005555590aa760 in vnc_worker_thread_loop (queue=queue@entry=0x55557becbc00) at ../ui/vnc-jobs.c:248
#4  0x00005555590ab318 in vnc_worker_thread (arg=arg@entry=0x55557becbc00) at ../ui/vnc-jobs.c:362
#5  0x000055555964ecd0 in qemu_thread_start (args=<optimized out>) at ../util/qemu-thread-posix.c:541
#6  0x00007ffff17e4d90 in ?? () from target:/usr/lib64/libc.so.6
#7  0x00007ffff185f60c in ?? () from target:/usr/lib64/libc.so.6

Thread 16 (Thread 0x7ff7baff6940 (LWP 2463586) "CPU 7/KVM"):
#0  0x00007ffff17e0bb8 in ?? () from target:/usr/lib64/libc.so.6
#1  0x00007ffff17e4054 in pthread_cond_wait () from target:/usr/lib64/libc.so.6
#2  0x000055555964fc34 in qemu_cond_wait_impl (cond=0x55557a42c910, mutex=0x555559ca2fd0 <qemu_global_mutex>, file=0x555559736af8 "../system/cpus.c", line=<optimized out>) at ../util/qemu-thread-posix.c:225
#3  0x00005555592a4d44 in qemu_wait_io_event (cpu=cpu@entry=0x55557a41b120) at ../system/cpus.c:445
#4  0x0000555559494348 in kvm_vcpu_thread_fn (arg=arg@entry=0x55557a41b120) at ../accel/kvm/kvm-accel-ops.c:56
--Type <RET> for more, q to quit, c to continue without paging--
#5  0x000055555964ecd0 in qemu_thread_start (args=<optimized out>) at ../util/qemu-thread-posix.c:541
#6  0x00007ffff17e4d90 in ?? () from target:/usr/lib64/libc.so.6
#7  0x00007ffff185f60c in ?? () from target:/usr/lib64/libc.so.6

Thread 15 (Thread 0x7ff7d1ffe940 (LWP 2463585) "CPU 6/KVM"):
#0  0x00007ffff17e0bb8 in ?? () from target:/usr/lib64/libc.so.6
#1  0x00007ffff17e4054 in pthread_cond_wait () from target:/usr/lib64/libc.so.6
#2  0x000055555964fc34 in qemu_cond_wait_impl (cond=0x55557a41ad10, mutex=0x555559ca2fd0 <qemu_global_mutex>, file=0x555559736af8 "../system/cpus.c", line=<optimized out>) at ../util/qemu-thread-posix.c:225
#3  0x00005555592a4d44 in qemu_wait_io_event (cpu=cpu@entry=0x55557a409520) at ../system/cpus.c:445
#4  0x0000555559494348 in kvm_vcpu_thread_fn (arg=arg@entry=0x55557a409520) at ../accel/kvm/kvm-accel-ops.c:56
#5  0x000055555964ecd0 in qemu_thread_start (args=<optimized out>) at ../util/qemu-thread-posix.c:541
#6  0x00007ffff17e4d90 in ?? () from target:/usr/lib64/libc.so.6
#7  0x00007ffff185f60c in ?? () from target:/usr/lib64/libc.so.6

Thread 14 (Thread 0x7fffd2ff6940 (LWP 2463584) "CPU 5/KVM"):
#0  0x00007ffff17e0bb8 in ?? () from target:/usr/lib64/libc.so.6
#1  0x00007ffff17e4054 in pthread_cond_wait () from target:/usr/lib64/libc.so.6
#2  0x000055555964fc34 in qemu_cond_wait_impl (cond=0x55557a409110, mutex=0x555559ca2fd0 <qemu_global_mutex>, file=0x555559736af8 "..--Type <RET> for more, q to quit, c to continue without paging--
/system/cpus.c", line=<optimized out>) at ../util/qemu-thread-posix.c:225
#3  0x00005555592a4d44 in qemu_wait_io_event (cpu=cpu@entry=0x55557a3f79c0) at ../system/cpus.c:445
#4  0x0000555559494348 in kvm_vcpu_thread_fn (arg=arg@entry=0x55557a3f79c0) at ../accel/kvm/kvm-accel-ops.c:56
#5  0x000055555964ecd0 in qemu_thread_start (args=<optimized out>) at ../util/qemu-thread-posix.c:541
#6  0x00007ffff17e4d90 in ?? () from target:/usr/lib64/libc.so.6
#7  0x00007ffff185f60c in ?? () from target:/usr/lib64/libc.so.6

Thread 13 (Thread 0x7fffd37fa940 (LWP 2463583) "CPU 4/KVM"):
#0  0x00007ffff17e0bb8 in ?? () from target:/usr/lib64/libc.so.6
#1  0x00007ffff17e4054 in pthread_cond_wait () from target:/usr/lib64/libc.so.6
#2  0x000055555964fc34 in qemu_cond_wait_impl (cond=0x55557a3f75d0, mutex=0x555559ca2fd0 <qemu_global_mutex>, file=0x555559736af8 "../system/cpus.c", line=<optimized out>) at ../util/qemu-thread-posix.c:225
#3  0x00005555592a4d44 in qemu_wait_io_event (cpu=cpu@entry=0x55557a3e5c80) at ../system/cpus.c:445
#4  0x0000555559494348 in kvm_vcpu_thread_fn (arg=arg@entry=0x55557a3e5c80) at ../accel/kvm/kvm-accel-ops.c:56
#5  0x000055555964ecd0 in qemu_thread_start (args=<optimized out>) at ../util/qemu-thread-posix.c:541
#6  0x00007ffff17e4d90 in ?? () from target:/usr/lib64/libc.so.6
#7  0x00007ffff185f60c in ?? () from target:/usr/lib64/libc.so.6

Thread 12 (Thread 0x7fffd3ffe940 (LWP 2463582) "CPU 3/KVM"):
--Type <RET> for more, q to quit, c to continue without paging--
#0  0x00007ffff17e0bb8 in ?? () from target:/usr/lib64/libc.so.6
#1  0x00007ffff17e4054 in pthread_cond_wait () from target:/usr/lib64/libc.so.6
#2  0x000055555964fc34 in qemu_cond_wait_impl (cond=0x55557a3e5870, mutex=0x555559ca2fd0 <qemu_global_mutex>, file=0x555559736af8 "../system/cpus.c", line=<optimized out>) at ../util/qemu-thread-posix.c:225
#3  0x00005555592a4d44 in qemu_wait_io_event (cpu=cpu@entry=0x55557a3d4080) at ../system/cpus.c:445
#4  0x0000555559494348 in kvm_vcpu_thread_fn (arg=arg@entry=0x55557a3d4080) at ../accel/kvm/kvm-accel-ops.c:56
#5  0x000055555964ecd0 in qemu_thread_start (args=<optimized out>) at ../util/qemu-thread-posix.c:541
#6  0x00007ffff17e4d90 in ?? () from target:/usr/lib64/libc.so.6
#7  0x00007ffff185f60c in ?? () from target:/usr/lib64/libc.so.6

Thread 11 (Thread 0x7ff7d17fa940 (LWP 2463581) "CPU 2/KVM"):
#0  0x00007ffff17e0bb8 in ?? () from target:/usr/lib64/libc.so.6
#1  0x00007ffff17e4054 in pthread_cond_wait () from target:/usr/lib64/libc.so.6
#2  0x000055555964fc34 in qemu_cond_wait_impl (cond=0x55557a3d3e70, mutex=0x555559ca2fd0 <qemu_global_mutex>, file=0x555559736af8 "../system/cpus.c", line=<optimized out>) at ../util/qemu-thread-posix.c:225
#3  0x00005555592a4d44 in qemu_wait_io_event (cpu=cpu@entry=0x55557a3c2680) at ../system/cpus.c:445
#4  0x0000555559494348 in kvm_vcpu_thread_fn (arg=arg@entry=0x55557a3c2680) at ../accel/kvm/kvm-accel-ops.c:56
#5  0x000055555964ecd0 in qemu_thread_start (args=<optimized out>) at ../util/qemu-thread-posix.c:541
#6  0x00007ffff17e4d90 in ?? () from target:/usr/lib64/libc.so.6
--Type <RET> for more, q to quit, c to continue without paging--
#7  0x00007ffff185f60c in ?? () from target:/usr/lib64/libc.so.6

Thread 10 (Thread 0x7ff7d0ff6940 (LWP 2463580) "CPU 1/KVM"):
#0  0x00007ffff17e0bb8 in ?? () from target:/usr/lib64/libc.so.6
#1  0x00007ffff17e4054 in pthread_cond_wait () from target:/usr/lib64/libc.so.6
#2  0x000055555964fc34 in qemu_cond_wait_impl (cond=0x55557a3c2470, mutex=0x555559ca2fd0 <qemu_global_mutex>, file=0x555559736af8 "../system/cpus.c", line=<optimized out>) at ../util/qemu-thread-posix.c:225
#3  0x00005555592a4d44 in qemu_wait_io_event (cpu=cpu@entry=0x55557a3b0c90) at ../system/cpus.c:445
#4  0x0000555559494348 in kvm_vcpu_thread_fn (arg=arg@entry=0x55557a3b0c90) at ../accel/kvm/kvm-accel-ops.c:56
#5  0x000055555964ecd0 in qemu_thread_start (args=<optimized out>) at ../util/qemu-thread-posix.c:541
#6  0x00007ffff17e4d90 in ?? () from target:/usr/lib64/libc.so.6
#7  0x00007ffff185f60c in ?? () from target:/usr/lib64/libc.so.6

Thread 9 (Thread 0x7ff7bbffe940 (LWP 2463579) "CPU 0/KVM"):
#0  0x00007ffff17e0bb8 in ?? () from target:/usr/lib64/libc.so.6
#1  0x00007ffff17e4054 in pthread_cond_wait () from target:/usr/lib64/libc.so.6
#2  0x000055555964fc34 in qemu_cond_wait_impl (cond=0x55557a3b0a80, mutex=0x555559ca2fd0 <qemu_global_mutex>, file=0x555559736af8 "../system/cpus.c", line=<optimized out>) at ../util/qemu-thread-posix.c:225
#3  0x00005555592a4d44 in qemu_wait_io_event (cpu=cpu@entry=0x55557a39f0f0) at ../system/cpus.c:445
--Type <RET> for more, q to quit, c to continue without paging--
#4  0x0000555559494348 in kvm_vcpu_thread_fn (arg=arg@entry=0x55557a39f0f0) at ../accel/kvm/kvm-accel-ops.c:56
#5  0x000055555964ecd0 in qemu_thread_start (args=<optimized out>) at ../util/qemu-thread-posix.c:541
#6  0x00007ffff17e4d90 in ?? () from target:/usr/lib64/libc.so.6
#7  0x00007ffff185f60c in ?? () from target:/usr/lib64/libc.so.6

Thread 8 (Thread 0x7ff7bb7fa940 (LWP 2463578) "IO mon_iothread"):
#0  0x00007ffff185398c in poll () from target:/usr/lib64/libc.so.6
#1  0x00007ffff1ba122c in ?? () from target:/usr/lib64/libglib-2.0.so.0
#2  0x00007ffff1ba1ecc in g_main_loop_run () from target:/usr/lib64/libglib-2.0.so.0
#3  0x00005555594de5d8 in iothread_run (opaque=opaque@entry=0x55557a0a8490) at ../iothread.c:70
#4  0x000055555964ecd0 in qemu_thread_start (args=<optimized out>) at ../util/qemu-thread-posix.c:541
#5  0x00007ffff17e4d90 in ?? () from target:/usr/lib64/libc.so.6
#6  0x00007ffff185f60c in ?? () from target:/usr/lib64/libc.so.6

Thread 7 (Thread 0x7fffee2f2940 (LWP 2463514) "IO iothread4"):
#0  0x00007ffff1853fd0 in ppoll () from target:/usr/lib64/libc.so.6
#1  0x0000555559669be4 in ppoll (__ss=0x0, __timeout=0x0, __nfds=<optimized out>, __fds=<optimized out>) at /usr/include/bits/poll2.h:88
#2  0x000055555964be50 in fdmon_poll_wait (ctx=0x55557a2b3250, ready_list=0x7fffee2f1f40, timeout=-1) at ../util/fdmon-poll.c:79
--Type <RET> for more, q to quit, c to continue without paging--
#3  0x000055555964b36c in aio_poll (ctx=0x55557a2b3250, blocking=blocking@entry=true) at ../util/aio-posix.c:670
#4  0x00005555594de584 in iothread_run (opaque=opaque@entry=0x55557a0db090) at ../iothread.c:63
#5  0x000055555964ecd0 in qemu_thread_start (args=<optimized out>) at ../util/qemu-thread-posix.c:541
#6  0x00007ffff17e4d90 in ?? () from target:/usr/lib64/libc.so.6
#7  0x00007ffff185f60c in ?? () from target:/usr/lib64/libc.so.6

Thread 6 (Thread 0x7fffeeaf6940 (LWP 2463512) "IO iothread3"):
#0  0x00007ffff1853fd0 in ppoll () from target:/usr/lib64/libc.so.6
#1  0x0000555559669be4 in ppoll (__ss=0x0, __timeout=0x0, __nfds=<optimized out>, __fds=<optimized out>) at /usr/include/bits/poll2.h:88
#2  0x000055555964be50 in fdmon_poll_wait (ctx=0x55557a2b0ea0, ready_list=0x7fffeeaf5f40, timeout=-1) at ../util/fdmon-poll.c:79
#3  0x000055555964b36c in aio_poll (ctx=0x55557a2b0ea0, blocking=blocking@entry=true) at ../util/aio-posix.c:670
#4  0x00005555594de584 in iothread_run (opaque=opaque@entry=0x55557a0db1a0) at ../iothread.c:63
#5  0x000055555964ecd0 in qemu_thread_start (args=<optimized out>) at ../util/qemu-thread-posix.c:541
#6  0x00007ffff17e4d90 in ?? () from target:/usr/lib64/libc.so.6
#7  0x00007ffff185f60c in ?? () from target:/usr/lib64/libc.so.6

Thread 5 (Thread 0x7fffef2fa940 (LWP 2463510) "IO iothread2"):
#0  0x00007ffff1853fd0 in ppoll () from target:/usr/lib64/libc.so.6
--Type <RET> for more, q to quit, c to continue without paging--
#1  0x0000555559669be4 in ppoll (__ss=0x0, __timeout=0x0, __nfds=<optimized out>, __fds=<optimized out>) at /usr/include/bits/poll2.h:88
#2  0x000055555964be50 in fdmon_poll_wait (ctx=0x55557a2b1af0, ready_list=0x7fffef2f9f40, timeout=-1) at ../util/fdmon-poll.c:79
#3  0x000055555964b36c in aio_poll (ctx=0x55557a2b1af0, blocking=blocking@entry=true) at ../util/aio-posix.c:670
#4  0x00005555594de584 in iothread_run (opaque=opaque@entry=0x55557a1440a0) at ../iothread.c:63
#5  0x000055555964ecd0 in qemu_thread_start (args=<optimized out>) at ../util/qemu-thread-posix.c:541
#6  0x00007ffff17e4d90 in ?? () from target:/usr/lib64/libc.so.6
#7  0x00007ffff185f60c in ?? () from target:/usr/lib64/libc.so.6

Thread 4 (Thread 0x7fffefafe940 (LWP 2463508) "IO iothread1"):
#0  0x00007ffff1853fd0 in ppoll () from target:/usr/lib64/libc.so.6
#1  0x0000555559669be4 in ppoll (__ss=0x0, __timeout=0x0, __nfds=<optimized out>, __fds=<optimized out>) at /usr/include/bits/poll2.h:88
#2  0x000055555964be50 in fdmon_poll_wait (ctx=0x55557a2af770, ready_list=0x7fffefafdf40, timeout=-1) at ../util/fdmon-poll.c:79
#3  0x000055555964b36c in aio_poll (ctx=0x55557a2af770, blocking=blocking@entry=true) at ../util/aio-posix.c:670
#4  0x00005555594de584 in iothread_run (opaque=opaque@entry=0x55557a175650) at ../iothread.c:63
#5  0x000055555964ecd0 in qemu_thread_start (args=<optimized out>) at ../util/qemu-thread-posix.c:541
#6  0x00007ffff17e4d90 in ?? () from target:/usr/lib64/libc.so.6
#7  0x00007ffff185f60c in ?? () from target:/usr/lib64/libc.so.6
--Type <RET> for more, q to quit, c to continue without paging--

Thread 3 (Thread 0x7ffff0302940 (LWP 2463507) "TC tc-ram-node0"):
#0  0x00007ffff17e0bb8 in ?? () from target:/usr/lib64/libc.so.6
#1  0x00007ffff17e4054 in pthread_cond_wait () from target:/usr/lib64/libc.so.6
#2  0x000055555964fc34 in qemu_cond_wait_impl (cond=0x55557a15ae78, mutex=0x55557a15ae40, file=0x5555597e5270 "../util/qemu-thread-posix.c", line=<optimized out>) at ../util/qemu-thread-posix.c:225
#3  0x0000555559650284 in qemu_sem_wait (sem=0x55557a15ae40) at ../util/qemu-thread-posix.c:314
#4  0x00005555596802c8 in thread_context_run (opaque=opaque@entry=0x55557a15ad90) at ../util/thread-context.c:81
#5  0x000055555964ecd0 in qemu_thread_start (args=<optimized out>) at ../util/qemu-thread-posix.c:541
#6  0x00007ffff17e4d90 in ?? () from target:/usr/lib64/libc.so.6
#7  0x00007ffff185f82c in ?? () from target:/usr/lib64/libc.so.6

Thread 2 (Thread 0x7ffff0c0a940 (LWP 2463499) "qemu-system-loo"):
#0  0x00007ffff185d1e8 in syscall () from target:/usr/lib64/libc.so.6
#1  0x0000555559650440 in qemu_futex_wait (val=<optimized out>, f=<optimized out>) at /usr/src/debug/qemu-8.2.0.939-571ed885b.loongarch64/include/qemu/futex.h:29
#2  qemu_event_wait (ev=ev@entry=0x555559cc9660 <rcu_call_ready_event>) at ../util/qemu-thread-posix.c:464
#3  0x000055555965ae44 in call_rcu_thread (opaque=opaque@entry=0x0) at ../util/rcu.c:278
#4  0x000055555964ecd0 in qemu_thread_start (args=<optimized out>) at ../util/qemu-thread-posix.c:541
--Type <RET> for more, q to quit, c to continue without paging--
#5  0x00007ffff17e4d90 in ?? () from target:/usr/lib64/libc.so.6
#6  0x00007ffff185f82c in ?? () from target:/usr/lib64/libc.so.6

Thread 1 (Thread 0x7ffff0c0f820 (LWP 2463493) "qemu-system-loo"):
#0  0x00007ffff1853fd0 in ppoll () from target:/usr/lib64/libc.so.6
#1  0x0000555559669b68 in ppoll (__ss=0x0, __timeout=0x7ffffbb45bc8, __nfds=<optimized out>, __fds=<optimized out>) at /usr/include/bits/poll2.h:88
#2  qemu_poll_ns (fds=<optimized out>, nfds=<optimized out>, timeout=<optimized out>) at ../util/qemu-timer.c:420
#3  0x0000555559666604 in os_host_main_loop_wait (timeout=1000000000) at ../util/main-loop.c:308
#4  main_loop_wait (nonblocking=nonblocking@entry=0) at ../util/main-loop.c:592
#5  0x00005555592b1924 in qemu_main_loop () at ../system/runstate.c:794
#6  0x0000555559498a84 in qemu_default_main () at ../system/main.c:39
#7  0x00007ffff17847c8 in ?? () from target:/usr/lib64/libc.so.6
#8  0x00007ffff17848b0 in __libc_start_main () from target:/usr/lib64/libc.so.6
#9  0x00005555590695c0 in _start ()
```

> TODO
>
> 待进一步分析
