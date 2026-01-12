## 现象
热迁移卡住，通过ps命令查看源端，发现源端有QEMU 线程D住。

```sh
Thread 24 (Thread 0x7fe666d2d640 (LWP 38054) "live_migration"):
#0  0x00007ff6f46912fe in ppoll () from target:/usr/lib64/libc.so.6
#1  0x00005640e1fd98fd in ppoll (__ss=0x0, __timeout=0x0, __nfds=<optimized out>, __fds=<optimized out>) at /usr/include/bits/poll2.h:81
#2  0x00005640e1fb6909 in fdmon_poll_wait (ctx=0x5640e315a720, ready_list=0x7fe666d2c248, timeout=-1) at ../util/fdmon-poll.c:80
#3  0x00005640e1fb5f5a in aio_poll (ctx=0x5640e315a720, blocking=blocking@entry=true) at ../util/aio-posix.c:607
#4  0x00005640e1eee93b in bdrv_do_drained_begin (bs=bs@entry=0x5640e339e720, recursive=recursive@entry=false, parent=parent@entry=0x0, ignore_bds_parents=ignore_bds_parents@entry=false, poll=poll@entry=true) at ../block/io.c:474
#5  0x00005640e1eeea92 in bdrv_do_drained_begin (poll=true, ignore_bds_parents=false, parent=0x0, recursive=false, bs=0x5640e339e720) at ../block/io.c:481
#6  0x00005640e1ec51c3 in bdrv_set_aio_context_ignore (bs=bs@entry=0x5640e339e720, new_context=new_context@entry=0x5640e315a720, ignore=ignore@entry=0x7fe666d2c350) at ../block.c:7110
#7  0x00005640e1ec6374 in bdrv_child_try_set_aio_context (bs=bs@entry=0x5640e339e720, ctx=ctx@entry=0x5640e315a720, ignore_child=<optimized out>, errp=errp@entry=0x0) at ../block.c:7249
#8  0x00005640e1ee4a6c in blk_do_set_aio_context (errp=0x0, update_root_node=true, new_context=0x5640e315a720, blk=0x5640e423a180) at ../block/block-backend.c:2116
#9  blk_set_aio_context (blk=0x5640e423a180, new_context=0x5640e315a720, errp=errp@entry=0x0) at ../block/block-backend.c:2137
#10 0x00005640e1d901e6 in virtio_blk_data_plane_stop (vdev=<optimized out>) at ../hw/block/dataplane/virtio-blk.c:337
#11 0x00005640e1ca7856 in virtio_bus_stop_ioeventfd (bus=0x5640e4235f00) at ../hw/virtio/virtio-bus.c:250
#12 virtio_bus_stop_ioeventfd (bus=0x5640e4235f00) at ../hw/virtio/virtio-bus.c:237
#13 0x00005640e1dded43 in virtio_vmstate_change (opaque=0x5640e4235f80, running=<optimized out>, state=<optimized out>) at ../hw/virtio/virtio.c:3276
#14 0x00005640e1d5cc30 in vm_state_notify (running=running@entry=false, state=state@entry=RUN_STATE_FINISH_MIGRATE) at ../softmmu/runstate.c:334
#15 0x00005640e1d4da4a in do_vm_stop (send_stop=true, state=RUN_STATE_FINISH_MIGRATE) at ../softmmu/cpus.c:262
#16 vm_stop (state=RUN_STATE_FINISH_MIGRATE) at ../softmmu/cpus.c:658
#17 0x00005640e1ad2074 in migration_completion (s=0x5640e33a4400) at ../migration/migration.c:3214
#18 migration_iteration_run (s=0x5640e33a4400) at ../migration/migration.c:3634
#19 migration_thread (opaque=opaque@entry=0x5640e33a4400) at ../migration/migration.c:3873
#20 0x00005640e1fb8c99 in qemu_thread_start (args=0x7fe666d2c690) at ../util/qemu-thread-posix.c:556
#21 0x00007ff6f461c32a in ?? () from target:/usr/lib64/libc.so.6
#22 0x00007ff6f469e370 in ?? () from target:/usr/lib64/libc.so.6
```
看起来是卡IO了。

但是在环境上执行`xxd -l xx /dev/nbdxxx`, 以及`iostat`  以及 `cat /sys/block/nbdxxx/infight` 来看,
均无卡IO的现象.

## coredump查看

使用gcore命令dump虚拟机内存。然后通过gdb查看
```
(gdb) p mon_vblkdevs.head->tqh_first->next->tqe_next->vdev->vq[8]
$41 = {vring = {num = 256, num_default = 256, align = 4096, desc = 68458545152, avail = 68458549248, used = 68458549824, caches = 0x7fe63080f220}, used_elems = 0x5640e425f420, last_avail_idx = 2440, last_avail_wrap_counter = true,
  shadow_avail_idx = 2440, shadow_avail_wrap_counter = true, used_idx = 2440, used_wrap_counter = true, signalled_used = 2440, signalled_used_valid = true, notification = true, queue_index = 8, inuse = 0, nr_request = 264584,
  nr_response = 264584, nr_errors = 0, error_code = 0, vector = 9, handle_output = 0x5640e1d8db30 <virtio_blk_handle_output>, handle_aio_output = 0x0, vdev = 0x5640e4235f80, guest_notifier = {rfd = 180, wfd = 180, initialized = true},
  host_notifier = {rfd = 196, wfd = 196, initialized = true}, host_notifier_enabled = true, node = {le_next = 0x0, le_prev = 0x5640e4240508}}
(gdb) p mon_vblkdevs.head->tqh_first->next->tqe_next->vdev->vq[0]
$42 = {vring = {num = 256, num_default = 256, align = 4096, desc = 68458389504, avail = 68458393600, used = 68458394176, caches = 0x7fe63080e6a0}, used_elems = 0x5640e42433a0, last_avail_idx = 57785, last_avail_wrap_counter = true,
  shadow_avail_idx = 57785, shadow_avail_wrap_counter = true, used_idx = 57785, used_wrap_counter = true, signalled_used = 57785, signalled_used_valid = true, notification = true, queue_index = 0, inuse = 0, nr_request = 188857,
  nr_response = 188857, nr_errors = 0, error_code = 0, vector = 1, handle_output = 0x5640e1d8db30 <virtio_blk_handle_output>, handle_aio_output = 0x0, vdev = 0x5640e4235f80, guest_notifier = {rfd = 172, wfd = 172, initialized = true},
  host_notifier = {rfd = 188, wfd = 188, initialized = true}, host_notifier_enabled = true, node = {le_next = 0x0, le_prev = 0x5640e42404c8}}
(gdb) p mon_vblkdevs.head->tqh_first->next->tqe_next->vdev->vq[1]
$43 = {vring = {num = 256, num_default = 256, align = 4096, desc = 68458397696, avail = 68458401792, used = 68458402368, caches = 0x7fe63080e810}, used_elems = 0x5640e4246bb0, last_avail_idx = 37509, last_avail_wrap_counter = true,
  shadow_avail_idx = 37509, shadow_avail_wrap_counter = true, used_idx = 37509, used_wrap_counter = true, signalled_used = 37509, signalled_used_valid = true, notification = true, queue_index = 1, inuse = 0, nr_request = 430725,
  nr_response = 430725, nr_errors = 0, error_code = 0, vector = 2, handle_output = 0x5640e1d8db30 <virtio_blk_handle_output>, handle_aio_output = 0x0, vdev = 0x5640e4235f80, guest_notifier = {rfd = 173, wfd = 173, initialized = true},
  host_notifier = {rfd = 189, wfd = 189, initialized = true}, host_notifier_enabled = true, node = {le_next = 0x0, le_prev = 0x5640e42404d0}}
(gdb) p mon_vblkdevs.head->tqh_first->next->tqe_next->vdev->vq[2]
$44 = {vring = {num = 256, num_default = 256, align = 4096, desc = 68458405888, avail = 68458409984, used = 68458410560, caches = 0x7fe63080e980}, used_elems = 0x5640e424a3c0, last_avail_idx = 40985, last_avail_wrap_counter = true,
  shadow_avail_idx = 40985, shadow_avail_wrap_counter = true, used_idx = 40985, used_wrap_counter = true, signalled_used = 40985, signalled_used_valid = true, notification = true, queue_index = 2, inuse = 0, nr_request = 368665,
  nr_response = 368665, nr_errors = 0, error_code = 0, vector = 3, handle_output = 0x5640e1d8db30 <virtio_blk_handle_output>, handle_aio_output = 0x0, vdev = 0x5640e4235f80, guest_notifier = {rfd = 174, wfd = 174, initialized = true},
  host_notifier = {rfd = 190, wfd = 190, initialized = true}, host_notifier_enabled = true, node = {le_next = 0x0, le_prev = 0x5640e42404d8}}
(gdb) p mon_vblkdevs.head->tqh_first->next->tqe_next->vdev->vq[3]
$45 = {vring = {num = 256, num_default = 256, align = 4096, desc = 68570791936, avail = 68570796032, used = 68570796608, caches = 0x7fe63080eaf0}, used_elems = 0x5640e424dbd0, last_avail_idx = 27738, last_avail_wrap_counter = true,
  shadow_avail_idx = 27738, shadow_avail_wrap_counter = true, used_idx = 27738, used_wrap_counter = true, signalled_used = 27738, signalled_used_valid = true, notification = true, queue_index = 3, inuse = 0, nr_request = 289882,
  nr_response = 289882, nr_errors = 0, error_code = 0, vector = 4, handle_output = 0x5640e1d8db30 <virtio_blk_handle_output>, handle_aio_output = 0x0, vdev = 0x5640e4235f80, guest_notifier = {rfd = 175, wfd = 175, initialized = true},
  host_notifier = {rfd = 191, wfd = 191, initialized = true}, host_notifier_enabled = true, node = {le_next = 0x0, le_prev = 0x5640e42404e0}}
(gdb) p mon_vblkdevs.head->tqh_first->next->tqe_next->vdev->vq[4]
$46 = {vring = {num = 256, num_default = 256, align = 4096, desc = 68581425152, avail = 68581429248, used = 68581429824, caches = 0x7fe63080ec60}, used_elems = 0x5640e42513e0, last_avail_idx = 46481, last_avail_wrap_counter = true,
  shadow_avail_idx = 46481, shadow_avail_wrap_counter = true, used_idx = 46479, used_wrap_counter = true, signalled_used = 46479, signalled_used_valid = true, notification = true, queue_index = 4, inuse = 2, nr_request = 243089,
  nr_response = 243087, nr_errors = 0, error_code = 0, vector = 5, handle_output = 0x5640e1d8db30 <virtio_blk_handle_output>, handle_aio_output = 0x0, vdev = 0x5640e4235f80, guest_notifier = {rfd = 176, wfd = 176, initialized = true},
  host_notifier = {rfd = 192, wfd = 192, initialized = true}, host_notifier_enabled = true, node = {le_next = 0x0, le_prev = 0x5640e42404e8}}
```
随便查看几个队列，发现有pending的`io`。（`nr_response < nr_request` , `avaid_index > used_idx`), 说明有io下发到内核，没有返回。
