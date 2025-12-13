## bt -f
```
Thread 5 (LWP 1515342):
#0  0x0000fffff6a5e610 in pthread_cond_wait () from /usr/lib64/libpthread.so.0
#1  0x0000aaaaab31f958 in qemu_cond_wait_impl (cond=0xaaaaac24c740, mutex=0xaaaaab93ef78 <qemu_global_mutex>, file=0xaaaaab35d7b0 "/root/rpmbuild/BUILD/qemu-4.1.0.30/cpus.c", line=1245) at util/qemu-thread-posix.c:161
#2  0x0000aaaaaadd4b2c in qemu_wait_io_event (cpu=cpu@entry=0xaaaaacc0d000) at /usr/src/debug/qemu-4.1.0.30-3253dce3.ky10.aarch64/cpus.c:1245
#3  0x0000aaaaaadd4ea0 in qemu_kvm_cpu_thread_fn (arg=0xaaaaacc0d000) at /usr/src/debug/qemu-4.1.0.30-3253dce3.ky10.aarch64/cpus.c:1290
#4  0x0000aaaaab31f15c in qemu_thread_start (args=0xaaaaac654900) at util/qemu-thread-posix.c:502
#5  0x0000fffff6a587a0 in ?? () from /usr/lib64/libpthread.so.0
#6  0x0000fffff699bcbc in ?? () from /usr/lib64/libc.so.6

Thread 4 (LWP 1515341):
#0  kvm_set_irq (s=0xaaaaac7b0000, irq=<optimized out>, level=<optimized out>) at /usr/src/debug/qemu-4.1.0.30-3253dce3.ky10.aarch64/accel/kvm/kvm-all.c:1272
#1  0x0000aaaaaaf203bc in write_cpustate_to_list (cpu=cpu@entry=0xaaaaacc00000, kvm_sync=kvm_sync@entry=true) at /usr/src/debug/qemu-4.1.0.30-3253dce3.ky10.aarch64/target/arm/helper.c:323
#2  0x0000aaaaaaf36578 in kvm_arch_put_registers (cs=0xaaaaacc00000, level=43690) at /usr/src/debug/qemu-4.1.0.30-3253dce3.ky10.aarch64/target/arm/kvm64.c:912
#3  0x0000aaaaaadf4114 in do_kvm_cpu_synchronize_post_init (cpu=0xaaaaacc00000, arg=...) at /usr/src/debug/qemu-4.1.0.30-3253dce3.ky10.aarch64/accel/kvm/kvm-all.c:2199
#4  0x0000aaaaaaffd350 in process_queued_cpu_work (cpu=0xaaaaacc00000) at cpus-common.c:342
#5  0x0000aaaaaadd4b50 in qemu_wait_io_event (cpu=cpu@entry=0xaaaaacc00000) at /usr/src/debug/qemu-4.1.0.30-3253dce3.ky10.aarch64/cpus.c:1254
#6  0x0000aaaaaadd4ea0 in qemu_kvm_cpu_thread_fn (arg=0xaaaaacc00000) at /usr/src/debug/qemu-4.1.0.30-3253dce3.ky10.aarch64/cpus.c:1290
#7  0x0000aaaaab31f15c in qemu_thread_start (args=0xaaaaac6540e0) at util/qemu-thread-posix.c:502
#8  0x0000fffff6a587a0 in ?? () from /usr/lib64/libpthread.so.0
#9  0x0000fffff699bcbc in ?? () from /usr/lib64/libc.so.6

Thread 3 (LWP 1515339):
#0  0x0000fffff68f7474 in sigtimedwait () from /usr/lib64/libc.so.6
#1  0x0000fffff6a63534 in sigwait () from /usr/lib64/libpthread.so.0
#2  0x0000aaaaab31d084 in sigwait_compat (opaque=0xaaaaac232d00) at util/compatfd.c:35
#3  0x0000aaaaab31f15c in qemu_thread_start (args=0xaaaaac652b60) at util/qemu-thread-posix.c:502
#4  0x0000fffff6a587a0 in ?? () from /usr/lib64/libpthread.so.0
#5  0x0000fffff699bcbc in ?? () from /usr/lib64/libc.so.6

Thread 2 (LWP 1515338):
#0  0x0000fffff6a61f4c in ?? () from /usr/lib64/libpthread.so.0
#1  0x0000fffff6a5adb0 in pthread_mutex_lock () from /usr/lib64/libpthread.so.0
#2  0x0000aaaaab31f2c4 in qemu_mutex_lock_impl (mutex=0xaaaaab93ef78 <qemu_global_mutex>, file=0xaaaaab4df968 "util/rcu.c", line=268) at util/qemu-thread-posix.c:66
#3  0x0000aaaaaadd4cb0 in qemu_mutex_lock_iothread_impl (file=file@entry=0xaaaaab4df968 "util/rcu.c", line=line@entry=268) at /usr/src/debug/qemu-4.1.0.30-3253dce3.ky10.aarch64/cpus.c:1859
#4  0x0000aaaaab332244 in call_rcu_thread (opaque=<optimized out>) at util/rcu.c:268
#5  0x0000aaaaab31f15c in qemu_thread_start (args=0xaaaaac3f57a0) at util/qemu-thread-posix.c:502
#6  0x0000fffff6a587a0 in ?? () from /usr/lib64/libpthread.so.0
#7  0x0000fffff699bcbc in ?? () from /usr/lib64/libc.so.6

Thread 1 (LWP 1515331):
#0  0x0000fffff6a5e610 in pthread_cond_wait () from /usr/lib64/libpthread.so.0
#1  0x0000aaaaab31f958 in qemu_cond_wait_impl (cond=0xaaaaab961dc8 <qemu_work_cond>, mutex=0xaaaaab93ef78 <qemu_global_mutex>, file=0xaaaaab3c34e8 "cpus-common.c", line=152) at ut--Type <RET> for more, q to quit, c to continue without paging--
il/qemu-thread-posix.c:161
#2  0x0000aaaaaaffcd28 in do_run_on_cpu (cpu=<optimized out>, func=<optimized out>, data=..., mutex=mutex@entry=0xaaaaab93ef78 <qemu_global_mutex>) at cpus-common.c:152
#3  0x0000aaaaaadd4980 in run_on_cpu (cpu=<optimized out>, func=<optimized out>, data=...) at /usr/src/debug/qemu-4.1.0.30-3253dce3.ky10.aarch64/cpus.c:1191
#4  0x0000000000355030 in ?? ()
#5  0x0000aaaaaadd4810 in cpu_synchronize_post_init (cpu=0xaaaaacc00000) at /usr/src/debug/qemu-4.1.0.30-3253dce3.ky10.aarch64/include/sysemu/hw_accel.h:48
#6  cpu_synchronize_all_post_init () at /usr/src/debug/qemu-4.1.0.30-3253dce3.ky10.aarch64/cpus.c:1052
#7  0x0000aaaaaad89a74 in main (argc=<optimized out>, argv=<optimized out>, envp=<optimized out>) at vl.c:4511
```
