```
0xffffffffc0991e70 <vcpu_enter_guest>:	nopl   0x0(%rax,%rax,1) [FTRACE NOP]
0xffffffffc0991e75 <vcpu_enter_guest+5>:	push   %rbp
0xffffffffc0991e76 <vcpu_enter_guest+6>:	mov    %rsp,%rbp
0xffffffffc0991e79 <vcpu_enter_guest+9>:	push   %r15
0xffffffffc0991e7b <vcpu_enter_guest+11>:	push   %r14
0xffffffffc0991e7d <vcpu_enter_guest+13>:	push   %r13
0xffffffffc0991e7f <vcpu_enter_guest+15>:	push   %r12
0xffffffffc0991e81 <vcpu_enter_guest+17>:	push   %rbx
0xffffffffc0991e82 <vcpu_enter_guest+18>:	mov    %rdi,%rbx
0xffffffffc0991e85 <vcpu_enter_guest+21>:	and    $0xfffffffffffffff0,%rsp
0xffffffffc0991e89 <vcpu_enter_guest+25>:	sub    $0x48,%rsp
0xffffffffc0991e8d <vcpu_enter_guest+29>:	mov    (%rdi),%rdx
0xffffffffc0991e90 <vcpu_enter_guest+32>:	mov    %gs:0x28,%rax
0xffffffffc0991e99 <vcpu_enter_guest+41>:	mov    %rax,0x40(%rsp)
0xffffffffc0991e9e <vcpu_enter_guest+46>:	xor    %eax,%eax
0xffffffffc0991ea0 <vcpu_enter_guest+48>:	mov    0x80(%rdi),%rax
0xffffffffc0991ea7 <vcpu_enter_guest+55>:	cmpb   $0x0,(%rax)
0xffffffffc0991eaa <vcpu_enter_guest+58>:	je     0xffffffffc0992230 <vcpu_enter_guest+960>
0xffffffffc0991eb0 <vcpu_enter_guest+64>:	cmpq   $0x0,0x3068(%rdx)
0xffffffffc0991eb8 <vcpu_enter_guest+72>:	jne    0xffffffffc0992230 <vcpu_enter_guest+960>
0xffffffffc0991ebe <vcpu_enter_guest+78>:	callq  0xffffffffc098df90 <kvm_cpu_accept_dm_intr>
0xffffffffc0991ec3 <vcpu_enter_guest+83>:	test   %eax,%eax
0xffffffffc0991ec5 <vcpu_enter_guest+85>:	setne  %r12b
0xffffffffc0991ec9 <vcpu_enter_guest+89>:	mov    %r12d,%r14d
0xffffffffc0991ecc <vcpu_enter_guest+92>:	and    $0x1,%r14d
0xffffffffc0991ed0 <vcpu_enter_guest+96>:	cmpq   $0x0,0x30(%rbx)
0xffffffffc0991ed5 <vcpu_enter_guest+101>:	jne    0xffffffffc0992241 <vcpu_enter_guest+977>
0xffffffffc0991edb <vcpu_enter_guest+107>:	nopl   0x0(%rax,%rax,1)
0xffffffffc0991ee0 <vcpu_enter_guest+112>:	mov    0x2f0(%rbx),%rax
0xffffffffc0991ee7 <vcpu_enter_guest+119>:	nopl   0x0(%rax,%rax,1)
0xffffffffc0991eec <vcpu_enter_guest+124>:	jmpq   0xffffffffc0992790 <vcpu_enter_guest+2336>
0xffffffffc0991ef1 <vcpu_enter_guest+129>:	cmpb   $0x0,0x2f8(%rbx)
0xffffffffc0991ef8 <vcpu_enter_guest+136>:	je     0xffffffffc0991f20 <vcpu_enter_guest+176>
0xffffffffc0991efa <vcpu_enter_guest+138>:	mov    0x4ecef(%rip),%rax        # 0xffffffffc09e0bf0
0xffffffffc0991f01 <vcpu_enter_guest+145>:	mov    %rbx,%rdi
0xffffffffc0991f04 <vcpu_enter_guest+148>:	mov    0x210(%rax),%r13
0xffffffffc0991f0b <vcpu_enter_guest+155>:	callq  0xffffffffc09b83e0 <kvm_lapic_find_highest_irr>
0xffffffffc0991f10 <vcpu_enter_guest+160>:	mov    %rbx,%rdi
0xffffffffc0991f13 <vcpu_enter_guest+163>:	mov    %eax,%esi
0xffffffffc0991f15 <vcpu_enter_guest+165>:	callq  *%r13
0xffffffffc0991f18 <vcpu_enter_guest+168>:	nopl   0x0(%rax,%rax,1)
0xffffffffc0991f20 <vcpu_enter_guest+176>:	mov    0x30(%rbx),%rax
0xffffffffc0991f24 <vcpu_enter_guest+180>:	test   $0x8,%ah
0xffffffffc0991f27 <vcpu_enter_guest+183>:	je     0xffffffffc099255d <vcpu_enter_guest+1773>
0xffffffffc0991f2d <vcpu_enter_guest+189>:	lock andb $0xf7,0x31(%rbx)
0xffffffffc0991f32 <vcpu_enter_guest+194>:	mov    %rbx,%rdi
0xffffffffc0991f35 <vcpu_enter_guest+197>:	callq  0xffffffffc09ba4d0 <kvm_apic_accept_events>
0xffffffffc0991f3a <vcpu_enter_guest+202>:	cmpl   $0x2,0x32c(%rbx)
0xffffffffc0991f41 <vcpu_enter_guest+209>:	mov    $0x1,%eax
0xffffffffc0991f46 <vcpu_enter_guest+214>:	je     0xffffffffc0992517 <vcpu_enter_guest+1703>
0xffffffffc0991f4c <vcpu_enter_guest+220>:	cmpb   $0x0,0x9f4(%rbx)
0xffffffffc0991f53 <vcpu_enter_guest+227>:	jne    0xffffffffc0992978 <vcpu_enter_guest+2824>
0xffffffffc0991f59 <vcpu_enter_guest+233>:	cmpb   $0x0,0x2228(%rbx)
0xffffffffc0991f60 <vcpu_enter_guest+240>:	jne    0xffffffffc0992660 <vcpu_enter_guest+2032>
0xffffffffc0991f66 <vcpu_enter_guest+246>:	cmpb   $0x0,0x9fc(%rbx)
0xffffffffc0991f6d <vcpu_enter_guest+253>:	jne    0xffffffffc09926e8 <vcpu_enter_guest+2168>
0xffffffffc0991f73 <vcpu_enter_guest+259>:	testb  $0x20,0x2d8(%rbx)
0xffffffffc0991f7a <vcpu_enter_guest+266>:	je     0xffffffffc09925a8 <vcpu_enter_guest+1848>
0xffffffffc0991f80 <vcpu_enter_guest+272>:	mov    0x4ec69(%rip),%rax        # 0xffffffffc09e0bf0
0xffffffffc0991f87 <vcpu_enter_guest+279>:	mov    0x2b8(%rax),%rax
0xffffffffc0991f8e <vcpu_enter_guest+286>:	test   %rax,%rax
0xffffffffc0991f91 <vcpu_enter_guest+289>:	je     0xffffffffc09925a8 <vcpu_enter_guest+1848>
0xffffffffc0991f97 <vcpu_enter_guest+295>:	mov    %r14d,%esi
0xffffffffc0991f9a <vcpu_enter_guest+298>:	mov    %rbx,%rdi
0xffffffffc0991f9d <vcpu_enter_guest+301>:	callq  *%rax
0xffffffffc0991f9f <vcpu_enter_guest+303>:	test   %eax,%eax
0xffffffffc0991fa1 <vcpu_enter_guest+305>:	je     0xffffffffc09925a8 <vcpu_enter_guest+1848>
0xffffffffc0991fa7 <vcpu_enter_guest+311>:	mov    $0x1,%r13d
0xffffffffc0991fad <vcpu_enter_guest+317>:	nopl   (%rax)
0xffffffffc0991fb0 <vcpu_enter_guest+320>:	nopl   0x0(%rax,%rax,1)
0xffffffffc0991fb5 <vcpu_enter_guest+325>:	mov    0x2f0(%rbx),%rax
0xffffffffc0991fbc <vcpu_enter_guest+332>:	nopl   0x0(%rax,%rax,1)
0xffffffffc0991fc1 <vcpu_enter_guest+337>:	jmpq   0xffffffffc0992700 <vcpu_enter_guest+2192>
0xffffffffc0991fc6 <vcpu_enter_guest+342>:	mov    %rbx,%rdi
0xffffffffc0991fc9 <vcpu_enter_guest+345>:	callq  0xffffffffc098dcb0 <update_cr8_intercept>
0xffffffffc0991fce <vcpu_enter_guest+350>:	mov    %rbx,%rdi
0xffffffffc0991fd1 <vcpu_enter_guest+353>:	callq  0xffffffffc09b9f50 <kvm_lapic_sync_to_vapic>
0xffffffffc0991fd6 <vcpu_enter_guest+358>:	nopw   %cs:0x0(%rax,%rax,1)
0xffffffffc0991fe0 <vcpu_enter_guest+368>:	cmpq   $0xffffffffffffffff,0x398(%rbx)
0xffffffffc0991fe8 <vcpu_enter_guest+376>:	je     0xffffffffc0992eb3 <vcpu_enter_guest+4163>
0xffffffffc0991fee <vcpu_enter_guest+382>:	mov    0x4ebfb(%rip),%rax        # 0xffffffffc09e0bf0
0xffffffffc0991ff5 <vcpu_enter_guest+389>:	mov    %rbx,%rdi
0xffffffffc0991ff8 <vcpu_enter_guest+392>:	callq  *0x78(%rax)
0xffffffffc0991ffb <vcpu_enter_guest+395>:	mov    0x88(%rbx),%r11d
0xffffffffc0992002 <vcpu_enter_guest+402>:	test   %r11d,%r11d
0xffffffffc0992005 <vcpu_enter_guest+405>:	je     0xffffffffc0992017 <vcpu_enter_guest+423>
0xffffffffc0992007 <vcpu_enter_guest+407>:	mov    0x8c(%rbx),%r10d
0xffffffffc099200e <vcpu_enter_guest+414>:	test   %r10d,%r10d
0xffffffffc0992011 <vcpu_enter_guest+417>:	je     0xffffffffc0992940 <vcpu_enter_guest+2768>
0xffffffffc0992017 <vcpu_enter_guest+423>:	mov    (%rbx),%rax
0xffffffffc099201a <vcpu_enter_guest+426>:	mov    0x28(%rbx),%esi
0xffffffffc099201d <vcpu_enter_guest+429>:	movl   $0x1,0x2c(%rbx)
0xffffffffc0992024 <vcpu_enter_guest+436>:	lea    0x48(%rax),%rdi
0xffffffffc0992028 <vcpu_enter_guest+440>:	callq  0xffffffff810b5e10 <__srcu_read_unlock>
0xffffffffc099202d <vcpu_enter_guest+445>:	cli    
0xffffffffc099202e <vcpu_enter_guest+446>:	nopw   0x0(%rax,%rax,1)
0xffffffffc0992034 <vcpu_enter_guest+452>:	cmpl   $0x2,0x2c(%rbx)
0xffffffffc0992038 <vcpu_enter_guest+456>:	je     0xffffffffc0992718 <vcpu_enter_guest+2216>
0xffffffffc099203e <vcpu_enter_guest+462>:	cmpq   $0x0,0x30(%rbx)
0xffffffffc0992043 <vcpu_enter_guest+467>:	jne    0xffffffffc0992718 <vcpu_enter_guest+2216>
0xffffffffc0992049 <vcpu_enter_guest+473>:	mov    %gs:0xcdf8,%rax
0xffffffffc0992052 <vcpu_enter_guest+482>:	mov    -0x3fc8(%rax),%rax
0xffffffffc0992059 <vcpu_enter_guest+489>:	test   $0x8,%al
0xffffffffc099205b <vcpu_enter_guest+491>:	jne    0xffffffffc0992718 <vcpu_enter_guest+2216>
0xffffffffc0992061 <vcpu_enter_guest+497>:	mov    %gs:0xce00,%rax
0xffffffffc099206a <vcpu_enter_guest+506>:	mov    0x8(%rax),%rax
0xffffffffc099206e <vcpu_enter_guest+510>:	mov    0x10(%rax),%rax
0xffffffffc0992072 <vcpu_enter_guest+514>:	test   $0x4,%al
0xffffffffc0992074 <vcpu_enter_guest+516>:	jne    0xffffffffc0992718 <vcpu_enter_guest+2216>
0xffffffffc099207a <vcpu_enter_guest+522>:	testb  $0x4,0x2c2(%rbx)
0xffffffffc0992081 <vcpu_enter_guest+529>:	je     0xffffffffc0992093 <vcpu_enter_guest+547>
0xffffffffc0992083 <vcpu_enter_guest+531>:	mov    0x90(%rbx),%r9d
0xffffffffc099208a <vcpu_enter_guest+538>:	test   %r9d,%r9d
0xffffffffc099208d <vcpu_enter_guest+541>:	je     0xffffffffc0992950 <vcpu_enter_guest+2784>
0xffffffffc0992093 <vcpu_enter_guest+547>:	test   %r13b,%r13b
0xffffffffc0992096 <vcpu_enter_guest+550>:	jne    0xffffffffc09929e8 <vcpu_enter_guest+2936>
0xffffffffc099209c <vcpu_enter_guest+556>:	mov    0x24(%rbx),%r13d
0xffffffffc09920a0 <vcpu_enter_guest+560>:	nopl   0x0(%rax,%rax,1)
0xffffffffc09920a5 <vcpu_enter_guest+565>:	mov    %rbx,%rdi
0xffffffffc09920a8 <vcpu_enter_guest+568>:	callq  0xffffffffc09b8c30 <wait_lapic_expire>
0xffffffffc09920ad <vcpu_enter_guest+573>:	nopl   0x0(%rax,%rax,1)
0xffffffffc09920b2 <vcpu_enter_guest+578>:	mov    %gs:0xce00,%rax
0xffffffffc09920bb <vcpu_enter_guest+587>:	orl    $0x10,0x14(%rax)
0xffffffffc09920bf <vcpu_enter_guest+591>:	nopl   0x0(%rax,%rax,1)
0xffffffffc09920c4 <vcpu_enter_guest+596>:	mov    %gs:0x3f67f7e5(%rip),%al        # 0x118b0
0xffffffffc09920cb <vcpu_enter_guest+603>:	test   %al,%al
0xffffffffc09920cd <vcpu_enter_guest+605>:	jne    0xffffffffc09920db <vcpu_enter_guest+619>
0xffffffffc09920cf <vcpu_enter_guest+607>:	mov    %gs:0x3f677f4e(%rip),%edi        # 0xa024
0xffffffffc09920d6 <vcpu_enter_guest+614>:	callq  0xffffffff811389a0 <rcu_note_context_switch>
0xffffffffc09920db <vcpu_enter_guest+619>:	mov    0x23a8(%rbx),%r8d
0xffffffffc09920e2 <vcpu_enter_guest+626>:	test   %r8d,%r8d
0xffffffffc09920e5 <vcpu_enter_guest+629>:	jne    0xffffffffc0992e0e <vcpu_enter_guest+3998>
0xffffffffc09920eb <vcpu_enter_guest+635>:	mov    0x4eafe(%rip),%rax        # 0xffffffffc09e0bf0
0xffffffffc09920f2 <vcpu_enter_guest+642>:	mov    %rbx,%rdi
0xffffffffc09920f5 <vcpu_enter_guest+645>:	callq  *0x178(%rax)
0xffffffffc09920fb <vcpu_enter_guest+651>:	testb  $0x2,0x23a8(%rbx)
0xffffffffc0992102 <vcpu_enter_guest+658>:	jne    0xffffffffc0992dd6 <vcpu_enter_guest+3942>
0xffffffffc0992108 <vcpu_enter_guest+664>:	mov    %gs:0x3f67ac10(%rip),%rax        # 0xcd20
0xffffffffc0992110 <vcpu_enter_guest+672>:	test   $0xaa,%al
0xffffffffc0992112 <vcpu_enter_guest+674>:	jne    0xffffffffc0992a00 <vcpu_enter_guest+2960>
0xffffffffc0992118 <vcpu_enter_guest+680>:	rdtsc  
0xffffffffc099211a <vcpu_enter_guest+682>:	mov    %eax,%ecx
0xffffffffc099211c <vcpu_enter_guest+684>:	mov    %rdx,%rax
0xffffffffc099211f <vcpu_enter_guest+687>:	mov    0x2218(%rbx),%rdx
0xffffffffc0992126 <vcpu_enter_guest+694>:	shl    $0x20,%rax
0xffffffffc099212a <vcpu_enter_guest+698>:	mov    0x21c8(%rbx),%rsi
0xffffffffc0992131 <vcpu_enter_guest+705>:	or     %rcx,%rax
0xffffffffc0992134 <vcpu_enter_guest+708>:	cmp    0x4ea95(%rip),%rdx        # 0xffffffffc09e0bd0
0xffffffffc099213b <vcpu_enter_guest+715>:	je     0xffffffffc0992155 <vcpu_enter_guest+741>
0xffffffffc099213d <vcpu_enter_guest+717>:	mul    %rdx
0xffffffffc0992140 <vcpu_enter_guest+720>:	movzbl 0x4ea99(%rip),%ecx        # 0xffffffffc09e0be0
0xffffffffc0992147 <vcpu_enter_guest+727>:	shrd   %cl,%rdx,%rax
0xffffffffc099214b <vcpu_enter_guest+731>:	shr    %cl,%rdx
0xffffffffc099214e <vcpu_enter_guest+734>:	and    $0x40,%ecx
0xffffffffc0992151 <vcpu_enter_guest+737>:	cmovne %rdx,%rax
0xffffffffc0992155 <vcpu_enter_guest+741>:	add    %rsi,%rax
0xffffffffc0992158 <vcpu_enter_guest+744>:	movl   $0x0,0x2c(%rbx)
0xffffffffc099215f <vcpu_enter_guest+751>:	mov    %rax,0x21d0(%rbx)
0xffffffffc0992166 <vcpu_enter_guest+758>:	mov    0x90(%rbx),%eax
0xffffffffc099216c <vcpu_enter_guest+764>:	test   %eax,%eax
0xffffffffc099216e <vcpu_enter_guest+766>:	je     0xffffffffc0992196 <vcpu_enter_guest+806>
0xffffffffc0992170 <vcpu_enter_guest+768>:	mov    0x4ea01(%rip),%rax        # 0xffffffffc09e0b78
0xffffffffc0992177 <vcpu_enter_guest+775>:	cmp    %rax,0x9b8(%rbx)
0xffffffffc099217e <vcpu_enter_guest+782>:	je     0xffffffffc099218c <vcpu_enter_guest+796>
0xffffffffc0992180 <vcpu_enter_guest+784>:	mov    %rax,%rdx
0xffffffffc0992183 <vcpu_enter_guest+787>:	xor    %ecx,%ecx
0xffffffffc0992185 <vcpu_enter_guest+789>:	shr    $0x20,%rdx
0xffffffffc0992189 <vcpu_enter_guest+793>:	xsetbv 
0xffffffffc099218c <vcpu_enter_guest+796>:	movl   $0x0,0x90(%rbx)
0xffffffffc0992196 <vcpu_enter_guest+806>:	mov    0x4ea53(%rip),%rax        # 0xffffffffc09e0bf0
0xffffffffc099219d <vcpu_enter_guest+813>:	mov    %rbx,%rdi
0xffffffffc09921a0 <vcpu_enter_guest+816>:	callq  *0x2a8(%rax)
0xffffffffc09921a6 <vcpu_enter_guest+822>:	addq   $0x1,0xe8(%rbx)
0xffffffffc09921ae <vcpu_enter_guest+830>:	pushfq 
0xffffffffc09921af <vcpu_enter_guest+831>:	pop    %rax
0xffffffffc09921b0 <vcpu_enter_guest+832>:	nopl   0x0(%rax,%rax,1)
0xffffffffc09921b5 <vcpu_enter_guest+837>:	mov    %rax,%r12
0xffffffffc09921b8 <vcpu_enter_guest+840>:	cli    
0xffffffffc09921b9 <vcpu_enter_guest+841>:	nopw   0x0(%rax,%rax,1)
0xffffffffc09921bf <vcpu_enter_guest+847>:	nopl   0x0(%rax,%rax,1)
0xffffffffc09921c4 <vcpu_enter_guest+852>:	nopl   0x0(%rax,%rax,1)
0xffffffffc09921c9 <vcpu_enter_guest+857>:	mov    %gs:0xce00,%rax
0xffffffffc09921d2 <vcpu_enter_guest+866>:	andl   $0xffffffef,0x14(%rax)
0xffffffffc09921d6 <vcpu_enter_guest+870>:	mov    %r12,%rdi
0xffffffffc09921d9 <vcpu_enter_guest+873>:	push   %rdi
0xffffffffc09921da <vcpu_enter_guest+874>:	popfq  
0xffffffffc09921db <vcpu_enter_guest+875>:	nopl   0x0(%rax,%rax,1)
0xffffffffc09921e0 <vcpu_enter_guest+880>:	mov    (%rbx),%rax
0xffffffffc09921e3 <vcpu_enter_guest+883>:	lea    0x48(%rax),%rdi
0xffffffffc09921e7 <vcpu_enter_guest+887>:	callq  0xffffffff810b5dc0 <__srcu_read_lock>
0xffffffffc09921ec <vcpu_enter_guest+892>:	cmpl   $0x4,-0x3ee753a3(%rip)        # 0xffffffff81b1ce50 <prof_on>
0xffffffffc09921f3 <vcpu_enter_guest+899>:	mov    %eax,0x28(%rbx)
0xffffffffc09921f6 <vcpu_enter_guest+902>:	je     0xffffffffc0992f75 <vcpu_enter_guest+4357>
0xffffffffc09921fc <vcpu_enter_guest+908>:	cmpb   $0x0,0x2201(%rbx)
0xffffffffc0992203 <vcpu_enter_guest+915>:	jne    0xffffffffc0992ea1 <vcpu_enter_guest+4145>
0xffffffffc0992209 <vcpu_enter_guest+921>:	cmpq   $0x0,0x320(%rbx)
0xffffffffc0992211 <vcpu_enter_guest+929>:	je     0xffffffffc099221b <vcpu_enter_guest+939>
0xffffffffc0992213 <vcpu_enter_guest+931>:	mov    %rbx,%rdi
0xffffffffc0992216 <vcpu_enter_guest+934>:	callq  0xffffffffc09b9de0 <kvm_lapic_sync_from_vapic>
0xffffffffc099221b <vcpu_enter_guest+939>:	mov    0x4e9ce(%rip),%rax        # 0xffffffffc09e0bf0
0xffffffffc0992222 <vcpu_enter_guest+946>:	mov    %rbx,%rdi
0xffffffffc0992225 <vcpu_enter_guest+949>:	callq  *0x180(%rax)
0xffffffffc099222b <vcpu_enter_guest+955>:	jmpq   0xffffffffc0992517 <vcpu_enter_guest+1703>
0xffffffffc0992230 <vcpu_enter_guest+960>:	xor    %r12d,%r12d
0xffffffffc0992233 <vcpu_enter_guest+963>:	xor    %r14d,%r14d
0xffffffffc0992236 <vcpu_enter_guest+966>:	cmpq   $0x0,0x30(%rbx)
0xffffffffc099223b <vcpu_enter_guest+971>:	je     0xffffffffc0991edb <vcpu_enter_guest+107>
0xffffffffc0992241 <vcpu_enter_guest+977>:	mov    0x30(%rbx),%rax
0xffffffffc0992245 <vcpu_enter_guest+981>:	test   $0x8,%al
0xffffffffc0992247 <vcpu_enter_guest+983>:	je     0xffffffffc0992256 <vcpu_enter_guest+998>
0xffffffffc0992249 <vcpu_enter_guest+985>:	lock andb $0xf7,0x30(%rbx)
0xffffffffc099224e <vcpu_enter_guest+990>:	mov    %rbx,%rdi
0xffffffffc0992251 <vcpu_enter_guest+993>:	callq  0xffffffffc09a3770 <kvm_mmu_unload>
0xffffffffc0992256 <vcpu_enter_guest+998>:	mov    0x30(%rbx),%rax
0xffffffffc099225a <vcpu_enter_guest+1002>:	test   $0x2,%al
0xffffffffc099225c <vcpu_enter_guest+1004>:	je     0xffffffffc099226b <vcpu_enter_guest+1019>
0xffffffffc099225e <vcpu_enter_guest+1006>:	lock andb $0xfd,0x30(%rbx)
0xffffffffc0992263 <vcpu_enter_guest+1011>:	mov    %rbx,%rdi
0xffffffffc0992266 <vcpu_enter_guest+1014>:	callq  0xffffffffc09b68b0 <__kvm_migrate_timers>
0xffffffffc099226b <vcpu_enter_guest+1019>:	mov    0x30(%rbx),%rax
0xffffffffc099226f <vcpu_enter_guest+1023>:	test   $0x40000,%eax
0xffffffffc0992274 <vcpu_enter_guest+1028>:	je     0xffffffffc0992283 <vcpu_enter_guest+1043>
0xffffffffc0992276 <vcpu_enter_guest+1030>:	lock andb $0xfb,0x32(%rbx)
0xffffffffc099227b <vcpu_enter_guest+1035>:	mov    (%rbx),%rdi
0xffffffffc099227e <vcpu_enter_guest+1038>:	callq  0xffffffffc098f820 <kvm_gen_update_masterclock>
0xffffffffc0992283 <vcpu_enter_guest+1043>:	mov    0x30(%rbx),%rax
0xffffffffc0992287 <vcpu_enter_guest+1047>:	test   $0x400000,%eax
0xffffffffc099228c <vcpu_enter_guest+1052>:	je     0xffffffffc09922c0 <vcpu_enter_guest+1104>
0xffffffffc099228e <vcpu_enter_guest+1054>:	lock andb $0xbf,0x32(%rbx)
0xffffffffc0992293 <vcpu_enter_guest+1059>:	mov    (%rbx),%r13
0xffffffffc0992296 <vcpu_enter_guest+1062>:	lock orb $0x1,0x31(%rbx)
0xffffffffc099229b <vcpu_enter_guest+1067>:	mov    $0x64,%edi
0xffffffffc09922a0 <vcpu_enter_guest+1072>:	callq  0xffffffff8108f0b0 <msecs_to_jiffies>
0xffffffffc09922a5 <vcpu_enter_guest+1077>:	mov    -0x3ee75554(%rip),%rsi        # 0xffffffff81b1cd58 <system_wq>
0xffffffffc09922ac <vcpu_enter_guest+1084>:	lea    0x3148(%r13),%rdx
0xffffffffc09922b3 <vcpu_enter_guest+1091>:	mov    %rax,%rcx
0xffffffffc09922b6 <vcpu_enter_guest+1094>:	mov    $0x1400,%edi
0xffffffffc09922bb <vcpu_enter_guest+1099>:	callq  0xffffffff810a6cb0 <queue_delayed_work_on>
0xffffffffc09922c0 <vcpu_enter_guest+1104>:	mov    0x30(%rbx),%rax
0xffffffffc09922c4 <vcpu_enter_guest+1108>:	test   $0x1,%ah
0xffffffffc09922c7 <vcpu_enter_guest+1111>:	je     0xffffffffc09924bc <vcpu_enter_guest+1612>
0xffffffffc09922cd <vcpu_enter_guest+1117>:	lock andb $0xfe,0x31(%rbx)
0xffffffffc09922d2 <vcpu_enter_guest+1122>:	mov    (%rbx),%r13
0xffffffffc09922d5 <vcpu_enter_guest+1125>:	lea    0x312c(%r13),%rax
0xffffffffc09922dc <vcpu_enter_guest+1132>:	mov    %rax,%rdi
0xffffffffc09922df <vcpu_enter_guest+1135>:	mov    %rax,0x18(%rsp)
0xffffffffc09922e4 <vcpu_enter_guest+1140>:	callq  0xffffffff816abb30 <_raw_qspin_lock>
0xffffffffc09922e9 <vcpu_enter_guest+1145>:	movzbl 0x3130(%r13),%r15d
0xffffffffc09922f1 <vcpu_enter_guest+1153>:	mov    0x18(%rsp),%rax
0xffffffffc09922f6 <vcpu_enter_guest+1158>:	test   %r15b,%r15b
0xffffffffc09922f9 <vcpu_enter_guest+1161>:	jne    0xffffffffc0992590 <vcpu_enter_guest+1824>
0xffffffffc09922ff <vcpu_enter_guest+1167>:	xor    %r9d,%r9d
0xffffffffc0992302 <vcpu_enter_guest+1170>:	xor    %r11d,%r11d
0xffffffffc0992305 <vcpu_enter_guest+1173>:	mov    %rax,%rdi
0xffffffffc0992308 <vcpu_enter_guest+1176>:	movb   $0x0,(%rdi)
0xffffffffc099230b <vcpu_enter_guest+1179>:	nopl   0x0(%rax)
0xffffffffc099230f <vcpu_enter_guest+1183>:	pushfq 
0xffffffffc0992310 <vcpu_enter_guest+1184>:	pop    %rax
0xffffffffc0992311 <vcpu_enter_guest+1185>:	nopl   0x0(%rax,%rax,1)
0xffffffffc0992316 <vcpu_enter_guest+1190>:	mov    %rax,%r13
0xffffffffc0992319 <vcpu_enter_guest+1193>:	cli    
0xffffffffc099231a <vcpu_enter_guest+1194>:	nopw   0x0(%rax,%rax,1)
0xffffffffc0992320 <vcpu_enter_guest+1200>:	mov    %gs:0x3f6857f8(%rip),%r8        # 0x17b20
0xffffffffc0992328 <vcpu_enter_guest+1208>:	test   %r8,%r8
0xffffffffc099232b <vcpu_enter_guest+1211>:	je     0xffffffffc0992ecb <vcpu_enter_guest+4187>
0xffffffffc0992331 <vcpu_enter_guest+1217>:	test   %r15b,%r15b
0xffffffffc0992334 <vcpu_enter_guest+1220>:	je     0xffffffffc09928a0 <vcpu_enter_guest+2608>
0xffffffffc099233a <vcpu_enter_guest+1226>:	mov    0x2218(%rbx),%rcx
0xffffffffc0992341 <vcpu_enter_guest+1233>:	cmp    0x4e888(%rip),%rcx        # 0xffffffffc09e0bd0
0xffffffffc0992348 <vcpu_enter_guest+1240>:	mov    0x21c8(%rbx),%rsi
0xffffffffc099234f <vcpu_enter_guest+1247>:	je     0xffffffffc0992372 <vcpu_enter_guest+1282>
0xffffffffc0992351 <vcpu_enter_guest+1249>:	mov    %rcx,%rax
0xffffffffc0992354 <vcpu_enter_guest+1252>:	movzbl 0x4e885(%rip),%ecx        # 0xffffffffc09e0be0
0xffffffffc099235b <vcpu_enter_guest+1259>:	mul    %r9
0xffffffffc099235e <vcpu_enter_guest+1262>:	mov    %rax,%r9
0xffffffffc0992361 <vcpu_enter_guest+1265>:	mov    %rdx,%r10
0xffffffffc0992364 <vcpu_enter_guest+1268>:	shrd   %cl,%rdx,%r9
0xffffffffc0992368 <vcpu_enter_guest+1272>:	shr    %cl,%r10
0xffffffffc099236b <vcpu_enter_guest+1275>:	and    $0x40,%ecx
0xffffffffc099236e <vcpu_enter_guest+1278>:	cmovne %r10,%r9
0xffffffffc0992372 <vcpu_enter_guest+1282>:	cmpb   $0x0,0x2200(%rbx)
0xffffffffc0992379 <vcpu_enter_guest+1289>:	lea    (%r9,%rsi,1),%r10
0xffffffffc099237d <vcpu_enter_guest+1293>:	je     0xffffffffc09923bc <vcpu_enter_guest+1356>
0xffffffffc099237f <vcpu_enter_guest+1295>:	mov    %r11,%rsi
0xffffffffc0992382 <vcpu_enter_guest+1298>:	sub    0x21e8(%rbx),%rsi
0xffffffffc0992389 <vcpu_enter_guest+1305>:	movsbl 0x2202(%rbx),%ecx
0xffffffffc0992390 <vcpu_enter_guest+1312>:	mov    0x2204(%rbx),%edx
0xffffffffc0992396 <vcpu_enter_guest+1318>:	mov    %rsi,%rax
0xffffffffc0992399 <vcpu_enter_guest+1321>:	shl    %cl,%rax
0xffffffffc099239c <vcpu_enter_guest+1324>:	test   %ecx,%ecx
0xffffffffc099239e <vcpu_enter_guest+1326>:	js     0xffffffffc0992a60 <vcpu_enter_guest+3056>
0xffffffffc09923a4 <vcpu_enter_guest+1332>:	mul    %rdx
0xffffffffc09923a7 <vcpu_enter_guest+1335>:	shrd   $0x20,%rdx,%rax
0xffffffffc09923ac <vcpu_enter_guest+1340>:	add    0x21f0(%rbx),%rax
0xffffffffc09923b3 <vcpu_enter_guest+1347>:	cmp    %rax,%r10
0xffffffffc09923b6 <vcpu_enter_guest+1350>:	jb     0xffffffffc0992a10 <vcpu_enter_guest+2976>
0xffffffffc09923bc <vcpu_enter_guest+1356>:	mov    %r13,%rdi
0xffffffffc09923bf <vcpu_enter_guest+1359>:	push   %rdi
0xffffffffc09923c0 <vcpu_enter_guest+1360>:	popfq  
0xffffffffc09923c1 <vcpu_enter_guest+1361>:	nopl   0x0(%rax,%rax,1)
0xffffffffc09923c6 <vcpu_enter_guest+1366>:	cmpb   $0x0,0x2148(%rbx)
0xffffffffc09923cd <vcpu_enter_guest+1373>:	je     0xffffffffc09924bc <vcpu_enter_guest+1612>
0xffffffffc09923d3 <vcpu_enter_guest+1379>:	mov    0x2118(%rbx),%eax
0xffffffffc09923d9 <vcpu_enter_guest+1385>:	cmp    %rax,%r8
0xffffffffc09923dc <vcpu_enter_guest+1388>:	jne    0xffffffffc0992eec <vcpu_enter_guest+4220>
0xffffffffc09923e2 <vcpu_enter_guest+1394>:	mov    (%rbx),%rdi
0xffffffffc09923e5 <vcpu_enter_guest+1397>:	mov    %r10,0x2100(%rbx)
0xffffffffc09923ec <vcpu_enter_guest+1404>:	lea    0x2120(%rbx),%rax
0xffffffffc09923f3 <vcpu_enter_guest+1411>:	lea    0x20(%rsp),%rdx
0xffffffffc09923f8 <vcpu_enter_guest+1416>:	mov    $0x20,%ecx
0xffffffffc09923fd <vcpu_enter_guest+1421>:	mov    %rax,%rsi
0xffffffffc0992400 <vcpu_enter_guest+1424>:	mov    %rax,0x10(%rsp)
0xffffffffc0992405 <vcpu_enter_guest+1429>:	add    0x30e0(%rdi),%r11
0xffffffffc099240c <vcpu_enter_guest+1436>:	mov    %r10,0x21d0(%rbx)
0xffffffffc0992413 <vcpu_enter_guest+1443>:	mov    %r11,0x2108(%rbx)
0xffffffffc099241a <vcpu_enter_guest+1450>:	callq  0xffffffffc0982290 <kvm_read_guest_cached>
0xffffffffc099241f <vcpu_enter_guest+1455>:	test   %eax,%eax
0xffffffffc0992421 <vcpu_enter_guest+1457>:	jne    0xffffffffc09924bc <vcpu_enter_guest+1612>
0xffffffffc0992427 <vcpu_enter_guest+1463>:	mov    0x20(%rsp),%ecx
0xffffffffc099242b <vcpu_enter_guest+1467>:	lea    0x20f8(%rbx),%r13
0xffffffffc0992432 <vcpu_enter_guest+1474>:	mov    0x10(%rsp),%rsi
0xffffffffc0992437 <vcpu_enter_guest+1479>:	mov    (%rbx),%rdi
0xffffffffc099243a <vcpu_enter_guest+1482>:	mov    %r13,%rdx
0xffffffffc099243d <vcpu_enter_guest+1485>:	lea    0x1(%rcx),%eax
0xffffffffc0992440 <vcpu_enter_guest+1488>:	mov    $0x4,%ecx
0xffffffffc0992445 <vcpu_enter_guest+1493>:	mov    %eax,0x20f8(%rbx)
0xffffffffc099244b <vcpu_enter_guest+1499>:	callq  0xffffffffc097ee70 <kvm_write_guest_cached>
0xffffffffc0992450 <vcpu_enter_guest+1504>:	movzbl 0x3d(%rsp),%eax
0xffffffffc0992455 <vcpu_enter_guest+1509>:	and    $0x2,%eax
0xffffffffc0992458 <vcpu_enter_guest+1512>:	cmpb   $0x0,0x2149(%rbx)
0xffffffffc099245f <vcpu_enter_guest+1519>:	je     0xffffffffc099246d <vcpu_enter_guest+1533>
0xffffffffc0992461 <vcpu_enter_guest+1521>:	movb   $0x0,0x2149(%rbx)
0xffffffffc0992468 <vcpu_enter_guest+1528>:	mov    $0x2,%eax
0xffffffffc099246d <vcpu_enter_guest+1533>:	mov    %eax,%edx
0xffffffffc099246f <vcpu_enter_guest+1535>:	or     $0x1,%edx
0xffffffffc0992472 <vcpu_enter_guest+1538>:	test   %r15b,%r15b
0xffffffffc0992475 <vcpu_enter_guest+1541>:	cmovne %edx,%eax
0xffffffffc0992478 <vcpu_enter_guest+1544>:	mov    %al,0x2115(%rbx)
0xffffffffc099247e <vcpu_enter_guest+1550>:	mov    0x24(%rbx),%eax
0xffffffffc0992481 <vcpu_enter_guest+1553>:	mov    %eax,0x18(%rsp)
0xffffffffc0992485 <vcpu_enter_guest+1557>:	nopl   0x0(%rax,%rax,1)
0xffffffffc099248a <vcpu_enter_guest+1562>:	mov    0x10(%rsp),%r15
0xffffffffc099248f <vcpu_enter_guest+1567>:	mov    (%rbx),%rdi
0xffffffffc0992492 <vcpu_enter_guest+1570>:	mov    $0x20,%ecx
0xffffffffc0992497 <vcpu_enter_guest+1575>:	mov    %r13,%rdx
0xffffffffc099249a <vcpu_enter_guest+1578>:	mov    %r15,%rsi
0xffffffffc099249d <vcpu_enter_guest+1581>:	callq  0xffffffffc097ee70 <kvm_write_guest_cached>
0xffffffffc09924a2 <vcpu_enter_guest+1586>:	addl   $0x1,0x20f8(%rbx)
0xffffffffc09924a9 <vcpu_enter_guest+1593>:	mov    (%rbx),%rdi
0xffffffffc09924ac <vcpu_enter_guest+1596>:	mov    $0x4,%ecx
0xffffffffc09924b1 <vcpu_enter_guest+1601>:	mov    %r13,%rdx
0xffffffffc09924b4 <vcpu_enter_guest+1604>:	mov    %r15,%rsi
0xffffffffc09924b7 <vcpu_enter_guest+1607>:	callq  0xffffffffc097ee70 <kvm_write_guest_cached>
0xffffffffc09924bc <vcpu_enter_guest+1612>:	mov    0x30(%rbx),%rax
0xffffffffc09924c0 <vcpu_enter_guest+1616>:	test   $0x80,%al
0xffffffffc09924c2 <vcpu_enter_guest+1618>:	je     0xffffffffc09924d1 <vcpu_enter_guest+1633>
0xffffffffc09924c4 <vcpu_enter_guest+1620>:	lock andb $0x7f,0x30(%rbx)
0xffffffffc09924c9 <vcpu_enter_guest+1625>:	mov    %rbx,%rdi
0xffffffffc09924cc <vcpu_enter_guest+1628>:	callq  0xffffffffc09a8c20 <kvm_mmu_sync_roots>
0xffffffffc09924d1 <vcpu_enter_guest+1633>:	mov    0x30(%rbx),%rax
0xffffffffc09924d5 <vcpu_enter_guest+1637>:	test   $0x1,%al
0xffffffffc09924d7 <vcpu_enter_guest+1639>:	je     0xffffffffc09924f6 <vcpu_enter_guest+1670>
0xffffffffc09924d9 <vcpu_enter_guest+1641>:	lock andb $0xfe,0x30(%rbx)
0xffffffffc09924de <vcpu_enter_guest+1646>:	mov    0x4e70b(%rip),%rax        # 0xffffffffc09e0bf0
0xffffffffc09924e5 <vcpu_enter_guest+1653>:	addq   $0x1,0xd8(%rbx)
0xffffffffc09924ed <vcpu_enter_guest+1661>:	mov    %rbx,%rdi
0xffffffffc09924f0 <vcpu_enter_guest+1664>:	callq  *0x170(%rax)
0xffffffffc09924f6 <vcpu_enter_guest+1670>:	mov    0x30(%rbx),%rax
0xffffffffc09924fa <vcpu_enter_guest+1674>:	test   $0x4,%al
0xffffffffc09924fc <vcpu_enter_guest+1676>:	je     0xffffffffc0992768 <vcpu_enter_guest+2296>
0xffffffffc0992502 <vcpu_enter_guest+1682>:	lock andb $0xfb,0x30(%rbx)
0xffffffffc0992507 <vcpu_enter_guest+1687>:	mov    0x80(%rbx),%rax
0xffffffffc099250e <vcpu_enter_guest+1694>:	movl   $0xc,0x8(%rax)
0xffffffffc0992515 <vcpu_enter_guest+1701>:	xor    %eax,%eax
0xffffffffc0992517 <vcpu_enter_guest+1703>:	mov    0x40(%rsp),%rsi
0xffffffffc099251c <vcpu_enter_guest+1708>:	xor    %gs:0x28,%rsi
0xffffffffc0992525 <vcpu_enter_guest+1717>:	jne    0xffffffffc0992db7 <vcpu_enter_guest+3911>
0xffffffffc099252b <vcpu_enter_guest+1723>:	lea    -0x28(%rbp),%rsp
0xffffffffc099252f <vcpu_enter_guest+1727>:	pop    %rbx
0xffffffffc0992530 <vcpu_enter_guest+1728>:	pop    %r12
0xffffffffc0992532 <vcpu_enter_guest+1730>:	pop    %r13
0xffffffffc0992534 <vcpu_enter_guest+1732>:	pop    %r14
0xffffffffc0992536 <vcpu_enter_guest+1734>:	pop    %r15
0xffffffffc0992538 <vcpu_enter_guest+1736>:	pop    %rbp
0xffffffffc0992539 <vcpu_enter_guest+1737>:	retq   
0xffffffffc099253a <vcpu_enter_guest+1738>:	nopw   0x0(%rax,%rax,1)
0xffffffffc0992540 <vcpu_enter_guest+1744>:	mov    0x2f0(%rbx),%rax
0xffffffffc0992547 <vcpu_enter_guest+1751>:	test   %rax,%rax
0xffffffffc099254a <vcpu_enter_guest+1754>:	jne    0xffffffffc0991ee7 <vcpu_enter_guest+119>
0xffffffffc0992550 <vcpu_enter_guest+1760>:	mov    0x30(%rbx),%rax
0xffffffffc0992554 <vcpu_enter_guest+1764>:	test   $0x8,%ah
0xffffffffc0992557 <vcpu_enter_guest+1767>:	jne    0xffffffffc0991f2d <vcpu_enter_guest+189>
0xffffffffc099255d <vcpu_enter_guest+1773>:	xor    %r13d,%r13d
0xffffffffc0992560 <vcpu_enter_guest+1776>:	test   %r12b,%r12b
0xffffffffc0992563 <vcpu_enter_guest+1779>:	je     0xffffffffc0991fe0 <vcpu_enter_guest+368>
0xffffffffc0992569 <vcpu_enter_guest+1785>:	jmpq   0xffffffffc0991f32 <vcpu_enter_guest+194>
0xffffffffc099256e <vcpu_enter_guest+1790>:	xchg   %ax,%ax
0xffffffffc0992570 <vcpu_enter_guest+1792>:	mov    0xa0(%rax),%rdx
0xffffffffc0992577 <vcpu_enter_guest+1799>:	testb  $0x8,0x2e9(%rdx)
0xffffffffc099257e <vcpu_enter_guest+1806>:	je     0xffffffffc0991f20 <vcpu_enter_guest+176>
0xffffffffc0992584 <vcpu_enter_guest+1812>:	jmpq   0xffffffffc0991eec <vcpu_enter_guest+124>
0xffffffffc0992589 <vcpu_enter_guest+1817>:	nopl   0x0(%rax)
0xffffffffc0992590 <vcpu_enter_guest+1824>:	mov    0x3140(%r13),%r9
0xffffffffc0992597 <vcpu_enter_guest+1831>:	mov    0x3138(%r13),%r11
0xffffffffc099259e <vcpu_enter_guest+1838>:	jmpq   0xffffffffc0992305 <vcpu_enter_guest+1173>
0xffffffffc09925a3 <vcpu_enter_guest+1843>:	nopl   0x0(%rax,%rax,1)
0xffffffffc09925a8 <vcpu_enter_guest+1848>:	cmpb   $0x0,0x2229(%rbx)
0xffffffffc09925af <vcpu_enter_guest+1855>:	je     0xffffffffc0992630 <vcpu_enter_guest+1984>
0xffffffffc09925b1 <vcpu_enter_guest+1857>:	testb  $0x40,0x2d8(%rbx)
0xffffffffc09925b8 <vcpu_enter_guest+1864>:	jne    0xffffffffc0992630 <vcpu_enter_guest+1984>
0xffffffffc09925ba <vcpu_enter_guest+1866>:	movb   $0x0,0x2229(%rbx)
0xffffffffc09925c1 <vcpu_enter_guest+1873>:	mov    %rbx,%rdi
0xffffffffc09925c4 <vcpu_enter_guest+1876>:	callq  0xffffffffc0991000 <enter_smm>
0xffffffffc09925c9 <vcpu_enter_guest+1881>:	nopl   0x0(%rax)
0xffffffffc09925d0 <vcpu_enter_guest+1888>:	xor    %r13d,%r13d
0xffffffffc09925d3 <vcpu_enter_guest+1891>:	cmpb   $0x0,0x2229(%rbx)
0xffffffffc09925da <vcpu_enter_guest+1898>:	je     0xffffffffc09925ef <vcpu_enter_guest+1919>
0xffffffffc09925dc <vcpu_enter_guest+1900>:	mov    0x2d8(%rbx),%r13d
0xffffffffc09925e3 <vcpu_enter_guest+1907>:	shr    $0x6,%r13d
0xffffffffc09925e7 <vcpu_enter_guest+1911>:	xor    $0x1,%r13d
0xffffffffc09925eb <vcpu_enter_guest+1915>:	and    $0x1,%r13d
0xffffffffc09925ef <vcpu_enter_guest+1919>:	mov    0x2224(%rbx),%r14d
0xffffffffc09925f6 <vcpu_enter_guest+1926>:	test   %r14d,%r14d
0xffffffffc09925f9 <vcpu_enter_guest+1929>:	jne    0xffffffffc0992ab0 <vcpu_enter_guest+3136>
0xffffffffc09925ff <vcpu_enter_guest+1935>:	mov    %rbx,%rdi
0xffffffffc0992602 <vcpu_enter_guest+1938>:	callq  0xffffffffc09b6840 <kvm_cpu_has_injectable_intr>
0xffffffffc0992607 <vcpu_enter_guest+1943>:	test   %eax,%eax
0xffffffffc0992609 <vcpu_enter_guest+1945>:	jne    0xffffffffc0992614 <vcpu_enter_guest+1956>
0xffffffffc099260b <vcpu_enter_guest+1947>:	test   %r12b,%r12b
0xffffffffc099260e <vcpu_enter_guest+1950>:	je     0xffffffffc0991fb0 <vcpu_enter_guest+320>
0xffffffffc0992614 <vcpu_enter_guest+1956>:	mov    0x4e5d5(%rip),%rax        # 0xffffffffc09e0bf0
0xffffffffc099261b <vcpu_enter_guest+1963>:	mov    %rbx,%rdi
0xffffffffc099261e <vcpu_enter_guest+1966>:	callq  *0x1f0(%rax)
0xffffffffc0992624 <vcpu_enter_guest+1972>:	jmpq   0xffffffffc0991fb0 <vcpu_enter_guest+320>
0xffffffffc0992629 <vcpu_enter_guest+1977>:	nopl   0x0(%rax)
0xffffffffc0992630 <vcpu_enter_guest+1984>:	mov    0x2224(%rbx),%r15d
0xffffffffc0992637 <vcpu_enter_guest+1991>:	test   %r15d,%r15d
0xffffffffc099263a <vcpu_enter_guest+1994>:	je     0xffffffffc0992678 <vcpu_enter_guest+2056>
0xffffffffc099263c <vcpu_enter_guest+1996>:	mov    0x4e5ad(%rip),%rax        # 0xffffffffc09e0bf0
0xffffffffc0992643 <vcpu_enter_guest+2003>:	mov    %rbx,%rdi
0xffffffffc0992646 <vcpu_enter_guest+2006>:	callq  *0x1d0(%rax)
0xffffffffc099264c <vcpu_enter_guest+2012>:	test   %eax,%eax
0xffffffffc099264e <vcpu_enter_guest+2014>:	je     0xffffffffc0992678 <vcpu_enter_guest+2056>
0xffffffffc0992650 <vcpu_enter_guest+2016>:	subl   $0x1,0x2224(%rbx)
0xffffffffc0992657 <vcpu_enter_guest+2023>:	movb   $0x1,0x2228(%rbx)
0xffffffffc099265e <vcpu_enter_guest+2030>:	xchg   %ax,%ax
0xffffffffc0992660 <vcpu_enter_guest+2032>:	mov    0x4e589(%rip),%rax        # 0xffffffffc09e0bf0
0xffffffffc0992667 <vcpu_enter_guest+2039>:	mov    %rbx,%rdi
0xffffffffc099266a <vcpu_enter_guest+2042>:	callq  *0x1b0(%rax)
0xffffffffc0992670 <vcpu_enter_guest+2048>:	jmpq   0xffffffffc09925d0 <vcpu_enter_guest+1888>
0xffffffffc0992675 <vcpu_enter_guest+2053>:	nopl   (%rax)
0xffffffffc0992678 <vcpu_enter_guest+2056>:	mov    %rbx,%rdi
0xffffffffc099267b <vcpu_enter_guest+2059>:	callq  0xffffffffc09b6840 <kvm_cpu_has_injectable_intr>
0xffffffffc0992680 <vcpu_enter_guest+2064>:	test   %eax,%eax
0xffffffffc0992682 <vcpu_enter_guest+2066>:	je     0xffffffffc09925d0 <vcpu_enter_guest+1888>
0xffffffffc0992688 <vcpu_enter_guest+2072>:	testb  $0x20,0x2d8(%rbx)
0xffffffffc099268f <vcpu_enter_guest+2079>:	mov    0x4e55a(%rip),%rdx        # 0xffffffffc09e0bf0
0xffffffffc0992696 <vcpu_enter_guest+2086>:	je     0xffffffffc09926bb <vcpu_enter_guest+2123>
0xffffffffc0992698 <vcpu_enter_guest+2088>:	mov    0x2b8(%rdx),%rax
0xffffffffc099269f <vcpu_enter_guest+2095>:	test   %rax,%rax
0xffffffffc09926a2 <vcpu_enter_guest+2098>:	je     0xffffffffc09926bb <vcpu_enter_guest+2123>
0xffffffffc09926a4 <vcpu_enter_guest+2100>:	mov    %r14d,%esi
0xffffffffc09926a7 <vcpu_enter_guest+2103>:	mov    %rbx,%rdi
0xffffffffc09926aa <vcpu_enter_guest+2106>:	callq  *%rax
0xffffffffc09926ac <vcpu_enter_guest+2108>:	test   %eax,%eax
0xffffffffc09926ae <vcpu_enter_guest+2110>:	jne    0xffffffffc0991fa7 <vcpu_enter_guest+311>
0xffffffffc09926b4 <vcpu_enter_guest+2116>:	mov    0x4e535(%rip),%rdx        # 0xffffffffc09e0bf0
0xffffffffc09926bb <vcpu_enter_guest+2123>:	mov    %rbx,%rdi
0xffffffffc09926be <vcpu_enter_guest+2126>:	callq  *0x1c8(%rdx)
0xffffffffc09926c4 <vcpu_enter_guest+2132>:	test   %eax,%eax
0xffffffffc09926c6 <vcpu_enter_guest+2134>:	je     0xffffffffc09925d0 <vcpu_enter_guest+1888>
0xffffffffc09926cc <vcpu_enter_guest+2140>:	mov    %rbx,%rdi
0xffffffffc09926cf <vcpu_enter_guest+2143>:	callq  0xffffffffc09b67d0 <kvm_cpu_get_interrupt>
0xffffffffc09926d4 <vcpu_enter_guest+2148>:	movb   $0x1,0x9fc(%rbx)
0xffffffffc09926db <vcpu_enter_guest+2155>:	movb   $0x0,0x9fd(%rbx)
0xffffffffc09926e2 <vcpu_enter_guest+2162>:	mov    %al,0x9fe(%rbx)
0xffffffffc09926e8 <vcpu_enter_guest+2168>:	mov    0x4e501(%rip),%rax        # 0xffffffffc09e0bf0
0xffffffffc09926ef <vcpu_enter_guest+2175>:	mov    %rbx,%rdi
0xffffffffc09926f2 <vcpu_enter_guest+2178>:	callq  *0x1a8(%rax)
0xffffffffc09926f8 <vcpu_enter_guest+2184>:	jmpq   0xffffffffc09925d0 <vcpu_enter_guest+1888>
0xffffffffc09926fd <vcpu_enter_guest+2189>:	nopl   (%rax)
0xffffffffc0992700 <vcpu_enter_guest+2192>:	cmpb   $0x0,0xa8(%rax)
0xffffffffc0992707 <vcpu_enter_guest+2199>:	jne    0xffffffffc0991fc6 <vcpu_enter_guest+342>
0xffffffffc099270d <vcpu_enter_guest+2205>:	nopl   (%rax)
0xffffffffc0992710 <vcpu_enter_guest+2208>:	jmpq   0xffffffffc0991fe0 <vcpu_enter_guest+368>
0xffffffffc0992715 <vcpu_enter_guest+2213>:	nopl   (%rax)
0xffffffffc0992718 <vcpu_enter_guest+2216>:	movl   $0x0,0x2c(%rbx)
0xffffffffc099271f <vcpu_enter_guest+2223>:	sti    
0xffffffffc0992720 <vcpu_enter_guest+2224>:	nopw   0x0(%rax,%rax,1)
0xffffffffc0992726 <vcpu_enter_guest+2230>:	mov    (%rbx),%rax
0xffffffffc0992729 <vcpu_enter_guest+2233>:	mov    $0x1,%r12d
0xffffffffc099272f <vcpu_enter_guest+2239>:	lea    0x48(%rax),%rdi
0xffffffffc0992733 <vcpu_enter_guest+2243>:	callq  0xffffffff810b5dc0 <__srcu_read_lock>
0xffffffffc0992738 <vcpu_enter_guest+2248>:	mov    %eax,0x28(%rbx)
0xffffffffc099273b <vcpu_enter_guest+2251>:	mov    0x4e4ae(%rip),%rax        # 0xffffffffc09e0bf0
0xffffffffc0992742 <vcpu_enter_guest+2258>:	mov    %rbx,%rdi
0xffffffffc0992745 <vcpu_enter_guest+2261>:	callq  *0x1c0(%rax)
0xffffffffc099274b <vcpu_enter_guest+2267>:	cmpq   $0x0,0x320(%rbx)
0xffffffffc0992753 <vcpu_enter_guest+2275>:	jne    0xffffffffc0992e91 <vcpu_enter_guest+4129>
0xffffffffc0992759 <vcpu_enter_guest+2281>:	mov    %r12d,%eax
0xffffffffc099275c <vcpu_enter_guest+2284>:	jmpq   0xffffffffc0992517 <vcpu_enter_guest+1703>
0xffffffffc0992761 <vcpu_enter_guest+2289>:	nopl   0x0(%rax)
0xffffffffc0992768 <vcpu_enter_guest+2296>:	mov    0x30(%rbx),%rax
0xffffffffc099276c <vcpu_enter_guest+2300>:	test   $0x10,%al
0xffffffffc099276e <vcpu_enter_guest+2302>:	je     0xffffffffc09928d0 <vcpu_enter_guest+2656>
0xffffffffc0992774 <vcpu_enter_guest+2308>:	lock andb $0xef,0x30(%rbx)
0xffffffffc0992779 <vcpu_enter_guest+2313>:	mov    0x80(%rbx),%rax
0xffffffffc0992780 <vcpu_enter_guest+2320>:	movl   $0x8,0x8(%rax)
0xffffffffc0992787 <vcpu_enter_guest+2327>:	xor    %eax,%eax
0xffffffffc0992789 <vcpu_enter_guest+2329>:	jmpq   0xffffffffc0992517 <vcpu_enter_guest+1703>
0xffffffffc099278e <vcpu_enter_guest+2334>:	xchg   %ax,%ax
0xffffffffc0992790 <vcpu_enter_guest+2336>:	cmpb   $0x0,0xa8(%rax)
0xffffffffc0992797 <vcpu_enter_guest+2343>:	jne    0xffffffffc0991ef1 <vcpu_enter_guest+129>
0xffffffffc099279d <vcpu_enter_guest+2349>:	jmpq   0xffffffffc0991f20 <vcpu_enter_guest+176>
0xffffffffc09927a2 <vcpu_enter_guest+2354>:	nopw   0x0(%rax,%rax,1)
0xffffffffc09927a8 <vcpu_enter_guest+2360>:	mov    0x2f0(%rbx),%rax
0xffffffffc09927af <vcpu_enter_guest+2367>:	test   %rax,%rax
0xffffffffc09927b2 <vcpu_enter_guest+2370>:	je     0xffffffffc0991fe0 <vcpu_enter_guest+368>
0xffffffffc09927b8 <vcpu_enter_guest+2376>:	jmpq   0xffffffffc0991fbc <vcpu_enter_guest+332>
0xffffffffc09927bd <vcpu_enter_guest+2381>:	nopl   (%rax)
0xffffffffc09927c0 <vcpu_enter_guest+2384>:	mov    0x504e9(%rip),%r15        # 0xffffffffc09e2cb0
0xffffffffc09927c7 <vcpu_enter_guest+2391>:	test   %r15,%r15
0xffffffffc09927ca <vcpu_enter_guest+2394>:	je     0xffffffffc09927e9 <vcpu_enter_guest+2425>
0xffffffffc09927cc <vcpu_enter_guest+2396>:	mov    (%r15),%rcx
0xffffffffc09927cf <vcpu_enter_guest+2399>:	nop
0xffffffffc09927d0 <vcpu_enter_guest+2400>:	mov    0x8(%r15),%rdi
0xffffffffc09927d4 <vcpu_enter_guest+2404>:	add    $0x10,%r15
0xffffffffc09927d8 <vcpu_enter_guest+2408>:	mov    %r13,%rdx
0xffffffffc09927db <vcpu_enter_guest+2411>:	mov    0x18(%rsp),%esi
0xffffffffc09927df <vcpu_enter_guest+2415>:	callq  *%rcx
0xffffffffc09927e1 <vcpu_enter_guest+2417>:	mov    (%r15),%rcx
0xffffffffc09927e4 <vcpu_enter_guest+2420>:	test   %rcx,%rcx
0xffffffffc09927e7 <vcpu_enter_guest+2423>:	jne    0xffffffffc09927d0 <vcpu_enter_guest+2400>
0xffffffffc09927e9 <vcpu_enter_guest+2425>:	jmpq   0xffffffffc099248a <vcpu_enter_guest+1562>
0xffffffffc09927ee <vcpu_enter_guest+2430>:	xchg   %ax,%ax
0xffffffffc09927f0 <vcpu_enter_guest+2432>:	mov    0x50cb9(%rip),%r12        # 0xffffffffc09e34b0
0xffffffffc09927f7 <vcpu_enter_guest+2439>:	test   %r12,%r12
0xffffffffc09927fa <vcpu_enter_guest+2442>:	je     0xffffffffc0992817 <vcpu_enter_guest+2471>
0xffffffffc09927fc <vcpu_enter_guest+2444>:	mov    (%r12),%rax
0xffffffffc0992800 <vcpu_enter_guest+2448>:	mov    0x8(%r12),%rdi
0xffffffffc0992805 <vcpu_enter_guest+2453>:	add    $0x10,%r12
0xffffffffc0992809 <vcpu_enter_guest+2457>:	mov    %r13d,%esi
0xffffffffc099280c <vcpu_enter_guest+2460>:	callq  *%rax
0xffffffffc099280e <vcpu_enter_guest+2462>:	mov    (%r12),%rax
0xffffffffc0992812 <vcpu_enter_guest+2466>:	test   %rax,%rax
0xffffffffc0992815 <vcpu_enter_guest+2469>:	jne    0xffffffffc0992800 <vcpu_enter_guest+2448>
0xffffffffc0992817 <vcpu_enter_guest+2471>:	jmpq   0xffffffffc09920a5 <vcpu_enter_guest+565>
0xffffffffc099281c <vcpu_enter_guest+2476>:	nopl   0x0(%rax)
0xffffffffc0992820 <vcpu_enter_guest+2480>:	mov    %gs:0x3f67f089(%rip),%al        # 0x118b0
0xffffffffc0992827 <vcpu_enter_guest+2487>:	test   %al,%al
0xffffffffc0992829 <vcpu_enter_guest+2489>:	je     0xffffffffc09921c9 <vcpu_enter_guest+857>
0xffffffffc099282f <vcpu_enter_guest+2495>:	mov    %gs:0xce00,%rdi
0xffffffffc0992838 <vcpu_enter_guest+2504>:	nopl   0x0(%rax,%rax,1)
0xffffffffc0992840 <vcpu_enter_guest+2512>:	callq  0xffffffff810c7d70 <vtime_guest_exit>
0xffffffffc0992845 <vcpu_enter_guest+2517>:	jmpq   0xffffffffc09921d6 <vcpu_enter_guest+870>
0xffffffffc099284a <vcpu_enter_guest+2522>:	nopw   0x0(%rax,%rax,1)
0xffffffffc0992850 <vcpu_enter_guest+2528>:	mov    $0x2,%edi
0xffffffffc0992855 <vcpu_enter_guest+2533>:	callq  0xffffffff811805f0 <context_tracking_exit>
0xffffffffc099285a <vcpu_enter_guest+2538>:	jmpq   0xffffffffc09921c4 <vcpu_enter_guest+852>
0xffffffffc099285f <vcpu_enter_guest+2543>:	nop
0xffffffffc0992860 <vcpu_enter_guest+2544>:	mov    $0x2,%edi
0xffffffffc0992865 <vcpu_enter_guest+2549>:	callq  0xffffffff811806d0 <context_tracking_enter>
0xffffffffc099286a <vcpu_enter_guest+2554>:	jmpq   0xffffffffc09920c4 <vcpu_enter_guest+596>
0xffffffffc099286f <vcpu_enter_guest+2559>:	nop
0xffffffffc0992870 <vcpu_enter_guest+2560>:	mov    %gs:0x3f67f039(%rip),%al        # 0x118b0
0xffffffffc0992877 <vcpu_enter_guest+2567>:	test   %al,%al
0xffffffffc0992879 <vcpu_enter_guest+2569>:	je     0xffffffffc09920b2 <vcpu_enter_guest+578>
0xffffffffc099287f <vcpu_enter_guest+2575>:	mov    %gs:0xce00,%rdi
0xffffffffc0992888 <vcpu_enter_guest+2584>:	nopl   0x0(%rax,%rax,1)
0xffffffffc0992890 <vcpu_enter_guest+2592>:	callq  0xffffffff810c7d20 <vtime_guest_enter>
0xffffffffc0992895 <vcpu_enter_guest+2597>:	jmpq   0xffffffffc09920bf <vcpu_enter_guest+591>
0xffffffffc099289a <vcpu_enter_guest+2602>:	nopw   0x0(%rax,%rax,1)
0xffffffffc09928a0 <vcpu_enter_guest+2608>:	mov    %r8,0x10(%rsp)
0xffffffffc09928a5 <vcpu_enter_guest+2613>:	rdtsc  
0xffffffffc09928a7 <vcpu_enter_guest+2615>:	mov    %rdx,%r9
0xffffffffc09928aa <vcpu_enter_guest+2618>:	mov    %eax,%eax
0xffffffffc09928ac <vcpu_enter_guest+2620>:	shl    $0x20,%r9
0xffffffffc09928b0 <vcpu_enter_guest+2624>:	or     %rax,%r9
0xffffffffc09928b3 <vcpu_enter_guest+2627>:	mov    %r9,0x18(%rsp)
0xffffffffc09928b8 <vcpu_enter_guest+2632>:	callq  0xffffffff810eab70 <ktime_get_boottime>
0xffffffffc09928bd <vcpu_enter_guest+2637>:	mov    0x10(%rsp),%r8
0xffffffffc09928c2 <vcpu_enter_guest+2642>:	mov    %rax,%r11
0xffffffffc09928c5 <vcpu_enter_guest+2645>:	mov    0x18(%rsp),%r9
0xffffffffc09928ca <vcpu_enter_guest+2650>:	jmpq   0xffffffffc099233a <vcpu_enter_guest+1226>
0xffffffffc09928cf <vcpu_enter_guest+2655>:	nop
0xffffffffc09928d0 <vcpu_enter_guest+2656>:	mov    0x30(%rbx),%rax
0xffffffffc09928d4 <vcpu_enter_guest+2660>:	test   $0x4,%ah
0xffffffffc09928d7 <vcpu_enter_guest+2663>:	je     0xffffffffc09928f8 <vcpu_enter_guest+2696>
0xffffffffc09928d9 <vcpu_enter_guest+2665>:	lock andb $0xfb,0x31(%rbx)
0xffffffffc09928de <vcpu_enter_guest+2670>:	mov    0x4e30b(%rip),%rax        # 0xffffffffc09e0bf0
0xffffffffc09928e5 <vcpu_enter_guest+2677>:	movl   $0x0,0x88(%rbx)
0xffffffffc09928ef <vcpu_enter_guest+2687>:	mov    %rbx,%rdi
0xffffffffc09928f2 <vcpu_enter_guest+2690>:	callq  *0x168(%rax)
0xffffffffc09928f8 <vcpu_enter_guest+2696>:	mov    0x30(%rbx),%rax
0xffffffffc09928fc <vcpu_enter_guest+2700>:	test   $0x10,%ah
0xffffffffc09928ff <vcpu_enter_guest+2703>:	je     0xffffffffc0992af3 <vcpu_enter_guest+3203>
0xffffffffc0992905 <vcpu_enter_guest+2709>:	lock andb $0xef,0x31(%rbx)
0xffffffffc099290a <vcpu_enter_guest+2714>:	mov    $0x1,%eax
0xffffffffc099290f <vcpu_enter_guest+2719>:	movb   $0x1,0x2a68(%rbx)
0xffffffffc0992916 <vcpu_enter_guest+2726>:	jmpq   0xffffffffc0992517 <vcpu_enter_guest+1703>
0xffffffffc099291b <vcpu_enter_guest+2731>:	nopl   0x0(%rax,%rax,1)
0xffffffffc0992920 <vcpu_enter_guest+2736>:	mov    0xa0(%rax),%rdx
0xffffffffc0992927 <vcpu_enter_guest+2743>:	testb  $0x8,0x2e9(%rdx)
0xffffffffc099292e <vcpu_enter_guest+2750>:	je     0xffffffffc0991fe0 <vcpu_enter_guest+368>
0xffffffffc0992934 <vcpu_enter_guest+2756>:	jmpq   0xffffffffc0991fc1 <vcpu_enter_guest+337>
0xffffffffc0992939 <vcpu_enter_guest+2761>:	nopl   0x0(%rax)
0xffffffffc0992940 <vcpu_enter_guest+2768>:	mov    %rbx,%rdi
0xffffffffc0992943 <vcpu_enter_guest+2771>:	callq  0xffffffffc098e920 <kvm_load_guest_fpu>
0xffffffffc0992948 <vcpu_enter_guest+2776>:	jmpq   0xffffffffc0992017 <vcpu_enter_guest+423>
0xffffffffc099294d <vcpu_enter_guest+2781>:	nopl   (%rax)
0xffffffffc0992950 <vcpu_enter_guest+2784>:	mov    0x9b8(%rbx),%rax
0xffffffffc0992957 <vcpu_enter_guest+2791>:	xor    %ecx,%ecx
0xffffffffc0992959 <vcpu_enter_guest+2793>:	mov    %rax,%rdx
0xffffffffc099295c <vcpu_enter_guest+2796>:	shr    $0x20,%rdx
0xffffffffc0992960 <vcpu_enter_guest+2800>:	xsetbv 
0xffffffffc0992963 <vcpu_enter_guest+2803>:	movl   $0x1,0x90(%rbx)
0xffffffffc099296d <vcpu_enter_guest+2813>:	jmpq   0xffffffffc0992093 <vcpu_enter_guest+547>
0xffffffffc0992972 <vcpu_enter_guest+2818>:	nopw   0x0(%rax,%rax,1)
0xffffffffc0992978 <vcpu_enter_guest+2824>:	mov    0x9f8(%rbx),%eax
0xffffffffc099297e <vcpu_enter_guest+2830>:	movzbl 0x9f5(%rbx),%r15d
0xffffffffc0992986 <vcpu_enter_guest+2838>:	movzbl 0x9f7(%rbx),%r14d
0xffffffffc099298e <vcpu_enter_guest+2846>:	mov    %eax,0x18(%rsp)
0xffffffffc0992992 <vcpu_enter_guest+2850>:	nopl   0x0(%rax,%rax,1)
0xffffffffc0992997 <vcpu_enter_guest+2855>:	movzbl %r14b,%edi
0xffffffffc099299b <vcpu_enter_guest+2859>:	callq  0xffffffffc098cdd0 <exception_type>
0xffffffffc09929a0 <vcpu_enter_guest+2864>:	test   %eax,%eax
0xffffffffc09929a2 <vcpu_enter_guest+2866>:	je     0xffffffffc0992bc1 <vcpu_enter_guest+3409>
0xffffffffc09929a8 <vcpu_enter_guest+2872>:	movzbl 0x9f7(%rbx),%esi
0xffffffffc09929af <vcpu_enter_guest+2879>:	cmp    $0x1,%sil
0xffffffffc09929b3 <vcpu_enter_guest+2883>:	je     0xffffffffc0992ac5 <vcpu_enter_guest+3157>
0xffffffffc09929b9 <vcpu_enter_guest+2889>:	mov    0x4e230(%rip),%rax        # 0xffffffffc09e0bf0
0xffffffffc09929c0 <vcpu_enter_guest+2896>:	movzbl 0x9f5(%rbx),%edx
0xffffffffc09929c7 <vcpu_enter_guest+2903>:	mov    %rbx,%rdi
0xffffffffc09929ca <vcpu_enter_guest+2906>:	movzbl 0x9f6(%rbx),%r8d
0xffffffffc09929d2 <vcpu_enter_guest+2914>:	mov    0x9f8(%rbx),%ecx
0xffffffffc09929d8 <vcpu_enter_guest+2920>:	callq  *0x1b8(%rax)
0xffffffffc09929de <vcpu_enter_guest+2926>:	jmpq   0xffffffffc09925d0 <vcpu_enter_guest+1888>
0xffffffffc09929e3 <vcpu_enter_guest+2931>:	nopl   0x0(%rax,%rax,1)
0xffffffffc09929e8 <vcpu_enter_guest+2936>:	lock orb $0x8,0x31(%rbx)
0xffffffffc09929ed <vcpu_enter_guest+2941>:	mov    0x20(%rbx),%edi
0xffffffffc09929f0 <vcpu_enter_guest+2944>:	callq  *-0x3ef8120e(%rip)        # 0xffffffff81a117e8 <smp_ops+40>
0xffffffffc09929f6 <vcpu_enter_guest+2950>:	jmpq   0xffffffffc099209c <vcpu_enter_guest+556>
0xffffffffc09929fb <vcpu_enter_guest+2955>:	nopl   0x0(%rax,%rax,1)
0xffffffffc0992a00 <vcpu_enter_guest+2960>:	callq  0xffffffff810327f0 <hw_breakpoint_restore>
0xffffffffc0992a05 <vcpu_enter_guest+2965>:	jmpq   0xffffffffc0992118 <vcpu_enter_guest+680>
0xffffffffc0992a0a <vcpu_enter_guest+2970>:	nopw   0x0(%rax,%rax,1)
0xffffffffc0992a10 <vcpu_enter_guest+2976>:	mov    0x4e1d9(%rip),%rcx        # 0xffffffffc09e0bf0
0xffffffffc0992a17 <vcpu_enter_guest+2983>:	mov    %rax,%rdx
0xffffffffc0992a1a <vcpu_enter_guest+2986>:	mov    %r8,(%rsp)
0xffffffffc0992a1e <vcpu_enter_guest+2990>:	sub    %r9,%rdx
0xffffffffc0992a21 <vcpu_enter_guest+2993>:	mov    %r11,0x8(%rsp)
0xffffffffc0992a26 <vcpu_enter_guest+2998>:	mov    %rax,0x10(%rsp)
0xffffffffc0992a2b <vcpu_enter_guest+3003>:	mov    %rdx,%rsi
0xffffffffc0992a2e <vcpu_enter_guest+3006>:	mov    %rdx,0x18(%rsp)
0xffffffffc0992a33 <vcpu_enter_guest+3011>:	mov    %rbx,%rdi
0xffffffffc0992a36 <vcpu_enter_guest+3014>:	callq  *0x290(%rcx)
0xffffffffc0992a3c <vcpu_enter_guest+3020>:	mov    0x18(%rsp),%rdx
0xffffffffc0992a41 <vcpu_enter_guest+3025>:	mov    0x10(%rsp),%rax
0xffffffffc0992a46 <vcpu_enter_guest+3030>:	mov    (%rsp),%r8
0xffffffffc0992a4a <vcpu_enter_guest+3034>:	mov    0x8(%rsp),%r11
0xffffffffc0992a4f <vcpu_enter_guest+3039>:	mov    %rdx,0x21c8(%rbx)
0xffffffffc0992a56 <vcpu_enter_guest+3046>:	mov    %rax,%r10
0xffffffffc0992a59 <vcpu_enter_guest+3049>:	jmpq   0xffffffffc09923bc <vcpu_enter_guest+1356>
0xffffffffc0992a5e <vcpu_enter_guest+3054>:	xchg   %ax,%ax
0xffffffffc0992a60 <vcpu_enter_guest+3056>:	neg    %ecx
0xffffffffc0992a62 <vcpu_enter_guest+3058>:	mov    %rsi,%rax
0xffffffffc0992a65 <vcpu_enter_guest+3061>:	shr    %cl,%rax
0xffffffffc0992a68 <vcpu_enter_guest+3064>:	jmpq   0xffffffffc09923a4 <vcpu_enter_guest+1332>
0xffffffffc0992a6d <vcpu_enter_guest+3069>:	nopl   (%rax)
0xffffffffc0992a70 <vcpu_enter_guest+3072>:	mov    0x507f9(%rip),%r13        # 0xffffffffc09e3270
0xffffffffc0992a77 <vcpu_enter_guest+3079>:	test   %r13,%r13
0xffffffffc0992a7a <vcpu_enter_guest+3082>:	je     0xffffffffc0992a9e <vcpu_enter_guest+3118>
0xffffffffc0992a7c <vcpu_enter_guest+3084>:	mov    0x0(%r13),%r8
0xffffffffc0992a80 <vcpu_enter_guest+3088>:	mov    0x8(%r13),%rdi
0xffffffffc0992a84 <vcpu_enter_guest+3092>:	add    $0x10,%r13
0xffffffffc0992a88 <vcpu_enter_guest+3096>:	mov    0x18(%rsp),%ecx
0xffffffffc0992a8c <vcpu_enter_guest+3100>:	mov    %r15d,%edx
0xffffffffc0992a8f <vcpu_enter_guest+3103>:	mov    %r14d,%esi
0xffffffffc0992a92 <vcpu_enter_guest+3106>:	callq  *%r8
0xffffffffc0992a95 <vcpu_enter_guest+3109>:	mov    0x0(%r13),%r8
0xffffffffc0992a99 <vcpu_enter_guest+3113>:	test   %r8,%r8
0xffffffffc0992a9c <vcpu_enter_guest+3116>:	jne    0xffffffffc0992a80 <vcpu_enter_guest+3088>
0xffffffffc0992a9e <vcpu_enter_guest+3118>:	movzbl 0x9f7(%rbx),%r14d
0xffffffffc0992aa6 <vcpu_enter_guest+3126>:	jmpq   0xffffffffc0992997 <vcpu_enter_guest+2855>
0xffffffffc0992aab <vcpu_enter_guest+3131>:	nopl   0x0(%rax,%rax,1)
0xffffffffc0992ab0 <vcpu_enter_guest+3136>:	mov    0x4e139(%rip),%rax        # 0xffffffffc09e0bf0
0xffffffffc0992ab7 <vcpu_enter_guest+3143>:	mov    %rbx,%rdi
0xffffffffc0992aba <vcpu_enter_guest+3146>:	callq  *0x1e8(%rax)
0xffffffffc0992ac0 <vcpu_enter_guest+3152>:	jmpq   0xffffffffc09925ff <vcpu_enter_guest+1935>
0xffffffffc0992ac5 <vcpu_enter_guest+3157>:	mov    0x23d8(%rbx),%rax
0xffffffffc0992acc <vcpu_enter_guest+3164>:	test   $0x20,%ah
0xffffffffc0992acf <vcpu_enter_guest+3167>:	je     0xffffffffc09929b9 <vcpu_enter_guest+2889>
0xffffffffc0992ad5 <vcpu_enter_guest+3173>:	and    $0xdf,%ah
0xffffffffc0992ad8 <vcpu_enter_guest+3176>:	mov    %rbx,%rdi
0xffffffffc0992adb <vcpu_enter_guest+3179>:	mov    %rax,0x23d8(%rbx)
0xffffffffc0992ae2 <vcpu_enter_guest+3186>:	callq  0xffffffffc09857f0 <kvm_update_dr7>
0xffffffffc0992ae7 <vcpu_enter_guest+3191>:	movzbl 0x9f7(%rbx),%esi
0xffffffffc0992aee <vcpu_enter_guest+3198>:	jmpq   0xffffffffc09929b9 <vcpu_enter_guest+2889>
0xffffffffc0992af3 <vcpu_enter_guest+3203>:	mov    0x30(%rbx),%rax
0xffffffffc0992af7 <vcpu_enter_guest+3207>:	test   $0x20,%ah
0xffffffffc0992afa <vcpu_enter_guest+3210>:	je     0xffffffffc0992b0e <vcpu_enter_guest+3230>
0xffffffffc0992afc <vcpu_enter_guest+3212>:	lock andb $0xdf,0x31(%rbx)
0xffffffffc0992b01 <vcpu_enter_guest+3217>:	testb  $0x1,0x2150(%rbx)
0xffffffffc0992b08 <vcpu_enter_guest+3224>:	jne    0xffffffffc0992ce3 <vcpu_enter_guest+3699>
0xffffffffc0992b0e <vcpu_enter_guest+3230>:	mov    0x30(%rbx),%rax
0xffffffffc0992b12 <vcpu_enter_guest+3234>:	test   $0x4000000,%eax
0xffffffffc0992b17 <vcpu_enter_guest+3239>:	je     0xffffffffc0992b2a <vcpu_enter_guest+3258>
0xffffffffc0992b19 <vcpu_enter_guest+3241>:	lock andb $0xfb,0x33(%rbx)
0xffffffffc0992b1e <vcpu_enter_guest+3246>:	movb   $0x1,0x2229(%rbx)
0xffffffffc0992b25 <vcpu_enter_guest+3253>:	lock orb $0x8,0x31(%rbx)
0xffffffffc0992b2a <vcpu_enter_guest+3258>:	mov    0x30(%rbx),%rax
0xffffffffc0992b2e <vcpu_enter_guest+3262>:	test   $0x40,%ah
0xffffffffc0992b31 <vcpu_enter_guest+3265>:	je     0xffffffffc0992b40 <vcpu_enter_guest+3280>
0xffffffffc0992b33 <vcpu_enter_guest+3267>:	lock andb $0xbf,0x31(%rbx)
0xffffffffc0992b38 <vcpu_enter_guest+3272>:	mov    %rbx,%rdi
0xffffffffc0992b3b <vcpu_enter_guest+3275>:	callq  0xffffffffc098f990 <process_nmi>
0xffffffffc0992b40 <vcpu_enter_guest+3280>:	mov    0x30(%rbx),%rax
0xffffffffc0992b44 <vcpu_enter_guest+3284>:	test   $0x80,%ah
0xffffffffc0992b47 <vcpu_enter_guest+3287>:	je     0xffffffffc0992b56 <vcpu_enter_guest+3302>
0xffffffffc0992b49 <vcpu_enter_guest+3289>:	lock andb $0x7f,0x31(%rbx)
0xffffffffc0992b4e <vcpu_enter_guest+3294>:	mov    %rbx,%rdi
0xffffffffc0992b51 <vcpu_enter_guest+3297>:	callq  0xffffffffc09bf570 <kvm_pmu_handle_event>
0xffffffffc0992b56 <vcpu_enter_guest+3302>:	mov    0x30(%rbx),%rax
0xffffffffc0992b5a <vcpu_enter_guest+3306>:	test   $0x10000,%eax
0xffffffffc0992b5f <vcpu_enter_guest+3311>:	je     0xffffffffc0992b6e <vcpu_enter_guest+3326>
0xffffffffc0992b61 <vcpu_enter_guest+3313>:	lock andb $0xfe,0x32(%rbx)
0xffffffffc0992b66 <vcpu_enter_guest+3318>:	mov    %rbx,%rdi
0xffffffffc0992b69 <vcpu_enter_guest+3321>:	callq  0xffffffffc09bf710 <kvm_pmu_deliver_pmi>
0xffffffffc0992b6e <vcpu_enter_guest+3326>:	mov    0x30(%rbx),%rax
0xffffffffc0992b72 <vcpu_enter_guest+3330>:	test   $0x10000000,%eax
0xffffffffc0992b77 <vcpu_enter_guest+3335>:	je     0xffffffffc0992bf7 <vcpu_enter_guest+3463>
0xffffffffc0992b79 <vcpu_enter_guest+3337>:	lock andb $0xef,0x33(%rbx)
0xffffffffc0992b7e <vcpu_enter_guest+3342>:	mov    0x2cfc(%rbx),%eax
0xffffffffc0992b84 <vcpu_enter_guest+3348>:	cmp    $0xff,%eax
0xffffffffc0992b89 <vcpu_enter_guest+3353>:	jg     0xffffffffc0992f9e <vcpu_enter_guest+4398>
0xffffffffc0992b8f <vcpu_enter_guest+3359>:	bt     %eax,0x300(%rbx)
0xffffffffc0992b96 <vcpu_enter_guest+3366>:	sbb    %eax,%eax
0xffffffffc0992b98 <vcpu_enter_guest+3368>:	test   %eax,%eax
0xffffffffc0992b9a <vcpu_enter_guest+3370>:	je     0xffffffffc0992bf7 <vcpu_enter_guest+3463>
0xffffffffc0992b9c <vcpu_enter_guest+3372>:	mov    0x80(%rbx),%rax
0xffffffffc0992ba3 <vcpu_enter_guest+3379>:	movl   $0x1a,0x8(%rax)
0xffffffffc0992baa <vcpu_enter_guest+3386>:	mov    0x80(%rbx),%rax
0xffffffffc0992bb1 <vcpu_enter_guest+3393>:	mov    0x2cfc(%rbx),%edx
0xffffffffc0992bb7 <vcpu_enter_guest+3399>:	mov    %dl,0x20(%rax)
0xffffffffc0992bba <vcpu_enter_guest+3402>:	xor    %eax,%eax
0xffffffffc0992bbc <vcpu_enter_guest+3404>:	jmpq   0xffffffffc0992517 <vcpu_enter_guest+1703>
0xffffffffc0992bc1 <vcpu_enter_guest+3409>:	mov    0x4e028(%rip),%rax        # 0xffffffffc09e0bf0
0xffffffffc0992bc8 <vcpu_enter_guest+3416>:	mov    %rbx,%rdi
0xffffffffc0992bcb <vcpu_enter_guest+3419>:	callq  *0x150(%rax)
0xffffffffc0992bd1 <vcpu_enter_guest+3425>:	mov    %rax,%rsi
0xffffffffc0992bd4 <vcpu_enter_guest+3428>:	mov    %rbx,%rdi
0xffffffffc0992bd7 <vcpu_enter_guest+3431>:	and    $0xfffffffffffffeff,%rsi
0xffffffffc0992bde <vcpu_enter_guest+3438>:	testb  $0x2,0x38(%rbx)
0xffffffffc0992be2 <vcpu_enter_guest+3442>:	cmove  %rax,%rsi
0xffffffffc0992be6 <vcpu_enter_guest+3446>:	or     $0x10000,%rsi
0xffffffffc0992bed <vcpu_enter_guest+3453>:	callq  0xffffffffc098fe10 <__kvm_set_rflags>
0xffffffffc0992bf2 <vcpu_enter_guest+3458>:	jmpq   0xffffffffc09929a8 <vcpu_enter_guest+2872>
0xffffffffc0992bf7 <vcpu_enter_guest+3463>:	mov    0x30(%rbx),%rax
0xffffffffc0992bfb <vcpu_enter_guest+3467>:	test   $0x200000,%eax
0xffffffffc0992c00 <vcpu_enter_guest+3472>:	je     0xffffffffc0992c94 <vcpu_enter_guest+3620>
0xffffffffc0992c06 <vcpu_enter_guest+3478>:	lock andb $0xdf,0x32(%rbx)
0xffffffffc0992c0b <vcpu_enter_guest+3483>:	mov    0x2f0(%rbx),%rax
0xffffffffc0992c12 <vcpu_enter_guest+3490>:	nopl   0x0(%rax,%rax,1)
0xffffffffc0992c17 <vcpu_enter_guest+3495>:	lea    0x300(%rbx),%r13
0xffffffffc0992c1e <vcpu_enter_guest+3502>:	xor    %eax,%eax
0xffffffffc0992c20 <vcpu_enter_guest+3504>:	mov    $0x20,%edx
0xffffffffc0992c25 <vcpu_enter_guest+3509>:	test   $0x1,%r13b
0xffffffffc0992c29 <vcpu_enter_guest+3513>:	mov    %r13,%rdi
0xffffffffc0992c2c <vcpu_enter_guest+3516>:	jne    0xffffffffc0992e89 <vcpu_enter_guest+4121>
0xffffffffc0992c32 <vcpu_enter_guest+3522>:	test   $0x2,%dil
0xffffffffc0992c36 <vcpu_enter_guest+3526>:	jne    0xffffffffc0992dcc <vcpu_enter_guest+3932>
0xffffffffc0992c3c <vcpu_enter_guest+3532>:	mov    %edx,%ecx
0xffffffffc0992c3e <vcpu_enter_guest+3534>:	shr    $0x2,%ecx
0xffffffffc0992c41 <vcpu_enter_guest+3537>:	test   $0x2,%dl
0xffffffffc0992c44 <vcpu_enter_guest+3540>:	rep stos %eax,%es:(%rdi)
0xffffffffc0992c46 <vcpu_enter_guest+3542>:	je     0xffffffffc0992c4a <vcpu_enter_guest+3546>
0xffffffffc0992c48 <vcpu_enter_guest+3544>:	stos   %ax,%es:(%rdi)
0xffffffffc0992c4a <vcpu_enter_guest+3546>:	and    $0x1,%edx
0xffffffffc0992c4d <vcpu_enter_guest+3549>:	je     0xffffffffc0992c50 <vcpu_enter_guest+3552>
0xffffffffc0992c4f <vcpu_enter_guest+3551>:	stos   %al,%es:(%rdi)
0xffffffffc0992c50 <vcpu_enter_guest+3552>:	mov    (%rbx),%rax
0xffffffffc0992c53 <vcpu_enter_guest+3555>:	cmpb   $0x0,0x32e8(%rax)
0xffffffffc0992c5a <vcpu_enter_guest+3562>:	jne    0xffffffffc0992dbc <vcpu_enter_guest+3916>
0xffffffffc0992c60 <vcpu_enter_guest+3568>:	cmpb   $0x0,0x2f8(%rbx)
0xffffffffc0992c67 <vcpu_enter_guest+3575>:	je     0xffffffffc0992c79 <vcpu_enter_guest+3593>
0xffffffffc0992c69 <vcpu_enter_guest+3577>:	mov    0x4df80(%rip),%rax        # 0xffffffffc09e0bf0
0xffffffffc0992c70 <vcpu_enter_guest+3584>:	mov    %rbx,%rdi
0xffffffffc0992c73 <vcpu_enter_guest+3587>:	callq  *0x240(%rax)
0xffffffffc0992c79 <vcpu_enter_guest+3593>:	mov    %r13,%rsi
0xffffffffc0992c7c <vcpu_enter_guest+3596>:	mov    %rbx,%rdi
0xffffffffc0992c7f <vcpu_enter_guest+3599>:	callq  0xffffffffc09bc3a0 <kvm_ioapic_scan_entry>
0xffffffffc0992c84 <vcpu_enter_guest+3604>:	mov    0x4df65(%rip),%rax        # 0xffffffffc09e0bf0
0xffffffffc0992c8b <vcpu_enter_guest+3611>:	mov    %rbx,%rdi
0xffffffffc0992c8e <vcpu_enter_guest+3614>:	callq  *0x220(%rax)
0xffffffffc0992c94 <vcpu_enter_guest+3620>:	mov    0x30(%rbx),%rax
0xffffffffc0992c98 <vcpu_enter_guest+3624>:	test   $0x2000000,%eax
0xffffffffc0992c9d <vcpu_enter_guest+3629>:	je     0xffffffffc0992cac <vcpu_enter_guest+3644>
0xffffffffc0992c9f <vcpu_enter_guest+3631>:	lock andb $0xfd,0x33(%rbx)
0xffffffffc0992ca4 <vcpu_enter_guest+3636>:	mov    %rbx,%rdi
0xffffffffc0992ca7 <vcpu_enter_guest+3639>:	callq  0xffffffffc098df10 <kvm_vcpu_reload_apic_access_page>
0xffffffffc0992cac <vcpu_enter_guest+3644>:	mov    0x30(%rbx),%rax
0xffffffffc0992cb0 <vcpu_enter_guest+3648>:	test   $0x8000000,%eax
0xffffffffc0992cb5 <vcpu_enter_guest+3653>:	je     0xffffffffc0991edb <vcpu_enter_guest+107>
0xffffffffc0992cbb <vcpu_enter_guest+3659>:	lock andb $0xf7,0x33(%rbx)
0xffffffffc0992cc0 <vcpu_enter_guest+3664>:	mov    0x80(%rbx),%rax
0xffffffffc0992cc7 <vcpu_enter_guest+3671>:	movl   $0x18,0x8(%rax)
0xffffffffc0992cce <vcpu_enter_guest+3678>:	mov    0x80(%rbx),%rax
0xffffffffc0992cd5 <vcpu_enter_guest+3685>:	movl   $0x3,0x20(%rax)
0xffffffffc0992cdc <vcpu_enter_guest+3692>:	xor    %eax,%eax
0xffffffffc0992cde <vcpu_enter_guest+3694>:	jmpq   0xffffffffc0992517 <vcpu_enter_guest+1703>
0xffffffffc0992ce3 <vcpu_enter_guest+3699>:	lea    0x2188(%rbx),%r15
0xffffffffc0992cea <vcpu_enter_guest+3706>:	lea    0x2160(%rbx),%r13
0xffffffffc0992cf1 <vcpu_enter_guest+3713>:	mov    (%rbx),%rdi
0xffffffffc0992cf4 <vcpu_enter_guest+3716>:	mov    $0x40,%ecx
0xffffffffc0992cf9 <vcpu_enter_guest+3721>:	mov    %r15,%rdx
0xffffffffc0992cfc <vcpu_enter_guest+3724>:	mov    %r13,%rsi
0xffffffffc0992cff <vcpu_enter_guest+3727>:	callq  0xffffffffc0982290 <kvm_read_guest_cached>
0xffffffffc0992d04 <vcpu_enter_guest+3732>:	test   %eax,%eax
0xffffffffc0992d06 <vcpu_enter_guest+3734>:	jne    0xffffffffc0992b0e <vcpu_enter_guest+3230>
0xffffffffc0992d0c <vcpu_enter_guest+3740>:	mov    0x2190(%rbx),%eax
0xffffffffc0992d12 <vcpu_enter_guest+3746>:	test   $0x1,%al
0xffffffffc0992d14 <vcpu_enter_guest+3748>:	je     0xffffffffc0992d1f <vcpu_enter_guest+3759>
0xffffffffc0992d16 <vcpu_enter_guest+3750>:	add    $0x1,%eax
0xffffffffc0992d19 <vcpu_enter_guest+3753>:	mov    %eax,0x2190(%rbx)
0xffffffffc0992d1f <vcpu_enter_guest+3759>:	addl   $0x1,0x2190(%rbx)
0xffffffffc0992d26 <vcpu_enter_guest+3766>:	mov    (%rbx),%rdi
0xffffffffc0992d29 <vcpu_enter_guest+3769>:	mov    $0x40,%ecx
0xffffffffc0992d2e <vcpu_enter_guest+3774>:	mov    %r15,%rdx
0xffffffffc0992d31 <vcpu_enter_guest+3777>:	mov    %r13,%rsi
0xffffffffc0992d34 <vcpu_enter_guest+3780>:	callq  0xffffffffc097ee70 <kvm_write_guest_cached>
0xffffffffc0992d39 <vcpu_enter_guest+3785>:	mov    %gs:0xce00,%rdx
0xffffffffc0992d42 <vcpu_enter_guest+3794>:	mov    0x418(%rdx),%rax
0xffffffffc0992d49 <vcpu_enter_guest+3801>:	add    0x2188(%rbx),%rax
0xffffffffc0992d50 <vcpu_enter_guest+3808>:	mov    $0x40,%ecx
0xffffffffc0992d55 <vcpu_enter_guest+3813>:	sub    0x2158(%rbx),%rax
0xffffffffc0992d5c <vcpu_enter_guest+3820>:	mov    (%rbx),%rdi
0xffffffffc0992d5f <vcpu_enter_guest+3823>:	mov    %r13,%rsi
0xffffffffc0992d62 <vcpu_enter_guest+3826>:	mov    %rax,0x2188(%rbx)
0xffffffffc0992d69 <vcpu_enter_guest+3833>:	mov    0x418(%rdx),%rax
0xffffffffc0992d70 <vcpu_enter_guest+3840>:	mov    %r15,%rdx
0xffffffffc0992d73 <vcpu_enter_guest+3843>:	mov    %rax,0x2158(%rbx)
0xffffffffc0992d7a <vcpu_enter_guest+3850>:	callq  0xffffffffc097ee70 <kvm_write_guest_cached>
0xffffffffc0992d7f <vcpu_enter_guest+3855>:	addl   $0x1,0x2190(%rbx)
0xffffffffc0992d86 <vcpu_enter_guest+3862>:	mov    (%rbx),%rdi
0xffffffffc0992d89 <vcpu_enter_guest+3865>:	mov    $0x40,%ecx
0xffffffffc0992d8e <vcpu_enter_guest+3870>:	mov    %r15,%rdx
0xffffffffc0992d91 <vcpu_enter_guest+3873>:	mov    %r13,%rsi
0xffffffffc0992d94 <vcpu_enter_guest+3876>:	callq  0xffffffffc097ee70 <kvm_write_guest_cached>
0xffffffffc0992d99 <vcpu_enter_guest+3881>:	jmpq   0xffffffffc0992b0e <vcpu_enter_guest+3230>
0xffffffffc0992d9e <vcpu_enter_guest+3886>:	mov    0xa0(%rax),%rax
0xffffffffc0992da5 <vcpu_enter_guest+3893>:	testb  $0x8,0x2e9(%rax)
0xffffffffc0992dac <vcpu_enter_guest+3900>:	je     0xffffffffc0992c94 <vcpu_enter_guest+3620>
0xffffffffc0992db2 <vcpu_enter_guest+3906>:	jmpq   0xffffffffc0992c17 <vcpu_enter_guest+3495>
0xffffffffc0992db7 <vcpu_enter_guest+3911>:	callq  0xffffffff81087720 <__stack_chk_fail>
0xffffffffc0992dbc <vcpu_enter_guest+3916>:	mov    %r13,%rsi
0xffffffffc0992dbf <vcpu_enter_guest+3919>:	mov    %rbx,%rdi
0xffffffffc0992dc2 <vcpu_enter_guest+3922>:	callq  0xffffffffc09bd980 <kvm_scan_ioapic_routes>
0xffffffffc0992dc7 <vcpu_enter_guest+3927>:	jmpq   0xffffffffc0992c84 <vcpu_enter_guest+3604>
0xffffffffc0992dcc <vcpu_enter_guest+3932>:	stos   %ax,%es:(%rdi)
0xffffffffc0992dce <vcpu_enter_guest+3934>:	sub    $0x2,%edx
0xffffffffc0992dd1 <vcpu_enter_guest+3937>:	jmpq   0xffffffffc0992c3c <vcpu_enter_guest+3532>
0xffffffffc0992dd6 <vcpu_enter_guest+3942>:	testb  $0x2,0x3a(%rbx)
0xffffffffc0992dda <vcpu_enter_guest+3946>:	jne    0xffffffffc0992f5f <vcpu_enter_guest+4335>
0xffffffffc0992de0 <vcpu_enter_guest+3952>:	mov    0x4de09(%rip),%rax        # 0xffffffffc09e0bf0
0xffffffffc0992de7 <vcpu_enter_guest+3959>:	mov    %rbx,%rdi
0xffffffffc0992dea <vcpu_enter_guest+3962>:	callq  *0x138(%rax)
0xffffffffc0992df0 <vcpu_enter_guest+3968>:	testb  $0x2,0x3a(%rbx)
0xffffffffc0992df4 <vcpu_enter_guest+3972>:	je     0xffffffffc0992f40 <vcpu_enter_guest+4304>
0xffffffffc0992dfa <vcpu_enter_guest+3978>:	mov    %rbx,%rdi
0xffffffffc0992dfd <vcpu_enter_guest+3981>:	callq  0xffffffffc09857f0 <kvm_update_dr7>
0xffffffffc0992e02 <vcpu_enter_guest+3986>:	andl   $0xfffffffb,0x23a8(%rbx)
0xffffffffc0992e09 <vcpu_enter_guest+3993>:	jmpq   0xffffffffc0992108 <vcpu_enter_guest+664>
0xffffffffc0992e0e <vcpu_enter_guest+3998>:	xor    %r12d,%r12d
0xffffffffc0992e11 <vcpu_enter_guest+4001>:	mov    $0x7,%edi
0xffffffffc0992e16 <vcpu_enter_guest+4006>:	mov    %r12,%rsi
0xffffffffc0992e19 <vcpu_enter_guest+4009>:	callq  0xffffffff810636d0 <native_set_debugreg>
0xffffffffc0992e1e <vcpu_enter_guest+4014>:	xchg   %ax,%ax
0xffffffffc0992e20 <vcpu_enter_guest+4016>:	mov    0x23e0(%rbx),%rsi
0xffffffffc0992e27 <vcpu_enter_guest+4023>:	mov    %r12,%rdi
0xffffffffc0992e2a <vcpu_enter_guest+4026>:	callq  0xffffffff810636d0 <native_set_debugreg>
0xffffffffc0992e2f <vcpu_enter_guest+4031>:	xchg   %ax,%ax
0xffffffffc0992e31 <vcpu_enter_guest+4033>:	mov    $0x1,%edi
0xffffffffc0992e36 <vcpu_enter_guest+4038>:	mov    0x23e8(%rbx),%rsi
0xffffffffc0992e3d <vcpu_enter_guest+4045>:	callq  0xffffffff810636d0 <native_set_debugreg>
0xffffffffc0992e42 <vcpu_enter_guest+4050>:	xchg   %ax,%ax
0xffffffffc0992e44 <vcpu_enter_guest+4052>:	mov    $0x2,%edi
0xffffffffc0992e49 <vcpu_enter_guest+4057>:	mov    0x23f0(%rbx),%rsi
0xffffffffc0992e50 <vcpu_enter_guest+4064>:	callq  0xffffffff810636d0 <native_set_debugreg>
0xffffffffc0992e55 <vcpu_enter_guest+4069>:	xchg   %ax,%ax
0xffffffffc0992e57 <vcpu_enter_guest+4071>:	mov    $0x3,%edi
0xffffffffc0992e5c <vcpu_enter_guest+4076>:	mov    0x23f8(%rbx),%rsi
0xffffffffc0992e63 <vcpu_enter_guest+4083>:	callq  0xffffffff810636d0 <native_set_debugreg>
0xffffffffc0992e68 <vcpu_enter_guest+4088>:	xchg   %ax,%ax
0xffffffffc0992e6a <vcpu_enter_guest+4090>:	mov    $0x6,%edi
0xffffffffc0992e6f <vcpu_enter_guest+4095>:	mov    0x23d0(%rbx),%rsi
0xffffffffc0992e76 <vcpu_enter_guest+4102>:	callq  0xffffffff810636d0 <native_set_debugreg>
0xffffffffc0992e7b <vcpu_enter_guest+4107>:	xchg   %ax,%ax
0xffffffffc0992e7d <vcpu_enter_guest+4109>:	andl   $0xfffffffb,0x23a8(%rbx)
0xffffffffc0992e84 <vcpu_enter_guest+4116>:	jmpq   0xffffffffc09920eb <vcpu_enter_guest+635>
0xffffffffc0992e89 <vcpu_enter_guest+4121>:	stos   %al,%es:(%rdi)
0xffffffffc0992e8a <vcpu_enter_guest+4122>:	mov    $0x1f,%dl
0xffffffffc0992e8c <vcpu_enter_guest+4124>:	jmpq   0xffffffffc0992c32 <vcpu_enter_guest+3522>
0xffffffffc0992e91 <vcpu_enter_guest+4129>:	mov    %rbx,%rdi
0xffffffffc0992e94 <vcpu_enter_guest+4132>:	callq  0xffffffffc09b9de0 <kvm_lapic_sync_from_vapic>
0xffffffffc0992e99 <vcpu_enter_guest+4137>:	mov    %r12d,%eax
0xffffffffc0992e9c <vcpu_enter_guest+4140>:	jmpq   0xffffffffc0992517 <vcpu_enter_guest+1703>
0xffffffffc0992ea1 <vcpu_enter_guest+4145>:	mov    %rbx,%rsi
0xffffffffc0992ea4 <vcpu_enter_guest+4148>:	mov    $0x8,%edi
0xffffffffc0992ea9 <vcpu_enter_guest+4153>:	callq  0xffffffffc098bc90 <kvm_make_request>
0xffffffffc0992eae <vcpu_enter_guest+4158>:	jmpq   0xffffffffc0992209 <vcpu_enter_guest+921>
0xffffffffc0992eb3 <vcpu_enter_guest+4163>:	mov    %rbx,%rdi
0xffffffffc0992eb6 <vcpu_enter_guest+4166>:	callq  0xffffffffc09a95a0 <kvm_mmu_load>
0xffffffffc0992ebb <vcpu_enter_guest+4171>:	test   %eax,%eax
0xffffffffc0992ebd <vcpu_enter_guest+4173>:	mov    %eax,%r12d
0xffffffffc0992ec0 <vcpu_enter_guest+4176>:	jne    0xffffffffc099273b <vcpu_enter_guest+2251>
0xffffffffc0992ec6 <vcpu_enter_guest+4182>:	jmpq   0xffffffffc0991fee <vcpu_enter_guest+382>
0xffffffffc0992ecb <vcpu_enter_guest+4187>:	mov    %r13,%rdi
0xffffffffc0992ece <vcpu_enter_guest+4190>:	push   %rdi
0xffffffffc0992ecf <vcpu_enter_guest+4191>:	popfq  
0xffffffffc0992ed0 <vcpu_enter_guest+4192>:	nopl   0x0(%rax,%rax,1)
0xffffffffc0992ed5 <vcpu_enter_guest+4197>:	mov    $0x8,%edi
0xffffffffc0992eda <vcpu_enter_guest+4202>:	mov    %rbx,%rsi
0xffffffffc0992edd <vcpu_enter_guest+4205>:	callq  0xffffffffc098bc90 <kvm_make_request>
0xffffffffc0992ee2 <vcpu_enter_guest+4210>:	mov    $0x1,%eax
0xffffffffc0992ee7 <vcpu_enter_guest+4215>:	jmpq   0xffffffffc0992517 <vcpu_enter_guest+1703>
0xffffffffc0992eec <vcpu_enter_guest+4220>:	cmpb   $0x0,0x4dcf5(%rip)        # 0xffffffffc09e0be8
0xffffffffc0992ef3 <vcpu_enter_guest+4227>:	mov    %r8,%rsi
0xffffffffc0992ef6 <vcpu_enter_guest+4230>:	je     0xffffffffc0992efe <vcpu_enter_guest+4238>
0xffffffffc0992ef8 <vcpu_enter_guest+4232>:	mov    0x2208(%rbx),%esi
0xffffffffc0992efe <vcpu_enter_guest+4238>:	lea    0x2110(%rbx),%rcx
0xffffffffc0992f05 <vcpu_enter_guest+4245>:	lea    0x2114(%rbx),%rdx
0xffffffffc0992f0c <vcpu_enter_guest+4252>:	mov    $0xf4240,%edi
0xffffffffc0992f11 <vcpu_enter_guest+4257>:	mov    %r8,0x8(%rsp)
0xffffffffc0992f16 <vcpu_enter_guest+4262>:	mov    %r11,0x10(%rsp)
0xffffffffc0992f1b <vcpu_enter_guest+4267>:	mov    %r10,0x18(%rsp)
0xffffffffc0992f20 <vcpu_enter_guest+4272>:	callq  0xffffffffc098c860 <kvm_get_time_scale>
0xffffffffc0992f25 <vcpu_enter_guest+4277>:	mov    0x8(%rsp),%r8
0xffffffffc0992f2a <vcpu_enter_guest+4282>:	mov    0x10(%rsp),%r11
0xffffffffc0992f2f <vcpu_enter_guest+4287>:	mov    0x18(%rsp),%r10
0xffffffffc0992f34 <vcpu_enter_guest+4292>:	mov    %r8d,0x2118(%rbx)
0xffffffffc0992f3b <vcpu_enter_guest+4299>:	jmpq   0xffffffffc09923e2 <vcpu_enter_guest+1394>
0xffffffffc0992f40 <vcpu_enter_guest+4304>:	mov    %rbx,%rdi
0xffffffffc0992f43 <vcpu_enter_guest+4307>:	callq  0xffffffffc098d730 <kvm_update_dr0123>
0xffffffffc0992f48 <vcpu_enter_guest+4312>:	testb  $0x2,0x3a(%rbx)
0xffffffffc0992f4c <vcpu_enter_guest+4316>:	jne    0xffffffffc0992dfa <vcpu_enter_guest+3978>
0xffffffffc0992f52 <vcpu_enter_guest+4322>:	mov    %rbx,%rdi
0xffffffffc0992f55 <vcpu_enter_guest+4325>:	callq  0xffffffffc098d780 <kvm_update_dr6>
0xffffffffc0992f5a <vcpu_enter_guest+4330>:	jmpq   0xffffffffc0992dfa <vcpu_enter_guest+3978>
0xffffffffc0992f5f <vcpu_enter_guest+4335>:	mov    $0x1a38,%esi
0xffffffffc0992f64 <vcpu_enter_guest+4340>:	mov    $0xffffffffc09d4150,%rdi
0xffffffffc0992f6b <vcpu_enter_guest+4347>:	callq  0xffffffff81087af0 <warn_slowpath_null>
0xffffffffc0992f70 <vcpu_enter_guest+4352>:	jmpq   0xffffffffc0992de0 <vcpu_enter_guest+3952>
0xffffffffc0992f75 <vcpu_enter_guest+4357>:	mov    0x298(%rbx),%rax
0xffffffffc0992f7c <vcpu_enter_guest+4364>:	test   $0x10000,%eax
0xffffffffc0992f81 <vcpu_enter_guest+4369>:	je     0xffffffffc0992fa0 <vcpu_enter_guest+4400>
0xffffffffc0992f83 <vcpu_enter_guest+4371>:	mov    0x290(%rbx),%rsi
0xffffffffc0992f8a <vcpu_enter_guest+4378>:	mov    $0x1,%edx
0xffffffffc0992f8f <vcpu_enter_guest+4383>:	mov    $0x4,%edi
0xffffffffc0992f94 <vcpu_enter_guest+4388>:	callq  0xffffffff810e8c70 <profile_hits>
0xffffffffc0992f99 <vcpu_enter_guest+4393>:	jmpq   0xffffffffc09921fc <vcpu_enter_guest+908>
0xffffffffc0992f9e <vcpu_enter_guest+4398>:	ud2    
0xffffffffc0992fa0 <vcpu_enter_guest+4400>:	mov    0x4dc49(%rip),%rax        # 0xffffffffc09e0bf0
0xffffffffc0992fa7 <vcpu_enter_guest+4407>:	mov    $0x10,%esi
0xffffffffc0992fac <vcpu_enter_guest+4412>:	mov    %rbx,%rdi
0xffffffffc0992faf <vcpu_enter_guest+4415>:	callq  *0x148(%rax)
0xffffffffc0992fb5 <vcpu_enter_guest+4421>:	cmpl   $0x4,-0x3ee7616c(%rip)        # 0xffffffff81b1ce50 <prof_on>
0xffffffffc0992fbc <vcpu_enter_guest+4428>:	mov    0x290(%rbx),%rsi
0xffffffffc0992fc3 <vcpu_enter_guest+4435>:	jne    0xffffffffc09921fc <vcpu_enter_guest+908>
0xffffffffc0992fc9 <vcpu_enter_guest+4441>:	jmp    0xffffffffc0992f8a <vcpu_enter_guest+4378>

```
