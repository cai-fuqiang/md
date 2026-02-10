## 现象

热迁移后, qemu hung住, 源端目的端执行 `virsh xxx $instance-id` 均
会卡住

### 源端堆栈

```
(gdb) thread apply all bt

Thread 18 (Thread 0x7ff79b802940 (LWP 1652338) "live_migration"):
#0  0x00007ffff1828eb0 in ?? () from target:/usr/lib64/libc.so.6
#1  0x00007ffff1830730 in pthread_mutex_lock () from target:/usr/lib64/libc.so.6
#2  0x000055555601f148 in qemu_mutex_lock_impl (mutex=0x5555612f32b0, file=0x5555561b7960 "../util/async.c", line=<optimized out>) at ../util/qemu-thread-posix.c:94
#3  0x0000555555ef4518 in bdrv_do_drained_begin (poll=<optimized out>, parent=0x0, bs=0x55556134c1c0) at ../block/io.c:399
#4  bdrv_do_drained_begin (bs=0x55556134c1c0, parent=0x0, poll=<optimized out>) at ../block/io.c:368
#5  0x0000555555ee802c in blk_drain (blk=0x555564fc22f0) at ../block/block-backend.c:2120
#6  0x0000555555ac0728 in virtio_blk_data_plane_stop (vdev=<optimized out>) at ../hw/block/dataplane/virtio-blk.c:320
#7  0x0000555555c3bc7c in virtio_bus_stop_ioeventfd (bus=0x555564fbdf80) at ../hw/virtio/virtio-bus.c:272
#8  0x0000555555dd2868 in virtio_vmstate_change (opaque=0x555564fbe000, running=<optimized out>, state=<optimized out>) at ../hw/virtio/virtio.c:3315
#9  0x0000555555c80c50 in vm_state_notify (running=running@entry=false, state=state@entry=RUN_STATE_FINISH_MIGRATE) at ../system/runstate.c:382
#10 0x0000555555c757dc in do_vm_stop (send_stop=true, state=RUN_STATE_FINISH_MIGRATE) at ../system/cpus.c:284
#11 vm_stop (state=<optimized out>) at ../system/cpus.c:713
#12 0x0000555555caf5b4 in migration_stop_vm (state=RUN_STATE_FINISH_MIGRATE) at ../migration/migration.c:167
#13 migration_completion_precopy (current_active_state=0x7ff79b801f3c, s=0x5555613add00) at ../migration/migration.c:2667
#14 migration_completion (s=0x5555613add00) at ../migration/migration.c:2748
#15 migration_iteration_run (s=0x5555613add00) at ../migration/migration.c:3145
#16 migration_thread (opaque=opaque@entry=0x5555613add00) at ../migration/migration.c:3403
#17 0x000055555601ecd0 in qemu_thread_start (args=<optimized out>) at ../util/qemu-thread-posix.c:541
#18 0x00007ffff182cd90 in ?? () from target:/usr/lib64/libc.so.6
#19 0x00007ffff18a760c in ?? () from target:/usr/lib64/libc.so.6

Thread 17 (Thread 0x7ff7a689e940 (LWP 1646433) "vnc_worker"):
#0  0x00007ffff1828bb8 in ?? () from target:/usr/lib64/libc.so.6
#1  0x00007ffff182c054 in pthread_cond_wait () from target:/usr/lib64/libc.so.6
#2  0x000055555601fc34 in qemu_cond_wait_impl (cond=0x555562337b40, mutex=0x555562337b78, file=0x5555560a4f50 "../ui/vnc-jobs.c", line=<optimized out>) at ../util/qemu-thread-posix.c:225
#3  0x0000555555a7a760 in vnc_worker_thread_loop (queue=queue@entry=0x555562337b40) at ../ui/vnc-jobs.c:248
#4  0x0000555555a7b318 in vnc_worker_thread (arg=arg@entry=0x555562337b40) at ../ui/vnc-jobs.c:362
#5  0x000055555601ecd0 in qemu_thread_start (args=<optimized out>) at ../util/qemu-thread-posix.c:541
#6  0x00007ffff182cd90 in ?? () from target:/usr/lib64/libc.so.6
#7  0x00007ffff18a760c in ?? () from target:/usr/lib64/libc.so.6

Thread 16 (Thread 0x7ff7baff6940 (LWP 1646418) "CPU 7/KVM"):
#0  0x00007ffff1828bb8 in ?? () from target:/usr/lib64/libc.so.6
#1  0x00007ffff182c054 in pthread_cond_wait () from target:/usr/lib64/libc.so.6
#2  0x000055555601fc34 in qemu_cond_wait_impl (cond=0x55556146c910, mutex=0x555556672fd0 <qemu_global_mutex>, file=0x555556106af8 "../system/cpus.c", line=<optimized out>) at ../util/qemu-thread-posix.c:225
#3  0x0000555555c74d44 in qemu_wait_io_event (cpu=cpu@entry=0x55556145b120) at ../system/cpus.c:445
#4  0x0000555555e64348 in kvm_vcpu_thread_fn (arg=arg@entry=0x55556145b120) at ../accel/kvm/kvm-accel-ops.c:56
#5  0x000055555601ecd0 in qemu_thread_start (args=<optimized out>) at ../util/qemu-thread-posix.c:541
#6  0x00007ffff182cd90 in ?? () from target:/usr/lib64/libc.so.6
#7  0x00007ffff18a760c in ?? () from target:/usr/lib64/libc.so.6

Thread 15 (Thread 0x7ff7d1ffe940 (LWP 1646417) "CPU 6/KVM"):
#0  0x00007ffff1828bb8 in ?? () from target:/usr/lib64/libc.so.6
#1  0x00007ffff182c054 in pthread_cond_wait () from target:/usr/lib64/libc.so.6
#2  0x000055555601fc34 in qemu_cond_wait_impl (cond=0x55556145ad10, mutex=0x555556672fd0 <qemu_global_mutex>, file=0x555556106af8 "../system/cpus.c", line=<optimized out>) at ../util/qemu-thread-posix.c:225
#3  0x0000555555c74d44 in qemu_wait_io_event (cpu=cpu@entry=0x555561449520) at ../system/cpus.c:445
#4  0x0000555555e64348 in kvm_vcpu_thread_fn (arg=arg@entry=0x555561449520) at ../accel/kvm/kvm-accel-ops.c:56
#5  0x000055555601ecd0 in qemu_thread_start (args=<optimized out>) at ../util/qemu-thread-posix.c:541
#6  0x00007ffff182cd90 in ?? () from target:/usr/lib64/libc.so.6
#7  0x00007ffff18a760c in ?? () from target:/usr/lib64/libc.so.6

Thread 14 (Thread 0x7fffd2ff6940 (LWP 1646416) "CPU 5/KVM"):
#0  0x00007ffff1828bb8 in ?? () from target:/usr/lib64/libc.so.6
#1  0x00007ffff182c054 in pthread_cond_wait () from target:/usr/lib64/libc.so.6
#2  0x000055555601fc34 in qemu_cond_wait_impl (cond=0x555561449110, mutex=0x555556672fd0 <qemu_global_mutex>, file=0x555556106af8 "../system/cpus.c", line=<optimized out>) at ../util/qemu-thread-posix.c:225
#3  0x0000555555c74d44 in qemu_wait_io_event (cpu=cpu@entry=0x5555614379c0) at ../system/cpus.c:445
#4  0x0000555555e64348 in kvm_vcpu_thread_fn (arg=arg@entry=0x5555614379c0) at ../accel/kvm/kvm-accel-ops.c:56
#5  0x000055555601ecd0 in qemu_thread_start (args=<optimized out>) at ../util/qemu-thread-posix.c:541
#6  0x00007ffff182cd90 in ?? () from target:/usr/lib64/libc.so.6
--Type <RET> for more, q to quit, c to continue without paging--
#7  0x00007ffff18a760c in ?? () from target:/usr/lib64/libc.so.6

Thread 13 (Thread 0x7fffd37fa940 (LWP 1646415) "CPU 4/KVM"):
#0  0x00007ffff1828bb8 in ?? () from target:/usr/lib64/libc.so.6
#1  0x00007ffff182c054 in pthread_cond_wait () from target:/usr/lib64/libc.so.6
#2  0x000055555601fc34 in qemu_cond_wait_impl (cond=0x5555614375d0, mutex=0x555556672fd0 <qemu_global_mutex>, file=0x555556106af8 "../system/cpus.c", line=<optimized out>) at ../util/qemu-thread-posix.c:225
#3  0x0000555555c74d44 in qemu_wait_io_event (cpu=cpu@entry=0x555561425c80) at ../system/cpus.c:445
#4  0x0000555555e64348 in kvm_vcpu_thread_fn (arg=arg@entry=0x555561425c80) at ../accel/kvm/kvm-accel-ops.c:56
#5  0x000055555601ecd0 in qemu_thread_start (args=<optimized out>) at ../util/qemu-thread-posix.c:541
#6  0x00007ffff182cd90 in ?? () from target:/usr/lib64/libc.so.6
#7  0x00007ffff18a760c in ?? () from target:/usr/lib64/libc.so.6

Thread 12 (Thread 0x7fffd3ffe940 (LWP 1646414) "CPU 3/KVM"):
#0  0x00007ffff1828bb8 in ?? () from target:/usr/lib64/libc.so.6
#1  0x00007ffff182c054 in pthread_cond_wait () from target:/usr/lib64/libc.so.6
#2  0x000055555601fc34 in qemu_cond_wait_impl (cond=0x555561425870, mutex=0x555556672fd0 <qemu_global_mutex>, file=0x555556106af8 "../system/cpus.c", line=<optimized out>) at ../util/qemu-thread-posix.c:225
#3  0x0000555555c74d44 in qemu_wait_io_event (cpu=cpu@entry=0x555561414080) at ../system/cpus.c:445
#4  0x0000555555e64348 in kvm_vcpu_thread_fn (arg=arg@entry=0x555561414080) at ../accel/kvm/kvm-accel-ops.c:56
#5  0x000055555601ecd0 in qemu_thread_start (args=<optimized out>) at ../util/qemu-thread-posix.c:541
#6  0x00007ffff182cd90 in ?? () from target:/usr/lib64/libc.so.6
#7  0x00007ffff18a760c in ?? () from target:/usr/lib64/libc.so.6

Thread 11 (Thread 0x7ff7d17fa940 (LWP 1646413) "CPU 2/KVM"):
#0  0x00007ffff1828bb8 in ?? () from target:/usr/lib64/libc.so.6
#1  0x00007ffff182c054 in pthread_cond_wait () from target:/usr/lib64/libc.so.6
#2  0x000055555601fc34 in qemu_cond_wait_impl (cond=0x555561413e70, mutex=0x555556672fd0 <qemu_global_mutex>, file=0x555556106af8 "../system/cpus.c", line=<optimized out>) at ../util/qemu-thread-posix.c:225
#3  0x0000555555c74d44 in qemu_wait_io_event (cpu=cpu@entry=0x555561402680) at ../system/cpus.c:445
#4  0x0000555555e64348 in kvm_vcpu_thread_fn (arg=arg@entry=0x555561402680) at ../accel/kvm/kvm-accel-ops.c:56
#5  0x000055555601ecd0 in qemu_thread_start (args=<optimized out>) at ../util/qemu-thread-posix.c:541
#6  0x00007ffff182cd90 in ?? () from target:/usr/lib64/libc.so.6
#7  0x00007ffff18a760c in ?? () from target:/usr/lib64/libc.so.6

Thread 10 (Thread 0x7ff7d0ff6940 (LWP 1646412) "CPU 1/KVM"):
#0  0x00007ffff1828bb8 in ?? () from target:/usr/lib64/libc.so.6
#1  0x00007ffff182c054 in pthread_cond_wait () from target:/usr/lib64/libc.so.6
#2  0x000055555601fc34 in qemu_cond_wait_impl (cond=0x555561402470, mutex=0x555556672fd0 <qemu_global_mutex>, file=0x555556106af8 "../system/cpus.c", line=<optimized out>) at ../util/qemu-thread-posix.c:225
#3  0x0000555555c74d44 in qemu_wait_io_event (cpu=cpu@entry=0x5555613f0c90) at ../system/cpus.c:445
#4  0x0000555555e64348 in kvm_vcpu_thread_fn (arg=arg@entry=0x5555613f0c90) at ../accel/kvm/kvm-accel-ops.c:56
#5  0x000055555601ecd0 in qemu_thread_start (args=<optimized out>) at ../util/qemu-thread-posix.c:541
#6  0x00007ffff182cd90 in ?? () from target:/usr/lib64/libc.so.6
#7  0x00007ffff18a760c in ?? () from target:/usr/lib64/libc.so.6

Thread 9 (Thread 0x7ff7bbffe940 (LWP 1646411) "CPU 0/KVM"):
#0  0x00007ffff1828bb8 in ?? () from target:/usr/lib64/libc.so.6
#1  0x00007ffff182c054 in pthread_cond_wait () from target:/usr/lib64/libc.so.6
#2  0x000055555601fc34 in qemu_cond_wait_impl (cond=0x5555613f0a80, mutex=0x555556672fd0 <qemu_global_mutex>, file=0x555556106af8 "../system/cpus.c", line=<optimized out>) at ../util/qemu-thread-posix.c:225
#3  0x0000555555c74d44 in qemu_wait_io_event (cpu=cpu@entry=0x5555613df0f0) at ../system/cpus.c:445
#4  0x0000555555e64348 in kvm_vcpu_thread_fn (arg=arg@entry=0x5555613df0f0) at ../accel/kvm/kvm-accel-ops.c:56
#5  0x000055555601ecd0 in qemu_thread_start (args=<optimized out>) at ../util/qemu-thread-posix.c:541
#6  0x00007ffff182cd90 in ?? () from target:/usr/lib64/libc.so.6
#7  0x00007ffff18a760c in ?? () from target:/usr/lib64/libc.so.6

Thread 8 (Thread 0x7ff7bb7fa940 (LWP 1646410) "IO mon_iothread"):
#0  0x00007ffff189b98c in poll () from target:/usr/lib64/libc.so.6
#1  0x00007ffff1be922c in ?? () from target:/usr/lib64/libglib-2.0.so.0
#2  0x00007ffff1be9ecc in g_main_loop_run () from target:/usr/lib64/libglib-2.0.so.0
#3  0x0000555555eae5d8 in iothread_run (opaque=opaque@entry=0x5555610e8490) at ../iothread.c:70
#4  0x000055555601ecd0 in qemu_thread_start (args=<optimized out>) at ../util/qemu-thread-posix.c:541
#5  0x00007ffff182cd90 in ?? () from target:/usr/lib64/libc.so.6
#6  0x00007ffff18a760c in ?? () from target:/usr/lib64/libc.so.6

--Type <RET> for more, q to quit, c to continue without paging--
Thread 7 (Thread 0x7fffee33a940 (LWP 1646169) "IO iothread4"):
#0  0x00007ffff1828eb0 in ?? () from target:/usr/lib64/libc.so.6
#1  0x00007ffff18306a8 in pthread_mutex_lock () from target:/usr/lib64/libc.so.6
#2  0x000055555601f148 in qemu_mutex_lock_impl (mutex=0x555556672fd0 <qemu_global_mutex>, file=0x5555561482a0 "../system/physmem.c", line=<optimized out>) at ../util/qemu-thread-posix.c:94
#3  0x0000555555c74f74 in qemu_mutex_lock_iothread_impl (file=file@entry=0x5555561482a0 "../system/physmem.c", line=line@entry=2694) at ../system/cpus.c:519
#4  0x0000555555e05674 in prepare_mmio_access (mr=0x5555617840f0) at ../system/physmem.c:2694
#5  address_space_stl_internal (endian=DEVICE_LITTLE_ENDIAN, result=0x0, attrs=..., val=127, addr=<optimized out>, as=<optimized out>) at /usr/src/debug/qemu-8.2.0.939-571ed885b.loongarch64/memory_ldst.c.inc:318
#6  address_space_stl_le (as=<optimized out>, addr=<optimized out>, val=<optimized out>, attrs=..., result=0x0) at /usr/src/debug/qemu-8.2.0.939-571ed885b.loongarch64/memory_ldst.c.inc:357
#7  0x0000555555d8e628 in virtio_blk_rw_complete (opaque=<optimized out>, ret=<optimized out>) at ../hw/block/virtio-blk.c:144
#8  0x0000555555ee6d08 in blk_aio_complete (acb=0x7fffd8004b80) at ../block/block-backend.c:1606
#9  blk_aio_complete (acb=0x7fffd8004b80) at ../block/block-backend.c:1603
#10 blk_aio_read_entry (opaque=0x7fffd8004b80) at ../block/block-backend.c:1661
#11 0x0000555556038694 in coroutine_trampoline (i0=<optimized out>, i1=<optimized out>) at ../util/coroutine-ucontext.c:190
#12 0x00007ffff17e796c in ?? () from target:/usr/lib64/libc.so.6
#13 0x0000000000000000 in ?? ()
Backtrace stopped: frame did not save the PC

Thread 6 (Thread 0x7fffeeb3e940 (LWP 1646167) "IO iothread3"):
#0  0x00007ffff189bfd0 in ppoll () from target:/usr/lib64/libc.so.6
#1  0x0000555556039be4 in ppoll (__ss=0x0, __timeout=0x0, __nfds=<optimized out>, __fds=<optimized out>) at /usr/include/bits/poll2.h:88
#2  0x000055555601be50 in fdmon_poll_wait (ctx=0x5555612f0ea0, ready_list=0x7fffeeb3df40, timeout=-1) at ../util/fdmon-poll.c:79
#3  0x000055555601b36c in aio_poll (ctx=0x5555612f0ea0, blocking=blocking@entry=true) at ../util/aio-posix.c:670
#4  0x0000555555eae584 in iothread_run (opaque=opaque@entry=0x55556111b1a0) at ../iothread.c:63
#5  0x000055555601ecd0 in qemu_thread_start (args=<optimized out>) at ../util/qemu-thread-posix.c:541
#6  0x00007ffff182cd90 in ?? () from target:/usr/lib64/libc.so.6
#7  0x00007ffff18a760c in ?? () from target:/usr/lib64/libc.so.6

Thread 5 (Thread 0x7fffef342940 (LWP 1646165) "IO iothread2"):
#0  0x00007ffff189bfd0 in ppoll () from target:/usr/lib64/libc.so.6
#1  0x0000555556039be4 in ppoll (__ss=0x0, __timeout=0x0, __nfds=<optimized out>, __fds=<optimized out>) at /usr/include/bits/poll2.h:88
#2  0x000055555601be50 in fdmon_poll_wait (ctx=0x5555612f1af0, ready_list=0x7fffef341f40, timeout=-1) at ../util/fdmon-poll.c:79
#3  0x000055555601b36c in aio_poll (ctx=0x5555612f1af0, blocking=blocking@entry=true) at ../util/aio-posix.c:670
#4  0x0000555555eae584 in iothread_run (opaque=opaque@entry=0x5555611840a0) at ../iothread.c:63
#5  0x000055555601ecd0 in qemu_thread_start (args=<optimized out>) at ../util/qemu-thread-posix.c:541
#6  0x00007ffff182cd90 in ?? () from target:/usr/lib64/libc.so.6
#7  0x00007ffff18a760c in ?? () from target:/usr/lib64/libc.so.6

Thread 4 (Thread 0x7fffefb46940 (LWP 1646163) "IO iothread1"):
#0  0x00007ffff189bfd0 in ppoll () from target:/usr/lib64/libc.so.6
#1  0x0000555556039be4 in ppoll (__ss=0x0, __timeout=0x0, __nfds=<optimized out>, __fds=<optimized out>) at /usr/include/bits/poll2.h:88
#2  0x000055555601be50 in fdmon_poll_wait (ctx=0x5555612ef770, ready_list=0x7fffefb45f40, timeout=-1) at ../util/fdmon-poll.c:79
#3  0x000055555601b36c in aio_poll (ctx=0x5555612ef770, blocking=blocking@entry=true) at ../util/aio-posix.c:670
#4  0x0000555555eae584 in iothread_run (opaque=opaque@entry=0x5555611b5650) at ../iothread.c:63
#5  0x000055555601ecd0 in qemu_thread_start (args=<optimized out>) at ../util/qemu-thread-posix.c:541
#6  0x00007ffff182cd90 in ?? () from target:/usr/lib64/libc.so.6
#7  0x00007ffff18a760c in ?? () from target:/usr/lib64/libc.so.6

Thread 3 (Thread 0x7ffff034a940 (LWP 1646162) "TC tc-ram-node0"):
#0  0x00007ffff1828bb8 in ?? () from target:/usr/lib64/libc.so.6
#1  0x00007ffff182c054 in pthread_cond_wait () from target:/usr/lib64/libc.so.6
#2  0x000055555601fc34 in qemu_cond_wait_impl (cond=0x55556119ae78, mutex=0x55556119ae40, file=0x5555561b5270 "../util/qemu-thread-posix.c", line=<optimized out>) at ../util/qemu-thread-posix.c:225
#3  0x0000555556020284 in qemu_sem_wait (sem=0x55556119ae40) at ../util/qemu-thread-posix.c:314
#4  0x00005555560502c8 in thread_context_run (opaque=opaque@entry=0x55556119ad90) at ../util/thread-context.c:81
#5  0x000055555601ecd0 in qemu_thread_start (args=<optimized out>) at ../util/qemu-thread-posix.c:541
#6  0x00007ffff182cd90 in ?? () from target:/usr/lib64/libc.so.6
#7  0x00007ffff18a782c in ?? () from target:/usr/lib64/libc.so.6

Thread 2 (Thread 0x7ffff0c52940 (LWP 1646161) "qemu-system-loo"):
#0  0x00007ffff18a51e8 in syscall () from target:/usr/lib64/libc.so.6
#1  0x0000555556020440 in qemu_futex_wait (val=<optimized out>, f=<optimized out>) at /usr/src/debug/qemu-8.2.0.939-571ed885b.loongarch64/include/qemu/futex.h:29
#2  qemu_event_wait (ev=ev@entry=0x555556699660 <rcu_call_ready_event>) at ../util/qemu-thread-posix.c:464
--Type <RET> for more, q to quit, c to continue without paging--
#3  0x000055555602ae44 in call_rcu_thread (opaque=opaque@entry=0x0) at ../util/rcu.c:278
#4  0x000055555601ecd0 in qemu_thread_start (args=<optimized out>) at ../util/qemu-thread-posix.c:541
#5  0x00007ffff182cd90 in ?? () from target:/usr/lib64/libc.so.6
#6  0x00007ffff18a782c in ?? () from target:/usr/lib64/libc.so.6

Thread 1 (Thread 0x7ffff0c57820 (LWP 1646145) "qemu-system-loo"):
#0  0x00007ffff1828eb0 in ?? () from target:/usr/lib64/libc.so.6
#1  0x00007ffff18306a8 in pthread_mutex_lock () from target:/usr/lib64/libc.so.6
#2  0x000055555601f148 in qemu_mutex_lock_impl (mutex=0x555556672fd0 <qemu_global_mutex>, file=0x5555561b7da0 "../util/main-loop.c", line=<optimized out>) at ../util/qemu-thread-posix.c:94
#3  0x0000555555c74f74 in qemu_mutex_lock_iothread_impl (file=file@entry=0x5555561b7da0 "../util/main-loop.c", line=line@entry=311) at ../system/cpus.c:519
#4  0x0000555556036618 in os_host_main_loop_wait (timeout=887000000) at ../util/main-loop.c:311
#5  main_loop_wait (nonblocking=nonblocking@entry=0) at ../util/main-loop.c:592
#6  0x0000555555c81924 in qemu_main_loop () at ../system/runstate.c:794
#7  0x0000555555e68a84 in qemu_default_main () at ../system/main.c:39
#8  0x00007ffff17cc7c8 in ?? () from target:/usr/lib64/libc.so.6
#9  0x00007ffff17cc8b0 in __libc_start_main () from target:/usr/lib64/libc.so.6
#10 0x0000555555a395c0 in _start ()
```

很多线程都在抢`bql`。查看该锁是谁在占用:

```
(gdb) p qemu_global_mutex
$1 = {lock = {__data = {__lock = 2, __count = 0, __owner = 1652338, __nusers = 9, __kind = 0, __spins = 0, __list = {__prev = 0x0, __next = 0x0}}, __size = "\002\000\000\000\000\000\000\000r6\031\000\t", '\000' <repeats 26 times>,
    __align = 2}, file = 0x555556106af8 "../system/cpus.c", line = 608, initialized = true}
```

查看代码:

```
   23   void pause_all_vcpus(void)
   22   {
   21   │   qemu_clock_enable(QEMU_CLOCK_VIRTUAL, false);
   20   │
   19   retry:
   18   │   request_pause_all_vcpus();
   17   │
   16   │   /* We need to drop the replay_lock so any vCPU threads woken up
   15   │    * can finish their replay tasks
   14   │    */
   13   │   replay_mutex_unlock();
   12   │
   11   │   while (!all_vcpus_paused()) {
   10   │   │   qemu_cond_wait(&qemu_pause_cond, &qemu_global_mutex);
    9   │   │   /* During we waited on qemu_pause_cond the bql was unlocked,
    8   │   │    * the vcpu's state may has been changed by other thread, so
    7   │   │    * we must request the pause state on all vcpus again.
    6   │   │    */
    5   │   │   request_pause_all_vcpus();
    4   │   }
    3   │
    2   │   qemu_mutex_unlock_iothread();
    1   │   replay_mutex_lock();
  608   │   qemu_mutex_lock_iothread();   ## here
    1   │
    2   │   /* During the bql was unlocked, the vcpu's state may has been
    3   │    * changed by other thread, so we must retry.
    4   │    */
    5   │   if (!all_vcpus_paused()) {
    6   │   │   goto retry;
    7   │   }
    8   }
```

> TODO
>
> 待进一步分析

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

## 分析源端卡死

有大量的线程卡在了等bql锁。以thread 1(主线程) 为例.

```
#2  0x000055555601f148 in qemu_mutex_lock_impl (mutex=0x555556672fd0 <qemu_global_mutex>, file=0x5555561b7da0 "../util/main-loop.c", line=<optimized out>) at ../util/qemu-thread-posix.c:94
#3  0x0000555555c74f74 in qemu_mutex_lock_iothread_impl (file=file@entry=0x5555561b7da0 "../util/main-loop.c", line=line@entry=311) at ../system/cpus.c:519
#4  0x0000555556036618 in os_host_main_loop_wait (timeout=887000000) at ../util/main-loop.c:311
```

通过gdb可以获取到bql锁，是谁在获取 :

```
(gdb) p qemu_global_mutex
$38 = {lock = {__data = {__lock = 2, __count = 0, __owner = 1652338, __nusers = 9, __kind = 0, __spins = 0, __list = {__prev = 0x0, __next = 0x0}},
    __size = "\002\000\000\000\000\000\000\000r6\031\000\t", '\000' <repeats 26 times>, __align = 2}, file = 0x555556106af8 "../system/cpus.c", line = 608, initialized = true}
```

查看代码:

```
   23   void pause_all_vcpus(void)
   22   {
   21   │   qemu_clock_enable(QEMU_CLOCK_VIRTUAL, false);
 ...
  608   │   qemu_mutex_lock_iothread();
    1   │
    2   │   /* During the bql was unlocked, the vcpu's state may has been
    3   │    * changed by other thread, so we must retry.
    4   │    */
    5   │   if (!all_vcpus_paused()) {
    6   │   │   goto retry;
    7   │   }
    8   }
```

可以看得到是在vcpu pause 过程中触发了死锁.

而migrate线程在拿着bql锁后，在等待其他锁:

### which lock is main loop wait ?

```
Thread 18 (Thread 0x7ff79b802940 (LWP 1652338) "live_migration"):
#0  0x00007ffff1828eb0 in ?? () from target:/usr/lib64/libc.so.6
#1  0x00007ffff1830730 in pthread_mutex_lock () from target:/usr/lib64/libc.so.6
#2  0x000055555601f148 in qemu_mutex_lock_impl (mutex=0x5555612f32b0, file=0x5555561b7960 "../util/async.c", line=<optimized out>) at ../util/qemu-thread-posix.c:94
#3  0x0000555555ef4518 in bdrv_do_drained_begin (poll=<optimized out>, parent=0x0, bs=0x55556134c1c0) at ../block/io.c:399
#4  bdrv_do_drained_begin (bs=0x55556134c1c0, parent=0x0, poll=<optimized out>) at ../block/io.c:368
#5  0x0000555555ee802c in blk_drain (blk=0x555564fc22f0) at ../block/block-backend.c:2120
#6  0x0000555555ac0728 in virtio_blk_data_plane_stop (vdev=<optimized out>) at ../hw/block/dataplane/virtio-blk.c:320
#7  0x0000555555c3bc7c in virtio_bus_stop_ioeventfd (bus=0x555564fbdf80) at ../hw/virtio/virtio-bus.c:272
#8  0x0000555555dd2868 in virtio_vmstate_change (opaque=0x555564fbe000, running=<optimized out>, state=<optimized out>) at ../hw/virtio/virtio.c:3315
#9  0x0000555555c80c50 in vm_state_notify (running=running@entry=false, state=state@entry=RUN_STATE_FINISH_MIGRATE) at ../system/runstate.c:382
#10 0x0000555555c757dc in do_vm_stop (send_stop=true, state=RUN_STATE_FINISH_MIGRATE) at ../system/cpus.c:284
#11 vm_stop (state=<optimized out>) at ../system/cpus.c:713
#12 0x0000555555caf5b4 in migration_stop_vm (state=RUN_STATE_FINISH_MIGRATE) at ../migration/migration.c:167
#13 migration_completion_precopy (current_active_state=0x7ff79b801f3c, s=0x5555613add00) at ../migration/migration.c:2667
#14 migration_completion (s=0x5555613add00) at ../migration/migration.c:2748
#15 migration_iteration_run (s=0x5555613add00) at ../migration/migration.c:3145
#16 migration_thread (opaque=opaque@entry=0x5555613add00) at ../migration/migration.c:3403
#17 0x000055555601ecd0 in qemu_thread_start (args=<optimized out>) at ../util/qemu-thread-posix.c:541
#18 0x00007ffff182cd90 in ?? () from target:/usr/lib64/libc.so.6
#19 0x00007ffff18a760c in ?? () from target:/usr/lib64/libc.so.6
```

在等待哪个锁呢?

```
#4  bdrv_do_drained_begin (bs=0x55556134c1c0, parent=0x0, poll=<optimized out>) at ../block/io.c:368
368     static void bdrv_do_drained_begin(BlockDriverState *bs, BdrvChild *parent,
(gdb) p &bs->aio_context->lock
$47 = (QemuRecMutex *) 0x5555612f32b0
(gdb) f 2
#2  0x000055555601f148 in qemu_mutex_lock_impl (mutex=0x5555612f32b0, file=0x5555561b7960 "../util/async.c", line=<optimized out>) at ../util/qemu-thread-posix.c:94
94          err = pthread_mutex_lock(&mutex->lock);
```

可以发现是在等某个blkdev aio_context的lock, 另外在哪个代码中获取的呢?

```sh
bdrv_do_drained_begin
...
=> if (poll)
   => BDRV_POLL_WHILE(bs, bdrv_drain_poll_top_level(bs, parent));
      => AIO_WAIT_WHILE(bdrv_get_aio_context(bs_), cond)
         => AIO_WAIT_WHILE_INTERNAL(ctx, cond, true)
```

具体代码:

```
#define AIO_WAIT_WHILE_INTERNAL(ctx, cond, unlock) ({              \
    bool waited_ = false;                                          \
    AioWait *wait_ = &global_aio_wait;                             \
    AioContext *ctx_ = (ctx);                                      \
    /* Increment wait_->num_waiters before evaluating cond. */     \
    qatomic_inc(&wait_->num_waiters);                              \
    /* Paired with smp_mb in aio_wait_kick(). */                   \
    smp_mb__after_rmw();                                           \
    ## 判断是否该线程负责运行该ctx
    ## 如果是自己负责的ctx, 则不用管锁.
    if (ctx_ && in_aio_context_home_thread(ctx_)) {                \
        while ((cond)) {                                           \
            aio_poll(ctx_, true);                                  \
            waited_ = true;                                        \
        }                                                          \
    ## 否则, 考虑解锁ctx锁
    } else {                                                       \
        ## 这两个获取的都是bql，条件满足
        assert(qemu_get_current_aio_context() ==                   \
               qemu_get_aio_context());                            \
        while ((cond)) {                                           \
            ## 先解锁，在加锁 ???
            if (unlock && ctx_) {                                  \
                aio_context_release(ctx_);                         \
            }                                                      \
            aio_poll(qemu_get_aio_context(), true);                \
            if (unlock && ctx_) {                                  \
                aio_context_acquire(ctx_);                         \
            }                                                      \
            waited_ = true;                                        \
        }                                                          \
    }                                                              \
    qatomic_dec(&wait_->num_waiters);                              \
    waited_; })

/*
in_aio_context_home_thread
=> if (ctx == qemu_get_current_aio_context())
      ## qemu_get_current_aio_context
         ## 有aiocontext, 则表示其为iothread或者main loop
         => if get_my_aiocontext()
            => return ctx
         ## 如果被 bql  锁了，则返回bql, 在当前的堆栈应该返回bql
         => get_mutex_iothread_locked()
            => return qemu_get_aio_context()
         => return NULL
   => return true;
## 如果返回空， 并且ctx 为 global ctx, 则获取bql锁
=> if (ctx == qemu_get_aio_context()) {
   => return qemu_mutex_iothread_locked();
=> else
   => return false;
*/
```

这个代码似乎没有问题，在 `aio_poll()` 之前解锁(我这个理解是`aio_poll()`中可能会访问ctx，
在访问ctx之前，其会加锁。这里为了防止死锁). 在 aio_poll() 之后加锁。

### io context lock owner ?

而该lock是被谁获取呢?

```
(gdb) p bs->aio_context->lock->m->file
$50 = 0x5555561b7960 "../util/async.c"
(gdb) p bs->aio_context->lock->m->line
$51 = 728
    2   void aio_context_acquire(AioContext *ctx)
    1   {
  728   │   qemu_rec_mutex_lock(&ctx->lock);
    1   }
```

这样就不太好看了，我们首先找到这个blkdev对应的iothread是哪个?

```
#6  0x0000555555ac0728 in virtio_blk_data_plane_stop (vdev=<optimized out>) at ../hw/block/dataplane/virtio-blk.c:320
320         blk_drain(s->conf->conf.blk);
(gdb) p vblk->parent_obj->parent_obj->canonical_path
$57 = 0x5555614e7020 "/machine/peripheral/virtio-disk3/virtio-backend"
```

根据xml中的配置，可以发现是iothread4:

```xml
    <disk type='block' device='disk'>
      <... iothread='4' />
      <alias name='virtio-disk3'/>
    </disk>
```

iothread 4 是在协程:

```
#7  0x0000555555d8e628 in virtio_blk_rw_complete (opaque=<optimized out>, ret=<optimized out>) at ../hw/block/virtio-blk.c:144
#8  0x0000555555dd2868 in virtio_vmstate_change (opaque=0x555564fbe000, running=<optimized out>, state=<optimized out>) at ../hw/virtio/virtio.c:3315
#9  blk_aio_complete (acb=0x7fffd8004b80) at ../block/block-backend.c:1603
#10 blk_aio_read_entry (opaque=0x7fffd8004b80) at ../block/block-backend.c:1661
#11 0x0000555556038694 in coroutine_trampoline (i0=<optimized out>, i1=<optimized out>) at ../util/coroutine-ucontext.c:190
#12 0x00007ffff17e796c in ?? () from target:/usr/lib64/libc.so.6
#13 0x0000000000000000 in ?? ()
```

并且在`virtio_blk_rw_complete`函数开始 就会调用`aio_context_acquire` 获取该aio_context的锁:

```
     6   static void virtio_blk_rw_complete(void *opaque, int ret)
     5   {
     4   │   VirtIOBlockReq *next = opaque;
     3   │   VirtIOBlock *s = next->dev;
     2   │   VirtIODevice *vdev = VIRTIO_DEVICE(s);
     1   │
   111   │   aio_context_acquire(blk_get_aio_context(s->conf.conf.blk));
```

而其在后续函数中又在获取`bql`...

```
Thread 7 (Thread 0x7fffee33a940 (LWP 1646169) "IO iothread4"):
#0  0x00007ffff1828eb0 in ?? () from target:/usr/lib64/libc.so.6
#1  0x00007ffff18306a8 in pthread_mutex_lock () from target:/usr/lib64/libc.so.6
#2  0x000055555601f148 in qemu_mutex_lock_impl (mutex=0x555556672fd0 <qemu_global_mutex>, file=0x5555561482a0 "../system/physmem.c", line=<optimized out>) at ../util/qemu-thread-posix.c:94
#3  0x0000555555c74f74 in qemu_mutex_lock_iothread_impl (file=file@entry=0x5555561482a0 "../system/physmem.c", line=line@entry=2694) at ../system/cpus.c:519
#4  0x0000555555e05674 in prepare_mmio_access (mr=0x5555617840f0) at ../system/physmem.c:2694
#5  address_space_stl_internal (endian=DEVICE_LITTLE_ENDIAN, result=0x0, attrs=..., val=127, addr=<optimized out>, as=<optimized out>) at /usr/src/debug/qemu-8.2.0.939-571ed885b.loongarch64/memory_ldst.c.inc:318
#6  address_space_stl_le (as=<optimized out>, addr=<optimized out>, val=<optimized out>, attrs=..., result=0x0) at /usr/src/debug/qemu-8.2.0.939-571ed885b.loongarch64/memory_ldst.c.inc:357
#7  0x0000555555d8e628 in virtio_blk_rw_complete (opaque=<optimized out>, ret=<optimized out>) at ../hw/block/virtio-blk.c:144
```

axiba, 又是非常恶心人的搜不到定义的代码。。。

先查看virtio_blk_rw_complete代码位置:

```cpp
         static void virtio_blk_rw_complete(void *opaque, int ret)
         { ...
   144   │   │   virtio_blk_req_complete(req, VIRTIO_BLK_S_OK);
                 /*
                 => virtqueue_push(req->vq, &req->elem, req->in_len);
                    ## 放到used 队列
                    => virtqueue_fill(vq, elem, len, 0);
                    ## 个人怀疑是这个地方
                    => virtqueue_flush(vq, 1);
                       => virtqueue_split_flush(vq, count);
                          ## 修改used idx
                          => vring_used_idx_set(vq, new);
                 ## guest notify
                 */
           ... 
         }
```

`virtio_blk_rw_complete` 需要做的大概有三个:

* 将req 信息(VRingUsedElem)填充到used vring
* 更新used_idx
* 发送guest notify(event index机制按需发送)

我们看下操作vring memory的接口实现:

**更新`VRingUsedElem`**

```sh
virtqueue_fill
=> virtqueue_split_fill
   => uelem.id = elem->index;
   => uelem.len = elem->len;
   => vring_used_write(vq, &uelem, idx);
      ## 源地址, 是一个caches，由base + offset构成
      => VRingMemoryRegionCaches *caches = vring_get_region_caches(vq);
      ## 目的地
      => hwaddr pa = offsetof(VRingUsed, ring[i]);
      ## 先write
      => address_space_write_cached(&caches->used, pa, uelem, sizeof(VRingUsedElem));
         => if (likely(cache->ptr))
            ## 直接写内存
            => memcpy(cache->ptr + addr, buf, len);
            => return OK
         => else
            => address_space_write_cached_slow()
      ## 再invalidate(invl 只有tcg需要) 标脏。由于是qemu侧通过HVA->GPA的映射页表，
      ## 写操作并不会置位EPT的dirty bit，需要手动标脏
      => address_space_cache_invalidate(&caches->used, pa, sizeof(VRingUsedElem));
         => if (likely(cache->ptr))
            => invalidate_and_set_dirty(cache->mrs.mr, addr + cache->xlat, access_len);
               ## 置位qemu相关dirty bitmap
               => cpu_physical_memory_set_dirty_range(addr, length, dirty_log_mask);
```

**更新used_idx**

```sh
virtqueue_flush
=> virtqueue_split_flush()
   => vring_used_idx_set(vq, new);
      => if (caches)
         => VRingMemoryRegionCaches *caches = vring_get_region_caches(vq);
         => hwaddr pa = offsetof(VRingUsed, idx);
         => virtio_stw_phys_cached(vq->vdev, &caches->used, pa, val)
         => address_space_cache_invalidate(&caches->used, pa, sizeof(val));
```

两个操作差不多，都是去根据caches去写内存，并且无论是更新elem, 还是used idx, 其都是位于
Used ring结构的一个偏移处, 其本质上是在写guest memory RAM !!(这也是virtio 在虚拟化中高
效的原因之一: 通过共享内存实现 ring). 而前面看到, 其会调用到`prepare_mmio_access()`

```sh
address_space_stw_internal
=> RCU_READ_LOCK();
=> mr = TRANSLATE(addr, &addr1, &l, true, attrs);
=> if (l < 2 || !memory_access_is_direct(mr, true))
   => release_lock |= prepare_mmio_access(mr);
   => r = memory_region_dispatch_write(mr, addr1, val,
              MO_32 | devend_memop(endian), attrs);
```

说明最终转换到一个mmio地址, 我们先看下translate 后的mr

```
(gdb) p mr->name
$15 = 0x555561751eb0 "loongarch_pch_msi"
(gdb) p/x mr->addr
$17 = 0x2ff00000
```

通过`mtree` 可以找到该range:

```
  000000002ff00000-000000002ff00007 (prio 0, i/o): loongarch_pch_msi
```

找到的这个地址很奇怪，并且堆栈中调用到`address_space_stl`并非`address_space_stw`(这里有点失误，应该
早注意到是`stl`而非`stw`), 那就说明不是这里。

那只剩下一个流程: **send guest notify**, guest notify 是中断，我们来看下是哪个中断.

### send guest notify

```sh
virtio_blk_req_complete
=> if s->dataplane_started && !s->dataplane_disabled
   => virtio_blk_data_plane_notify(s->dataplane, req->vq);
=> else
   => virtio_notify(vdev, req->vq);
```

这个地方有两个分支 -- 根据是否`dataplane_started`来决定是否用
dataplane notify。在热迁移过程中，会通过 notify 通知device将
device stop，对应`virtio_blk_data_plane_stop()` 在该过程中会将
irqfd解绑，就需要通过原始的方式通知KVM。

我们来看下 virtio blk dataplane 状态:

```
(gdb) f 7
#7  0x0000555555d8e628 in virtio_blk_rw_complete (opaque=<optimized out>, ret=<optimized out>) at ../hw/block/virtio-blk.c:144
144             virtio_blk_req_complete(req, VIRTIO_BLK_S_OK);
(gdb) p req->dev->dataplane_started
$16 = false
```

并未有开启dataplane。所以走`virtio_notify`

```sh
virtio_notify
=> virtio_set_isr(vq->vdev, 0x1);
=> virtio_notify_vector(vq->vdev, vq->vector);
   => k->notify(qbus->parent, vector);
```

查找k(`VirtioBusClass`)

```
(gdb) p req->dev->parent_obj->parent_obj->parent_bus->obj.class
$84 = (ObjectClass *) 0x55556123c000
(gdb) p ((VirtioBusClass *)0x55556123c000)->notify
$86 = (void (*)(DeviceState *, uint16_t)) 0x555555c3dd80 <virtio_pci_notify>
```

查看`virtio_pci_notify` 代码:

```sh
virtio_pci_notify
=> if (msix_enabled(&proxy->pci_dev))
        => return (dev->cap_present & QEMU_PCI_CAP_MSIX) &&
              (dev->config[dev->msix_cap + MSIX_CONTROL_OFFSET] &
               MSIX_ENABLE_MASK);
   => if (vector != VIRTIO_NO_VECTOR)
      => msix_notify(&proxy->pci_dev, vector);
=> else
   => VirtIODevice *vdev = virtio_bus_get_device(&proxy->bus);
   => pci_set_irq(&proxy->pci_dev, qatomic_read(&vdev->isr) & 1);
```

有两个分支一个是irq一个是msix, 我们来获取相关数据结构:

```
(gdb) p req->dev->parent_obj->parent_obj->parent_bus->parent
$98 = (DeviceState *) 0x555564fb5bd0
(gdb) p ((VirtIOPCIProxy *)0x555564fb5bd0)
$99 = (VirtIOPCIProxy *) 0x555564fb5bd0
(gdb) p ((VirtIOPCIProxy *)0x555564fb5bd0)->pci_dev->cap_present
## QEMU_PCI_CAP_MSIX = 0x2, 满足
$104 = 2822
(gdb) p ((VirtIOPCIProxy *)0x555564fb5bd0)->pci_dev->msix_cap +  3
$108 = 223
(gdb) p ((VirtIOPCIProxy *)0x555564fb5bd0)->pci_dev->config[223]
$109 = 128 '\200'
## msix cap enable 也满足，不展开
```

所以综上所述，使用的是`msix`,  查看msix table 的msi address,
msi data.

```
(gdb) p ((VirtIOPCIProxy *)0x555564fb5bd0)->pci_dev->msix_table
$116 = (uint8_t *) 0x55556509b620 ""
(gdb) x/10xg 0x55556509b620
0x55556509b620: 0x000000002ff00000(msi_address)      0x000000000000007c(msi_data)
0x55556509b630: 0x000000002ff00000      0x000000000000007d
0x55556509b640: 0x000000002ff00000      0x000000000000007e
0x55556509b650: 0x000000002ff00000      0x000000000000007f
0x55556509b660: 0x000000002ff00000      0x0000000000000080
```

一个地址, 那就说明是发送到一个cpu的不同vector, 这个`msi_address`和前面定位的
`prepare_mmio_access()` 的mr 是一个地址。`msix_notify()`代码:

```sh
msix_notify
=> msg = msix_get_message(dev, vector);
   ## 从 msi_table 找到对应vector的 msix address 以及data的地址（两者连续)
   => uint8_t *table_entry = dev->msix_table + vector * PCI_MSIX_ENTRY_SIZE;
   ## 读出来
   => msg.address = pci_get_quad(table_entry + PCI_MSIX_ENTRY_LOWER_ADDR);
   => msg.data = pci_get_long(table_entry + PCI_MSIX_ENTRY_DATA);
=> msi_send_message(dev, msg);
   => attrs.requester_id = pci_requester_id(dev);
   ## 通过
   => address_space_stl_le(&dev->bus_master_as, msg.address, msg.data,
                         attrs, NULL);
```

这块是IO 地址个人感觉没有问题哈, X86是如何呢?

```
  00000000fee00000-00000000feefffff (prio 4096, i/o): kvm-apic-msi
```

x86对应`fee00000` lapic 地址，也是IO空间.

## 那问题来了? 到底是谁的问题?

目前的死锁是因为:

migrate thread 线程, `virtio_blk_data_plane_stop()` 先拿了`bql`锁，后在 `bdrv_drain()`
中又在申请 `ctx` 锁。

```
do_vm_stop
### !!!!!!!!!!!
### re lock BQL
### !!!!!!!!!!!
=> pause_all_vcpus 
=> vm_state_notify
   ...
   => virtio_blk_data_plane_stop
   ## get ctx->lock
   => aio_context_acquire(s->ctx);
   => blk_drain(s->conf->conf.blk);
      => bdrv_drained_begin(bs);
         => BDRV_POLL_WHILE(bs, bdrv_drain_poll_top_level(bs, parent));
            => AIO_WAIT_WHILE(bdrv_get_aio_context(bs_))
               => AIO_WAIT_WHILE_INTERNAL()
                  => aio_context_release(ctx_)
                  ### !!!!!!!!!!!
                  ### re  lock 
                  ### !!!!!!!!!!!
                  => aio_poll(qemu_get_aio_context(), true);
                  => aio_context_acquire(ctx_)
   => aio_context_release(s->ctx);
```

所以是先获取BQL，在获取ctx lock

而 iothread 以相反的方向获取锁

```
coroutine_trampoline
=> virtio_blk_rw_complete
### !!!!!!!!!!!
### get ctx lock
### !!!!!!!!!!!
   => aio_context_acquire(blk_get_aio_context(s->conf.conf.blk));

=> virtio_blk_req_complete(req, VIRTIO_BLK_S_OK);
    ...
   => address_space_stw_internal
      => prepare_mmio_access()
         ### !!!!!!!!!!!
         ### get ctx lock
         ### !!!!!!!!!!!
         => qemu_mutex_lock_iothread();
```

该线程是先获取ctx lock, 再获取bql.

避免死锁的最好的方式，是永远不要持有两个锁。如果非得这样做，就以相同的顺序
加锁，这显然违反了这个原则。

> [!NOTE]
>
> 本来想的是 `blk_drain()` 是导致这个问题的根本原因, 但是其只是直接原因，原因在于其
> 会增加获取锁的概率（循环获取)。
>
> 经调试，以下几个值是相等的.
>
> ```
> (gdb) p s->conf->conf.blk->root->bs->aio_context
> $17 = (AioContext *) 0x5555612f3250
> (gdb) p s->conf->conf.blk->ctx
> $18 = (AioContext *) 0x5555612f3250
> (gdb) p s->ctx
> $19 = (AioContext *) 0x5555612f3250
> ```

按也就是说，在`virtio_blk_data_plane_stop->aio_context_acquire(s->ctx)`
就会获取锁，触发死锁。
<!--
我们先来确认下这几个ctx 是如何赋值的:

* s->ctx(VirtioBlockDataPlane) -- 来自于 iothread->ctx

  ```
  virtio_blk_data_plane_create
  => if conf->iothread
     => s->iothread = conf->iothread;
     => s->ctx = iothread_get_aio_context(s->iothread);
        => return iothread->ctx
  ```

* `s->conf->conf.blk->ctx`(BlockBackend) (略过初始化流程)

  ```sh
  virtio_blk_data_plane_start
  => r = blk_set_aio_context(s->conf->conf.blk, s->ctx, &local_err);
     => !
  ```

* `bs->aio_context`(BlockDriverState->aio_context)

  ```sh
  blk_set_aio_context
  bdrv_try_change_aio_context
  bdrv_change_aio_context
  bdrv_set_aio_context_commit
  bdrv_attach_aio_context
  => bs->aio_context = new_context;
  ```
-->
