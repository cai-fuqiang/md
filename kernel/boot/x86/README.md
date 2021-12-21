# x86 boot 简述
##  系统刚启动 : bios
x86 cpu 在加电后，进入的是16位实模式，其中, 和代码相关寄存器
值为：

```
IP  			0xfff0
CS selector 	0xf000
CS base 		0xffff0000
```
我们可以通过qemu来模拟下

qemu启动脚本
```
qemu-system-x86_64 -name guest=centos8,debug-threads=on \
        -m 2G -realtime mlock=off \
        -boot strict=on \
        -device virtio-scsi-pci,id=scsi0,bus=pci.0,addr=0x7 \
        -drive file=/root/workspace/centos_qcow2/CentOS-8-ec2-8.1.1911-20200113.3.x86_64.qcow2,if=none,id=drive-scsi0-0-0-0 \
        -device scsi-hd,bus=scsi0.0,channel=0,scsi-id=0,lun=0,drive=drive-scsi0-0-0-0,id=scsi0-0-0-0,bootindex=1 \
        -sandbox on,obsolete=deny,elevateprivileges=deny,spawn=deny,resourcecontrol=deny -msg timestamp=on -monitor stdio  -s -S
```
qemu monitor cmd：show register
```
EIP=0000fff0 EFL=00000002 [-------] CPL=0 II=0 A20=1 SMM=0 HLT=0
CS =f000 ffff0000 0000ffff 00009b00
SS =0000 00000000 0000ffff 00009300
DS =0000 00000000 0000ffff 00009300
FS =0000 00000000 0000ffff 00009300
GS =0000 00000000 0000ffff 00009300
LDT=0000 00000000 0000ffff 00008200
TR =0000 00000000 0000ffff 00008b00
GDT=     00000000 0000ffff
IDT=     00000000 0000ffff
DR0=0000000000000000 DR1=0000000000000000 DR2=0000000000000000 DR3=0000000000000000
EFER=0000000000000000
```
上面我们只看下EIP、CS selector、CR0
```
EIP=0000fff0 		CS=f000 	==> 	CS base: ffff0000
CR0=0000000000000000 			==> 	表明是16位实模式
```

bios在最后的流程中会将启动分区的`boot loader(OS loader), `(这里指的是传统的bios)，加载到
内存的`0x7c00`处,然后将跳转到`0x0000:0x7c00`, 进入boot loader的流程

##
