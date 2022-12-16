# 堆栈分析
## 获取的进程堆栈
```
[<0>] __kernfs_remove.part.15+0x172/0x1e0
[<0>] kernfs_remove_by_name_ns+0x4f/0x90
[<0>] acpi_power_meter_notify+0x124/0x1b0 [acpi_power_meter]
[<0>] acpi_ev_notify_dispatch+0x45/0x5a
[<0>] acpi_os_execute_deferred+0x16/0x20
[<0>] process_one_work+0x1a7/0x3b0
[<0>] worker_thread+0x30/0x390
[<0>] kthread+0x112/0x130
[<0>] ret_from_fork+0x35/0x40
[<0>] 0xffffffffffffffff
```

## 通过dis -l 获取__kernfs_remove执行的代码
```
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.x86_64/fs/kernfs/dir.c: 474
0xffffffffb5f45f0d <__kernfs_remove+349>:       xor    %esi,%esi
0xffffffffb5f45f0f <__kernfs_remove+351>:       mov    %rsp,%rdi
0xffffffffb5f45f12 <__kernfs_remove+354>:       add    $0x50,%r13
0xffffffffb5f45f16 <__kernfs_remove+358>:       callq  0xffffffffb5cfa7c0 <init_wait_entry>
0xffffffffb5f45f1b <__kernfs_remove+363>:       jmp    0xffffffffb5f45f22 <__kernfs_remove+370>
0xffffffffb5f45f1d <__kernfs_remove+365>:       callq  0xffffffffb6460e20 <schedule>
0xffffffffb5f45f22 <__kernfs_remove+370>:       mov    $0x2,%edx
0xffffffffb5f45f27 <__kernfs_remove+375>:       mov    %rsp,%rsi
0xffffffffb5f45f2a <__kernfs_remove+378>:       mov    %r13,%rdi
0xffffffffb5f45f2d <__kernfs_remove+381>:       callq  0xffffffffb5cfac30 <prepare_to_wait_event>
```
## 根据上面获取的行数查看相关代码 
### kernfs_drain
```cpp
static void kernfs_drain(struct kernfs_node *kn)
        __releases(&kernfs_mutex) __acquires(&kernfs_mutex)
{
        struct kernfs_root *root = kernfs_root(kn);

        lockdep_assert_held(&kernfs_mutex);
        WARN_ON_ONCE(kernfs_active(kn));

        mutex_unlock(&kernfs_mutex);

        if (kernfs_lockdep(kn)) {
                rwsem_acquire(&kn->dep_map, 0, 0, _RET_IP_);
                if (atomic_read(&kn->active) != KN_DEACTIVATED_BIAS)
                        lock_contended(&kn->dep_map, _RET_IP_);
        }

        /* but everyone should wait for draining */
        wait_event(root->deactivate_waitq, //!!!!==================line 474
                ¦  atomic_read(&kn->active) == KN_DEACTIVATED_BIAS);

        if (kernfs_lockdep(kn)) {
                lock_acquired(&kn->dep_map, _RET_IP_);
                rwsem_release(&kn->dep_map, 1, _RET_IP_);
        }

        kernfs_drain_open_files(kn);

        mutex_lock(&kernfs_mutex);
}
```
可以看到是在`wait_event`, 等待`kn->active == KN_DEACTIVATED_BIAS` 条件成立

### kernfs_put_active, kernfs_get_active
通过查看代码，标题中的函数，会对kn->active计数更改
```cpp
void kernfs_put_active(struct kernfs_node *kn)
{
        struct kernfs_root *root = kernfs_root(kn);
        int v;

        if (unlikely(!kn))
                return;

        if (kernfs_lockdep(kn))
                rwsem_release(&kn->dep_map, 1, _RET_IP_);
        v = atomic_dec_return(&kn->active);
        if (likely(v != KN_DEACTIVATED_BIAS))
                return;

        wake_up_all(&root->deactivate_waitq);
}
struct kernfs_node *kernfs_get_active(struct kernfs_node *kn)
{
        if (unlikely(!kn))
                return NULL;

        if (!atomic_inc_unless_negative(&kn->active))
                return NULL;

        if (kernfs_lockdep(kn))
                rwsem_acquire_read(&kn->dep_map, 0, 1, _RET_IP_);
        return kn;
}
```
在`kernfs_put_active`中，会检查active的值，如果==`KN_DEACTIVATED_BIAS`
则唤醒`KN_DEACTIVATED_BIAS`队列中的所有成员。


# 其他

## crash自调
```
crash> p sysfs_root_kn
sysfs_root_kn = $2 = (struct kernfs_node *) 0xffff927d46da03b8
```
init 
```cpp
int __init sysfs_init(void)
{
        int err;

        sysfs_root = kernfs_create_root(NULL, KERNFS_ROOT_EXTRA_OPEN_PERM_CHECK,
                                        NULL);
        if (IS_ERR(sysfs_root))
                return PTR_ERR(sysfs_root);

        sysfs_root_kn = sysfs_root->kn;

        err = register_filesystem(&sysfs_fs_type);
        if (err) {
                kernfs_destroy_root(sysfs_root);
                return err;
        }

        return 0;
}
```
## kernfs_next_descendant_post
```cpp
/**
 * kernfs_next_descendant_post - find the next descendant for post-order walk
 * @pos: the current position (%NULL to initiate traversal)
 * @root: kernfs_node whose descendants to walk
 *
 * Find the next descendant to visit for post-order traversal of @root's
 * descendants.  @root is included in the iteration and the last node to be
 * visited.
 */
/*
 * descendant : 后代
 * post-order: 向后次序
 */
static struct kernfs_node *kernfs_next_descendant_post(struct kernfs_node *pos,
                                                ¦      struct kernfs_node *root)
{
        struct rb_node *rbn;

        lockdep_assert_held(&kernfs_mutex);

        /* if first iteration, visit leftmost descendant which may be root */
        if (!pos)
                return kernfs_leftmost_descendant(root);

        /* if we visited @root, we're done */
        if (pos == root)
                return NULL;

        /* if there's an unvisited sibling, visit its leftmost descendant */
        rbn = rb_next(&pos->rb);
        if (rbn)
                return kernfs_leftmost_descendant(rb_to_kn(rbn));

        /* no sibling left, visit parent */
        return pos->parent;
}
```


# commit:
## kernfs activate
d35258ef702cca0c4e66d799f8e38b78c02ce8a5

## kernfs active lockdep
846f99749ab68bbc7f75c74fec305de675b1a1bf
sysfs: Add lockdep annotations for the sysfs active reference

## ops
```cpp
static const struct seq_operations kernfs_seq_ops = { 
        .start = kernfs_seq_start,                    
        .next = kernfs_seq_next,                      
        .stop = kernfs_seq_stop,                      
        .show = kernfs_seq_show,                      
};
static const struct kernfs_ops sysfs_file_kfops_ro = {
        .seq_show       = sysfs_kf_seq_show,
};
static const struct sysfs_ops dev_sysfs_ops = {
        .show   = dev_attr_show,
        .store  = dev_attr_store,
};
```
## lock
```
stack 1:
work: acpi_os_execute_deferred
acpi_ev_notify_dispatch
acpi_power_meter_notify
	mutex_lock(&resource->lock) <===争抢
remove_attrs
device_remove_file
sysfs_remove_file
sysfs_remove_file_ns
kernfs_remove_by_name_ns
	mutex_lock(&kernfs_mutex)
__kernfs_remove
kernfs_drain
	mutex_unlock(&kernfs_mutex);
	//在这里wait kn->active
	wait_event(root->deactivate_waitq, <===争抢
	     atomic_read(&kn->active) == KN_DEACTIVATED_BIAS);

stack 2:
seq_read
	mutex_lock(&m->lock); (struct seq_file *m)
m->op->start(); kernfs_seq_start
	mutex_lock(&of->mutex);
	kernfs_get_active(of->kn) <====争抢
m->op->show() : kernfs_seq_show
struct kernfs_ops->show(): sysfs_kf_seq_show()
m->op->stop() : kernfs_seq_stop
	kernfs_seq_stop_active
	kernfs_put_active

主要看下下面的调用
sysfs_kf_seq_show
struct sys_ops->show() : dev_attr_show
struct device_attribute->show(): show_power
	mutex_lock(&resource->lock); <====争抢
	update_meter();	
	mutex_unlock(&resource->lock);
```

#
