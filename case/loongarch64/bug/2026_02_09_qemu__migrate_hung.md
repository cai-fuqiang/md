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
