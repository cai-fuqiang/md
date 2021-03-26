# 堆栈分析

## dis do_machine_check
```
0xffffffff810454da <do_machine_check+2458>:     cmpl   $0x6,-0xc0(%rbp)
0xffffffff810454e1 <do_machine_check+2465>:     mov    -0xe8(%rbp),%r13
0xffffffff810454e8 <do_machine_check+2472>:     jle    0xffffffff810455da <do_machine_check+2714>
0xffffffff810454ee <do_machine_check+2478>:     cmpq   $0x0,-0xd0(%rbp)
0xffffffff810454f6 <do_machine_check+2486>:     je     0xffffffff810455da <do_machine_check+2714>
0xffffffff810454fc <do_machine_check+2492>:     cmpl   $0x2,0xaca879(%rip)        # 0xffffffff81b0fd7c <mca_cfg+12>
0xffffffff81045503 <do_machine_check+2499>:     jg     0xffffffff8104551f <do_machine_check+2527>
0xffffffff81045505 <do_machine_check+2501>:     mov    -0xe0(%rbp),%rdx
0xffffffff8104550c <do_machine_check+2508>:     mov    -0xd0(%rbp),%rsi
0xffffffff81045513 <do_machine_check+2515>:     mov    $0xffffffff818e86e4,%rdi
0xffffffff8104551a <do_machine_check+2522>:     callq  0xffffffff810438b0 <mce_panic>
0xffffffff8104551f <do_machine_check+2527>:     mov    $0xffffffff,%esi
0xffffffff81045524 <do_machine_check+2532>:     mov    $0xcf20,%rbx

```

## dis mce_panic
```
0xffffffff810438b0 <mce_panic>: nopl   0x0(%rax,%rax,1) [FTRACE NOP]
0xffffffff810438b5 <mce_panic+5>:       push   %rbp
0xffffffff810438b6 <mce_panic+6>:       mov    $0x1,%eax
0xffffffff810438bb <mce_panic+11>:      mov    %rsp,%rbp
0xffffffff810438be <mce_panic+14>:      push   %r15
0xffffffff810438c0 <mce_panic+16>:      mov    %rdx,%r15
0xffffffff810438c3 <mce_panic+19>:      push   %r14
0xffffffff810438c5 <mce_panic+21>:      push   %r13
0xffffffff810438c7 <mce_panic+23>:      push   %r12
0xffffffff810438c9 <mce_panic+25>:      push   %rbx
0xffffffff810438ca <mce_panic+26>:      mov    %rsi,%rbx
0xffffffff810438cd <mce_panic+29>:      sub    $0x8,%rsp
0xffffffff810438d1 <mce_panic+33>:      mov    %rdi,-0x30(%rbp)
```

## 分析结果
通过上面的栈可以分析出，出问题的原因为
"Processor context corrupt"



## 红帽解决方案:
https://access.redhat.com/solutions/378653

# 其他错误
```
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

## 红帽解决方案
https://access.redhat.com/solutions/48109
(实际上该错误可以忽略)
