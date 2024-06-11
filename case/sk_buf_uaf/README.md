```
 0xffffffff8248caf3 : tcp_queue_rcv+0xd3/0x150 [kernel]
 0xffffffff82493035 : tcp_data_queue+0x215/0xb80 [kernel]
 0xffffffff82493b62 : tcp_rcv_established+0x1c2/0x5c0 [kernel]
 0xffffffff824a056a : tcp_v4_do_rcv+0x12a/0x1e0 [kernel]
 0xffffffff824a28d3 : tcp_v4_rcv+0xb43/0xc50 [kernel]

 0xffffffff82475d4c : ip_protocol_deliver_rcu+0x2c/0x1d0 [kernel]
 0xffffffff82475f3d : ip_local_deliver_finish+0x4d/0x60 [kernel]
 0xffffffff82475fbb : ip_local_deliver+0x6b/0xf0 [kernel]
                                skb_dst(skb)->input(skb);
                                dst_input
                                ip_rcv_finish
 0xffffffff824762bb : ip_rcv+0x27b/0x36f [kernel]
 0xffffffff823df650 : __netif_receive_skb_core+0xba0/0xcb0 [kernel]
 0xffffffff823df7fd : netif_receive_skb_internal+0x3d/0xb0 [kernel]
 0xffffffff823e02a8 : napi_gro_receive+0x108/0x150 [kernel]
 0xffffffffc0427b40
 0xffffffffc0427b40 (inexact)
 0xffffffffc04286c4 (inexact)
 0xffffffff823e0b9d : __napi_poll+0x2d/0x130 [kernel] (inexact)
 0xffffffff823e1073 : net_rx_action+0x253/0x320 [kernel] (inexact)
 0xffffffff828000d7 : __do_softirq+0xd7/0x2c4 [kernel] (inexact)
 0xffffffff81cf445b : irq_exit_rcu+0xcb/0xd0 [kernel] (inexact)
 0xffffffff81cf446a : irq_exit+0xa/0x10 [kernel] (inexact)
 0xffffffff82601e7f : do_IRQ+0x7f/0xd0 [kernel] (inexact)
 0xffffffff82600a8f : ret_from_intr+0x0/0x1d [kernel] (inexact)
 0xffffffff825a4fee : native_safe_halt+0xe/0x10 [kernel] (inexact)
 0xffffffff825a4eaa : __cpuidle_text_start+0xa/0x10 [kernel] (inexact)
 0xffffffff825a5140 : default_idle_call+0x40/0xf0 [kernel] (inexact)
 0xffffffff81d25d5f : do_idle+0x20f/0x2c0 [kernel] (inexact)
 0xffffffff81d25fdf : cpu_startup_entry+0x6f/0x80 [kernel] (inexact)
 0xffffffff81c59ed6 : start_secondary+0x1a6/0x1e0 [kernel] (inexact)
 0xffffffff81c00107 : secondary_startup_64_no_verify+0xc2/0xcb [kernel] (inexact)
```


```
function is sock_rfree
 0xffffffff823bc630 : sock_rfree+0x0/0x60 [kernel]
 0xffffffff823c36a8 : skb_release_head_state+0x68/0x100 [kernel]
 0xffffffff823c374e : skb_release_all+0xe/0x30 [kernel]
 0xffffffff823c377e : __kfree_skb+0xe/0x20 [kernel]
                                                        //sk_eat_skb
 0xffffffff82488735 : tcp_recvmsg+0x7a5/0xa90 [kernel]
 0xffffffff824bc78b : inet_recvmsg+0x5b/0xd0 [kernel]
 0xffffffff823b69b2 : sock_read_iter+0x92/0xf0 [kernel]
 0xffffffff81f3e09f : new_sync_read+0x10f/0x150 [kernel]
 0xffffffff81f40ad1 : vfs_read+0x91/0x140 [kernel]
 0xffffffff81f40f0f : ksys_read+0x4f/0xb0 [kernel]
 0xffffffff81c0430b : do_syscall_64+0x5b/0x1a0 [kernel]
 0xffffffff826000ad : entry_SYSCALL_64_after_hwframe+0x65/0xca [kernel]
 0xffffffff826000ad : entry_SYSCALL_64_after_hwframe+0x65/0xca [kernel] (inexact)

```
