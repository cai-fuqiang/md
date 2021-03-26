# 信息截取

NOTE:该信息是从kernel vmcore文件中截取得到

## System Information
```
crash> sys
      KERNEL: vmlinux
    DUMPFILE: vmcore  [PARTIAL DUMP]
        CPUS: 80
        DATE: Wed Mar 24 02:39:14 2021
      UPTIME: 8 days, 18:01:49
LOAD AVERAGE: 6.01, 6.64, 6.55
       TASKS: 4114
    NODENAME: node-24.domain.tld
     RELEASE: 3.10.0-693.11.1.el7.es.10.x86_64
     VERSION: #1 SMP Mon Jul 2 23:17:05 CST 2018
     MACHINE: x86_64  (2000 Mhz)
      MEMORY: 511.7 GB
       PANIC: T"Kernel panic - not syncing: Fatal machine check"
```

## Kernel Ring Buffers

```
[754326.990348] mce: [Hardware Error]: PROCESSOR 0:50654 TIME 1616567954 SOCKET 0 APIC 32 microcode 200005e
[754327.004544] mce: [Hardware Error]: CPU 15: Machine Check Exception: 5 Bank 6: bb80000000000e0b
[754327.004803] mce: [Hardware Error]: RIP !INEXACT! 10:<ffffffff816ab596> {native_safe_halt+0x6/0x10}
[754327.005060] mce: [Hardware Error]: TSC 603312c7dd484 MISC 11000000
[754327.005193] mce: [Hardware Error]: PROCESSOR 0:50654 TIME 1616567954 SOCKET 0 APIC 26 microcode 200005e
[754327.019298] mce: [Hardware Error]: CPU 55: Machine Check Exception: 5 Bank 6: bb80000000000e0b
[754327.019548] mce: [Hardware Error]: RIP !INEXACT! 10:<ffffffff816ab596> {native_safe_halt+0x6/0x10}
[754327.019803] mce: [Hardware Error]: TSC 603312c7e4dc2 MISC 11000000
[754327.019939] mce: [Hardware Error]: PROCESSOR 0:50654 TIME 1616567954 SOCKET 0 APIC 27 microcode 200005e
[754327.034073] mce: [Hardware Error]: CPU 0: Machine Check Exception: 5 Bank 6: bb80000000000e0b
[754327.034324] mce: [Hardware Error]: RIP !INEXACT! 10:<ffffffff816ab596> {native_safe_halt+0x6/0x10}
[754327.034576] mce: [Hardware Error]: TSC 603312c7dc06c MISC 11000000
[754327.034709] mce: [Hardware Error]: PROCESSOR 0:50654 TIME 1616567954 SOCKET 0 APIC 0 microcode 200005e
[754327.048820] mce: [Hardware Error]: Machine check: Processor context corrupt
[754327.048948] Kernel panic - not syncing: Fatal machine check
```

## Backtraces

```
crash> bt
PID: 0      TASK: ffff8840939c2fa0  CPU: 29  COMMAND: "swapper/29"
 #0 [ffff885fbfa48c68] machine_kexec at ffffffff8105c52b
 #1 [ffff885fbfa48cc8] __crash_kexec at ffffffff81104a42
 #2 [ffff885fbfa48d98] panic at ffffffff8169dd4f
 #3 [ffff885fbfa48e18] mce_panic at ffffffff81043aaa
 #4 [ffff885fbfa48e58] do_machine_check at ffffffff8104551f
 #5 [ffff885fbfa48f50] machine_check at ffffffff816ac65f
    [exception RIP: native_safe_halt+6]
    RIP: ffffffff816ab596  RSP: ffff8840939d7ea8  RFLAGS: 00000286
    RAX: 00000000ffffffed  RBX: ffffffff81b1c860  RCX: 0100000000000000
    RDX: 0000000000000000  RSI: 0000000000000000  RDI: 0000000000000046
    RBP: ffff8840939d7ea8   R8: 0000000000000000   R9: 000000012d0cb94a
    R10: 0000000000000000  R11: 7fffffffffffffff  R12: 000000000000001d
    R13: ffff8840939d4000  R14: ffff8840939d4000  R15: ffff8840939d4000
    ORIG_RAX: ffffffffffffffff  CS: 0010  SS: 0018
--- <MCE exception stack> ---
 #6 [ffff8840939d7ea8] native_safe_halt at ffffffff816ab596
 #7 [ffff8840939d7eb0] default_idle at ffffffff816ab42e
 #8 [ffff8840939d7ed0] arch_cpu_idle at ffffffff81035006
 #9 [ffff8840939d7ee0] cpu_startup_entry at ffffffff810e7bda
#10 [ffff8840939d7f28] start_secondary at ffffffff81051b56
```

## 相关参数信息获取

`mce_panic`的第二个参数
```
crash> mce 0xffff881fbfc0cf20   <==该地址为栈中的mce_panic第二个参数的存储地址
struct mce {                                                                                                                                                                                                                                                                                                                                                                       
  status = 13510798882111491595,
  misc = 285212672,             
  addr = 0,                     
  mcgstatus = 5,                
  ip = 18446744071585838486,    
  tsc = 1692359594983532,       
  time = 1616567954,            
  cpuvendor = 0 '\000',         
  inject_flags = 0 '\000',      
  severity = 7 '\a',            
  pad = 0 '\000',               
  cpuid = 329300,               
  cs = 16 '\020',              
  bank = 6 '\006',             
  cpu = 0 '\000',
  finished = 1 '\001',
  extcpu = 0,
  socketid = 0,
  apicid = 0,
  mcgcap = 251660308,
  synd = 0,
  ipid = 0
}
```

# 初步分析
* `mce->status`存储的值为MSR`IA32_MCi_STATUS`, `mce->misc`存储的值为MSR`IA32_MCi_MISC`
* 根据这两个寄存器的值简单分析错误类型为`I/O Error`，而报告错误的PCIe device为`11:00.0`

# 结论
经过kernel代码分析并参照`参考链接[1]`，可得出结论：<br/>
**该问题是硬件问题，需要联系服务器厂商解决。**

**Note:** 根据`参考链接[1]`中的提示，已经将有关信息，汇总到上面的章节中，客户可以
给服务器厂商反馈这些问题时，提供这些信息。

# 参考链接
https://access.redhat.com/solutions/378653[1]<br />
https://www.kernel.org/doc/Documentation/devicetree/bindings/pci/pci-iommu.txt
