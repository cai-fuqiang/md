# 堆栈
```
 #5 [ffffadd8a8a27a50] do_page_fault at ffffffffab268342
    ffffadd8a8a27a58: 0000000000000000 0000000000000000
    ffffadd8a8a27a68: 0000000000000000 ffffffffabc01c2a
    ffffadd8a8a27a78: 000000000000000c 000000000000000d
    ffffadd8a8a27a88: ffff94753a820900 000000000000000d
    ffffadd8a8a27a98: ffffadd8a8a27b70 ffff9474b11c0000
    ffffadd8a8a27aa8: 001dd4e7b30b38c0[bp] 0000000000000000[bx]
    ffffadd8a8a27ab8: 0000000000d01f7b[r11] 0000000000d01f7b[r10]
    ffffadd8a8a27ac8: 0000000000000000[r9] 0000000000000830[r8]
    ffffadd8a8a27ad8: 0000000000000001[rax] 00000000000008fb[rcx]
    ffffadd8a8a27ae8: 0000000000000830[dx] ffffffffffffff13[rsi]
    ffffadd8a8a27af8: ffffffffab34ba2e[rdi]
 #6 [ffffadd8a8a27af8] smp_call_function_single at ffffffffab34ba2e [ip]
    ffffadd8a8a27b00: 0000000000000010[orgax] 0000000000000202[ip]
    ffffadd8a8a27b10: ffffadd8a8a27b20[cs] 0000000000000018[flags]

    ffffadd8a8a27b20: ffffadd8a8a27b38[sp] 0000000000000018[ss]

    ...
```
这是通过 calltrace推出来的.
```
    ffffadd8a8a27aa8: 001dd4e7b30b38c0 0000000000000000
    ffffadd8a8a27ab8: 0000000000d01f7b 0000000000d01f7b
    ffffadd8a8a27ac8: 0000000000000000 0000000000000830
    ffffadd8a8a27ad8: 0000000000000001 00000000000008fb
    ffffadd8a8a27ae8: 0000000000000830 ffffffffffffff13
    ffffadd8a8a27af8: ffffffffab34ba2e
 #6 [ffffadd8a8a27af8] smp_call_function_single at ffffffffab34ba2e[ip]
    ffffadd8a8a27b00: 0000000000000010[cs] 0000000000000202[rflags]
    ffffadd8a8a27b10: ffffadd8a8a27b20[sp] 0000000000000018[ss]
    ffffadd8a8a27b20: ffffadd8a8a27b38 0000000000000018
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
