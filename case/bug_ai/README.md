# JDDS 内核模块崩溃分析报告

## 1. 概述

JDDS 内核模块在处理 COW (Copy-On-Write) bitmap 回写时触发 `BUG_ON()`，导致内核 panic。崩溃发生在 `kworker/2:0` 工作线程中，异常类型为 `exc_invalid_op`（无效操作码），属于内核 BUG 断言失败。

## 2. 崩溃现场

### 2.1 基本信息

| 项目 | 内容 |
|------|------|
| 崩溃模块 | `jdds` |
| 崩溃函数 | `handle_each_bitmap_page_range+544` |
| 崩溃地址 | `ffffffffc0980710` |
| 进程 | `kworker/2:0` (PID: 22992) |
| CPU | 2 |
| 异常类型 | `exc_invalid_op` (BUG 断言) |

### 2.2 相关 Crash 命令

初始化 crash 会话：

```
crash> init_crash
  crash: /usr/bin/crash
  vmlinux: /usr/lib/debug/lib/modules/5.10.0-136.12.0.86.oe2203sp1.x86_64/vmlinux
  vmcore: /root/vmcore/vmcore
```

查看 backtrace：

```
crash> bt
PID: 22992  TASK: ff1374216cca3080  CPU: 2   COMMAND: "kworker/2:0"
 #0 [ff1e57e2805efbf0] crash_kexec at ffffffff8c1ad4f9
 #1 [ff1e57e2805efc00] oops_end at ffffffff8c0297d5
 #2 [ff1e57e2805efc20] do_trap at ffffffff8c0260ee
 #3 [ff1e57e2805efc70] do_error_trap at ffffffff8c0261a5
 #4 [ff1e57e2805efcb0] exc_invalid_op at ffffffff8ca9796e
 #5 [ff1e57e2805efcd0] asm_exc_invalid_op at ffffffff8cc00a92
    [exception RIP: handle_each_bitmap_page_range+544]
    RIP: ffffffffc0980710  RSP: ff1e57e2805efd80  RFLAGS: 00010206
    RAX: 0000000000000001  RBX: ff1e57e2805efe48  RCX: 0000000000000000
    RDX: 0000000000000000  RSI: 0000000000000018  RDI: ff137427fba5ac70
    RBP: 0000000000000019   R8: ff1e57e2805efe48   R9: ff1e57e2805efe38
    R10: 0000000000000002  R11: ff13742121016450  R12: 0000000000000028
    R13: ff1e57e2805efe40  R14: ff1e57e2805efe38  R15: ff137427fba5ac70
    ORIG_RAX: ffffffffffffffff  CS: 0010  SS: 0018
 #6 [ff1e57e2805efde8] handle_for_each_cow_bitmap_range at ffffffffc0980932 [jdds]
 #7 [ff1e57e2805efe88] writeback_bitmap_to_snapfile at ffffffffc0980a2e [jdds]
 #8 [ff1e57e2805efe98] process_one_work at ffffffff8c1074dd
 #9 [ff1e57e2805efed8] worker_thread at ffffffff8c107a39
#10 [ff1e57e2805eff10] kthread at ffffffff8c10c6db
#11 [ff1e57e2805eff50] ret_from_fork at ffffffff8c00358f
```

查看完整 backtrace（含栈帧数据）：

```
crash> bt -f
PID: 22992  TASK: ff1374216cca3080  CPU: 2   COMMAND: "kworker/2:0"
 #0 [ff1e57e2805efbf0] crash_kexec at ffffffff8c1ad4f9
    ff1e57e2805efbf8: ff1e57e2805efcd8 ffffffff8c0297d5
 #1 [ff1e57e2805efc00] oops_end at ffffffff8c0297d5
    ff1e57e2805efc08: 0000000000000006 0000000000000002
    ff1e57e2805efc18: ff1e57e2805efcd8 ffffffff8c0260ee
 #2 [ff1e57e2805efc20] do_trap at ffffffff8c0260ee
    ff1e57e2805efc28: ffffffffc0980710 ffffffff8d31c3c0
    ff1e57e2805efc38: 0000000000000006 ff1e57e2805efcd8
    ff1e57e2805efc48: 0000000000000006 0000000000000004
    ff1e57e2805efc58: ffffffff8d31c3c0 0000000000000000
    ff1e57e2805efc68: 0000000000000002 ffffffff8c0261a5
 #3 [ff1e57e2805efc70] do_error_trap at ffffffff8c0261a5
    ff1e57e2805efc78: ffffffffc0980710 0000000000000000
    ff1e57e2805efc88: ff1e57e2805efcd8 0000000000000000
    ff1e57e2805efc98: 0000000000000000 0000000000000000
    ff1e57e2805efca8: 0000000000000000 ffffffff8ca9796e
 #4 [ff1e57e2805efcb0] exc_invalid_op at ffffffff8ca9796e
    ff1e57e2805efcb8: ffffffffc0980710 0000000000000000
    ff1e57e2805efcc8: 0000000000000000 ffffffff8cc00a92
 #5 [ff1e57e2805efcd0] asm_exc_invalid_op at ffffffff8cc00a92
    [exception RIP: handle_each_bitmap_page_range+544]
    RIP: ffffffffc0980710  RSP: ff1e57e2805efd80  RFLAGS: 00010206
    RAX: 0000000000000001  RBX: ff1e57e2805efe48  RCX: 0000000000000000
    RDX: 0000000000000000  RSI: 0000000000000018  RDI: ff137427fba5ac70
    RBP: 0000000000000019   R8: ff1e57e2805efe48   R9: ff1e57e2805efe38
    R10: 0000000000000002  R11: ff13742121016450  R12: 0000000000000028
    R13: ff1e57e2805efe40  R14: ff1e57e2805efe38  R15: ff137427fba5ac70
    ORIG_RAX: ffffffffffffffff  CS: 0010  SS: 0018
    ff1e57e2805efcd8: ff137427fba5ac70 ff1e57e2805efe38
    ff1e57e2805efce8: ff1e57e2805efe40 0000000000000028
    ff1e57e2805efcf8: 0000000000000019 ff1e57e2805efe48
    ff1e57e2805efd08: ff13742121016450 0000000000000002
    ff1e57e2805efd18: ff1e57e2805efe38 ff1e57e2805efe48
    ff1e57e2805efd28: 0000000000000001 0000000000000000
    ff1e57e2805efd38: 0000000000000000 0000000000000018
    ff1e57e2805efd48: ff137427fba5ac70 ffffffffffffffff
    ff1e57e2805efd58: ffffffffc0980710 0000000000000010
    ff1e57e2805efd68: 0000000000010206 ff1e57e2805efd80
    ff1e57e2805efd78: 0000000000000018 ff1e57e2805efe3c
    ff1e57e2805efd88: 000009ccc82b22db ff137427fb1353c0
    ff1e57e2805efd98: 0000000000000296 00017e8000008f70
    ff1e57e2805efda8: 0000ffff8c140000 4884f94329ccc400
    ff1e57e2805efdb8: ff1e57e2805efe48 0000000000000019
    ff1e57e2805efdc8: 0000000000000028 ff1e57e2805efe40
    ff1e57e2805efdd8: ff1e57e2805efe38 ff137427fba5ac70
    ff1e57e2805efde8: ffffffffc0980932
 #6 [ff1e57e2805efde8] handle_for_each_cow_bitmap_range at ffffffffc0980932 [jdds]
    ff1e57e2805efdf0: ff1e57e2805efe3c 0000000000000001
    ff1e57e2805efe00: ff1e57e2805efe3c 0000000000000019
    ff1e57e2805efe10: ff1374212a0df400 000000000000191a
    ff1e57e2805efe20: 0000000e805efe18 000000000000000e
    ff1e57e2805efe30: 0000000000001000 0000000000000001
    ff1e57e2805efe40: 00000000402c7010 00000000402c8000
    ff1e57e2805efe50: 4884f94329ccc400 ff137427fba5ac98
    ff1e57e2805efe60: ff137427fba5ac98 ff137427fb134cc0
    ff1e57e2805efe70: 0000000000000000 ff137427fb13a700
    ff1e57e2805efe80: 0000000000000000 ffffffffc0980a2e
 #7 [ff1e57e2805efe88] writeback_bitmap_to_snapfile at ffffffffc0980a2e [jdds]
    ff1e57e2805efe90: ff137421001b4d80 ffffffff8c1074dd
 #8 [ff1e57e2805efe98] process_one_work at ffffffff8c1074dd
    ff1e57e2805efea0: ff137427fba5aca0 ff137421001b4da8
    ff1e57e2805efeb0: ff137421001b4d80 ff137427fb134cc0
    ff1e57e2805efec0: ff137427fb134ce0 ff1374216cca3080
    ff1e57e2805efed0: ff137427fb134cc0 ffffffff8c107a39
 #9 [ff1e57e2805efed8] worker_thread at ffffffff8c107a39
    ff1e57e2805efee0: ffffffff8c1079f0 ff13742100e4ba80
    ff1e57e2805efef0: ff137427fbde04c0 ff1e57e2887a7da8
    ff1e57e2805eff00: ff137421001b4d80 ff1374216cca3080
    ff1e57e2805eff10: ffffffff8c10c6db
#10 [ff1e57e2805eff10] kthread at ffffffff8c10c6db
    ff1e57e2805eff18: ff13742100e4bac0 ffffffff8c10c5e0
    ff1e57e2805eff28: 0000000000000000 ff137427fbde04c0
    ff1e57e2805eff38: 0000000000000000 0000000000000000
    ff1e57e2805eff48: 0000000000000000 ffffffff8c00358f
#11 [ff1e57e2805eff50] ret_from_fork at ffffffff8c00358f
```

### 2.3 寄存器在 bt -f 中的位置

`bt -f` 输出中，寄存器信息出现在 **#5 号栈帧** (`asm_exc_invalid_op`) 中，位于 `[exception RIP: ...]` 行 之下。这是因为：

1. 当 CPU 执行到 `ud2`（BUG 宏生成的无符号指令）时，触发 `#UD` 异常
2. CPU 硬件自动将 **RIP、CS、RFLAGS、RSP、SS** 压入内核栈，形成异常栈帧
3. `asm_exc_invalid_op` 汇编入口通过 `push` 指令保存其余通用寄存器（RAX、RBX、RCX、RDX、RSI、RDI、RBP、R8-R15、ORIG_RAX）
4. crash 工具解析该异常帧，将寄存器值展示在 `#5` 帧中

```
#5 [ff1e57e2805efcd0] asm_exc_invalid_op at ffffffff8cc00a92
    [exception RIP: handle_each_bitmap_page_range+544]    ← 触发 BUG 的指令地址
    RIP: ffffffffc0980710  RSP: ff1e57e2805efd80  RFLAGS: 00010206    ← 硬件保存
    RAX: 0000000000000001  RBX: ff1e57e2805efe48  RCX: 0000000000000000    ← 软件保存
    RDX: 0000000000000000  RSI: 0000000000000018  RDI: ff137427fba5ac70    ← 关键: RSI/RDX
    RBP: 0000000000000019   R8: ff1e57e2805efe48   R9: ff1e57e2805efe38
    R10: 0000000000000002  R11: ff13742121016450  R12: 0000000000000028
    R13: ff1e57e2805efe40  R14: ff1e57e2805efe38  R15: ff137427fba5ac70
    ORIG_RAX: ffffffffffffffff  CS: 0010  SS: 0018
    ff1e57e2805efcd8: ff137427fba5ac70 ff1e57e2805efe38     ← 后续为原始栈数据
    ...
```

寄存器与函数参数的对应关系（x86_64 调用约定）：

| 寄存器 | 参数位置 | 变量名 | 崩溃值 | 含义 |
|--------|----------|--------|--------|------|
| `%rdi` | 第 1 参数 | `cow_bitmap` | `ff137427fba5ac70` | COW bitmap 指针 |
| `%rsi` | 第 2 参数 | `range_start` | `0x18 (24)` | 范围起始 → 异常值 |
| `%rdx` | 第 3 参数 | `range_end` | `0x0 (0)` | 范围结束 → 异常值 |
| `%rcx` | 第 4 参数 | `entry_file_pos_p` | `0x0` | 在 BUG_ON 时已被覆写 |
| `%r8` | 第 5 参数 | `bitmap_page_file_pos_p` | `ff1e57e2805efe48` | |
| `%r9` | 第 6 参数 | `entry_count_p` | `ff1e57e2805efe38` | |

> **关键**: `RSI=0x18 (24)` 且 `RDX=0x0 (0)`，满足 `range_start > range_end`，这正是 `BUG_ON` 的条件。

查看函数符号信息：

```
crash> sym handle_each_bitmap_page_range
ffffffffc09804f0 (T) handle_each_bitmap_page_range [jdds]
```

### 2.4 关键汇编分析

崩溃点反汇编：

```
crash> dis handle_each_bitmap_page_range 64
0xffffffffc09804f0 <handle_each_bitmap_page_range>:     nopl   0x0(%rax,%rax,1) [FTRACE NOP]
0xffffffffc09804f5 <handle_each_bitmap_page_range+5>:   push   %r15
0xffffffffc09804f7 <handle_each_bitmap_page_range+7>:   push   %r14
0xffffffffc09804f9 <handle_each_bitmap_page_range+9>:   push   %r13
0xffffffffc09804fb <handle_each_bitmap_page_range+11>:  mov    %rcx,%r13
0xffffffffc09804fe <handle_each_bitmap_page_range+14>:  push   %r12
0xffffffffc0980500 <handle_each_bitmap_page_range+16>:  push   %rbp
0xffffffffc0980501 <handle_each_bitmap_page_range+17>:  push   %rbx
0xffffffffc0980502 <handle_each_bitmap_page_range+18>:  sub    $0x38,%rsp
...
0xffffffffc0980521 <handle_each_bitmap_page_range+49>:  xor    %ecx,%ecx
0xffffffffc0980523 <handle_each_bitmap_page_range+51>:  cmp    %edx,%esi       ; ★ 比较 range_start(%esi) 与 range_end(%edx)
0xffffffffc0980525 <handle_each_bitmap_page_range+53>:  ja     0xffffffffc0980710 <handle_each_bitmap_page_range+544>  ; ★ BUG_ON 跳转
```

**关键分析**：

- x86_64 调用约定中，`%rsi`/`%esi` 为第二参数（`range_start`），`%rdx`/`%edx` 为第三参数（`range_end`）
- `cmp %edx, %esi` 在 AT&T 语法中计算结果为 `range_start - range_end`
- `ja`（Jump if Above, unsigned）当 `range_start > range_end` 时跳转至 `+544`（崩溃点）
- **寄存器值**：`RSI=0x18 (24)`，`RDX=0x0 (0)` → `24 > 0` 条件成立 → 触发 `BUG_ON`

## 3. 根因分析

### 3.1 调用链

```
writeback_bitmap_to_snapfile()
  └── handle_for_each_cow_bitmap_range()
       └── handle_each_bitmap_page_range()   ← 崩溃点
```

### 3.2 涉及的关键代码

#### `handle_each_bitmap_page_range()` — 崩溃函数

```c
// jdds/jdds_fs.c:1058-1076
int handle_each_bitmap_page_range(struct cow_bitmap *cow_bitmap,
                                   unsigned int range_start,   // ← %rsi
                                   unsigned int range_end,     // ← %rdx
                                   loff_t *entry_file_pos_p,
                                   loff_t *bitmap_page_file_pos_p,
                                   u32 *entry_count_p,
                                   u32 *bitmap_page_count_p,
                                   enum bitmap_entry_type entry_type)
{
    // ...
    BUG_ON(range_start > range_end);  // ★ 第 1076 行：崩溃点
    // ...
}
```

#### `handle_for_each_cow_bitmap_range()` — 调用方（Bug 所在）

```c
// jdds/jdds_fs.c:1185-1242（修复前）
int handle_for_each_cow_bitmap_range(struct cow_bitmap *cow_bitmap)
{
    struct jdds_metadata *md;
    u32 clear_bitmap_index_start, clear_bitmap_index_end;  // ★ clear_bitmap_index_end 未初始化
    // ...

    clear_bitmap_index_start = 0;       // 初始化为 0
    cur_entry_count = cur_bitmap_count = 0;

    bitmap_for_each_set_region(cow_bitmap->cow_range_bitmap,
                               rs, re, 0,
                               cow_bitmap->bitmap_count) {
        // ★ 第一次调用：处理清除区间 [clear_start, clear_end)
        ret = handle_each_bitmap_page_range(cow_bitmap,
                                            clear_bitmap_index_start,   // → range_start
                                            clear_bitmap_index_end,     // → range_end (未初始化!)
                                            // ...
                                            BITMAP_ENTRY_TYPE_BITMAP);
        if (ret)
            goto no_bitmap;

        // 第二次调用：处理 set region [rs, re)
        ret = handle_each_bitmap_page_range(cow_bitmap,
                                            rs,                        // → range_start
                                            re,                        // → range_end
                                            // ...
                                            BITMAP_ENTRY_TYPE_SET_RANGE);
        if (ret)
            goto no_bitmap;

        clear_bitmap_index_start = re;   // 更新为当前 region 结束位置
    }
    // ...
}
```

### 3.3 执行流程还原

该函数通过 `bitmap_for_each_set_region` 遍历 `cow_range_bitmap` 中所有置位区域 `[rs, re)`。对于每个区域，先写 BITMAP 条目覆盖前一个清除区间，再写一个 RANGE 条目覆盖该置位区间。

| 迭代 | `clear_start` | `clear_end` (应为 `rs`) | 实际 `clear_end` | 结果 |
|------|---------------|-------------------------|-------------------|------|
| 第 1 次 | 0 | `rs₁` | 栈残留 (0) | 如果 `rs₁ > 0` 则正常，因为 `0 > 0` 为假 |
| 第 2 次 | `re₁` (如 24) | `rs₂` | 栈残留 (0) | **BUG!** `24 > 0` 为真，触发 `BUG_ON` |

当循环进入第二次迭代时：
- `clear_bitmap_index_start` 已被更新为上一个 `re` 值（24）
- `clear_bitmap_index_end` 仍然未初始化，栈上的值为 0
- `handle_each_bitmap_page_range(24, 0, ...)` → `BUG_ON(24 > 0)` → **内核崩溃**

## 4. 修复方案

### 4.1 修复内容

| 文件 | 行号 | 修改 |
|------|------|------|
| `jdds/jdds_fs.c` | 1188 | 删除未使用的变量 `clear_bitmap_index_end` |
| `jdds/jdds_fs.c` | 1222 | 将 `clear_bitmap_index_end` 替换为 `rs` |

### 4.2 修复前后对比

**修复前**：

```c
// 第 1188 行 — 声明未初始化变量
u32 clear_bitmap_index_start, clear_bitmap_index_end;

// 第 1220-1222 行 — 使用未初始化变量
ret = handle_each_bitmap_page_range(cow_bitmap,
                                    clear_bitmap_index_start,
                                    clear_bitmap_index_end,  // BUG: 未初始化
                                    // ...
                                    BITMAP_ENTRY_TYPE_BITMAP);
```

**修复后**：

```c
// 第 1188 行 — 只保留需要的变量
u32 clear_bitmap_index_start;

// 第 1220-1222 行 — 使用正确的边界值
ret = handle_each_bitmap_page_range(cow_bitmap,
                                    clear_bitmap_index_start,
                                    rs,                      // 正确: set region 起始位置
                                    // ...
                                    BITMAP_ENTRY_TYPE_BITMAP);
```

### 4.3 修复逻辑说明

`rs` 是 `bitmap_for_each_set_region` 输出的当前 set region 起始位置。清除区间 `[clear_bitmap_index_start, rs)` 恰好位于前一个 set region 结束位置（或起始 0）与当前 set region 起始位置之间，语义正确：

- `clear_bitmap_index_start == rs` 时，该函数直接返回 0（无清除区间，无需处理）
- `clear_bitmap_index_start < rs` 时，正常写出 `[clear_start, rs)` 范围内的 BITMAP 条目
- `clear_bitmap_index_start > rs` 永远不会发生（`bitmap_for_each_set_region` 保证 `rs` 递增）

## 5. 验证建议

1. **静态检查**：编译内核模块，确认 `clear_bitmap_index_end` 不再被引用
2. **功能测试**：在至少包含 2 个不连续 set region 的 `cow_range_bitmap` 场景下触发 writeback 路径
3. **边界测试**：
   - `cow_range_bitmap` 从 index 0 开始有 set region（首轮 `clear_start == rs == 0`）
   - `cow_range_bitmap` 中间有空隙（触发清除区间写入）
   - 末尾无 set region 的尾随清除区间（最后的 `handle_each_bitmap_page_range` 使用 `bitmap_count`）

