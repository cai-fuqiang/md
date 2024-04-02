# 堆栈
```
 #6 [ffffadd8a8a27af8] smp_call_function_single at ffffffffab34ba2e [ip]
    ffffadd8a8a27b00: 0000000000000010[cs] 0000000000000202[eflags]
    ffffadd8a8a27b10: ffffadd8a8a27b20[sp] 0000000000000018[ss]
    ffffadd8a8a27b20: ffffadd8a8a27b38[b20] 0000000000000018[b28]
    ffffadd8a8a27b30: ffff9474b11c3bb0[b30] 0000000000000003[b38]
    ffffadd8a8a27b40: ffffadd8a8a27b50 b2c55e33b93e3e00
    ffffadd8a8a27b50: ffffffffaba42ce2
 #7 [ffffadd8a8a27b50] search_extable at ffffffffaba42ce2
    ffffadd8a8a27b58: 2c592a2d4f618700[58] 0000000000000046[60]
    ffffadd8a8a27b68: 0000000000000000 ffffadd8a8a27c80[rbp]
    ffffadd8a8a27b78: ffffffffc35dfe83
 #8 [ffffadd8a8a27b78] vmx_vcpu_load at ffffffffc35dfe83 [kvm_intel]
    ffffadd8a8a27b80: 0000000000000046 ffffffffab267960
    ffffadd8a8a27b90: 0000000000000046 ffff9474b45667fc
    ffffadd8a8a27ba0: 0000000000000046 b2c55e33b93e3e00
    ffffadd8a8a27bb0: 0000000000000000 ffffadd8a8a27c18
    ffffadd8a8a27bc0: ffffffffabc001f1 ffffffffabc001e5
    ffffadd8a8a27bd0: ffffffffabc001f1 ffffffffabc001e5
    ffffadd8a8a27be0: ffffffffabc001f1 ffffffffabc001e5
    ffffadd8a8a27bf0: ffffffffabc001f1 ffffffffabc001e5
    ffffadd8a8a27c00: ffffffffabc001f1 ffffffffabc001e5
    ffffadd8a8a27c10: ffffffffabc001f1 ffffffffabc001e5
    ffffadd8a8a27c20: ffffffffabc001f1 ffffffffabc001e5
    ffffadd8a8a27c30: ffffffffabc001f1 2c592a2d4f618700
    ffffadd8a8a27c40: ffffffffabc001f1
```

# dis smp_call_function_single
```
0xffffffffab34b960 <smp_call_function_single>:  nopl   0x0(%rax,%rax,1) [FTRACE NOP]
0xffffffffab34b965 <smp_call_function_single+5>:        push   %rbp
0xffffffffab34b966 <smp_call_function_single+6>:        mov    %rsp,%rbp
0xffffffffab34b969 <smp_call_function_single+9>:        and    $0xffffffffffffffe0,%rsp
0xffffffffab34b96d <smp_call_function_single+13>:       sub    $0x40,%rsp
```
