# mov rsp, rbp
* 没有mov的: `intel_pt_handle_vmx`
* 有mov的: `__schedule`

mov的原因感觉如下:
```
0xffffffffb5c605f0 <__schedule>:        push   %rbp
0xffffffffb5c605f1 <__schedule+1>:      mov    %rsp,%rbp
0xffffffffb5c605f4 <__schedule+4>:      push   %r15
0xffffffffb5c605f6 <__schedule+6>:      push   %r14
0xffffffffb5c605f8 <__schedule+8>:      push   %r13
0xffffffffb5c605fa <__schedule+10>:     mov    %edi,%r13d
0xffffffffb5c605fd <__schedule+13>:     push   %r12
0xffffffffb5c605ff <__schedule+15>:     push   %rbx
0xffffffffb5c60600 <__schedule+16>:     sub    $0x30,%rsp
0xffffffffb5c60604 <__schedule+20>:     mov    %gs:0x28,%rax
0xffffffffb5c6060d <__schedule+29>:     mov    %rax,-0x30(%rbp) <-----(1)
```
(1) 这个地方会将rbp
