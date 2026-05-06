# sysrq_alt_f12_kprobe

Out-of-tree kprobe module that makes `Alt+F12` behave like `Alt+SysRq`.
After the trigger is active, the existing kernel SysRq input state machine
handles the normal SysRq subkey.

## Build

```sh
make KDIR=/path/to/prepared/kernel/build
```

For this workspace, if `kernel/` has been configured and prepared:

```sh
make KDIR=../kernel
```

## Load

```sh
sudo insmod sysrq_alt_f12_kprobe.ko
sudo rmmod sysrq_alt_f12_kprobe
```

## Usage

Hold `Alt`, press `F12`, then press a SysRq subkey such as `t`, `m`, `s`,
or `b`.  The module only rewrites the `KEY_F12` argument seen by
`sysrq_filter()` into `KEY_SYSRQ`; all command dispatch still goes through
the kernel's original SysRq implementation.

When `RightAlt` is released while `F12` is still held, the module changes
that release event's value to `2` for `sysrq_filter()`, so the original SysRq
state machine keeps its active state.  A delayed worker injects
`KEY_RIGHTALT value=1/0` after 5 seconds, allowing the original path to exit
SysRq mode cleanly. The press is included so the input core does not drop the
release as an unchanged key state.

## Requirements

The target kernel needs `CONFIG_MAGIC_SYSRQ`, `CONFIG_INPUT`, `CONFIG_KPROBES`
and kallsyms support sufficient for kprobes to resolve the static
`sysrq_filter` symbol.
