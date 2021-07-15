```
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8509
0xffffffffc079e8c0 <vmx_vcpu_run>:	nopl   0x0(%rax,%rax,1) [FTRACE NOP]
0xffffffffc079e8c5 <vmx_vcpu_run+5>:	push   %rbp
0xffffffffc079e8c6 <vmx_vcpu_run+6>:	mov    %rsp,%rbp
0xffffffffc079e8c9 <vmx_vcpu_run+9>:	push   %r15
0xffffffffc079e8cb <vmx_vcpu_run+11>:	push   %r14
0xffffffffc079e8cd <vmx_vcpu_run+13>:	push   %r13
0xffffffffc079e8cf <vmx_vcpu_run+15>:	push   %r12
0xffffffffc079e8d1 <vmx_vcpu_run+17>:	push   %rbx
0xffffffffc079e8d2 <vmx_vcpu_run+18>:	sub    $0x38,%rsp
0xffffffffc079e8d6 <vmx_vcpu_run+22>:	mov    %rdi,-0x58(%rbp)
0xffffffffc079e8da <vmx_vcpu_run+26>:	mov    %gs:0x28,%rax
0xffffffffc079e8e3 <vmx_vcpu_run+35>:	mov    %rax,-0x30(%rbp)
0xffffffffc079e8e7 <vmx_vcpu_run+39>:	xor    %eax,%eax
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8514
0xffffffffc079e8e9 <vmx_vcpu_run+41>:	testb  $0x20,0x194fc(%rip)        # 0xffffffffc07b7dec <vmcs_config+12>
0xffffffffc079e8f0 <vmx_vcpu_run+48>:	je     0xffffffffc079ef8b <vmx_vcpu_run+1739>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8519
0xffffffffc079e8f6 <vmx_vcpu_run+54>:	mov    -0x58(%rbp),%rax
0xffffffffc079e8fa <vmx_vcpu_run+58>:	cmpb   $0x0,0x3084(%rax)
0xffffffffc079e901 <vmx_vcpu_run+65>:	jne    0xffffffffc079ec60 <vmx_vcpu_run+928>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8522
0xffffffffc079e907 <vmx_vcpu_run+71>:	cmpb   $0x0,0x323c(%rax)
0xffffffffc079e90e <vmx_vcpu_run+78>:	jne    0xffffffffc079ecc0 <vmx_vcpu_run+1024>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8527
0xffffffffc079e914 <vmx_vcpu_run+84>:	mov    -0x58(%rbp),%rax
0xffffffffc079e918 <vmx_vcpu_run+88>:	cmpb   $0x0,0x3130(%rax)
0xffffffffc079e91f <vmx_vcpu_run+95>:	jne    0xffffffffc079ece8 <vmx_vcpu_run+1064>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/include/asm/bitops.h: 319
0xffffffffc079e925 <vmx_vcpu_run+101>:	mov    -0x58(%rbp),%rax
0xffffffffc079e929 <vmx_vcpu_run+105>:	mov    0x29c(%rax),%rax
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8532
0xffffffffc079e930 <vmx_vcpu_run+112>:	test   $0x10,%al
0xffffffffc079e932 <vmx_vcpu_run+114>:	jne    0xffffffffc079ee20 <vmx_vcpu_run+1376>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/include/asm/bitops.h: 319
0xffffffffc079e938 <vmx_vcpu_run+120>:	mov    -0x58(%rbp),%rax
0xffffffffc079e93c <vmx_vcpu_run+124>:	mov    0x29c(%rax),%rax
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8534
0xffffffffc079e943 <vmx_vcpu_run+131>:	test   $0x10000,%eax
0xffffffffc079e948 <vmx_vcpu_run+136>:	jne    0xffffffffc079ee40 <vmx_vcpu_run+1408>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/include/asm/paravirt.h: 86
0xffffffffc079e94e <vmx_vcpu_run+142>:	callq  0xffffffff810631f0 <native_read_cr4>
0xffffffffc079e953 <vmx_vcpu_run+147>:	xchg   %ax,%ax
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8538
0xffffffffc079e955 <vmx_vcpu_run+149>:	mov    -0x58(%rbp),%rsi
0xffffffffc079e959 <vmx_vcpu_run+153>:	cmp    %rax,0x2ee0(%rsi)
0xffffffffc079e960 <vmx_vcpu_run+160>:	jne    0xffffffffc079efb5 <vmx_vcpu_run+1781>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8548
0xffffffffc079e966 <vmx_vcpu_run+166>:	mov    -0x58(%rbp),%rax
0xffffffffc079e96a <vmx_vcpu_run+170>:	testb  $0x2,0x38(%rax)
0xffffffffc079e96e <vmx_vcpu_run+174>:	jne    0xffffffffc079ee60 <vmx_vcpu_run+1440>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8495
0xffffffffc079e974 <vmx_vcpu_run+180>:	lea    -0x4c(%rbp),%r13
0xffffffffc079e978 <vmx_vcpu_run+184>:	mov    %r13,%rdi
0xffffffffc079e97b <vmx_vcpu_run+187>:	callq  0xffffffff81008140 <perf_guest_get_msrs>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8497
0xffffffffc079e980 <vmx_vcpu_run+192>:	test   %rax,%rax
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8495
0xffffffffc079e983 <vmx_vcpu_run+195>:	mov    %rax,%r12
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8497
0xffffffffc079e986 <vmx_vcpu_run+198>:	je     0xffffffffc079e9d7 <vmx_vcpu_run+279>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8500
0xffffffffc079e988 <vmx_vcpu_run+200>:	mov    -0x4c(%rbp),%edi
0xffffffffc079e98b <vmx_vcpu_run+203>:	xor    %ebx,%ebx
0xffffffffc079e98d <vmx_vcpu_run+205>:	test   %edi,%edi
0xffffffffc079e98f <vmx_vcpu_run+207>:	jle    0xffffffffc079e9d7 <vmx_vcpu_run+279>
0xffffffffc079e991 <vmx_vcpu_run+209>:	mov    -0x58(%rbp),%r14
0xffffffffc079e995 <vmx_vcpu_run+213>:	jmp    0xffffffffc079e9ad <vmx_vcpu_run+237>
0xffffffffc079e997 <vmx_vcpu_run+215>:	nopw   0x0(%rax,%rax,1)
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8504
0xffffffffc079e9a0 <vmx_vcpu_run+224>:	callq  0xffffffffc0797ff0 <add_atomic_switch_msr>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8500
0xffffffffc079e9a5 <vmx_vcpu_run+229>:	add    $0x1,%ebx
0xffffffffc079e9a8 <vmx_vcpu_run+232>:	cmp    -0x4c(%rbp),%ebx
0xffffffffc079e9ab <vmx_vcpu_run+235>:	jge    0xffffffffc079e9d7 <vmx_vcpu_run+279>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8501
0xffffffffc079e9ad <vmx_vcpu_run+237>:	movslq %ebx,%rax
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8502
0xffffffffc079e9b0 <vmx_vcpu_run+240>:	mov    %r14,%rdi
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8501
0xffffffffc079e9b3 <vmx_vcpu_run+243>:	lea    (%rax,%rax,2),%rax
0xffffffffc079e9b7 <vmx_vcpu_run+247>:	lea    (%r12,%rax,8),%rax
0xffffffffc079e9bb <vmx_vcpu_run+251>:	mov    0x8(%rax),%rcx
0xffffffffc079e9bf <vmx_vcpu_run+255>:	mov    0x10(%rax),%rdx
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8502
0xffffffffc079e9c3 <vmx_vcpu_run+259>:	mov    (%rax),%esi
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8501
0xffffffffc079e9c5 <vmx_vcpu_run+261>:	cmp    %rdx,%rcx
0xffffffffc079e9c8 <vmx_vcpu_run+264>:	jne    0xffffffffc079e9a0 <vmx_vcpu_run+224>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8502
0xffffffffc079e9ca <vmx_vcpu_run+266>:	callq  0xffffffffc0797e50 <clear_atomic_switch_msr>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8500
0xffffffffc079e9cf <vmx_vcpu_run+271>:	add    $0x1,%ebx
0xffffffffc079e9d2 <vmx_vcpu_run+274>:	cmp    -0x4c(%rbp),%ebx
0xffffffffc079e9d5 <vmx_vcpu_run+277>:	jl     0xffffffffc079e9ad <vmx_vcpu_run+237>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/include/asm/paravirt.h: 129
0xffffffffc079e9d7 <vmx_vcpu_run+279>:	mov    $0x1d9,%edi
0xffffffffc079e9dc <vmx_vcpu_run+284>:	mov    %r13,%rsi
0xffffffffc079e9df <vmx_vcpu_run+287>:	callq  0xffffffff81063130 <native_read_msr_safe>
0xffffffffc079e9e4 <vmx_vcpu_run+292>:	xchg   %ax,%ax
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8554
0xffffffffc079e9e6 <vmx_vcpu_run+294>:	mov    -0x58(%rbp),%rcx
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/include/asm/paravirt.h: 129
0xffffffffc079e9ea <vmx_vcpu_run+298>:	mov    %rax,-0x60(%rbp)
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8555
0xffffffffc079e9ee <vmx_vcpu_run+302>:	mov    $0x6c14,%edx
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8554
0xffffffffc079e9f3 <vmx_vcpu_run+307>:	mov    0x2d98(%rcx),%rax
0xffffffffc079e9fa <vmx_vcpu_run+314>:	mov    0x14(%rax),%esi
0xffffffffc079e9fd <vmx_vcpu_run+317>:	test   %esi,%esi
0xffffffffc079e9ff <vmx_vcpu_run+319>:	setne  0x2da0(%rcx)
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8555
0xffffffffc079ea06 <vmx_vcpu_run+326>:	push   %rdx
0xffffffffc079ea07 <vmx_vcpu_run+327>:	push   %rbp
0xffffffffc079ea08 <vmx_vcpu_run+328>:	push   %rcx
0xffffffffc079ea09 <vmx_vcpu_run+329>:	push   %rcx
0xffffffffc079ea0a <vmx_vcpu_run+330>:	cmp    %rsp,0x2d20(%rcx)
0xffffffffc079ea11 <vmx_vcpu_run+337>:	je     0xffffffffc079ea1d <vmx_vcpu_run+349>
0xffffffffc079ea13 <vmx_vcpu_run+339>:	mov    %rsp,0x2d20(%rcx)
0xffffffffc079ea1a <vmx_vcpu_run+346>:	vmwrite %rsp,%rdx
0xffffffffc079ea1d <vmx_vcpu_run+349>:	mov    0x2b0(%rcx),%rax
0xffffffffc079ea24 <vmx_vcpu_run+356>:	mov    %cr2,%rdx
0xffffffffc079ea27 <vmx_vcpu_run+359>:	cmp    %rax,%rdx
0xffffffffc079ea2a <vmx_vcpu_run+362>:	je     0xffffffffc079ea2f <vmx_vcpu_run+367>
0xffffffffc079ea2c <vmx_vcpu_run+364>:	mov    %rax,%cr2
0xffffffffc079ea2f <vmx_vcpu_run+367>:	cmpl   $0x0,0x2da0(%rcx)
0xffffffffc079ea36 <vmx_vcpu_run+374>:	mov    0x210(%rcx),%rax
0xffffffffc079ea3d <vmx_vcpu_run+381>:	mov    0x228(%rcx),%rbx
0xffffffffc079ea44 <vmx_vcpu_run+388>:	mov    0x220(%rcx),%rdx
0xffffffffc079ea4b <vmx_vcpu_run+395>:	mov    0x240(%rcx),%rsi
0xffffffffc079ea52 <vmx_vcpu_run+402>:	mov    0x248(%rcx),%rdi
0xffffffffc079ea59 <vmx_vcpu_run+409>:	mov    0x238(%rcx),%rbp
0xffffffffc079ea60 <vmx_vcpu_run+416>:	mov    0x250(%rcx),%r8
0xffffffffc079ea67 <vmx_vcpu_run+423>:	mov    0x258(%rcx),%r9
0xffffffffc079ea6e <vmx_vcpu_run+430>:	mov    0x260(%rcx),%r10
0xffffffffc079ea75 <vmx_vcpu_run+437>:	mov    0x268(%rcx),%r11
0xffffffffc079ea7c <vmx_vcpu_run+444>:	mov    0x270(%rcx),%r12
0xffffffffc079ea83 <vmx_vcpu_run+451>:	mov    0x278(%rcx),%r13
0xffffffffc079ea8a <vmx_vcpu_run+458>:	mov    0x280(%rcx),%r14
0xffffffffc079ea91 <vmx_vcpu_run+465>:	mov    0x288(%rcx),%r15
0xffffffffc079ea98 <vmx_vcpu_run+472>:	mov    0x218(%rcx),%rcx
0xffffffffc079ea9f <vmx_vcpu_run+479>:	jne    0xffffffffc079eaa6 <vmx_vcpu_run+486>
0xffffffffc079eaa1 <vmx_vcpu_run+481>:	vmlaunch 
0xffffffffc079eaa4 <vmx_vcpu_run+484>:	jmp    0xffffffffc079eaa9 <vmx_vcpu_run+489>
0xffffffffc079eaa6 <vmx_vcpu_run+486>:	vmresume 
0xffffffffc079eaa9 <vmx_vcpu_run+489>:	mov    %rcx,0x8(%rsp)
0xffffffffc079eaae <vmx_vcpu_run+494>:	pop    %rcx
0xffffffffc079eaaf <vmx_vcpu_run+495>:	mov    %rax,0x210(%rcx)
0xffffffffc079eab6 <vmx_vcpu_run+502>:	mov    %rbx,0x228(%rcx)
0xffffffffc079eabd <vmx_vcpu_run+509>:	popq   0x218(%rcx)
0xffffffffc079eac3 <vmx_vcpu_run+515>:	mov    %rdx,0x220(%rcx)
0xffffffffc079eaca <vmx_vcpu_run+522>:	mov    %rsi,0x240(%rcx)
0xffffffffc079ead1 <vmx_vcpu_run+529>:	mov    %rdi,0x248(%rcx)
0xffffffffc079ead8 <vmx_vcpu_run+536>:	mov    %rbp,0x238(%rcx)
0xffffffffc079eadf <vmx_vcpu_run+543>:	mov    %r8,0x250(%rcx)
0xffffffffc079eae6 <vmx_vcpu_run+550>:	mov    %r9,0x258(%rcx)
0xffffffffc079eaed <vmx_vcpu_run+557>:	mov    %r10,0x260(%rcx)
0xffffffffc079eaf4 <vmx_vcpu_run+564>:	mov    %r11,0x268(%rcx)
0xffffffffc079eafb <vmx_vcpu_run+571>:	mov    %r12,0x270(%rcx)
0xffffffffc079eb02 <vmx_vcpu_run+578>:	mov    %r13,0x278(%rcx)
0xffffffffc079eb09 <vmx_vcpu_run+585>:	mov    %r14,0x280(%rcx)
0xffffffffc079eb10 <vmx_vcpu_run+592>:	mov    %r15,0x288(%rcx)
0xffffffffc079eb17 <vmx_vcpu_run+599>:	mov    %cr2,%rax
0xffffffffc079eb1a <vmx_vcpu_run+602>:	mov    %rax,0x2b0(%rcx)
0xffffffffc079eb21 <vmx_vcpu_run+609>:	pop    %rbp
0xffffffffc079eb22 <vmx_vcpu_run+610>:	pop    %rdx
0xffffffffc079eb23 <vmx_vcpu_run+611>:	setbe  0x2d28(%rcx)
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8661
0xffffffffc079eb2a <vmx_vcpu_run+618>:	cmpq   $0x0,-0x60(%rbp)
0xffffffffc079eb2f <vmx_vcpu_run+623>:	jne    0xffffffffc079ee70 <vmx_vcpu_run+1456>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8677
0xffffffffc079eb35 <vmx_vcpu_run+629>:	mov    -0x58(%rbp),%rbx
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 1477
0xffffffffc079eb39 <vmx_vcpu_run+633>:	mov    $0x4408,%edx
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8677
0xffffffffc079eb3e <vmx_vcpu_run+638>:	movl   $0xffe0ffef,0x298(%rbx)
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8682
0xffffffffc079eb48 <vmx_vcpu_run+648>:	movl   $0x0,0x29c(%rbx)
0xffffffffc079eb52 <vmx_vcpu_run+658>:	vmread %rdx,%rax
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 1489
0xffffffffc079eb55 <vmx_vcpu_run+661>:	mov    %eax,0x2d30(%rbx)
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8686
0xffffffffc079eb5b <vmx_vcpu_run+667>:	mov    0x2d98(%rbx),%rax
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 1477
0xffffffffc079eb62 <vmx_vcpu_run+674>:	mov    $0x2,%dl
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8686
0xffffffffc079eb64 <vmx_vcpu_run+676>:	movl   $0x1,0x14(%rax)
0xffffffffc079eb6b <vmx_vcpu_run+683>:	vmread %rdx,%rax
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8695
0xffffffffc079eb6e <vmx_vcpu_run+686>:	cmpb   $0x0,0x314d(%rbx)
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 1489
0xffffffffc079eb75 <vmx_vcpu_run+693>:	mov    %eax,%edx
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8688
0xffffffffc079eb77 <vmx_vcpu_run+695>:	mov    %eax,0x30a0(%rbx)
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8695
0xffffffffc079eb7d <vmx_vcpu_run+701>:	jne    0xffffffffc079ee90 <vmx_vcpu_run+1488>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8698
0xffffffffc079eb83 <vmx_vcpu_run+707>:	mov    -0x58(%rbp),%rax
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8299
0xffffffffc079eb87 <vmx_vcpu_run+711>:	test   %edx,%edx
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8698
0xffffffffc079eb89 <vmx_vcpu_run+713>:	movb   $0x0,0x314d(%rax)
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8299
0xffffffffc079eb90 <vmx_vcpu_run+720>:	jne    0xffffffffc079ec88 <vmx_vcpu_run+968>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 1477
0xffffffffc079eb96 <vmx_vcpu_run+726>:	mov    $0x4404,%edx
0xffffffffc079eb9b <vmx_vcpu_run+731>:	vmread %rdx,%rax
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8303
0xffffffffc079eb9e <vmx_vcpu_run+734>:	mov    -0x58(%rbp),%rsi
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 1020
0xffffffffc079eba2 <vmx_vcpu_run+738>:	mov    %eax,%edx
0xffffffffc079eba4 <vmx_vcpu_run+740>:	and    $0x800007ff,%edx
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8307
0xffffffffc079ebaa <vmx_vcpu_run+746>:	cmp    $0x80000312,%edx
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8303
0xffffffffc079ebb0 <vmx_vcpu_run+752>:	mov    %eax,0x2d2c(%rsi)
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8307
0xffffffffc079ebb6 <vmx_vcpu_run+758>:	je     0xffffffffc079ef4b <vmx_vcpu_run+1675>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 1269
0xffffffffc079ebbc <vmx_vcpu_run+764>:	and    $0x80000700,%eax
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8311
0xffffffffc079ebc1 <vmx_vcpu_run+769>:	cmp    $0x80000200,%eax
0xffffffffc079ebc6 <vmx_vcpu_run+774>:	je     0xffffffffc079ef20 <vmx_vcpu_run+1632>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8384
0xffffffffc079ebcc <vmx_vcpu_run+780>:	mov    -0x58(%rbp),%rax
0xffffffffc079ebd0 <vmx_vcpu_run+784>:	mov    0x2d30(%rax),%ecx
0xffffffffc079ebd6 <vmx_vcpu_run+790>:	mov    %ecx,%edi
0xffffffffc079ebd8 <vmx_vcpu_run+792>:	and    $0x80000000,%edi
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8386
0xffffffffc079ebde <vmx_vcpu_run+798>:	testb  $0x20,0x19207(%rip)        # 0xffffffffc07b7dec <vmcs_config+12>
0xffffffffc079ebe5 <vmx_vcpu_run+805>:	je     0xffffffffc079eca0 <vmx_vcpu_run+992>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8387
0xffffffffc079ebeb <vmx_vcpu_run+811>:	cmpb   $0x0,0x2d29(%rax)
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8384
0xffffffffc079ebf2 <vmx_vcpu_run+818>:	mov    %ecx,%esi
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8387
0xffffffffc079ebf4 <vmx_vcpu_run+820>:	jne    0xffffffffc079ec31 <vmx_vcpu_run+881>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 1477
0xffffffffc079ebf6 <vmx_vcpu_run+822>:	mov    $0x4404,%edx
0xffffffffc079ebfb <vmx_vcpu_run+827>:	vmread %rdx,%rax
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8406
0xffffffffc079ebfe <vmx_vcpu_run+830>:	test   $0x10,%ah
0xffffffffc079ec01 <vmx_vcpu_run+833>:	je     0xffffffffc079ec14 <vmx_vcpu_run+852>
0xffffffffc079ec03 <vmx_vcpu_run+835>:	mov    %eax,%edx
0xffffffffc079ec05 <vmx_vcpu_run+837>:	shr    $0x1f,%edx
0xffffffffc079ec08 <vmx_vcpu_run+840>:	test   %dl,%dl
0xffffffffc079ec0a <vmx_vcpu_run+842>:	je     0xffffffffc079ec14 <vmx_vcpu_run+852>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8407
0xffffffffc079ec0c <vmx_vcpu_run+844>:	cmp    $0x8,%al
0xffffffffc079ec0e <vmx_vcpu_run+846>:	jne    0xffffffffc079ef5d <vmx_vcpu_run+1693>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 1477
0xffffffffc079ec14 <vmx_vcpu_run+852>:	mov    $0x4824,%edx
0xffffffffc079ec19 <vmx_vcpu_run+857>:	vmread %rdx,%rax
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8412
0xffffffffc079ec1c <vmx_vcpu_run+860>:	mov    -0x58(%rbp),%rsi
0xffffffffc079ec20 <vmx_vcpu_run+864>:	shr    $0x3,%eax
0xffffffffc079ec23 <vmx_vcpu_run+867>:	xor    $0x1,%eax
0xffffffffc079ec26 <vmx_vcpu_run+870>:	and    $0x1,%eax
0xffffffffc079ec29 <vmx_vcpu_run+873>:	mov    %al,0x2d29(%rsi)
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8384
0xffffffffc079ec2f <vmx_vcpu_run+879>:	mov    %ecx,%esi
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8430
0xffffffffc079ec31 <vmx_vcpu_run+881>:	mov    -0x58(%rbp),%rax
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8434
0xffffffffc079ec35 <vmx_vcpu_run+885>:	test   %edi,%edi
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8430
0xffffffffc079ec37 <vmx_vcpu_run+887>:	movb   $0x0,0x2228(%rax)
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/x86.h: 12
0xffffffffc079ec3e <vmx_vcpu_run+894>:	movb   $0x0,0x9f4(%rax)
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/x86.h: 25
0xffffffffc079ec45 <vmx_vcpu_run+901>:	movb   $0x0,0x9fc(%rax)
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8434
0xffffffffc079ec4c <vmx_vcpu_run+908>:	je     0xffffffffc079ec60 <vmx_vcpu_run+928>
0xffffffffc079ec4e <vmx_vcpu_run+910>:	mov    $0x440a,%ecx
0xffffffffc079ec53 <vmx_vcpu_run+915>:	mov    $0x440c,%edx
0xffffffffc079ec58 <vmx_vcpu_run+920>:	mov    %rax,%rdi
0xffffffffc079ec5b <vmx_vcpu_run+923>:	callq  0xffffffffc079a2e0 <__vmx_complete_interrupts>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8703
0xffffffffc079ec60 <vmx_vcpu_run+928>:	mov    -0x30(%rbp),%rax
0xffffffffc079ec64 <vmx_vcpu_run+932>:	xor    %gs:0x28,%rax
0xffffffffc079ec6d <vmx_vcpu_run+941>:	jne    0xffffffffc079efb0 <vmx_vcpu_run+1776>
0xffffffffc079ec73 <vmx_vcpu_run+947>:	add    $0x38,%rsp
0xffffffffc079ec77 <vmx_vcpu_run+951>:	pop    %rbx
0xffffffffc079ec78 <vmx_vcpu_run+952>:	pop    %r12
0xffffffffc079ec7a <vmx_vcpu_run+954>:	pop    %r13
0xffffffffc079ec7c <vmx_vcpu_run+956>:	pop    %r14
0xffffffffc079ec7e <vmx_vcpu_run+958>:	pop    %r15
0xffffffffc079ec80 <vmx_vcpu_run+960>:	pop    %rbp
0xffffffffc079ec81 <vmx_vcpu_run+961>:	retq   
0xffffffffc079ec82 <vmx_vcpu_run+962>:	nopw   0x0(%rax,%rax,1)
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8299
0xffffffffc079ec88 <vmx_vcpu_run+968>:	cmp    $0x29,%edx
0xffffffffc079ec8b <vmx_vcpu_run+971>:	je     0xffffffffc079eb96 <vmx_vcpu_run+726>
0xffffffffc079ec91 <vmx_vcpu_run+977>:	jmpq   0xffffffffc079ebcc <vmx_vcpu_run+780>
0xffffffffc079ec96 <vmx_vcpu_run+982>:	nopw   %cs:0x0(%rax,%rax,1)
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8414
0xffffffffc079eca0 <vmx_vcpu_run+992>:	mov    -0x58(%rbp),%rax
0xffffffffc079eca4 <vmx_vcpu_run+996>:	mov    0x3088(%rax),%edx
0xffffffffc079ecaa <vmx_vcpu_run+1002>:	test   %edx,%edx
0xffffffffc079ecac <vmx_vcpu_run+1004>:	jne    0xffffffffc079efda <vmx_vcpu_run+1818>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8384
0xffffffffc079ecb2 <vmx_vcpu_run+1010>:	mov    %ecx,%esi
0xffffffffc079ecb4 <vmx_vcpu_run+1012>:	jmpq   0xffffffffc079ec31 <vmx_vcpu_run+881>
0xffffffffc079ecb9 <vmx_vcpu_run+1017>:	nopl   0x0(%rax)
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 1525
0xffffffffc079ecc0 <vmx_vcpu_run+1024>:	mov    0x3238(%rax),%esi
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8523
0xffffffffc079ecc6 <vmx_vcpu_run+1030>:	movb   $0x0,0x323c(%rax)
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 1525
0xffffffffc079eccd <vmx_vcpu_run+1037>:	mov    $0x4022,%edi
0xffffffffc079ecd2 <vmx_vcpu_run+1042>:	callq  0xffffffffc0795270 <vmcs_writel>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8527
0xffffffffc079ecd7 <vmx_vcpu_run+1047>:	mov    -0x58(%rbp),%rax
0xffffffffc079ecdb <vmx_vcpu_run+1051>:	cmpb   $0x0,0x3130(%rax)
0xffffffffc079ece2 <vmx_vcpu_run+1058>:	je     0xffffffffc079e925 <vmx_vcpu_run+101>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 7081
0xffffffffc079ece8 <vmx_vcpu_run+1064>:	mov    0x2d78(%rax),%r14
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 7074
0xffffffffc079ecef <vmx_vcpu_run+1071>:	mov    0x18773(%rip),%ebx        # 0xffffffffc07b7468 <max_shadow_read_write_fields>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 7081
0xffffffffc079ecf5 <vmx_vcpu_run+1077>:	mov    %rax,%r15
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 7070
0xffffffffc079ecf8 <vmx_vcpu_run+1080>:	movq   $0xffffffffc07b7480,-0x40(%rbp)
0xffffffffc079ed00 <vmx_vcpu_run+1088>:	movq   $0xffffffffc07b7560,-0x38(%rbp)
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 7083
0xffffffffc079ed08 <vmx_vcpu_run+1096>:	xor    %r13d,%r13d
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 7074
0xffffffffc079ed0b <vmx_vcpu_run+1099>:	movl   $0x9,-0x44(%rbp)
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 7083
0xffffffffc079ed12 <vmx_vcpu_run+1106>:	mov    %r14,%rdi
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 7074
0xffffffffc079ed15 <vmx_vcpu_run+1109>:	mov    %ebx,-0x48(%rbp)
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 7083
0xffffffffc079ed18 <vmx_vcpu_run+1112>:	callq  0xffffffffc0794810 <vmcs_load>
0xffffffffc079ed1d <vmx_vcpu_run+1117>:	mov    %ebx,%edx
0xffffffffc079ed1f <vmx_vcpu_run+1119>:	mov    %r14,-0x60(%rbp)
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 7080
0xffffffffc079ed23 <vmx_vcpu_run+1123>:	xor    %ebx,%ebx
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 7086
0xffffffffc079ed25 <vmx_vcpu_run+1125>:	test   %edx,%edx
0xffffffffc079ed27 <vmx_vcpu_run+1127>:	jle    0xffffffffc079ee00 <vmx_vcpu_run+1344>
0xffffffffc079ed2d <vmx_vcpu_run+1133>:	mov    -0x40(%rbp,%r13,2),%rcx
0xffffffffc079ed32 <vmx_vcpu_run+1138>:	sub    $0x1,%edx
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 7087
0xffffffffc079ed35 <vmx_vcpu_run+1141>:	mov    (%rcx),%rdi
0xffffffffc079ed38 <vmx_vcpu_run+1144>:	lea    0x8(%rcx),%r14
0xffffffffc079ed3c <vmx_vcpu_run+1148>:	lea    (%r14,%rdx,8),%r12
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 848
0xffffffffc079ed40 <vmx_vcpu_run+1152>:	cmp    $0x6c16,%rdi
0xffffffffc079ed47 <vmx_vcpu_run+1159>:	ja     0xffffffffc079edcd <vmx_vcpu_run+1293>
0xffffffffc079ed4d <vmx_vcpu_run+1165>:	nopl   (%rax)
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 849
0xffffffffc079ed50 <vmx_vcpu_run+1168>:	movswq -0x3f8599c0(%rdi,%rdi,1),%rdx
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 848
0xffffffffc079ed59 <vmx_vcpu_run+1177>:	test   %dx,%dx
0xffffffffc079ed5c <vmx_vcpu_run+1180>:	je     0xffffffffc079edcd <vmx_vcpu_run+1293>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 6975
0xffffffffc079ed5e <vmx_vcpu_run+1182>:	js     0xffffffffc079edcd <vmx_vcpu_run+1293>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 6952
0xffffffffc079ed60 <vmx_vcpu_run+1184>:	mov    %rdi,%rcx
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 6978
0xffffffffc079ed63 <vmx_vcpu_run+1187>:	add    0x3128(%r15),%rdx
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 6952
0xffffffffc079ed6a <vmx_vcpu_run+1194>:	and    $0x1,%ecx
0xffffffffc079ed6d <vmx_vcpu_run+1197>:	jne    0xffffffffc079edd8 <vmx_vcpu_run+1304>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 6954
0xffffffffc079ed6f <vmx_vcpu_run+1199>:	mov    %rdi,%rsi
0xffffffffc079ed72 <vmx_vcpu_run+1202>:	shr    $0xd,%rsi
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 6980
0xffffffffc079ed76 <vmx_vcpu_run+1206>:	cmp    $0x2,%esi
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 6954
0xffffffffc079ed79 <vmx_vcpu_run+1209>:	mov    %esi,%r8d
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 6980
0xffffffffc079ed7c <vmx_vcpu_run+1212>:	je     0xffffffffc079edd8 <vmx_vcpu_run+1304>
0xffffffffc079ed7e <vmx_vcpu_run+1214>:	cmp    $0x3,%esi
0xffffffffc079ed81 <vmx_vcpu_run+1217>:	je     0xffffffffc079eed0 <vmx_vcpu_run+1552>
0xffffffffc079ed87 <vmx_vcpu_run+1223>:	cmp    $0x1,%esi
0xffffffffc079ed8a <vmx_vcpu_run+1226>:	je     0xffffffffc079eed0 <vmx_vcpu_run+1552>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 6985
0xffffffffc079ed90 <vmx_vcpu_run+1232>:	movzwl (%rdx),%ebx
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 7090
0xffffffffc079ed93 <vmx_vcpu_run+1235>:	cmp    $0x2,%r8d
0xffffffffc079ed97 <vmx_vcpu_run+1239>:	je     0xffffffffc079ede9 <vmx_vcpu_run+1321>
0xffffffffc079ed99 <vmx_vcpu_run+1241>:	cmp    $0x3,%r8d
0xffffffffc079ed9d <vmx_vcpu_run+1245>:	je     0xffffffffc079eec0 <vmx_vcpu_run+1536>
0xffffffffc079eda3 <vmx_vcpu_run+1251>:	cmp    $0x1,%r8d
0xffffffffc079eda7 <vmx_vcpu_run+1255>:	je     0xffffffffc079eec0 <vmx_vcpu_run+1536>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 1520
0xffffffffc079edad <vmx_vcpu_run+1261>:	movzwl %bx,%esi
0xffffffffc079edb0 <vmx_vcpu_run+1264>:	callq  0xffffffffc0795270 <vmcs_writel>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 7086
0xffffffffc079edb5 <vmx_vcpu_run+1269>:	cmp    %r12,%r14
0xffffffffc079edb8 <vmx_vcpu_run+1272>:	mov    %r14,%rcx
0xffffffffc079edbb <vmx_vcpu_run+1275>:	je     0xffffffffc079ee00 <vmx_vcpu_run+1344>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 7087
0xffffffffc079edbd <vmx_vcpu_run+1277>:	mov    (%rcx),%rdi
0xffffffffc079edc0 <vmx_vcpu_run+1280>:	add    $0x8,%r14
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 848
0xffffffffc079edc4 <vmx_vcpu_run+1284>:	cmp    $0x6c16,%rdi
0xffffffffc079edcb <vmx_vcpu_run+1291>:	jbe    0xffffffffc079ed50 <vmx_vcpu_run+1168>
0xffffffffc079edcd <vmx_vcpu_run+1293>:	mov    %rdi,%rcx
0xffffffffc079edd0 <vmx_vcpu_run+1296>:	and    $0x1,%ecx
0xffffffffc079edd3 <vmx_vcpu_run+1299>:	jmp    0xffffffffc079ede0 <vmx_vcpu_run+1312>
0xffffffffc079edd5 <vmx_vcpu_run+1301>:	nopl   (%rax)
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 6988
0xffffffffc079edd8 <vmx_vcpu_run+1304>:	mov    (%rdx),%ebx
0xffffffffc079edda <vmx_vcpu_run+1306>:	nopw   0x0(%rax,%rax,1)
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 6952
0xffffffffc079ede0 <vmx_vcpu_run+1312>:	test   %rcx,%rcx
0xffffffffc079ede3 <vmx_vcpu_run+1315>:	je     0xffffffffc079ef3b <vmx_vcpu_run+1659>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 1525
0xffffffffc079ede9 <vmx_vcpu_run+1321>:	mov    %ebx,%esi
0xffffffffc079edeb <vmx_vcpu_run+1323>:	callq  0xffffffffc0795270 <vmcs_writel>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 7086
0xffffffffc079edf0 <vmx_vcpu_run+1328>:	cmp    %r12,%r14
0xffffffffc079edf3 <vmx_vcpu_run+1331>:	mov    %r14,%rcx
0xffffffffc079edf6 <vmx_vcpu_run+1334>:	jne    0xffffffffc079edbd <vmx_vcpu_run+1277>
0xffffffffc079edf8 <vmx_vcpu_run+1336>:	nopl   0x0(%rax,%rax,1)
0xffffffffc079ee00 <vmx_vcpu_run+1344>:	add    $0x4,%r13
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 7085
0xffffffffc079ee04 <vmx_vcpu_run+1348>:	cmp    $0x8,%r13
0xffffffffc079ee08 <vmx_vcpu_run+1352>:	je     0xffffffffc079eee0 <vmx_vcpu_run+1568>
0xffffffffc079ee0e <vmx_vcpu_run+1358>:	mov    -0x48(%rbp,%r13,1),%edx
0xffffffffc079ee13 <vmx_vcpu_run+1363>:	jmpq   0xffffffffc079ed25 <vmx_vcpu_run+1125>
0xffffffffc079ee18 <vmx_vcpu_run+1368>:	nopl   0x0(%rax,%rax,1)
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8533
0xffffffffc079ee20 <vmx_vcpu_run+1376>:	mov    -0x58(%rbp),%rax
0xffffffffc079ee24 <vmx_vcpu_run+1380>:	mov    $0x681c,%edi
0xffffffffc079ee29 <vmx_vcpu_run+1385>:	mov    0x230(%rax),%rsi
0xffffffffc079ee30 <vmx_vcpu_run+1392>:	callq  0xffffffffc0795270 <vmcs_writel>
0xffffffffc079ee35 <vmx_vcpu_run+1397>:	jmpq   0xffffffffc079e938 <vmx_vcpu_run+120>
0xffffffffc079ee3a <vmx_vcpu_run+1402>:	nopw   0x0(%rax,%rax,1)
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8535
0xffffffffc079ee40 <vmx_vcpu_run+1408>:	mov    -0x58(%rbp),%rax
0xffffffffc079ee44 <vmx_vcpu_run+1412>:	mov    $0x681e,%edi
0xffffffffc079ee49 <vmx_vcpu_run+1417>:	mov    0x290(%rax),%rsi
0xffffffffc079ee50 <vmx_vcpu_run+1424>:	callq  0xffffffffc0795270 <vmcs_writel>
0xffffffffc079ee55 <vmx_vcpu_run+1429>:	jmpq   0xffffffffc079e94e <vmx_vcpu_run+142>
0xffffffffc079ee5a <vmx_vcpu_run+1434>:	nopw   0x0(%rax,%rax,1)
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8549
0xffffffffc079ee60 <vmx_vcpu_run+1440>:	xor    %esi,%esi
0xffffffffc079ee62 <vmx_vcpu_run+1442>:	mov    %rax,%rdi
0xffffffffc079ee65 <vmx_vcpu_run+1445>:	callq  0xffffffffc0795320 <vmx_set_interrupt_shadow>
0xffffffffc079ee6a <vmx_vcpu_run+1450>:	jmpq   0xffffffffc079e974 <vmx_vcpu_run+180>
0xffffffffc079ee6f <vmx_vcpu_run+1455>:	nop
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/include/asm/paravirt.h: 134
0xffffffffc079ee70 <vmx_vcpu_run+1456>:	mov    -0x60(%rbp),%rdx
0xffffffffc079ee74 <vmx_vcpu_run+1460>:	mov    $0x1d9,%edi
0xffffffffc079ee79 <vmx_vcpu_run+1465>:	mov    %edx,%esi
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/include/asm/processor.h: 793
0xffffffffc079ee7b <vmx_vcpu_run+1467>:	shr    $0x20,%rdx
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/include/asm/paravirt.h: 134
0xffffffffc079ee7f <vmx_vcpu_run+1471>:	callq  0xffffffff81063150 <native_write_msr_safe>
0xffffffffc079ee84 <vmx_vcpu_run+1476>:	xchg   %ax,%ax
0xffffffffc079ee86 <vmx_vcpu_run+1478>:	jmpq   0xffffffffc079eb35 <vmx_vcpu_run+629>
0xffffffffc079ee8b <vmx_vcpu_run+1483>:	nopl   0x0(%rax,%rax,1)
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/include/asm/bitops.h: 66
0xffffffffc079ee90 <vmx_vcpu_run+1488>:	mov    -0x58(%rbp),%rax
0xffffffffc079ee94 <vmx_vcpu_run+1492>:	lock orb $0x8,0x31(%rax)
0xffffffffc079ee99 <vmx_vcpu_run+1497>:	mov    0x30a0(%rax),%edx
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8698
0xffffffffc079ee9f <vmx_vcpu_run+1503>:	mov    -0x58(%rbp),%rax
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8299
0xffffffffc079eea3 <vmx_vcpu_run+1507>:	test   %edx,%edx
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8698
0xffffffffc079eea5 <vmx_vcpu_run+1509>:	movb   $0x0,0x314d(%rax)
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8299
0xffffffffc079eeac <vmx_vcpu_run+1516>:	je     0xffffffffc079eb96 <vmx_vcpu_run+726>
0xffffffffc079eeb2 <vmx_vcpu_run+1522>:	jmpq   0xffffffffc079ec88 <vmx_vcpu_run+968>
0xffffffffc079eeb7 <vmx_vcpu_run+1527>:	nopw   0x0(%rax,%rax,1)
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 7101
0xffffffffc079eec0 <vmx_vcpu_run+1536>:	mov    %rbx,%rsi
0xffffffffc079eec3 <vmx_vcpu_run+1539>:	callq  0xffffffffc0795270 <vmcs_writel>
0xffffffffc079eec8 <vmx_vcpu_run+1544>:	jmpq   0xffffffffc079edb5 <vmx_vcpu_run+1269>
0xffffffffc079eecd <vmx_vcpu_run+1549>:	nopl   (%rax)
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 6991
0xffffffffc079eed0 <vmx_vcpu_run+1552>:	mov    (%rdx),%rbx
0xffffffffc079eed3 <vmx_vcpu_run+1555>:	jmpq   0xffffffffc079ed93 <vmx_vcpu_run+1235>
0xffffffffc079eed8 <vmx_vcpu_run+1560>:	nopl   0x0(%rax,%rax,1)
0xffffffffc079eee0 <vmx_vcpu_run+1568>:	mov    -0x60(%rbp),%r14
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 7110
0xffffffffc079eee4 <vmx_vcpu_run+1572>:	mov    %r14,%rdi
0xffffffffc079eee7 <vmx_vcpu_run+1575>:	callq  0xffffffffc0794710 <vmcs_clear>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 7111
0xffffffffc079eeec <vmx_vcpu_run+1580>:	mov    -0x58(%rbp),%rbx
0xffffffffc079eef0 <vmx_vcpu_run+1584>:	mov    0x2d98(%rbx),%rax
0xffffffffc079eef7 <vmx_vcpu_run+1591>:	mov    (%rax),%rdi
0xffffffffc079eefa <vmx_vcpu_run+1594>:	callq  0xffffffffc0794810 <vmcs_load>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/include/asm/bitops.h: 319
0xffffffffc079eeff <vmx_vcpu_run+1599>:	mov    -0x58(%rbp),%rax
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8529
0xffffffffc079ef03 <vmx_vcpu_run+1603>:	movb   $0x0,0x3130(%rbx)
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/include/asm/bitops.h: 319
0xffffffffc079ef0a <vmx_vcpu_run+1610>:	mov    0x29c(%rax),%rax
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8532
0xffffffffc079ef11 <vmx_vcpu_run+1617>:	test   $0x10,%al
0xffffffffc079ef13 <vmx_vcpu_run+1619>:	je     0xffffffffc079e938 <vmx_vcpu_run+120>
0xffffffffc079ef19 <vmx_vcpu_run+1625>:	jmpq   0xffffffffc079ee20 <vmx_vcpu_run+1376>
0xffffffffc079ef1e <vmx_vcpu_run+1630>:	xchg   %ax,%ax
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8312
0xffffffffc079ef20 <vmx_vcpu_run+1632>:	mov    -0x58(%rbp),%rbx
0xffffffffc079ef24 <vmx_vcpu_run+1636>:	mov    %rbx,%rdi
0xffffffffc079ef27 <vmx_vcpu_run+1639>:	callq  0xffffffffc0985e90 <kvm_before_handle_nmi>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8313
0xffffffffc079ef2c <vmx_vcpu_run+1644>:	int    $0x2
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8314
0xffffffffc079ef2e <vmx_vcpu_run+1646>:	mov    %rbx,%rdi
0xffffffffc079ef31 <vmx_vcpu_run+1649>:	callq  0xffffffffc0985eb0 <kvm_after_handle_nmi>
0xffffffffc079ef36 <vmx_vcpu_run+1654>:	jmpq   0xffffffffc079ebcc <vmx_vcpu_run+780>
0xffffffffc079ef3b <vmx_vcpu_run+1659>:	mov    %rdi,%r8
0xffffffffc079ef3e <vmx_vcpu_run+1662>:	shr    $0xd,%r8
0xffffffffc079ef42 <vmx_vcpu_run+1666>:	and    $0x3,%r8d
0xffffffffc079ef46 <vmx_vcpu_run+1670>:	jmpq   0xffffffffc079ed93 <vmx_vcpu_run+1235>
0xffffffffc079ef4b <vmx_vcpu_run+1675>:	mov    %rax,-0x60(%rbp)
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8308
0xffffffffc079ef4f <vmx_vcpu_run+1679>:	callq  0xffffffffc0795f40 <kvm_machine_check>
0xffffffffc079ef54 <vmx_vcpu_run+1684>:	mov    -0x60(%rbp),%rax
0xffffffffc079ef58 <vmx_vcpu_run+1688>:	jmpq   0xffffffffc079ebbc <vmx_vcpu_run+764>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8407
0xffffffffc079ef5d <vmx_vcpu_run+1693>:	test   %edi,%edi
0xffffffffc079ef5f <vmx_vcpu_run+1695>:	jne    0xffffffffc079ec14 <vmx_vcpu_run+852>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8408
0xffffffffc079ef65 <vmx_vcpu_run+1701>:	mov    $0x8,%esi
0xffffffffc079ef6a <vmx_vcpu_run+1706>:	mov    $0x4824,%edi
0xffffffffc079ef6f <vmx_vcpu_run+1711>:	callq  0xffffffffc0795500 <vmcs_set_bits>
0xffffffffc079ef74 <vmx_vcpu_run+1716>:	mov    -0x58(%rbp),%rax
0xffffffffc079ef78 <vmx_vcpu_run+1720>:	mov    0x2d30(%rax),%esi
0xffffffffc079ef7e <vmx_vcpu_run+1726>:	mov    %esi,%edi
0xffffffffc079ef80 <vmx_vcpu_run+1728>:	and    $0x80000000,%edi
0xffffffffc079ef86 <vmx_vcpu_run+1734>:	jmpq   0xffffffffc079ec31 <vmx_vcpu_run+881>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8514
0xffffffffc079ef8b <vmx_vcpu_run+1739>:	mov    0x3088(%rdi),%r8d
0xffffffffc079ef92 <vmx_vcpu_run+1746>:	test   %r8d,%r8d
0xffffffffc079ef95 <vmx_vcpu_run+1749>:	je     0xffffffffc079e8f6 <vmx_vcpu_run+54>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8515
0xffffffffc079ef9b <vmx_vcpu_run+1755>:	callq  0xffffffff810ea5b0 <ktime_get>
0xffffffffc079efa0 <vmx_vcpu_run+1760>:	mov    -0x58(%rbp),%rsi
0xffffffffc079efa4 <vmx_vcpu_run+1764>:	mov    %rax,0x3090(%rsi)
0xffffffffc079efab <vmx_vcpu_run+1771>:	jmpq   0xffffffffc079e8f6 <vmx_vcpu_run+54>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8703
0xffffffffc079efb0 <vmx_vcpu_run+1776>:	callq  0xffffffff81087720 <__stack_chk_fail>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8539
0xffffffffc079efb5 <vmx_vcpu_run+1781>:	mov    %rax,%rsi
0xffffffffc079efb8 <vmx_vcpu_run+1784>:	mov    $0x6c04,%edi
0xffffffffc079efbd <vmx_vcpu_run+1789>:	mov    %rax,-0x60(%rbp)
0xffffffffc079efc1 <vmx_vcpu_run+1793>:	callq  0xffffffffc0795270 <vmcs_writel>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8540
0xffffffffc079efc6 <vmx_vcpu_run+1798>:	mov    -0x58(%rbp),%rsi
0xffffffffc079efca <vmx_vcpu_run+1802>:	mov    -0x60(%rbp),%rax
0xffffffffc079efce <vmx_vcpu_run+1806>:	mov    %rax,0x2ee0(%rsi)
0xffffffffc079efd5 <vmx_vcpu_run+1813>:	jmpq   0xffffffffc079e966 <vmx_vcpu_run+166>
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8415
0xffffffffc079efda <vmx_vcpu_run+1818>:	mov    %rax,%r15
0xffffffffc079efdd <vmx_vcpu_run+1821>:	mov    0x3098(%rax),%rbx
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8416
0xffffffffc079efe4 <vmx_vcpu_run+1828>:	callq  0xffffffff810ea5b0 <ktime_get>
0xffffffffc079efe9 <vmx_vcpu_run+1833>:	sub    0x3090(%r15),%rax
0xffffffffc079eff0 <vmx_vcpu_run+1840>:	mov    0x2d30(%r15),%esi
0xffffffffc079eff7 <vmx_vcpu_run+1847>:	mov    %esi,%edi
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 8415
0xffffffffc079eff9 <vmx_vcpu_run+1849>:	add    %rbx,%rax
0xffffffffc079effc <vmx_vcpu_run+1852>:	and    $0x80000000,%edi
0xffffffffc079f002 <vmx_vcpu_run+1858>:	mov    %rax,0x3098(%r15)
0xffffffffc079f009 <vmx_vcpu_run+1865>:	jmpq   0xffffffffc079ec31 <vmx_vcpu_run+881>

```
