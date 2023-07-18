# 热迁移过程中丢失中断问题
## 调试过程
### 修改qemu代码, 增加调试信息
```cpp
diff --git a/hw/virtio/trace-events b/hw/virtio/trace-events
index e28ba48d..75c7cdcc 100644
--- a/hw/virtio/trace-events
+++ b/hw/virtio/trace-events
@@ -28,8 +28,10 @@ virtqueue_flush(void *vq, unsigned int count) "vq %p count %u"
 virtio_set_status(void *vdev, uint8_t val) "vdev %p val %u"
+virtio_split_should_notify(uint16_t old, uint16_t new, bool v, uint16_t used_event_idx, void* vdev, void* vq) "old %u new %u bool %d used_event_idx %u vdev %p vq %p"

static bool virtio_split_should_notify(VirtIODevice *vdev, VirtQueue *vq)
 {
-    uint16_t old, new;
+    uint16_t old, new, used_event_idx;
     bool v;
     /* We need to expose used array entries before checking used event. */
     smp_mb();
@@ -2387,7 +2386,9 @@ static bool virtio_split_should_notify(VirtIODevice *vdev, VirtQueue *vq)
     vq->signalled_used_valid = true;
     old = vq->signalled_used;
     new = vq->signalled_used = vq->used_idx;
-    return !v || vring_need_event(vring_get_used_event(vq), new, old);
+    used_event_idx =  vring_get_used_event(vq);
+    trace_virtio_split_should_notify(old, new, v, used_event_idx, vdev, vq);
+    return !v || vring_need_event(used_event_idx, new, old);
 }
 ```

主要是增加调试信息，获取`used_event_idx`

### 编写stap脚本，进行调试
在东旭提供的脚本上，进行修改:
```cpp
global count = 0
global print_cnt = 50;
global old_arr[128]
global new_arr[128];
global used_event_idx[128];
global last_avail_idx_arr[128];
global signalled_used_valid_arr[128];
global signalled_used_arr[128];

global func_enter_flag_arr[128];

global old_arr_bak[128];
global new_arr_bak[128];
global used_event_idx_bak[128];
global last_avail_idx_bak[128];
global signalled_used_valid_bak[128];
global signalled_used_bak[128];
global func_enter_flag_bak[128];
global count_bak;

global total_count = 0;

global func_virtio_notify_irqfd = 0x1;
global func_virtio_blk_data_plane_notify = 0x2
global func_virtio_notify = 0x4;

global func_trace_num = 3

global func_name_arr[3];

// global func_virtio_notify_irqfd = 0x4

global first_print = 1;

global enter_irq_complete = 0;
function print_all_msg()
{
        printf("new arr\n");
        printf("count is %d \n", count);
        for(i = 0; i < count; i++) {
                printf("idx(%d) old(%d) new(%d) used_event_idx(%d) last_avail_idx(%d) "
                        "sig_used_valid(%d) sig_used(%d)\n", i,
                        old_arr[i], new_arr[i], used_event_idx[i], last_avail_idx_arr[i],
                        signalled_used_valid_arr[i], signalled_used_arr[i]);
                printf("func: ");
                for (j = 0; j < func_trace_num; j++) {
                        if (func_enter_flag_arr[i] & 1 << j) {
                                printf("|%s", func_name_arr[j]);
                        }
                }
                if (j > 0) {
                        printf("|\n");
                } else {
                        printf("\n");
                }
        }
        if (!first_print) {
                printf("\nold arr\n");
                printf("old_count is %d \n", count_bak);
                for (i = 0; i < count_bak; i++) {
                        printf("idx(%d) old(%d) new(%d) used_event_idx(%d) last_avail_idx(%d) "
                                "sig_used_valid(%d) sig_used(%d)\n",
                                i, old_arr_bak[i], new_arr_bak[i],
                                used_event_idx_bak[i],
                                last_avail_idx_bak[i],
                                signalled_used_valid_bak[i],
                                signalled_used_bak[i]);
                         for (j = 0; j < func_trace_num; j++) {
                                if (func_enter_flag_bak[i] & 1 << j) {
                                        printf("|%s", func_name_arr[j]);
                                }
                        }
                        if (j > 0) {
                                printf("|\n");
                        } else {
                                printf("\n");
                        }
                }
        }
}
probe process("/usr/libexec/qemu-kvm").mark("virtio_notify_irqfd") {
        vdev=$arg1
        if (vdev != $1)
        {
                next;
        }
        if (count > print_cnt) {
                printf("%s, %d, vdev = %lx\n", probefunc(), count, vdev)
                printf("BEG(%d)=========================================\n", total_count);
                print_all_msg();
                printf("END(%d)=========================================\n\n\n", total_count);
                first_print = 0;
        }
        count_bak = count;
        for(i=0;i < count_bak;i++) {
                old_arr_bak[i] = old_arr[i];
                new_arr_bak[i] = new_arr[i];
                used_event_idx_bak[i] = used_event_idx[i];
                last_avail_idx_bak[i] =  last_avail_idx_arr[i];

                signalled_used_valid_bak[i] = signalled_used_valid_arr[i];
                signalled_used_bak[i] = signalled_used_arr[i];
                func_enter_flag_bak[i] = func_enter_flag_arr[i];
                func_enter_flag_arr[i] = 0;
        }
        total_count++;
        count = 0
}
probe process("/usr/libexec/qemu-kvm").mark("virtio_split_should_notify")
{
        //old, new, v, used_event_idx
        //vdev=$cast("VirtIODevice", $arg5)
        last_avail_idx = @cast($arg6, "VirtQueue")->last_avail_idx;
        signalled_used_valid = @cast($arg6, "VirtQueue")->signalled_used_valid;
        signalled_used = @cast($arg6, "VirtQueue")->signalled_used;
        if ($vdev != $1) {
                next;
        }
        if (count > print_cnt) {
                printf("BEG======virtio_split_should_notify=====\n");
                printf("count (%d)\n", count);
                printf("old(%d) new(%d) v(%d) used_event_idx(%d)\n", $arg1, $arg2, $arg3, $arg4);
                printf("vq->signalled_used_valid(%d) vq->last_avail_idx(%d)\n", signalled_used_valid,
                                last_avail_idx);
                printf("END======virtio_split_should_notify=====\n\n\n");
        }
        old_arr[count-1]=$arg1
        new_arr[count-1]=$arg2
        used_event_idx[count-1]=$arg4
        last_avail_idx_arr[count-1]=last_avail_idx
        signalled_used_valid_arr[count-1]=signalled_used_valid
        signalled_used_arr[count-1]=signalled_used
}

probe process("/usr/libexec/qemu-kvm").mark("virtio_blk_req_complete") {
        vdev=$arg1
       if(vdev == $1)    // $vdev是fio测试盘在qemu中的数据结构（struct VirtIODevice）
       {
                count = count + 1
       }
}
#probe process("/usr/libexec/qemu-kvm").statement("virtio_blk_req_complete@/usr/src/debug/qemu-kvm-4.2.0-59.es8_6_test.aarch64/hw/block/virtio-blk.c:80") {
probe process("/usr/libexec/qemu-kvm").statement("virtio_blk_req_complete@/usr/src/debug/qemu-4.2.0/hw/block/virtio-blk.c:82")
{
        if ($s->dataplane_started && !$s->dataplane_disabled) {
                next;
        }
        printf("s->dataplane_started(%d) s->dataplane_disabled(%d)\n",
                $s->dataplane_started, $s->dataplane_disabled);

}
probe process("/usr/libexec/qemu-kvm").function("virtio_blk_data_plane_notify") {
        if($vq->vdev == $1) {
                func_enter_flag_arr[count-1] |= func_virtio_blk_data_plane_notify;
        }
}

probe process("/usr/libexec/qemu-kvm").function("virtio_notify_irqfd")
{
        if($vq->vdev == $1) {
                func_enter_flag_arr[count-1] |= func_virtio_notify_irqfd;
        }
}

probe process("/usr/libexec/qemu-kvm").function("virtio_notify") {
        if($vq->vdev == $1) {
                func_enter_flag_arr[count-1] |= func_virtio_notify;
        }
}
probe begin {
        func_name_arr[0] = "virtio_notify_irqfd";
        func_name_arr[1] = "virtio_blk_data_plane_notify";
        func_name_arr[2] = "virtio_notify ";
        for (i=0; i < 128; i++) {
                func_enter_flag_arr[i] = 0;
                func_enter_flag_bak[i] = 0;
        }
}
probe end {
        printf("BEG(%d) end scripts=================================\n", total_count);
        printf("count = %ld\n", count)
        print_all_msg();
        printf("END(%d) end scripts=================================\n", total_count);
}
```
脚本比较长，主要注意以下几点:
* 在`virtio_blk_req_complete`中, 将`count++`, 表示收到后端存储设备请求。
* 在 `virtio_notify_irqfd`, 将count = 0, 表示已经发送了中断给kvm。
* 在`virtio_split_should_notify` 记录队列的一些状态，主要有`used_event_idx`, `old`, `new`, 
 `last_avail_idx`, 将其记录到一个数组中
* 在`virtio_notify_irqfd`, `virtio_notify`, `virtio_blk_data_plane_notify`函数执行后，在
 `func_enter_flag_arr`数组中，将相应位置1, 表示该函数执行过。
* 在`virtio_notify_irqfd`中，会将这一轮次的数组备份到 另一个数组中。在打印数组时，会将上一轮次
 的数组打印，方便对比。同时如果`count > print_count`, 则会打印数组。
* 在脚本退出时，打印数组。

我们直接看，脚本退出时，数组中的数据情况:
```
BEG(474178) end scripts=================================
count = 128
new arr
count is 128
/////////////(3)//////////////////
idx(0) old(33249) new(33250) used_event_idx(33249) last_avail_idx(33377) sig_used_valid(1) sig_used(33250)
/////////////(5)//////////////////
func: |virtio_notify |
idx(1) old(33250) new(33251) used_event_idx(33249) last_avail_idx(33377) sig_used_valid(1) sig_used(33251)
func: |virtio_notify |
idx(2) old(33251) new(33252) used_event_idx(33249) last_avail_idx(33377) sig_used_valid(1) sig_used(33252)
func: |virtio_notify |
idx(3) old(33252) new(33253) used_event_idx(33249) last_avail_idx(33377) sig_used_valid(1) sig_used(33253)
func: |virtio_notify |
...
省略类似行
...
idx(126) old(33375) new(33376) used_event_idx(33249) last_avail_idx(33377) sig_used_valid(1) sig_used(33376)
func: |virtio_notify |
/////////////(4)//////////////////
idx(127) old(33376) new(33377) used_event_idx(33249) last_avail_idx(33377) sig_used_valid(1) sig_used(33377)
func: |virtio_notify |

old arr
old_count is 1
/////////////(1)//////////////////
idx(0) old(33248) new(33249) used_event_idx(33248) last_avail_idx(33376) sig_used_valid(1) sig_used(33249)
/////////////(2)//////////////////
|virtio_notify_irqfd|virtio_blk_data_plane_notify|
END(474178) end scripts=================================
```
1. 该处表示，上一轮发中断时，队列的状态，这里需要注意的是，此次处理，实际上是新一轮数据的请求。
(guest 收到上一轮次的中断，更新vring->avail_idx,  vring->used_event_idx, 然后notify guest, guest 
收到notify， 更新last_avail_idx, 然后下发请求到后端存储设备， 后端存储设备IO completion, 通知qemu，
qemu进行处理， 此时，qemu就是处理的这个数据）。并且此时，队列是满的(last_avail_idx  - old(old_used_idx) = 128)
2. 并且可以看到，此次发中断的流程仍然是通过`virtio_notify_irqfd()`完成
3. 发完中断后，guest收到请求，更新vring->used_event_idx, vring->avail_idx, 此时，guest实际上又往队列中avail
vring中放了个数据，avail vring 仍然是满的(128), 此时old == used_event_idx，这时应该会发中断，但是并没有通过
`virtio_notify_irqfd`函数发送，所以count 没有置0.
4. 实际上到idx = 127 这个点，还是不需要发送中断的, 因为 old > used_event_idx。此时guest 应该是还没有收到上一
轮发送的中断: (3)这个点发送的中断。
5. 在(3)处，并没有通过`virtio_notify_irqfd`, 而是执行了`virtio_notify` 这个函数，那么该函数是怎么执行到的呢？
有没有发送中断呢? 我们来调试下。

`virtio_blk_req_complete`:
```cpp
static void virtio_blk_req_complete(VirtIOBlockReq *req, unsigned char status)
{
	...
	if (s->dataplane_started && !s->dataplane_disabled) {
            //一般走走个流程
	    virtio_blk_data_plane_notify(s->dataplane, req->vq);
	} else {
	    //热迁移过程中走了这个函数
	    virtio_notify(vdev, req->vq);
	}
}
```
我们再来看下， 上面的判断条件如何变化的：
在热迁移中，会走下面的流程:
```
vm_stop
  do_vm_stop
    vm_state_notify {
        QTAILQ_FOREACH_REVERSE_SAFE(e, &vm_change_state_head, entries, next) {
          e->cb(e->opaque, running, state);
        }
    }//vm_state_notify 
```
在virtio设备初始化时，会将自己的callback加到该链表中:
```
void virtio_init(VirtIODevice *vdev, const char *name,
                 uint16_t device_id, size_t config_size)
{
	//该接口会加，这里不再展开
	vdev->vmstate = qdev_add_vm_change_state_handler(DEVICE(vdev),
	        virtio_vmstate_change, vdev);
}

virtio_vmstate_change
  k->vmstate_change() //virtio_pci_vmstate_change
    virtio_pci_stop_ioeventfd
      virtio_bus_stop_ioeventfd
        vdc->stop_ioeventfd() //virtio_blk_data_plane_stop
```
`virtio_blk_data_plane_stop`:
```cpp
void virtio_blk_data_plane_stop(VirtIODevice *vdev)
{
	...
	aio_context_release(s->ctx);
	///////////(1)////////////////
	for (i = 0; i < nvqs; i++) {
	    virtio_bus_set_host_notifier(VIRTIO_BUS(qbus), i, false);
	    virtio_bus_cleanup_host_notifier(VIRTIO_BUS(qbus), i);
	}
	
	qemu_bh_cancel(s->bh);
	notify_guest_bh(s); /* final chance to notify guest */
	
	/* Clean up guest notifier (irq) */
	///////////(2)////////////////
	k->set_guest_notifiers(qbus->parent, nvqs, false);
	
	///////////(3)////////////////
	vblk->dataplane_started = false;
	s->stopping = false;
}
```
1. 这里会释放所有的host_notify(关闭eventfd, 不再展开)
2. 这里会release 所有中队列中断的irqfd
3. 将`vblk->dataplane_started`, 置为false。

可见，此时qemu 已经不想接收 driver（guest)传过来的请求。
也放弃了通过irqfd发送中断的方式。

但是刚刚也提到了，`virtio_blk_req_complete`中会走第二个分支
`virtio_notify`, 个人感觉可能是为了处理，在关闭host_notifiers
之前发送给后端存储的请求。

`virtio_notify`相关流程:
```cpp
void virtio_notify(VirtIODevice *vdev, VirtQueue *vq)
{
    WITH_RCU_READ_LOCK_GUARD() {
	//这里也会判断是否要发送notify给guest kernel
        if (!virtio_should_notify(vdev, vq)) {
            return;
        }
    }

    trace_virtio_notify(vdev, vq);
    virtio_irq(vq);
}
static void virtio_irq(VirtQueue *vq)
{
    virtio_set_isr(vq->vdev, 0x1);
    virtio_notify_vector(vq->vdev, vq->vector);
}
static void virtio_notify_vector(VirtIODevice *vdev, uint16_t vector)
{
    BusState *qbus = qdev_get_parent_bus(DEVICE(vdev));
    VirtioBusClass *k = VIRTIO_BUS_GET_CLASS(qbus);

    if (unlikely(vdev->broken)) {
        return;
    }

    if (k->notify) {
	//在这里发送的notify, 为 virtio_pci_notify
        k->notify(qbus->parent, vector);
    }
}
```
后面的流程比较复杂，我们简单列下堆栈:
```
kvm_its_send_msi
gicv3_its_trans_write
memory_region_write_with_attrs_accessor
access_with_adjusted_size
memory_region_dispatch_write
address_space_stl_le
msi_send_message
msix_notify
virtio_notify_vector
```
其大概流程为模拟设备发送msi的流程，msi对于pci设备来说，
是对中断控制器地址进行write操作。在`address_space_stl_le`
函数流程中会找到其MemoryRegion(mr), 然后在
`memory_region_write_with_attrs_accessor`函数中，
会调用该mr的`write_with_attrs`函数指针(gicv3_its_trans_write)。

而`kvm_its_send_msi()`, 函数最终会调用`kvm_vm_ioctl(kvm_state, 
KVM_SIGNAL_MSI, &msi);`发送中断请求给kvm。

所以说从这里来看，qemu还是发送中断给kvm，但是迁移过程中，貌似丢失了.

## LPI Pending tables
一开始想看下，qemu这边有没有去迁移`ICH_LR_EL2`, 但是后来经过调试，发现
并没有迁移该寄存器。但是还有一个地方可能会保存intr status的信息:
`LPI Pending tables`(在gicv3,v4 spec 5.1.2 LPI Pending tables)章节有讲。
简单来说，OS可以配置`GICR_PENDBASER`寄存器，指向一块内存，该内存中保存这
LPI interrupt的status信息。我们知道LPI中断只有pending, inactive两个状态，
所以, 仅用一个bit 就可以表示该中断的状态。
对于该LPI来说，状态bit计算如下:
```
The corresponding byte in the LPI Pending table is (base address + (N / 8)).
The bit position in the byte is (N MOD 8).
```
> NOTE
>
> 该寄存器是GICR_* 系列的寄存器，GIC virt并不支持该方面的虚拟化（目前只支持ICC(ICV)),
> 所以我这边怀疑如果guest要访问该memory，可能还是会trap el2去处理，但是还未调试
> 该流程。

我这里采用了一种比较笨的方法查找kernel调用流程:
```
probe process("/usr/libexec/qemu-kvm").function("vm_stop")
{
        printf("enter %s\n", ppfunc());
        vm_stop = 1;
        vm_stop_pid = pid();
}
probe kernel.function("*@arch/arm64/kvm/hyp/../../../../virt/kvm/arm/hyp/vgic-v3-sr.c")
{
        if (!vm_stop || vm_stop_pid != pid()) {
                next;
        }
        printf("enter %s\n", ppfunc());
}

probe kernel.function("*@arch/arm64/kvm/../../../virt/kvm/arm/vgic/vgic.c")
{
        if (!vm_stop || vm_stop_pid != pid()) {
                next;
        }
        printf("enter %s\n", ppfunc());
}
```
相当于在`vm_stop`之后，获取kernel vgic, vgicv3的全部动作, 其中找到了一个相关堆栈:
```
pid 32190 func vgic_v3_save_pending_tables
 0xffff0000100ca140 : vgic_v3_save_pending_tables+0x0/0x198 [kernel]
 0xffff0000100ca140 : vgic_v3_save_pending_tables+0x0/0x198 [kernel]
 0xffff0000100d051c : vgic_v3_set_attr+0x3c/0x48 [kernel]
 0xffff0000100a75dc : kvm_device_ioctl_attr+0x8c/0xc0 [kernel]
 0xffff0000100a76b4 : kvm_device_ioctl+0xa4/0x108 [kernel]
 0xffff0000103259bc : do_vfs_ioctl+0xc4/0x890 [kernel]
 0xffff00001032620c : ksys_ioctl+0x84/0xb8 [kernel]
 0xffff000010326268 : __arm64_sys_ioctl+0x28/0x38 [kernel]
 0xffff000010095378 : el0_svc_handler+0xa0/0x128 [kernel]
 0xffff000010083788 : el0_svc+0x8/0xc [kernel]
 0xffffaf0060ec [/usr/lib64/libc-2.17.so+0xd60ec/0x190000]
```
`vgic_v3_save_pending_tables`:
```cpp
int vgic_v3_save_pending_tables(struct kvm *kvm)
{
        struct vgic_dist *dist = &kvm->arch.vgic;
        int last_byte_offset = -1;
        struct vgic_irq *irq;
        int ret;
        u8 val;
	/////////////////(1)/////////////////
        list_for_each_entry(irq, &dist->lpi_list_head, lpi_list) {
                int byte_offset, bit_nr;
                struct kvm_vcpu *vcpu;
                gpa_t pendbase, ptr;
                bool stored;

                vcpu = irq->target_vcpu;
                if (!vcpu)
                        continue;
		/////////////////(2)/////////////////
                pendbase = GICR_PENDBASER_ADDRESS(vcpu->arch.vgic_cpu.pendbaser);

		/////////////////(3)/////////////////
                byte_offset = irq->intid / BITS_PER_BYTE;
                bit_nr = irq->intid % BITS_PER_BYTE;
                ptr = pendbase + byte_offset;

                if (byte_offset != last_byte_offset) {
			/////////////////(4)/////////////////
                        ret = kvm_read_guest_lock(kvm, ptr, &val, 1);
                        if (ret)
                                return ret;
                        last_byte_offset = byte_offset;
                }

                stored = val & (1U << bit_nr);
		/////////////////(5)/////////////////
                if (stored == irq->pending_latch)
                        continue;

		/////////////////(6)/////////////////
                if (irq->pending_latch)
                        val |= 1 << bit_nr;
                else
                        val &= ~(1 << bit_nr);

		/////////////////(7)/////////////////
                ret = kvm_write_guest_lock(kvm, ptr, &val, 1);
                if (ret)
                        return ret;
        }
        return 0;
}
```
该函数流程比较简单:
1. 遍历lpi_list_head中的每一项，获取 vgic_irq
2. 获取pendbase(GICR_PENDBASER)
3. 计算其位置
4. 获取guest page 中原有数据
5. 判断原有数据和irq中保存的中断状态是否一致，如果一致continue
6. 如果一致，更新中断状态
7. 将新值写会guest page

我们看其是在何时执行的:
```
global func1_cnt = 0
global vm_stop_pid = 0;
global virtio_notify_trace_on  = 0;
//////////////////(1)////////////////
probe process("/usr/libexec/qemu-kvm").mark("virtio_notify_irqfd")
{
        if (pid() != $1) {
                next;
        }
        while (func1_cnt < 10)
        {
              printf("pid %d func  virtio_notify_irqfd\n", pid());
              func1_cnt++;
        }
}

probe process("/usr/libexec/qemu-kvm").function("vm_stop")
{
        if (pid() == $1) {
                virtio_notify_trace_on = 1;
                printf("vm stop \n");
        }
}
//////////////////(2)////////////////
probe process("/usr/libexec/qemu-kvm").statement("kvm_its_send_msi@/usr/src/debug/qemu-4.2.0/hw/intc/arm_gicv3_its_kvm.c:65")
{
        if (pid() != $1 || !virtio_notify_trace_on) {
                next;
        }
        printf("func(%s) msi->data(%u) msi->devid(%u) msi->data(%u) \n", ppfunc(), $msi->data, $msi->devid, $msi->data);
        print_ubacktrace();
}
//////////////////(3)////////////////
probe kernel.statement("vgic_v3_save_pending_tables@arch/arm64/kvm/../../../virt/kvm/arm/vgic/vgic-v3.c:387")
{
        if (pid() != $1 || !virtio_notify_trace_on) {
                next;
        }
        printf("vgic_v3_save_pending_tables:387: irq->intid(%u) irq->pending_latch(%d)\n",
                $irq->intid,  $irq->pending_latch);

}
probe kernel.statement("vgic_v3_save_pending_tables@arch/arm64/kvm/../../../virt/kvm/arm/vgic/vgic-v3.c:413")
{
        if (pid() != $1 || !virtio_notify_trace_on) {
                next;
        }
        printf("func(vgic_v3_save_pending_tables:387): irq->intid(%u) irq->pending_latch(%d) value (%d)\n",
                $irq->intid,  $irq->pending_latch, $val);

}

probe process("/usr/libexec/qemu-kvm").function("kvm_device_access")
{
        if (pid() != $1 || !virtio_notify_trace_on) {
                next;
        }
        printf("func(%s) group(%d) attr(%lx) \n", ppfunc(), $group, $attr);
        if ($group == 4 && $attr == 3) {
                print_ubacktrace();
        }
}

probe kernel.function("vgic_its_inject_msi")
{
        if (pid() != $1 || !virtio_notify_trace_on) {
                next;
        }

        printf("func(%s) : devid(%u) eventid(%u)\n", ppfunc(), $msi->devid, $msi->data);
}


probe kernel.statement("vgic_its_trigger_msi@arch/arm64/kvm/../../../virt/kvm/arm/vgic/vgic-its.c:611")
{
        if (pid() != $1 || !virtio_notify_trace_on) {
                next;
        }
        printf("func(%s:611) : irq->intid(%u), devid(%u) eventid(%u)\n", ppfunc(), $irq->intid, $devid, $eventid);
}
```
1. 这里实际上为了方便调试，使用systemtap 时，可能会遇到本次执行不成功的情况，所以，抓了一个
频繁执行的函数，如果在执行stap -v + stap脚本后，没有相关打印，就说明本次stap动作失败了。
2. 这里抓了下，发送中断的时间点
3. 这里抓了lpi_list_head中所有中断的状态

其他的probe点，均和中断发送有关。我们来看下调试输出：
```
vm stop
func(kvm_device_access) group(4) attr(1)
func(kvm_device_access) group(4) attr(3)
 //////////////////(1)//////////////////
 0xaaaac0869dac : kvm_device_access+0x0/0x10c [/usr/libexec/qemu-kvm]
 0xaaaac08931b8 : vm_change_state_handler+0x60/0x84 [/usr/libexec/qemu-kvm]
 0xaaaac09b7068 : vm_state_notify+0xb4/0xc8 [/usr/libexec/qemu-kvm]
 0xaaaac084ed18 : do_vm_stop+0x48/0x60 [/usr/libexec/qemu-kvm]
 0xaaaac0a3f6ec : migration_thread+0x3fc/0xb48 [/usr/libexec/qemu-kvm]
 0xaaaac0b77244 : qemu_thread_start+0x5c/0xa8 [/usr/libexec/qemu-kvm]
 0xffff85cf7d38 [/usr/lib64/libpthread-2.17.so+0x7d38/0x40000]
 0xffff85c360ec [/usr/lib64/libc-2.17.so+0xd60ec/0x190000]
 //////////////////(2)//////////////////
vgic_v3_save_pending_tables:387: irq->intid(8192) irq->pending_latch(0)
...
vgic_v3_save_pending_tables:387: irq->intid(8223) irq->pending_latch(0)
...
vgic_v3_save_pending_tables:387: irq->intid(8230) irq->pending_latch(0)
...

 //////////////////(3)//////////////////
func(kvm_its_send_msi) msi->data(40) msi->devid(488898280) msi->data(40)
 0xaaaac0895444 : kvm_its_send_msi+0x34/0xd8 [/usr/libexec/qemu-kvm]
 0xaaaac0a06684 : gicv3_its_trans_write+0xbc/0x100 [/usr/libexec/qemu-kvm]
 0xaaaac0856408 : memory_region_write_with_attrs_accessor+0xb0/0xd8 [/usr/libexec/qemu-kvm]
 0xaaaac0856028 : access_with_adjusted_size+0xa8/0x154 [/usr/libexec/qemu-kvm]
 0xaaaac085a264 : memory_region_dispatch_write+0x198/0x1a0 [/usr/libexec/qemu-kvm]
 0xaaaac0813efc : address_space_stl_le+0xc0/0x1b0 [/usr/libexec/qemu-kvm]
 0xaaaac0a14fc4 : msi_send_message+0x38/0x44 [/usr/libexec/qemu-kvm]
 0xaaaac0a13e2c : msix_notify+0xb4/0xb8 [/usr/libexec/qemu-kvm]
 0xaaaac08b1284 : virtio_notify_vector+0x88/0x9c [/usr/libexec/qemu-kvm]
 0xaaaac0888340 : virtio_blk_rw_complete+0xf4/0x164 [/usr/libexec/qemu-kvm]
 0xaaaac0ae03f0 : blk_aio_complete.part.6+0x20/0x4c [/usr/libexec/qemu-kvm]
 0xaaaac0b8ae64 : coroutine_trampoline+0x5c/0x80 [/usr/libexec/qemu-kvm]
 0xffff85ba49b0 [/usr/lib64/libc-2.17.so+0x449b0/0x190000]
func(vgic_its_inject_msi) : devid(1280) eventid(1)
func(vgic_its_trigger_msi:611) : irq->intid(8230), devid(1280) eventid(1)

 //////////////////(4)//////////////////
func(kvm_its_send_msi) msi->data(0) msi->devid(3421747248) msi->data(0)
 0xaaaac0895444 : kvm_its_send_msi+0x34/0xd8 [/usr/libexec/qemu-kvm]
 0xaaaac0a06684 : gicv3_its_trans_write+0xbc/0x100 [/usr/libexec/qemu-kvm]
 0xaaaac0856408 : memory_region_write_with_attrs_accessor+0xb0/0xd8 [/usr/libexec/qemu-kvm]
 0xaaaac0856028 : access_with_adjusted_size+0xa8/0x154 [/usr/libexec/qemu-kvm]
 0xaaaac085a264 : memory_region_dispatch_write+0x198/0x1a0 [/usr/libexec/qemu-kvm]
 0xaaaac0813efc : address_space_stl_le+0xc0/0x1b0 [/usr/libexec/qemu-kvm]
 0xaaaac0a14fc4 : msi_send_message+0x38/0x44 [/usr/libexec/qemu-kvm]
 0xaaaac0a13e2c : msix_notify+0xb4/0xb8 [/usr/libexec/qemu-kvm]
 0xaaaac08b1284 : virtio_notify_vector+0x88/0x9c [/usr/libexec/qemu-kvm]
 0xaaaac08a07a8 : virtio_net_set_link_status+0x68/0x8c [/usr/libexec/qemu-kvm]
 0xaaaac0a60924 : qemu_del_net_client+0x170/0x1f4 [/usr/libexec/qemu-kvm]
 0xaaaac0a613d8 : net_cleanup+0x38/0x70 [/usr/libexec/qemu-kvm]
 0xaaaac0805708 : main+0x3a4c/0x4564 [/usr/libexec/qemu-kvm]
 0xffff85b81714 [/usr/lib64/libc-2.17.so+0x21714/0x190000]
func(vgic_its_inject_msi) : devid(256) eventid(0)
func(vgic_its_trigger_msi:611) : irq->intid(8223), devid(256) eventid(0)
```
1. 该处为调用`vgic_v3_save_pending_tables`的用户态栈
2. 这里打印的每个irq->pending_latch都是0, 说明记录的每个irq都是inactive的状态
3. 这里发送中断, 为virtio_blk设备发送的中断
4. 这里还有一个virtio_net设备发送中断

可以看到在`vgic_v3_save_pending_tables`流程之后，居然才发送中断。这时pending
 tables中 并未保存该中断的pending状态。
