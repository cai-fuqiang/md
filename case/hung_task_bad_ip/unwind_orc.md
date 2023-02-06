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


# objtool 测试
kernel build objtool cmd
```
./tools/objtool/objtool orc generate  --module --no-fp  --retpoline "/root/wangfuqiang/kernel_test/orc/.tmp_proc-test.o";
```
获取编译参数:
```
gcc -Wp,-MD,/root/wangfuqiang/kernel_test/orc/.proc-test.o.d	\
	-nostdinc -isystem /usr/lib/gcc/x86_64-redhat-linux/8/include	\
	-I./arch/x86/include -I./arch/x86/include/generated	\
	-I./include/drm-backport -I./include -I./arch/x86/include/uapi  \
	-I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi  \
	-include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h  \
	-D__KERNEL__ -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing   \
	-fno-common -fshort-wchar -Werror-implicit-function-declaration -Wno-format-security   \
	-std=gnu89 -fno-PIE -DCC_HAVE_ASM_GOTO -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx   \
	-m64 -falign-jumps=1 -falign-loops=1 -mno-80387 -mno-fp-ret-in-387 -mpreferred-stack-boundary=3   \
	-mskip-rax-setup -mtune=generic -mno-red-zone -mcmodel=kernel -funit-at-a-time   \
	-DCONFIG_AS_CFI=1 -DCONFIG_AS_CFI_SIGNAL_FRAME=1 -DCONFIG_AS_CFI_SECTIONS=1   \
	-DCONFIG_AS_FXSAVEQ=1 -DCONFIG_AS_SSSE3=1 -DCONFIG_AS_CRC32=1 -DCONFIG_AS_AVX=1   \
	-DCONFIG_AS_AVX2=1 -DCONFIG_AS_AVX512=1 -DCONFIG_AS_SHA1_NI=1 -DCONFIG_AS_SHA256_NI=1   \
	-pipe -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern   \
	-mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks   \
	-Wno-frame-address -Wno-format-truncation -Wno-format-overflow   \
	-Wno-int-in-bool-context -O2 --param=allow-store-data-races=0   \
	-Wframe-larger-than=2048 -fstack-protector-strong -Wno-unused-but-set-variable   \
	-Wno-unused-const-variable -fno-var-tracking-assignments -g -gdwarf-4   \
	-pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -fno-inline-functions-called-once   \
	-Wdeclaration-after-statement -Wno-pointer-sign -Wno-stringop-truncation   \
	-fno-strict-overflow -fno-merge-all-constants -fmerge-constants   \
	-fno-stack-check -fconserve-stack -Werror=implicit-int -Werror=strict-prototypes   \
	-Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init   \
	-fmacro-prefix-map=./= -Wno-packed-not-aligned  -DMODULE    \
	-DKBUILD_BASENAME='"proc_test"' -DKBUILD_MODNAME='"proctest"'   \
	-c -o /root/wangfuqiang/kernel_test/orc/.tmp_proc-test.o /root/wangfuqiang/kernel_test/orc/proc-test.c
```
可以看到是O2的编译, 并且没有`-fno-optimize-sibling-calls`
用gcc翻译成汇编看下
```
my_init:
.LFB1893:
        .loc 1 17 1 is_stmt 1 view -0
        .cfi_startproc
1:      call    __fentry__
        .section __mcount_loc, "a",@progbits
        .quad 1b
        .previous
        .loc 1 18 5 view .LVU12
        .loc 1 8 2 view .LVU13
        .loc 1 9 2 view .LVU14
        .loc 1 9 14 view .LVU15
        .loc 1 13 2 view .LVU16
        .loc 1 19 5 view .LVU17
        .loc 1 20 1 is_stmt 0 view .LVU18
        xorl    %eax, %eax
        ret
        .cfi_endproc
```
可以看到仍然有dwarf相关的注释

修改`script/Makefile.build`， 使其在编译时中断：
```
diff --git a/scripts/Makefile.build b/scripts/Makefile.build
index 723ea41..99e210d 100644
--- a/scripts/Makefile.build
+++ b/scripts/Makefile.build
@@ -272,7 +272,7 @@ endif
 # 'OBJECT_FILES_NON_STANDARD_foo.o := 'n': override directory skip for a file
 cmd_objtool = $(if $(patsubst y%,, \
        $(OBJECT_FILES_NON_STANDARD_$(basetarget).o)$(OBJECT_FILES_NON_STANDARD)n), \
-       $(__objtool_obj) $(objtool_args) "$(objtool_o)";)
+       exit;$(__objtool_obj) $(objtool_args) "$(objtool_o)";)
 objtool_obj = $(if $(patsubst y%,, \
        $(OBJECT_FILES_NON_STANDARD_$(basetarget).o)$(OBJECT_FILES_NON_STANDARD)n), \
        $(__objtool_obj))
```

产生`.tmp_proc-test.o`
但是通过`readelf -wF .tmp_proc-test.o`，来看, dwarf相关table不存在
```
[root@bogon orc]# readelf -wF .tmp_proc-test.o
Contents of the .debug_frame section:


00000000 0000000000000014 ffffffff CIE "" cf=1 df=-8 ra=16
   LOC           CFA      ra
0000000000000000 rsp+8    c-8

00000018 0000000000000014 00000000 FDE cie=00000000 pc=0000000000000000..000000000000000d

00000030 0000000000000014 00000000 FDE cie=00000000 pc=0000000000000000..0000000000000008

00000048 0000000000000014 00000000 FDE cie=00000000 pc=0000000000000000..0000000000000001
```

## 自己做测试
```
#include <stdio.h>
int func()
{
        return 0;
}
int main()
{
        return 0;
}
```
反汇编看下:
```
func:
.LFB0:
        .cfi_startproc
        pushq   %rbp
        .cfi_def_cfa_offset 16
        .cfi_offset 6, -16
        movq    %rsp, %rbp
        .cfi_def_cfa_register 6
        movl    $0, %eax
        popq    %rbp
        .cfi_def_cfa 7, 8
        ret
        .cfi_endproc
```
里面有堆栈操作，也有cfi相关注释。
然后通过`readelf -wF`看下:
```
.eh_frame 节的内容:


00000000 0000000000000014 00000000 CIE "zR" cf=1 df=-8 ra=16
   LOC           CFA      ra
0000000000000000 rsp+8    c-8

00000018 0000000000000018 0000001c FDE cie=00000000 pc=0000000000000000..00000000000000ab
   LOC           CFA      ra
0000000000000000 rsp+8    c-8
0000000000000007 rsp+432  c-8
00000000000000a5 rsp+8    c-8
00000000000000a6 rsp+432  c-8

00000034 0000000000000018 00000038 FDE cie=00000000 pc=0000000000000000..0000000000000022
   LOC           CFA      ra
0000000000000000 rsp+8    c-8
0000000000000004 rsp+16   c-8
0000000000000021 rsp+8    c-8
```
可以发现有内容。
假如说，我们通过-O2编译下:
```
func:
.LFB11:
        .cfi_startproc
        xorl    %eax, %eax
        ret
        .cfi_endproc
```
可以看到这些东西给优化了。该函数没有堆栈操作，也就不需要相关注释了。
在通过`readelf -wF`看下
```
.eh_frame 节的内容:


00000000 0000000000000014 00000000 CIE "zR" cf=1 df=-8 ra=16
   LOC           CFA      ra
0000000000000000 rsp+8    c-8

00000018 0000000000000010 0000001c FDE cie=00000000 pc=0000000000000000..0000000000000003

0000002c 0000000000000010 00000030 FDE cie=00000000 pc=0000000000000000..0000000000000003
```
可以发现，也是这样的输出。kernel也是同样的情况。
我们把kernel代码变复杂，使其访问堆栈:

```
int add_num(void)
{
        int a[100];
        int ret = 0;
        int i = 0;
        for (i = 0; i < 100; i++) {
                a[i] = i;
        }
        for (i = 0; i < 100; i++) {
                ret += a[i];
        }
        return ret;
}
```
我们再编译看下:
```
00000000 0000000000000014 ffffffff CIE "" cf=1 df=-8 ra=16
   LOC           CFA      ra
0000000000000000 rsp+8    c-8

00000018 0000000000000024 00000000 FDE cie=00000000 pc=0000000000000000..0000000000000063
   LOC           CFA      ra
0000000000000000 rsp+8    c-8
000000000000000c rsp+416  c-8
000000000000005d rsp+8    c-8
000000000000005e rsp+416  c-8

00000040 0000000000000014 00000000 FDE cie=00000000 pc=0000000000000000..000000000000000a

00000058 0000000000000014 00000000 FDE cie=00000000 pc=0000000000000000..0000000000000001
```
