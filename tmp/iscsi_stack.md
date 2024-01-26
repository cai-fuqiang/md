# bt -f 
```
#2 [ffffabf37834bd00] schedule_timeout at ffffffff9aa6504d
    ffffabf37834bd08: ffffffff9bc38730 0000000000000048
    ffffabf37834bd18: ffffffff9b604ce0 0000000000000047
    ffffabf37834bd28: ffffffff9b538220 0000000000000047
    ffffabf37834bd38: 0000000000000002 0000000000000000
    ffffabf37834bd48: 0000000000000002 0000000000000000
    ffffabf37834bd58: 0000000000000000 0000000000000037
    ffffabf37834bd68: 9b1cd35b7ffee300 ffffabf37834be10
    ffffabf37834bd78: ffffabf37834be08 7fffffffffffffff
    ffffabf37834bd88: ffffabf37834bdc0 ffffabf37834be18
    ffffabf37834bd98: ffff8a58b75e5388 ffffffff9aa6182f
 #3 [ffffabf37834bda0] wait_for_completion at ffffffff9aa6182f
    ffffabf37834bda8: 0000000000000001 ffff8a4b14b217c0
    ffffabf37834bdb8: ffffffff9a2e0380 ffffabf37834be18
    ffffabf37834bdc8: ffffabf37834be18 9b1cd35b7ffee300
    ffffabf37834bdd8: ffff8a873881fe88[rbx] ffff8a4f46b3a000[rbp]
    ffffabf37834bde8: ffff8a58b75e5300[r12] 0000000050000058[r13]
    ffffabf37834bdf8: ffff8a4afd60c500[r14] ffffffffc0ab9cee[ip]
 #4 [ffffabf37834be00] target_put_cmd_and_wait at ffffffffc0ab9cee [target_core_mod]
    ffffabf37834be08: ffffffff00000000 0000000000000000
    ffffabf37834be18: ffffabf37834bdc0 ffffabf37834bdc0
    ffffabf37834be28: 9b1cd35b7ffee300 ffffffffc0ac377b
 #5 [ffffabf37834be30] core_tmr_abort_task.cold.6 at ffffffffc0ac377b [target_core_mod]
    ffffabf37834be38: ffff8a58b75e5398 0000000000000282
    ffffabf37834be48: ffff8a873881f6b8 ffff8a873881f588
    ffffabf37834be58: ffff8a4afd60c500 0000000000000000
    ffffabf37834be68: ffff8a4cea4c5b00 ffff8a873881f6c0
    ffffabf37834be78: ffffffffc0ab780a
 #6 [ffffabf37834be78] target_tmr_work at ffffffffc0ab780a [target_core_mod]
    ffffabf37834be80: ffff8a873881f6b8 ffff8a511eda2a00
    ffffabf37834be90: ffff8a511eda8700 ffffffff9a2cd5a7
```

# core_tmr_abort_task.cold.6+80
```
0xffffffffc0ac3773 <core_tmr_abort_task.cold.6+80>:     mov    %rbx,%rdi
0xffffffffc0ac3776 <core_tmr_abort_task.cold.6+83>:     callq  0xffffffffc0ab9c80 <target_put_cmd_and_wait>
0xffffffffc0ac377b <core_tmr_abort_task.cold.6+88>:     mov    %r13,%rsi
```

# 
