## dmesg中第一个进程堆栈
```
PID: 451417  TASK: ffffa0ba50b620c0  CPU: 60  COMMAND: "dockerd-current"
 #0 [ffffbf3ecfa8bca0] __schedule at ffffffff83c6f786
 #1 [ffffbf3ecfa8bd30] schedule at ffffffff83c6fba6
 #2 [ffffbf3ecfa8bd48] rwsem_down_write_slowpath at ffffffff8334c5d0
 #3 [ffffbf3ecfa8bdf8] xfs_remove at ffffffffc0a630b5 [xfs]
 #4 [ffffbf3ecfa8be50] xfs_vn_unlink at ffffffffc0a5dbe3 [xfs]
 #5 [ffffbf3ecfa8be88] vfs_unlink at ffffffff835747bf
 #6 [ffffbf3ecfa8bec0] do_unlinkat at ffffffff835792e3
 #7 [ffffbf3ecfa8bf38] do_syscall_64 at ffffffff83c62d0d
 #8 [ffffbf3ecfa8bf50] entry_SYSCALL_64_after_hwframe at ffffffff83e00099
    RIP: 00000000004eab34  RSP: 000000c42658abe8  RFLAGS: 00000246
    RAX: ffffffffffffffda  RBX: 0000000000000000  RCX: 00000000004eab34
    RDX: 0000000000000000  RSI: 000000c422f0fef0  RDI: ffffffffffffff9c
    RBP: 000000c42658ac58   R8: 0000000000000000   R9: 0000000000000000
    R10: 0000000000000000  R11: 0000000000000246  R12: 0000000000000000
    R13: 00000000ffffffbb  R14: 0000000000001ef0  R15: 000000000000002e
    ORIG_RAX: 0000000000000107  CS: 0033  SS: 002b
```
查看xfs_remove `ffffffffc0a630b5`具体代码:
```
0xffffffffc0a630a8 <xfs_remove+312>:    mov    $0x4,%esi
0xffffffffc0a630ad <xfs_remove+317>:    mov    %rbp,%rdi
0xffffffffc0a630b0 <xfs_remove+320>:    callq  0xffffffffc0a61dd0 <xfs_lock_two_inodes> <-- here
0xffffffffc0a630b5 <xfs_remove+325>:    mov    0x10(%rsp),%rdi
```

似乎是调用到了`xfs_lock_two_inodes`函数，但是堆栈bt堆栈上没有打印，疑似
crash程序有问题

查看汇编`xfs_lock_two_inodes`占用`8*8`个堆栈（不算call押入的ip），我们
把隐藏的栈找出来:
```
 #2 [ffffbf3ecfa8bd48] rwsem_down_write_slowpath at ffffffff8334c5d0
    ffffbf3ecfa8bd50: ffffa0b6a1fc9a0c 0000000200000000
    ffffbf3ecfa8bd60: 0000000000000000 0000000000000000
    ffffbf3ecfa8bd70: ffffffffc0a6ed84 0000000000000001
    ffffbf3ecfa8bd80: ffffbf3ecfa8bd78 ffffa0b6a1fc9a10
    ffffbf3ecfa8bd90: ffffa0b6a1fc9a10 ffffa0ba50b620c0
    ffffbf3ecfa8bda0: ffffa0e400000000 00000001428c850e
    ffffbf3ecfa8bdb0: ffffffffc0a61e5c 
 # 2.5 xfs_lock_two_inodes at ffffffffc0a61e5c
    ffffbf3ecfa8bdb8: 0000000400000000
    ffffbf3ecfa8bdc0: 7bf6d1bfd6866500 ffffa111feacc000
    ffffbf3ecfa8bdd0: ffffa0e401865500[rbp] ffffa0b6a1fc9980[r12]
    ffffbf3ecfa8bde0: ffffbf3ecfa8be60[r13] 000000000000001c[r14]
    ffffbf3ecfa8bdf0: 00000000000081ed[r15] ffffffffc0a630b5
 #3 [ffffbf3ecfa8bdf8] xfs_remove at ffffffffc0a630b5 [xfs]
    ffffbf3ecfa8be00: ffffa111feacc360 0000000000000002
    ffffbf3ecfa8be10: ffffa0b390bf8740 7bf6d1bfd6866500
    ffffbf3ecfa8be20: ffffa0e401865638 ffffa0b3db2a9a28
    ffffbf3ecfa8be30: 0000000000000000 ffffa0e401865638
    ffffbf3ecfa8be40: ffffbf3ecfa8bed8 ffffa0b6a1fc9b58
    ffffbf3ecfa8be50: ffffffffc0a5dbe3

crash> dis -l ffffffffc0a61e5c
/usr/src/debug/kernel-5.10.0-60.70.0.94.jdcloud.1.x86_64/linux-5.10.0-60.70.0.94.jdcloud.1.x86_64/fs/xfs/xfs_inode.c: 588
0xffffffffc0a61e5c <xfs_lock_two_inodes+140>:   mov    0x70(%rbx),%ra
```

查看588行代码:
```cpp
xfs_lock_two_inodes(
    struct xfs_inode    *ip0,
    uint            ip0_mode,
    struct xfs_inode    *ip1,
    uint            ip1_mode)
{
    ...
again:
    xfs_ilock(ip0, xfs_lock_inumorder(ip0_mode, 0));

    ...
}
void
xfs_ilock(
    xfs_inode_t     *ip,
    uint            lock_flags)
{
    trace_xfs_ilock(ip, lock_flags, _RET_IP_);

    ...

    if (lock_flags & XFS_IOLOCK_EXCL) {
        down_write_nested(&VFS_I(ip)->i_rwsem,
                  XFS_IOLOCK_DEP(lock_flags));
    } else if (lock_flags & XFS_IOLOCK_SHARED) {
        down_read_nested(&VFS_I(ip)->i_rwsem,
                 XFS_IOLOCK_DEP(lock_flags));
    }
    ...
}
```
查看`ip0`:
```
/usr/src/debug/kernel-5.10.0-60.70.0.94.jdcloud.1.x86_64/linux-5.10.0-60.70.0.94.jdcloud.1.x86_64/fs/xfs/xfs_inode.c: 2911
0xffffffffc0a63098 <xfs_remove+296>:    test   %eax,%eax
0xffffffffc0a6309a <xfs_remove+298>:    jne    0xffffffffc0a62fce <xfs_remove+94>
0xffffffffc0a630a0 <xfs_remove+304>:    mov    $0x4,%ecx
0xffffffffc0a630a5 <xfs_remove+309>:    mov    %r12,%rdx
0xffffffffc0a630a8 <xfs_remove+312>:    mov    $0x4,%esi
0xffffffffc0a630ad <xfs_remove+317>:    mov    %rbp,%rdi
0xffffffffc0a630b0 <xfs_remove+320>:    callq  0xffffffffc0a61dd0 <xfs_lock_two_inodes>
0xffffffffc0a630b5 <xfs_remove+325>:    mov    0x10(%rsp),%rdi
```

`rbp`为第一个参数:

```
/usr/src/debug/kernel-5.10.0-60.70.0.94.jdcloud.1.x86_64/linux-5.10.0-60.70.0.94.jdcloud.1.x86_64/fs/xfs/xfs_inode.c: 550
0xffffffffc0a61dd0 <xfs_lock_two_inodes>:       nopl   0x0(%rax,%rax,1) [FTRACE NOP]
0xffffffffc0a61dd5 <xfs_lock_two_inodes+5>:     push   %r15
0xffffffffc0a61dd7 <xfs_lock_two_inodes+7>:     push   %r14
0xffffffffc0a61dd9 <xfs_lock_two_inodes+9>:     mov    %esi,%r14d
0xffffffffc0a61ddc <xfs_lock_two_inodes+12>:    push   %r13
0xffffffffc0a61dde <xfs_lock_two_inodes+14>:    mov    %rdx,%r13
/usr/src/debug/kernel-5.10.0-60.70.0.94.jdcloud.1.x86_64/linux-5.10.0-60.70.0.94.jdcloud.1.x86_64/fs/xfs/xfs_inode.c: 571
0xffffffffc0a61de1 <xfs_lock_two_inodes+17>:    push   %r12
0xffffffffc0a61de3 <xfs_lock_two_inodes+19>:    push   %rbp  //------[here]
0xffffffffc0a61de4 <xfs_lock_two_inodes+20>:    push   %rbx
0xffffffffc0a61de5 <xfs_lock_two_inodes+21>:    mov    %rdi,%rbx
```

根据上面的堆栈. 可以得到 `p0`为`ffffa0e401865638`
```
crash> struct xfs_inode.i_vnode -o ffffa0e401865500
struct xfs_inode {
  [ffffa0e401865638] struct inode i_vnode;
}

crash> struct inode.i_rwsem ffffa0e401865638
  i_rwsem = {
    count = {
      counter = 1
    },
    owner = {
      counter = -104752898236224
    },
    osq = {
      tail = {
        counter = 0
      }
    },
    wait_lock = {
      raw_lock = {
        {
          val = {
            counter = 0
          },
          {
            locked = 0 '\000',
            pending = 0 '\000'
          },
          {
            locked_pending = 0,
            tail = 0
          }
        }
      }
    },
    wait_list = {
      next = 0xffffa0e4018656f0,
      prev = 0xffffa0e4018656f0
    }
  }
```
查看其owner.
```
crash> struct inode.i_rwsem.owner ffffa0e401865638 -x
  i_rwsem.owner = {
    counter = 0xffffa0ba50b620c0
  },
```
查看该task_struct:
```
crash> bt 0xffffa0ba50b620c0
PID: 451417  TASK: ffffa0ba50b620c0  CPU: 60  COMMAND: "dockerd-current"
 #0 [ffffbf3ecfa8bca0] __schedule at ffffffff83c6f786
 #1 [ffffbf3ecfa8bd30] schedule at ffffffff83c6fba6
 #2 [ffffbf3ecfa8bd48] rwsem_down_write_slowpath at ffffffff8334c5d0
 #3 [ffffbf3ecfa8bdf8] xfs_remove at ffffffffc0a630b5 [xfs]
 #4 [ffffbf3ecfa8be50] xfs_vn_unlink at ffffffffc0a5dbe3 [xfs]
 #5 [ffffbf3ecfa8be88] vfs_unlink at ffffffff835747bf
 #6 [ffffbf3ecfa8bec0] do_unlinkat at ffffffff835792e3
 #7 [ffffbf3ecfa8bf38] do_syscall_64 at ffffffff83c62d0d
 #8 [ffffbf3ecfa8bf50] entry_SYSCALL_64_after_hwframe at ffffffff83e00099
    RIP: 00000000004eab34  RSP: 000000c42658abe8  RFLAGS: 00000246
    RAX: ffffffffffffffda  RBX: 0000000000000000  RCX: 00000000004eab34
    RDX: 0000000000000000  RSI: 000000c422f0fef0  RDI: ffffffffffffff9c
    RBP: 000000c42658ac58   R8: 0000000000000000   R9: 0000000000000000
    R10: 0000000000000000  R11: 0000000000000246  R12: 0000000000000000
    R13: 00000000ffffffbb  R14: 0000000000001ef0  R15: 000000000000002e
    ORIG_RAX: 0000000000000107  CS: 0033  SS: 002b
```
这不就是第一个报hung的进程么 ???

## rwsem_down_write_slowpath

但是我们如果从这个函数来看:
```
0xffffffff8334c2b0 <rwsem_down_write_slowpath>: nopl   0x0(%rax,%rax,1) [FTRACE NOP]
0xffffffff8334c2b5 <rwsem_down_write_slowpath+5>:       push   %r15
0xffffffff8334c2b7 <rwsem_down_write_slowpath+7>:       push   %r14
0xffffffff8334c2b9 <rwsem_down_write_slowpath+9>:       push   %r13
0xffffffff8334c2bb <rwsem_down_write_slowpath+11>:      push   %r12
0xffffffff8334c2bd <rwsem_down_write_slowpath+13>:      mov    %rdi,%r12
0xffffffff8334c2c0 <rwsem_down_write_slowpath+16>:      push   %rbp
0xffffffff8334c2c1 <rwsem_down_write_slowpath+17>:      push   %rbx
0xffffffff8334c2c2 <rwsem_down_write_slowpath+18>:      sub    $0x78,%rsp
```

这个函数会开非常大的栈，栈顶就是`xfs_remove()`函数, 非常奇怪，栈乱套了!!
