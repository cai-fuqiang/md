EasyStack Cloud Linux 8.4 (AltArch)
Kernel 4.18.0-147.5.1.es8_24.aarch64 on an aarch64

bootstrap login: [ 1291.048268] sd 0:0:0:1: [sdd] Synchronizing SCSI cache
[ 1291.049228] sd 0:0:0:2: [sdc] Synchronizing SCSI cache
[ 1291.049820] sd 0:0:0:3: [sdb] Synchronizing SCSI cache
[ 1291.050451] sd 0:0:0:0: [sda] Synchronizing SCSI cache
[ 1291.089297] reboot: Restarting system
UEFI firmware starting.
[2J[01;01H[=3h[2J[01;01H[0m[35m[40m[0m[37m[40m
>>Start PXE over IPv4.
  Station IP address is 10.20.0.6

  Server IP address is 10.20.0.2
  NBP filename is grub/grubaa64.efi
  NBP filesize is 1998584 Bytes
 Downloading NBP file...

  NBP file downloaded successfully.
BdsDxe: loading Boot0003 "UEFI PXEv4 (MAC:FA163EFBE7BA)" from PciRoot(0x0)/Pci(0x1,0x0)/Pci(0x0,0x0)/MAC(FA163EFBE7BA,0x1)/IPv4(0.0.0.0,0x0,DHCP,0.0.0.0,0.0.0.0,0.0.0.0)
BdsDxe: starting Boot0003 "UEFI PXEv4 (MAC:FA163EFBE7BA)" from PciRoot(0x0)/Pci(0x1,0x0)/Pci(0x0,0x0)/MAC(FA163EFBE7BA,0x1)/IPv4(0.0.0.0,0x0,DHCP,0.0.0.0,0.0.0.0,0.0.0.0)
EFI stub: Booting Linux Kernel...
EFI stub: EFI_RNG_PROTOCOL unavailable, no randomness supplied
EFI stub: Using DTB from configuration table
EFI stub: Exiting boot services and installing virtual address map...
[    0.000000] Booting Linux on physical CPU 0x0000000000 [0x481fd010]
[    0.000000] Linux version 4.18.0-147.5.1.es8_22.aarch64 (root@aarch64-kojid) (gcc version 8.4.1 20200928 (Red Hat 8.4.1-1) (GCC)) #1 SMP Fri Mar 25 17:55:23 CST 2022
[    0.000000] efi: Getting EFI parameters from FDT:
[    0.000000] efi: EFI v2.70 by EDK II
[    0.000000] efi:  SMBIOS 3.0=0x103f630000  MEMATTR=0x103dded698  ACPI 2.0=0x103bcf0000  MEMRESERVE=0x103c2a3e98 
[    0.000000] ACPI: Early table checksum verification disabled
[    0.000000] ACPI: RSDP 0x000000103BCF0000 000024 (v02 BOCHS )
[    0.000000] ACPI: XSDT 0x000000103BCE0000 00005C (v01 BOCHS  BXPCFACP 00000001      01000013)
[    0.000000] ACPI: FACP 0x000000103BC10000 00010C (v05 BOCHS  BXPCFACP 00000001 BXPC 00000001)
[    0.000000] ACPI: DSDT 0x000000103BC20000 001353 (v02 BOCHS  BXPCDSDT 00000001 BXPC 00000001)
[    0.000000] ACPI: APIC 0x000000103BC00000 000528 (v03 BOCHS  BXPCAPIC 00000001 BXPC 00000001)
[    0.000000] ACPI: GTDT 0x000000103BBF0000 000060 (v02 BOCHS  BXPCGTDT 00000001 BXPC 00000001)
[    0.000000] ACPI: MCFG 0x000000103BBE0000 00003C (v01 BOCHS  BXPCMCFG 00000001 BXPC 00000001)
[    0.000000] ACPI: SPCR 0x000000103BBD0000 000050 (v02 BOCHS  BXPCSPCR 00000001 BXPC 00000001)
[    0.000000] ACPI: IORT 0x000000103BBC0000 00007C (v00 BOCHS  BXPCIORT 00000001 BXPC 00000001)
[    0.000000] ACPI: BGRT 0x000000103BC40000 000038 (v01 INTEL  EDK2     00000002      01000013)
[    0.000000] ACPI: SPCR: console: pl011,mmio,0x9000000,9600
[    0.000000] ACPI: NUMA: Failed to initialise from firmware
[    0.000000] NUMA: Faking a node at [mem 0x0000000000000000-0x000000103fffffff]
[    0.000000] NUMA: NODE_DATA [mem 0x103f7ce600-0x103f7cffff]
[    0.000000] Zone ranges:
[    0.000000]   DMA32    [mem 0x0000000040000000-0x00000000ffffffff]
[    0.000000]   Normal   [mem 0x0000000100000000-0x000000103fffffff]
[    0.000000] Movable zone start for each node
[    0.000000] Early memory node ranges
[    0.000000]   node   0: [mem 0x0000000040000000-0x000000103bd4ffff]
[    0.000000]   node   0: [mem 0x000000103bd50000-0x000000103bffffff]
[    0.000000]   node   0: [mem 0x000000103c000000-0x000000103c01ffff]
[    0.000000]   node   0: [mem 0x000000103c020000-0x000000103c29ffff]
[    0.000000]   node   0: [mem 0x000000103c2a0000-0x000000103f57ffff]
[    0.000000]   node   0: [mem 0x000000103f580000-0x000000103f60ffff]
[    0.000000]   node   0: [mem 0x000000103f610000-0x000000103f61ffff]
[    0.000000]   node   0: [mem 0x000000103f620000-0x000000103f73ffff]
[    0.000000]   node   0: [mem 0x000000103f740000-0x000000103fffffff]
[    0.000000] Zeroed struct page in unavailable ranges: 110 pages
[    0.000000] Initmem setup node 0 [mem 0x0000000040000000-0x000000103fffffff]
[    0.000000] psci: probing for conduit method from ACPI.
[    0.000000] psci: PSCIv1.0 detected in firmware.
[    0.000000] psci: Using standard PSCI v0.2 function IDs
[    0.000000] psci: Trusted OS migration not required
[    0.000000] psci: SMC Calling Convention v1.1
[    0.000000] percpu: Embedded 3 pages/cpu s127832 r8192 d60584 u196608
[    0.000000] Detected VIPT I-cache on CPU0
[    0.000000] ARM_SMCCC_ARCH_WORKAROUND_1 missing from firmware
[    0.000000] Speculative Store Bypass Disable mitigation not required
[    0.000000] CPU features: detected: Hardware dirty bit management
[    0.000000] Built 1 zonelists, mobility grouping on.  Total pages: 1047552
[    0.000000] Policy zone: Normal
[    0.000000] Kernel command line: BOOT_IMAGE=(tftp)/images/centos-aarch64/vmlinuz append ksdevice=bootif lang= sshd=1 console=tty0 transparent_hugepage=never locale=en_US text rd.net.timeout.carrier=30 net.ifnames=1 priority=critical udevrules=FA:16:3E:FB:E7:BA_enp1s0,FA:16:3E:67:9D:68_enp2s0,FA:16:3E:67:32:C8_enp3s0,FA:16:3E:4B:75:A8_enp4s0,FA:16:3E:AB:9B:12_enp5s0,FA:16:3E:37:14:22_enp6s0,FA:16:3E:5E:BA:53_enp7s0,FA:16:3E:70:F5:41_enp8s0 inst.repo=http://10.20.0.2:8880/roller/centos/rollerweb/aarch64/ dhcptimeout=120 netcfg/choose_interface=FA:16:3E:FB:E7:BA iommu.passthrough=1 ks=http://10.20.0.2:8880/cblr/svc/op/ks/system/node-3 fixrtc vram=48M omapfb.vram=0:24M BOOTIF=fa:16:3e:fb:e7:ba
[    0.000000] Specific versions of hardware are certified with Red Hat Enterprise Linux 8. Please see the list of hardware certified with Red Hat Enterprise Linux 8 at https://access.redhat.com/ecosystem.
[    0.000000] software IO TLB: mapped [mem 0xfbff0000-0xffff0000] (64MB)
[    0.000000] Memory: 66847296K/67108864K available (9276K kernel code, 1934K rwdata, 5888K rodata, 4480K init, 9231K bss, 261568K reserved, 0K cma-reserved)
[    0.000000] random: get_random_u64 called from cache_random_seq_create+0x88/0x158 with crng_init=0
[    0.000000] SLUB: HWalign=64, Order=0-3, MinObjects=0, CPUs=16, Nodes=1
[    0.000000] ftrace: allocating 33233 entries in 9 pages
[    0.000000] rcu: Hierarchical RCU implementation.
[    0.000000] rcu: 	RCU restricting CPUs from NR_CPUS=4096 to nr_cpu_ids=16.
[    0.000000] rcu: RCU calculated value of scheduler-enlistment delay is 10 jiffies.
[    0.000000] rcu: Adjusting geometry for rcu_fanout_leaf=16, nr_cpu_ids=16
[    0.000000] NR_IRQS: 64, nr_irqs: 64, preallocated irqs: 0
[    0.000000] GICv3: Distributor has no Range Selector support
[    0.000000] GICv3: no VLPI support, no direct LPI support
[    0.000000] GICv3: CPU0: found redistributor 0 region 0:0x00000000080a0000
[    0.000000] ACPI: SRAT not present
[    0.000000] ITS [mem 0x08080000-0x0809ffff]
[    0.000000] ITS@0x0000000008080000: allocated 8192 Devices @1000370000 (indirect, esz 8, psz 64K, shr 1)
[    0.000000] ITS@0x0000000008080000: allocated 8192 Interrupt Collections @1000380000 (flat, esz 8, psz 64K, shr 1)
[    0.000000] GICv3: using LPI property table @0x0000001000390000
[    0.000000] GICv3: CPU0: using allocated LPI pending table @0x00000010003b0000
[    0.000000] rcu: 	Offload RCU callbacks from CPUs: (none).
[    0.000000] arch_timer: cp15 timer(s) running at 100.00MHz (virt).
[    0.000000] clocksource: arch_sys_counter: mask: 0xffffffffffffff max_cycles: 0x171024e7e0, max_idle_ns: 440795205315 ns
[    0.000001] sched_clock: 56 bits at 100MHz, resolution 10ns, wraps every 4398046511100ns
[    0.000080] Console: colour dummy device 80x25
[    0.000215] console [tty0] enabled
[    0.000575] ACPI: Core revision 20180531
[    0.000719] Calibrating delay loop (skipped), value calculated using timer frequency.. 200.00 BogoMIPS (lpj=1000000)
[    0.000724] pid_max: default: 32768 minimum: 301
[    0.000797] Security Framework initialized
[    0.000802] Yama: becoming mindful.
[    0.000816] SELinux:  Initializing.
[    0.021149] Dentry cache hash table entries: 8388608 (order: 10, 67108864 bytes)
[    0.032339] Inode-cache hash table entries: 4194304 (order: 9, 33554432 bytes)
[    0.032722] Mount-cache hash table entries: 131072 (order: 4, 1048576 bytes)
[    0.033228] Mountpoint-cache hash table entries: 131072 (order: 4, 1048576 bytes)
[    0.034286] ACPI PPTT: No PPTT table found, cpu topology may be inaccurate
[    0.034601] ASID allocator initialised with 32768 entries
[    0.034671] rcu: Hierarchical SRCU implementation.
[    0.035138] Platform MSI: ITS@0x8080000 domain created
[    0.035160] PCI/MSI: ITS@0x8080000 domain created
[    0.035169] Remapping and enabling EFI services.
[    0.035945] smp: Bringing up secondary CPUs ...
[    0.080443] Detected VIPT I-cache on CPU1
[    0.080464] GICv3: CPU1: found redistributor 1 region 0:0x00000000080c0000
[    0.080555] GICv3: CPU1: using allocated LPI pending table @0x00000010003c0000
[    0.080591] CPU1: Booted secondary processor 0x0000000001 [0x481fd010]
[    0.125221] Detected VIPT I-cache on CPU2
[    0.125243] GICv3: CPU2: found redistributor 2 region 0:0x00000000080e0000
[    0.125334] GICv3: CPU2: using allocated LPI pending table @0x00000010003d0000
[    0.125364] CPU2: Booted secondary processor 0x0000000002 [0x481fd010]
[    0.170017] Detected VIPT I-cache on CPU3
[    0.170038] GICv3: CPU3: found redistributor 3 region 0:0x0000000008100000
[    0.170258] GICv3: CPU3: using allocated LPI pending table @0x00000010003e0000
[    0.170287] CPU3: Booted secondary processor 0x0000000003 [0x481fd010]
[    0.214805] Detected VIPT I-cache on CPU4
[    0.214832] GICv3: CPU4: found redistributor 4 region 0:0x0000000008120000
[    0.214934] GICv3: CPU4: using allocated LPI pending table @0x00000010003f0000
[    0.214970] CPU4: Booted secondary processor 0x0000000004 [0x481fd010]
[    0.259397] Detected VIPT I-cache on CPU5
[    0.259426] GICv3: CPU5: found redistributor 5 region 0:0x0000000008140000
[    0.259524] GICv3: CPU5: using allocated LPI pending table @0x0000001000400000
[    0.259558] CPU5: Booted secondary processor 0x0000000005 [0x481fd010]
[    0.304161] Detected VIPT I-cache on CPU6
[    0.304188] GICv3: CPU6: found redistributor 6 region 0:0x0000000008160000
[    0.304284] GICv3: CPU6: using allocated LPI pending table @0x0000001000410000
[    0.304309] CPU6: Booted secondary processor 0x0000000006 [0x481fd010]
[    0.349521] Detected VIPT I-cache on CPU7
[    0.349551] GICv3: CPU7: found redistributor 7 region 0:0x0000000008180000
[    0.349639] GICv3: CPU7: using allocated LPI pending table @0x0000001000420000
[    0.349675] CPU7: Booted secondary processor 0x0000000007 [0x481fd010]
[    0.394396] Detected VIPT I-cache on CPU8
[    0.394431] GICv3: CPU8: found redistributor 8 region 0:0x00000000081a0000
[    0.394527] GICv3: CPU8: using allocated LPI pending table @0x0000001000430000
[    0.394558] CPU8: Booted secondary processor 0x0000000008 [0x481fd010]
[    0.439607] Detected VIPT I-cache on CPU9
[    0.439640] GICv3: CPU9: found redistributor 9 region 0:0x00000000081c0000
[    0.439737] GICv3: CPU9: using allocated LPI pending table @0x0000001000440000
[    0.439767] CPU9: Booted secondary processor 0x0000000009 [0x481fd010]
[    0.485467] Detected VIPT I-cache on CPU10
[    0.485501] GICv3: CPU10: found redistributor a region 0:0x00000000081e0000
[    0.485589] GICv3: CPU10: using allocated LPI pending table @0x0000001000450000
[    0.485626] CPU10: Booted secondary processor 0x000000000a [0x481fd010]
[    0.530891] Detected VIPT I-cache on CPU11
[    0.530928] GICv3: CPU11: found redistributor b region 0:0x0000000008200000
[    0.531026] GICv3: CPU11: using allocated LPI pending table @0x0000001000460000
[    0.531058] CPU11: Booted secondary processor 0x000000000b [0x481fd010]
[    0.576352] Detected VIPT I-cache on CPU12
[    0.576390] GICv3: CPU12: found redistributor c region 0:0x0000000008220000
[    0.576484] GICv3: CPU12: using allocated LPI pending table @0x0000001000470000
[    0.576512] CPU12: Booted secondary processor 0x000000000c [0x481fd010]
[    0.621881] Detected VIPT I-cache on CPU13
[    0.621920] GICv3: CPU13: found redistributor d region 0:0x0000000008240000
[    0.622021] GICv3: CPU13: using allocated LPI pending table @0x0000001000480000
[    0.622054] CPU13: Booted secondary processor 0x000000000d [0x481fd010]
[    0.667711] Detected VIPT I-cache on CPU14
[    0.667767] GICv3: CPU14: found redistributor e region 0:0x0000000008260000
[    0.667869] GICv3: CPU14: using allocated LPI pending table @0x0000001000490000
[    0.667903] CPU14: Booted secondary processor 0x000000000e [0x481fd010]
[    0.713825] Detected VIPT I-cache on CPU15
[    0.713865] GICv3: CPU15: found redistributor f region 0:0x0000000008280000
[    0.713959] GICv3: CPU15: using allocated LPI pending table @0x00000010004a0000
[    0.713990] CPU15: Booted secondary processor 0x000000000f [0x481fd010]
[    0.714087] smp: Brought up 1 node, 16 CPUs
[    0.714178] SMP: Total of 16 processors activated.
[    0.714183] CPU features: detected: GIC system register CPU interface
[    0.714185] CPU features: detected: Privileged Access Never
[    0.714187] CPU features: detected: LSE atomic instructions
[    0.714189] CPU features: detected: User Access Override
[    0.714191] CPU features: detected: Data cache clean to Point of Persistence
[    0.714193] CPU features: detected: RAS Extension Support
[    0.714196] CPU features: detected: Common not Private translations
[    0.728360] CPU: All CPU(s) started at EL1
[    0.728431] alternatives: patching kernel code
[    0.732528] devtmpfs: initialized
[    0.733475] clocksource: jiffies: mask: 0xffffffff max_cycles: 0xffffffff, max_idle_ns: 19112604462750000 ns
[    0.733510] futex hash table entries: 4096 (order: 2, 262144 bytes)
[    0.733901] pinctrl core: initialized pinctrl subsystem
[    0.734105] SMBIOS 3.0.0 present.
[    0.734111] DMI: QEMU KVM Virtual Machine, BIOS 0.0.0 02/06/2015
[    0.734552] NET: Registered protocol family 16
[    0.735179] audit: initializing netlink subsys (disabled)
[    0.735503] audit: type=2000 audit(0.730:1): state=initialized audit_enabled=0 res=1
[    0.735802] cpuidle: using governor menu
[    0.736006] vdso: 2 pages (1 code @ (____ptrval____), 1 data @ (____ptrval____))
[    0.736011] hw-breakpoint: found 6 breakpoint and 4 watchpoint registers.
[    0.736820] DMA: preallocated 256 KiB pool for atomic allocations
[    0.736974] ACPI: bus type PCI registered
[    0.736985] acpiphp: ACPI Hot Plug PCI Controller Driver version: 0.5
[    0.737085] Serial: AMBA PL011 UART driver
[    0.739861] HugeTLB registered 2.00 MiB page size, pre-allocated 0 pages
[    0.739868] HugeTLB registered 512 MiB page size, pre-allocated 0 pages
[    0.740477] cryptd: max_cpu_qlen set to 1000
[    0.741377] ACPI: Added _OSI(Module Device)
[    0.741382] ACPI: Added _OSI(Processor Device)
[    0.741384] ACPI: Added _OSI(3.0 _SCP Extensions)
[    0.741386] ACPI: Added _OSI(Processor Aggregator Device)
[    0.741389] ACPI: Added _OSI(Linux-Dell-Video)
[    0.741996] ACPI: 1 ACPI AML tables successfully acquired and loaded
[    0.742828] ACPI: Interpreter enabled
[    0.742832] ACPI: Using GIC for interrupt routing
[    0.742847] ACPI: MCFG table detected, 1 entries
[    0.745389] ARMH0011:00: ttyAMA0 at MMIO 0x9000000 (irq = 4, base_baud = 0) is a SBSA
[    0.843907] console [ttyAMA0] enabled
[    0.845301] ACPI: PCI Root Bridge [PCI0] (domain 0000 [bus 00-0f])
[    0.845847] acpi PNP0A08:00: _OSC: OS supports [ExtendedConfig ASPM ClockPM Segments MSI]
[    0.846617] acpi PNP0A08:00: _OSC: platform does not support [LTR]
[    0.847180] acpi PNP0A08:00: _OSC: OS now controls [PCIeHotplug PME AER PCIeCapability]
[    0.848052] acpi PNP0A08:00: ECAM area [mem 0x3f000000-0x3fffffff] reserved by PNP0C02:00
[    0.848741] acpi PNP0A08:00: ECAM at [mem 0x3f000000-0x3fffffff] for [bus 00-0f]
[    0.849347] Remapped I/O 0x000000003eff0000 to [io  0x0000-0xffff window]
[    0.849986] PCI host bridge to bus 0000:00
[    0.850334] pci_bus 0000:00: root bus resource [mem 0x10000000-0x3efeffff window]
[    0.850940] pci_bus 0000:00: root bus resource [io  0x0000-0xffff window]
[    0.851486] pci_bus 0000:00: root bus resource [mem 0x8000000000-0xffffffffff window]
[    0.852102] pci_bus 0000:00: root bus resource [bus 00-0f]
[    1.024367] pci 0000:00:01.0: BAR 14: assigned [mem 0x10000000-0x101fffff]
[    1.027052] pci 0000:00:01.0: BAR 15: assigned [mem 0x8000000000-0x80001fffff 64bit pref]
[    1.027720] pci 0000:00:01.1: BAR 14: assigned [mem 0x10200000-0x103fffff]
[    1.028270] pci 0000:00:01.1: BAR 15: assigned [mem 0x8000200000-0x80003fffff 64bit pref]
[    1.028919] pci 0000:00:01.2: BAR 14: assigned [mem 0x10400000-0x105fffff]
[    1.029459] pci 0000:00:01.2: BAR 15: assigned [mem 0x8000400000-0x80005fffff 64bit pref]
[    1.030132] pci 0000:00:01.3: BAR 14: assigned [mem 0x10600000-0x107fffff]
[    1.030672] pci 0000:00:01.3: BAR 15: assigned [mem 0x8000600000-0x80007fffff 64bit pref]
[    1.031335] pci 0000:00:01.4: BAR 14: assigned [mem 0x10800000-0x109fffff]
[    1.031883] pci 0000:00:01.4: BAR 15: assigned [mem 0x8000800000-0x80009fffff 64bit pref]
[    1.032527] pci 0000:00:01.5: BAR 14: assigned [mem 0x10a00000-0x10bfffff]
[    1.033076] pci 0000:00:01.5: BAR 15: assigned [mem 0x8000a00000-0x8000bfffff 64bit pref]
[    1.033718] pci 0000:00:01.6: BAR 14: assigned [mem 0x10c00000-0x10dfffff]
[    1.034265] pci 0000:00:01.6: BAR 15: assigned [mem 0x8000c00000-0x8000dfffff 64bit pref]
[    1.034914] pci 0000:00:01.7: BAR 14: assigned [mem 0x10e00000-0x10ffffff]
[    1.035455] pci 0000:00:01.7: BAR 15: assigned [mem 0x8000e00000-0x8000ffffff 64bit pref]
[    1.036099] pci 0000:00:02.0: BAR 14: assigned [mem 0x11000000-0x111fffff]
[    1.036646] pci 0000:00:02.0: BAR 15: assigned [mem 0x8001000000-0x80011fffff 64bit pref]
[    1.037323] pci 0000:00:02.1: BAR 14: assigned [mem 0x11200000-0x113fffff]
[    1.037882] pci 0000:00:02.1: BAR 15: assigned [mem 0x8001200000-0x80013fffff 64bit pref]
[    1.038539] pci 0000:00:02.2: BAR 14: assigned [mem 0x11400000-0x115fffff]
[    1.039081] pci 0000:00:02.2: BAR 15: assigned [mem 0x8001400000-0x80015fffff 64bit pref]
[    1.039726] pci 0000:00:02.3: BAR 14: assigned [mem 0x11600000-0x117fffff]
[    1.040280] pci 0000:00:02.3: BAR 15: assigned [mem 0x8001600000-0x80017fffff 64bit pref]
[    1.040942] pci 0000:00:02.4: BAR 14: assigned [mem 0x11800000-0x119fffff]
[    1.041507] pci 0000:00:02.4: BAR 15: assigned [mem 0x8001800000-0x80019fffff 64bit pref]
[    1.042197] pci 0000:00:02.5: BAR 14: assigned [mem 0x11a00000-0x11bfffff]
[    1.042749] pci 0000:00:02.5: BAR 15: assigned [mem 0x8001a00000-0x8001bfffff 64bit pref]
[    1.043392] pci 0000:00:02.6: BAR 14: assigned [mem 0x11c00000-0x11dfffff]
[    1.043934] pci 0000:00:02.6: BAR 15: assigned [mem 0x8001c00000-0x8001dfffff 64bit pref]
[    1.044580] pci 0000:00:01.0: BAR 0: assigned [mem 0x11e00000-0x11e00fff]
[    1.045854] pci 0000:00:01.0: BAR 13: assigned [io  0x1000-0x1fff]
[    1.046364] pci 0000:00:01.1: BAR 0: assigned [mem 0x11e01000-0x11e01fff]
[    1.047572] pci 0000:00:01.1: BAR 13: assigned [io  0x2000-0x2fff]
[    1.048080] pci 0000:00:01.2: BAR 0: assigned [mem 0x11e02000-0x11e02fff]
[    1.049240] pci 0000:00:01.2: BAR 13: assigned [io  0x3000-0x3fff]
[    1.049737] pci 0000:00:01.3: BAR 0: assigned [mem 0x11e03000-0x11e03fff]
[    1.050954] pci 0000:00:01.3: BAR 13: assigned [io  0x4000-0x4fff]
[    1.051449] pci 0000:00:01.4: BAR 0: assigned [mem 0x11e04000-0x11e04fff]
[    1.052673] pci 0000:00:01.4: BAR 13: assigned [io  0x5000-0x5fff]
[    1.053175] pci 0000:00:01.5: BAR 0: assigned [mem 0x11e05000-0x11e05fff]
[    1.054349] pci 0000:00:01.5: BAR 13: assigned [io  0x6000-0x6fff]
[    1.054851] pci 0000:00:01.6: BAR 0: assigned [mem 0x11e06000-0x11e06fff]
[    1.056047] pci 0000:00:01.6: BAR 13: assigned [io  0x7000-0x7fff]
[    1.056572] pci 0000:00:01.7: BAR 0: assigned [mem 0x11e07000-0x11e07fff]
[    1.057756] pci 0000:00:01.7: BAR 13: assigned [io  0x8000-0x8fff]
[    1.058266] pci 0000:00:02.0: BAR 0: assigned [mem 0x11e08000-0x11e08fff]
[    1.059517] pci 0000:00:02.0: BAR 13: assigned [io  0x9000-0x9fff]
[    1.060011] pci 0000:00:02.1: BAR 0: assigned [mem 0x11e09000-0x11e09fff]
[    1.061176] pci 0000:00:02.1: BAR 13: assigned [io  0xa000-0xafff]
[    1.061678] pci 0000:00:02.2: BAR 0: assigned [mem 0x11e0a000-0x11e0afff]
[    1.062924] pci 0000:00:02.2: BAR 13: assigned [io  0xb000-0xbfff]
[    1.063421] pci 0000:00:02.3: BAR 0: assigned [mem 0x11e0b000-0x11e0bfff]
[    1.064596] pci 0000:00:02.3: BAR 13: assigned [io  0xc000-0xcfff]
[    1.065092] pci 0000:00:02.4: BAR 0: assigned [mem 0x11e0c000-0x11e0cfff]
[    1.068211] pci 0000:00:02.4: BAR 13: assigned [io  0xd000-0xdfff]
[    1.068718] pci 0000:00:02.5: BAR 0: assigned [mem 0x11e0d000-0x11e0dfff]
[    1.069996] pci 0000:00:02.5: BAR 13: assigned [io  0xe000-0xefff]
[    1.070518] pci 0000:00:02.6: BAR 0: assigned [mem 0x11e0e000-0x11e0efff]
[    1.071663] pci 0000:00:02.6: BAR 13: assigned [io  0xf000-0xffff]
[    1.072173] pci 0000:01:00.0: BAR 4: assigned [mem 0x8000000000-0x8000003fff 64bit pref]
[    1.074148] pci 0000:01:00.0: BAR 1: assigned [mem 0x10000000-0x10000fff]
[    1.075344] pci 0000:00:01.0: PCI bridge to [bus 01]
[    1.075759] pci 0000:00:01.0:   bridge window [io  0x1000-0x1fff]
[    1.077352] pci 0000:00:01.0:   bridge window [mem 0x10000000-0x101fffff]
[    1.079895] pci 0000:00:01.0:   bridge window [mem 0x8000000000-0x80001fffff 64bit pref]
[    1.082064] pci 0000:02:00.0: BAR 4: assigned [mem 0x8000200000-0x8000203fff 64bit pref]
[    1.082749] pci 0000:02:00.0: BAR 1: assigned [mem 0x10200000-0x10200fff]
[    1.083294] pci 0000:00:01.1: PCI bridge to [bus 02]
[    1.083691] pci 0000:00:01.1:   bridge window [io  0x2000-0x2fff]
[    1.085197] pci 0000:00:01.1:   bridge window [mem 0x10200000-0x103fffff]
[    1.086522] pci 0000:00:01.1:   bridge window [mem 0x8000200000-0x80003fffff 64bit pref]
[    1.088618] pci 0000:03:00.0: BAR 4: assigned [mem 0x8000400000-0x8000403fff 64bit pref]
[    1.089303] pci 0000:03:00.0: BAR 1: assigned [mem 0x10400000-0x10400fff]
[    1.089860] pci 0000:00:01.2: PCI bridge to [bus 03]
[    1.090278] pci 0000:00:01.2:   bridge window [io  0x3000-0x3fff]
[    1.091847] pci 0000:00:01.2:   bridge window [mem 0x10400000-0x105fffff]
[    1.093088] pci 0000:00:01.2:   bridge window [mem 0x8000400000-0x80005fffff 64bit pref]
[    1.095231] pci 0000:04:00.0: BAR 4: assigned [mem 0x8000600000-0x8000603fff 64bit pref]
[    1.095919] pci 0000:04:00.0: BAR 1: assigned [mem 0x10600000-0x10600fff]
[    1.096477] pci 0000:00:01.3: PCI bridge to [bus 04]
[    1.096879] pci 0000:00:01.3:   bridge window [io  0x4000-0x4fff]
[    1.098383] pci 0000:00:01.3:   bridge window [mem 0x10600000-0x107fffff]
[    1.099684] pci 0000:00:01.3:   bridge window [mem 0x8000600000-0x80007fffff 64bit pref]
[    1.103918] pci 0000:05:00.0: BAR 4: assigned [mem 0x8000800000-0x8000803fff 64bit pref]
[    1.104605] pci 0000:05:00.0: BAR 1: assigned [mem 0x10800000-0x10800fff]
[    1.105151] pci 0000:00:01.4: PCI bridge to [bus 05]
[    1.105557] pci 0000:00:01.4:   bridge window [io  0x5000-0x5fff]
[    1.107143] pci 0000:00:01.4:   bridge window [mem 0x10800000-0x109fffff]
[    1.108518] pci 0000:00:01.4:   bridge window [mem 0x8000800000-0x80009fffff 64bit pref]
[    1.110524] pci 0000:06:00.0: BAR 4: assigned [mem 0x8000a00000-0x8000a03fff 64bit pref]
[    1.111218] pci 0000:06:00.0: BAR 1: assigned [mem 0x10a00000-0x10a00fff]
[    1.111763] pci 0000:00:01.5: PCI bridge to [bus 06]
[    1.112164] pci 0000:00:01.5:   bridge window [io  0x6000-0x6fff]
[    1.113682] pci 0000:00:01.5:   bridge window [mem 0x10a00000-0x10bfffff]
[    1.114993] pci 0000:00:01.5:   bridge window [mem 0x8000a00000-0x8000bfffff 64bit pref]
[    1.117081] pci 0000:07:00.0: BAR 4: assigned [mem 0x8000c00000-0x8000c03fff 64bit pref]
[    1.117777] pci 0000:07:00.0: BAR 1: assigned [mem 0x10c00000-0x10c00fff]
[    1.118321] pci 0000:00:01.6: PCI bridge to [bus 07]
[    1.118750] pci 0000:00:01.6:   bridge window [io  0x7000-0x7fff]
[    1.120371] pci 0000:00:01.6:   bridge window [mem 0x10c00000-0x10dfffff]
[    1.121609] pci 0000:00:01.6:   bridge window [mem 0x8000c00000-0x8000dfffff 64bit pref]
[    1.126113] pci 0000:08:00.0: BAR 4: assigned [mem 0x8000e00000-0x8000e03fff 64bit pref]
[    1.126799] pci 0000:08:00.0: BAR 1: assigned [mem 0x10e00000-0x10e00fff]
[    1.127342] pci 0000:00:01.7: PCI bridge to [bus 08]
[    1.127748] pci 0000:00:01.7:   bridge window [io  0x8000-0x8fff]
[    1.129364] pci 0000:00:01.7:   bridge window [mem 0x10e00000-0x10ffffff]
[    1.130707] pci 0000:00:01.7:   bridge window [mem 0x8000e00000-0x8000ffffff 64bit pref]
[    1.132735] pci 0000:09:00.0: BAR 4: assigned [mem 0x8001000000-0x8001003fff 64bit pref]
[    1.134599] pci 0000:09:00.0: BAR 1: assigned [mem 0x11000000-0x11000fff]
[    1.136860] pci 0000:00:02.0: PCI bridge to [bus 09]
[    1.137270] pci 0000:00:02.0:   bridge window [io  0x9000-0x9fff]
[    1.138811] pci 0000:00:02.0:   bridge window [mem 0x11000000-0x111fffff]
[    1.140151] pci 0000:00:02.0:   bridge window [mem 0x8001000000-0x80011fffff 64bit pref]
[    1.142310] pci 0000:0a:00.0: BAR 0: assigned [mem 0x11200000-0x11203fff 64bit]
[    1.142923] pci 0000:00:02.1: PCI bridge to [bus 0a]
[    1.143319] pci 0000:00:02.1:   bridge window [io  0xa000-0xafff]
[    1.144796] pci 0000:00:02.1:   bridge window [mem 0x11200000-0x113fffff]
[    1.146106] pci 0000:00:02.1:   bridge window [mem 0x8001200000-0x80013fffff 64bit pref]
[    1.149755] pci 0000:0b:00.0: BAR 0: assigned [mem 0x11400000-0x11403fff 64bit]
[    1.150405] pci 0000:00:02.2: PCI bridge to [bus 0b]
[    1.150803] pci 0000:00:02.2:   bridge window [io  0xb000-0xbfff]
[    1.152454] pci 0000:00:02.2:   bridge window [mem 0x11400000-0x115fffff]
[    1.153639] pci 0000:00:02.2:   bridge window [mem 0x8001400000-0x80015fffff 64bit pref]
[    1.155742] pci 0000:0c:00.0: BAR 4: assigned [mem 0x8001600000-0x8001603fff 64bit pref]
[    1.156411] pci 0000:00:02.3: PCI bridge to [bus 0c]
[    1.156807] pci 0000:00:02.3:   bridge window [io  0xc000-0xcfff]
[    1.158368] pci 0000:00:02.3:   bridge window [mem 0x11600000-0x117fffff]
[    1.159679] pci 0000:00:02.3:   bridge window [mem 0x8001600000-0x80017fffff 64bit pref]
[    1.161807] pci 0000:0d:00.0: BAR 4: assigned [mem 0x8001800000-0x8001803fff 64bit pref]
[    1.163682] pci 0000:0d:00.0: BAR 1: assigned [mem 0x11800000-0x11800fff]
[    1.164832] pci 0000:00:02.4: PCI bridge to [bus 0d]
[    1.165235] pci 0000:00:02.4:   bridge window [io  0xd000-0xdfff]
[    1.166756] pci 0000:00:02.4:   bridge window [mem 0x11800000-0x119fffff]
[    1.170684] pci 0000:00:02.4:   bridge window [mem 0x8001800000-0x80019fffff 64bit pref]
[    1.172790] pci 0000:00:02.5: PCI bridge to [bus 0e]
[    1.173197] pci 0000:00:02.5:   bridge window [io  0xe000-0xefff]
[    1.174775] pci 0000:00:02.5:   bridge window [mem 0x11a00000-0x11bfffff]
[    1.176063] pci 0000:00:02.5:   bridge window [mem 0x8001a00000-0x8001bfffff 64bit pref]
[    1.178211] pci 0000:00:02.6: PCI bridge to [bus 0f]
[    1.178621] pci 0000:00:02.6:   bridge window [io  0xf000-0xffff]
[    1.180395] pci 0000:00:02.6:   bridge window [mem 0x11c00000-0x11dfffff]
[    1.183088] pci 0000:00:02.6:   bridge window [mem 0x8001c00000-0x8001dfffff 64bit pref]
[    1.185257] ACPI: PCI Interrupt Link [GSI0] (IRQs *35)
[    1.185701] ACPI: PCI Interrupt Link [GSI1] (IRQs *36)
[    1.186118] ACPI: PCI Interrupt Link [GSI2] (IRQs *37)
[    1.186538] ACPI: PCI Interrupt Link [GSI3] (IRQs *38)
[    1.187716] vgaarb: loaded
[    1.188170] SCSI subsystem initialized
[    1.188701] ACPI: bus type USB registered
[    1.189070] usbcore: registered new interface driver usbfs
[    1.189517] usbcore: registered new interface driver hub
[    1.190075] usbcore: registered new device driver usb
[    1.190549] pps_core: LinuxPPS API ver. 1 registered
[    1.190949] pps_core: Software ver. 5.3.6 - Copyright 2005-2007 Rodolfo Giometti <giometti@linux.it>
[    1.191671] PTP clock support registered
[    1.192269] EDAC MC: Ver: 3.0.0
[    1.192929] Registered efivars operations
[    1.194801] NetLabel: Initializing
[    1.195086] NetLabel:  domain hash size = 128
[    1.195427] NetLabel:  protocols = UNLABELED CIPSOv4 CALIPSO
[    1.195907] NetLabel:  unlabeled traffic allowed by default
[    1.196683] clocksource: Switched to clocksource arch_sys_counter
[    1.212305] VFS: Disk quotas dquot_6.6.0
[    1.212725] VFS: Dquot-cache hash table entries: 8192 (order 0, 65536 bytes)
[    1.213456] pnp: PnP ACPI init
[    1.213899] system 00:00: [mem 0x3f000000-0x3fffffff] could not be reserved
[    1.214469] pnp: PnP ACPI: found 1 devices
[    1.216533] NET: Registered protocol family 2
[    1.217259] tcp_listen_portaddr_hash hash table entries: 32768 (order: 3, 524288 bytes)
[    1.218106] TCP established hash table entries: 524288 (order: 6, 4194304 bytes)
[    1.219832] TCP bind hash table entries: 65536 (order: 4, 1048576 bytes)
[    1.220677] TCP: Hash tables configured (established 524288 bind 65536)
[    1.221341] UDP hash table entries: 32768 (order: 4, 1048576 bytes)
[    1.222112] UDP-Lite hash table entries: 32768 (order: 4, 1048576 bytes)
[    1.222956] NET: Registered protocol family 1
[    1.223315] NET: Registered protocol family 44
[    1.224669] pci 0000:0a:00.0: enabling device (0000 -> 0002)
[    1.226346] pci 0000:0b:00.0: enabling device (0000 -> 0002)
[    1.227511] Unpacking initramfs...
[    2.174641] Freeing initrd memory: 77120K
[    2.177849] hw perfevents: enabled with armv8_pmuv3_0 PMU driver, 13 counters available
[    2.178501] kvm [1]: HYP mode not available
[    2.213100] Initialise system trusted keyrings
[    2.213646] workingset: timestamp_bits=43 max_order=20 bucket_order=0
[    2.215536] zbud: loaded
[    2.216066] pstore: using deflate compression
[    2.251625] NET: Registered protocol family 38
[    2.252029] Key type asymmetric registered
[    2.252355] Asymmetric key parser 'x509' registered
[    2.252756] Block layer SCSI generic (bsg) driver version 0.4 loaded (major 246)
[    2.253540] io scheduler mq-deadline registered
[    2.253905] io scheduler kyber registered
[    2.254297] io scheduler bfq registered
[    2.254965] atomic64_test: passed
[    2.255743] pl061_gpio ARMH0061:00: PL061 GPIO chip @0x0000000009030000 registered
[    2.256602] PCI Interrupt Link [GSI1] enabled at IRQ 36
[    2.259084] pcieport 0000:00:01.0: Signaling PME with IRQ 44
[    2.259724] aer 0000:00:01.0:pcie002: AER enabled with IRQ 44
[    2.260242] pciehp 0000:00:01.0:pcie004: Slot #0 AttnBtn+ PwrCtrl+ MRL- AttnInd+ PwrInd+ HotPlug+ Surprise+ Interlock+ NoCompl- LLActRep-
[    2.263177] pcieport 0000:00:01.1: Signaling PME with IRQ 45
[    2.263803] aer 0000:00:01.1:pcie002: AER enabled with IRQ 45
[    2.264298] pciehp 0000:00:01.1:pcie004: Slot #0 AttnBtn+ PwrCtrl+ MRL- AttnInd+ PwrInd+ HotPlug+ Surprise+ Interlock+ NoCompl- LLActRep-
[    2.267273] pcieport 0000:00:01.2: Signaling PME with IRQ 46
[    2.267918] aer 0000:00:01.2:pcie002: AER enabled with IRQ 46
[    2.268417] pciehp 0000:00:01.2:pcie004: Slot #0 AttnBtn+ PwrCtrl+ MRL- AttnInd+ PwrInd+ HotPlug+ Surprise+ Interlock+ NoCompl- LLActRep-
[    2.271361] pcieport 0000:00:01.3: Signaling PME with IRQ 47
[    2.272001] aer 0000:00:01.3:pcie002: AER enabled with IRQ 47
[    2.272492] pciehp 0000:00:01.3:pcie004: Slot #0 AttnBtn+ PwrCtrl+ MRL- AttnInd+ PwrInd+ HotPlug+ Surprise+ Interlock+ NoCompl- LLActRep-
[    2.275465] pcieport 0000:00:01.4: Signaling PME with IRQ 48
[    2.276105] aer 0000:00:01.4:pcie002: AER enabled with IRQ 48
[    2.276595] pciehp 0000:00:01.4:pcie004: Slot #0 AttnBtn+ PwrCtrl+ MRL- AttnInd+ PwrInd+ HotPlug+ Surprise+ Interlock+ NoCompl- LLActRep-
[    2.281436] pcieport 0000:00:01.5: Signaling PME with IRQ 49
[    2.282080] aer 0000:00:01.5:pcie002: AER enabled with IRQ 49
[    2.282570] pciehp 0000:00:01.5:pcie004: Slot #0 AttnBtn+ PwrCtrl+ MRL- AttnInd+ PwrInd+ HotPlug+ Surprise+ Interlock+ NoCompl- LLActRep-
[    2.285525] pcieport 0000:00:01.6: Signaling PME with IRQ 50
[    2.286156] aer 0000:00:01.6:pcie002: AER enabled with IRQ 50
[    2.286645] pciehp 0000:00:01.6:pcie004: Slot #0 AttnBtn+ PwrCtrl+ MRL- AttnInd+ PwrInd+ HotPlug+ Surprise+ Interlock+ NoCompl- LLActRep-
[    2.289729] pcieport 0000:00:01.7: Signaling PME with IRQ 51
[    2.290400] aer 0000:00:01.7:pcie002: AER enabled with IRQ 51
[    2.290893] pciehp 0000:00:01.7:pcie004: Slot #0 AttnBtn+ PwrCtrl+ MRL- AttnInd+ PwrInd+ HotPlug+ Surprise+ Interlock+ NoCompl- LLActRep-
[    2.292190] PCI Interrupt Link [GSI2] enabled at IRQ 37
[    2.294196] pcieport 0000:00:02.0: Signaling PME with IRQ 52
[    2.294826] aer 0000:00:02.0:pcie002: AER enabled with IRQ 52
[    2.295320] pciehp 0000:00:02.0:pcie004: Slot #0 AttnBtn+ PwrCtrl+ MRL- AttnInd+ PwrInd+ HotPlug+ Surprise+ Interlock+ NoCompl- LLActRep-
[    2.297341] pcieport 0000:00:02.1: Signaling PME with IRQ 53
[    2.298006] aer 0000:00:02.1:pcie002: AER enabled with IRQ 53
[    2.298500] pciehp 0000:00:02.1:pcie004: Slot #0 AttnBtn+ PwrCtrl+ MRL- AttnInd+ PwrInd+ HotPlug+ Surprise+ Interlock+ NoCompl- LLActRep-
[    2.300480] pcieport 0000:00:02.2: Signaling PME with IRQ 54
[    2.301143] aer 0000:00:02.2:pcie002: AER enabled with IRQ 54
[    2.301636] pciehp 0000:00:02.2:pcie004: Slot #0 AttnBtn+ PwrCtrl+ MRL- AttnInd+ PwrInd+ HotPlug+ Surprise+ Interlock+ NoCompl- LLActRep-
[    2.304535] pcieport 0000:00:02.3: Signaling PME with IRQ 55
[    2.305163] aer 0000:00:02.3:pcie002: AER enabled with IRQ 55
[    2.305658] pciehp 0000:00:02.3:pcie004: Slot #0 AttnBtn+ PwrCtrl+ MRL- AttnInd+ PwrInd+ HotPlug+ Surprise+ Interlock+ NoCompl- LLActRep-
[    2.308753] pcieport 0000:00:02.4: Signaling PME with IRQ 56
[    2.309409] aer 0000:00:02.4:pcie002: AER enabled with IRQ 56
[    2.311871] pciehp 0000:00:02.4:pcie004: Slot #0 AttnBtn+ PwrCtrl+ MRL- AttnInd+ PwrInd+ HotPlug+ Surprise+ Interlock+ NoCompl- LLActRep-
[    2.315014] pcieport 0000:00:02.5: Signaling PME with IRQ 57
[    2.315644] aer 0000:00:02.5:pcie002: AER enabled with IRQ 57
[    2.316139] pciehp 0000:00:02.5:pcie004: Slot #0 AttnBtn+ PwrCtrl+ MRL- AttnInd+ PwrInd+ HotPlug+ Surprise+ Interlock+ NoCompl- LLActRep-
[    2.317364] pciehp 0000:00:02.5:pcie004: Slot(0-13): Link Up
[    2.319412] pcieport 0000:00:02.6: Signaling PME with IRQ 58
[    2.320140] aer 0000:00:02.6:pcie002: AER enabled with IRQ 58
[    2.320636] pciehp 0000:00:02.6:pcie004: Slot #0 AttnBtn+ PwrCtrl+ MRL- AttnInd+ PwrInd+ HotPlug+ Surprise+ Interlock+ NoCompl- LLActRep-
[    2.321778] pciehp 0000:00:02.6:pcie004: Slot(0-14): Link Up
[    2.322130] input: Power Button as /devices/LNXSYSTM:00/LNXSYBUS:00/PNP0C0C:00/input/input0
[    2.323033] ACPI: Power Button [PWRB]
[    2.324766] virtio-pci 0000:02:00.0: enabling device (0000 -> 0002)
[    2.327514] virtio-pci 0000:03:00.0: enabling device (0000 -> 0002)
[    2.330280] virtio-pci 0000:04:00.0: enabling device (0000 -> 0002)
[    2.333172] virtio-pci 0000:05:00.0: enabling device (0000 -> 0002)
[    2.335981] virtio-pci 0000:06:00.0: enabling device (0000 -> 0002)
[    2.338855] virtio-pci 0000:07:00.0: enabling device (0000 -> 0002)
[    2.341739] virtio-pci 0000:08:00.0: enabling device (0000 -> 0002)
[    2.348160] virtio-pci 0000:0c:00.0: enabling device (0000 -> 0002)
[    2.351348] Serial: 8250/16550 driver, 4 ports, IRQ sharing enabled
[    2.352667] ACPI PPTT: No PPTT table found, cache topology may be inaccurate
[    2.353244] ACPI PPTT: No PPTT table found, cache topology may be inaccurate
[    2.353795] cacheinfo: Unable to detect cache hierarchy for CPU 0
[    2.354298] rdac: device handler registered
[    2.354776] hp_sw: device handler registered
[    2.355122] emc: device handler registered
[    2.355676] alua: device handler registered
[    2.356194] libphy: Fixed MDIO Bus: probed
[    2.356706] ehci_hcd: USB 2.0 'Enhanced' Host Controller (EHCI) Driver
[    2.357230] ehci-pci: EHCI PCI platform driver
[    2.357605] ohci_hcd: USB 1.1 'Open' Host Controller (OHCI) Driver
[    2.358103] ohci-pci: OHCI PCI platform driver
[    2.358473] uhci_hcd: USB Universal Host Controller Interface driver
[    2.359994] xhci_hcd 0000:0a:00.0: xHCI Host Controller
[    2.377973] xhci_hcd 0000:0a:00.0: new USB bus registered, assigned bus number 1
[    2.378792] xhci_hcd 0000:0a:00.0: hcc params 0x00087001 hci version 0x100 quirks 0x0000000000000010
[    2.380416] usb usb1: New USB device found, idVendor=1d6b, idProduct=0002, bcdDevice= 4.18
[    2.381094] usb usb1: New USB device strings: Mfr=3, Product=2, SerialNumber=1
[    2.381664] usb usb1: Product: xHCI Host Controller
[    2.382047] usb usb1: Manufacturer: Linux 4.18.0-147.5.1.es8_22.aarch64 xhci-hcd
[    2.382627] usb usb1: SerialNumber: 0000:0a:00.0
[    2.383142] hub 1-0:1.0: USB hub found
[    2.383465] hub 1-0:1.0: 4 ports detected
[    2.383978] xhci_hcd 0000:0a:00.0: xHCI Host Controller
[    2.384437] xhci_hcd 0000:0a:00.0: new USB bus registered, assigned bus number 2
[    2.385040] xhci_hcd 0000:0a:00.0: Host supports USB 3.0  SuperSpeed
[    2.385564] usb usb2: We don't know the algorithms for LPM for this host, disabling LPM.
[    2.386220] usb usb2: New USB device found, idVendor=1d6b, idProduct=0003, bcdDevice= 4.18
[    2.386924] usb usb2: New USB device strings: Mfr=3, Product=2, SerialNumber=1
[    2.387500] usb usb2: Product: xHCI Host Controller
[    2.387887] usb usb2: Manufacturer: Linux 4.18.0-147.5.1.es8_22.aarch64 xhci-hcd
[    2.388471] usb usb2: SerialNumber: 0000:0a:00.0
[    2.388966] hub 2-0:1.0: USB hub found
[    2.389287] hub 2-0:1.0: 4 ports detected
[    2.390571] xhci_hcd 0000:0b:00.0: xHCI Host Controller
[    2.391062] xhci_hcd 0000:0b:00.0: new USB bus registered, assigned bus number 3
[    2.391821] xhci_hcd 0000:0b:00.0: hcc params 0x00087001 hci version 0x100 quirks 0x0000000000000010
[    2.393360] usb usb3: New USB device found, idVendor=1d6b, idProduct=0002, bcdDevice= 4.18
[    2.394014] usb usb3: New USB device strings: Mfr=3, Product=2, SerialNumber=1
[    2.394580] usb usb3: Product: xHCI Host Controller
[    2.394962] usb usb3: Manufacturer: Linux 4.18.0-147.5.1.es8_22.aarch64 xhci-hcd
[    2.395539] usb usb3: SerialNumber: 0000:0b:00.0
[    2.396001] hub 3-0:1.0: USB hub found
[    2.396319] hub 3-0:1.0: 4 ports detected
[    2.396814] xhci_hcd 0000:0b:00.0: xHCI Host Controller
[    2.397265] xhci_hcd 0000:0b:00.0: new USB bus registered, assigned bus number 4
[    2.397872] xhci_hcd 0000:0b:00.0: Host supports USB 3.0  SuperSpeed
[    2.398398] usb usb4: We don't know the algorithms for LPM for this host, disabling LPM.
[    2.399050] usb usb4: New USB device found, idVendor=1d6b, idProduct=0003, bcdDevice= 4.18
[    2.399718] usb usb4: New USB device strings: Mfr=3, Product=2, SerialNumber=1
[    2.400307] usb usb4: Product: xHCI Host Controller
[    2.400702] usb usb4: Manufacturer: Linux 4.18.0-147.5.1.es8_22.aarch64 xhci-hcd
[    2.401290] usb usb4: SerialNumber: 0000:0b:00.0
[    2.401758] hub 4-0:1.0: USB hub found
[    2.402079] hub 4-0:1.0: 4 ports detected
[    2.402630] usbcore: registered new interface driver usbserial_generic
[    2.403158] usbserial: USB Serial support registered for generic
[    2.403710] mousedev: PS/2 mouse device common for all mice
[    2.404344] rtc-efi rtc-efi: rtc core: registered rtc-efi as rtc0
[    2.407135] hidraw: raw HID events driver (C) Jiri Kosina
[    2.407626] usbcore: registered new interface driver usbhid
[    2.408079] usbhid: USB HID core driver
[    2.408660] drop_monitor: Initializing network drop monitor service
[    2.409246] Initializing XFRM netlink socket
[    2.409750] NET: Registered protocol family 10
[    2.410614] Segment Routing with IPv6
[    2.410938] NET: Registered protocol family 17
[    2.411387] mpls_gso: MPLS GSO support
[    2.411955] registered taskstats version 1
[    2.412305] Loading compiled-in X.509 certificates
[    2.440879] Loaded X.509 cert 'Red Hat Enterprise Linux kernel signing key: 460ffd8dbffbbf966cf8713b2df9ee57251da63f'
[    2.442038] Loaded X.509 cert 'Red Hat Enterprise Linux Driver Update Program (key 3): bf57f3e87362bc7229d9f465321773dfd1f77a80'
[    2.443247] Loaded X.509 cert 'Red Hat Enterprise Linux kpatch signing key: 4d38fd864ebe18c5f0b72e3852e2014c3a676fc8'
[    2.444426] zswap: loaded using pool lzo/zbud
[    2.447464] Key type big_key registered
[    2.448044] rtc-efi rtc-efi: setting system clock to 2022-05-24 21:03:38 UTC (1653426218)
[    2.451321] Freeing unused kernel memory: 4480K
[    2.534062] systemd[1]: systemd 239 (239-45.es8_5) running in system mode. (+PAM +AUDIT +SELINUX +IMA -APPARMOR +SMACK +SYSVINIT +UTMP +LIBCRYPTSETUP +GCRYPT +GNUTLS +ACL +XZ +LZ4 +SECCOMP +BLKID +ELFUTILS +KMOD +IDN2 -IDN +PCRE2 default-hierarchy=legacy)
[    2.535921] systemd[1]: Detected virtualization kvm.
[    2.536323] systemd[1]: Detected architecture arm64.
[    2.536733] systemd[1]: Running in initial RAM disk.
[    2.542281] systemd[1]: No hostname configured.
[    2.568643] systemd[1]: Set hostname to <localhost>.
[    2.569142] systemd[1]: Initializing machine ID from KVM UUID.
[    2.583827] random: ln: uninitialized urandom read (6 bytes read)
[    2.584235] random: ln: uninitialized urandom read (6 bytes read)
[    2.616915] random: systemd: uninitialized urandom read (16 bytes read)
[    2.617670] systemd[1]: Listening on udev Kernel Socket.
[    2.618304] systemd[1]: Reached target Swap.
[    2.618806] systemd[1]: Listening on Journal Socket.
[    2.644945] Loading iSCSI transport class v2.0-870.
[    2.756711] usb 1-1: new high-speed USB device number 2 using xhci_hcd
[    2.947053] usb 1-1: New USB device found, idVendor=0627, idProduct=0001, bcdDevice= 0.00
[    2.947757] usb 1-1: New USB device strings: Mfr=1, Product=3, SerialNumber=5
[    2.948316] usb 1-1: Product: QEMU USB Tablet
[    2.948657] usb 1-1: Manufacturer: QEMU
[    2.948959] usb 1-1: SerialNumber: 42
[    2.949999] input: QEMU QEMU USB Tablet as /devices/pci0000:00/0000:00:02.1/0000:0a:00.0/usb1/1-1/1-1:1.0/0003:0627:0001.0001/input/input1
[    2.951144] hid-generic 0003:0627:0001.0001: input,hidraw0: USB HID v0.01 Mouse [QEMU QEMU USB Tablet] on usb-0000:0a:00.0-1/input0
[    2.951943] iscsi: registered transport (tcp)
[    2.974218] iscsi: registered transport (qla4xxx)
[    2.974707] QLogic iSCSI HBA Driver
[    2.981251] libcxgbi:libcxgbi_init_module: Chelsio iSCSI driver library libcxgbi v0.9.1-ko (Apr. 2015)
[    3.009791] Chelsio T4-T6 iSCSI Driver cxgb4i v0.9.5-ko (Apr. 2015)
[    3.010371] iscsi: registered transport (cxgb4i)
[    3.018999] iscsi: registered transport (be2iscsi)
[    3.019414] In beiscsi_module_init, tt=(____ptrval____)
[    3.096708] usb 1-2: new high-speed USB device number 3 using xhci_hcd
[    3.287006] usb 1-2: New USB device found, idVendor=0627, idProduct=0001, bcdDevice= 0.00
[    3.287684] usb 1-2: New USB device strings: Mfr=1, Product=4, SerialNumber=5
[    3.288267] usb 1-2: Product: QEMU USB Keyboard
[    3.288622] usb 1-2: Manufacturer: QEMU
[    3.288932] usb 1-2: SerialNumber: 42
[    3.290089] input: QEMU QEMU USB Keyboard as /devices/pci0000:00/0000:00:02.1/0000:0a:00.0/usb1/1-2/1-2:1.0/0003:0627:0001.0002/input/input2
[    3.356885] hid-generic 0003:0627:0001.0002: input,hidraw1: USB HID v1.11 Keyboard [QEMU QEMU USB Keyboard] on usb-0000:0a:00.0-2/input0
[    4.099301] squashfs: version 4.0 (2009/01/31) Phillip Lougher
[    4.336696] raid6: int64x1  gen()  6591 MB/s
[    4.343797] random: crng init done
[    4.344123] random: 7 urandom warning(s) missed due to ratelimiting
[    4.506696] raid6: int64x1  xor()  1975 MB/s
[    4.676695] raid6: int64x2  gen()  8282 MB/s
[    4.846699] raid6: int64x2  xor()  2426 MB/s
[    5.016689] raid6: int64x4  gen()  7580 MB/s
[    5.186702] raid6: int64x4  xor()  2198 MB/s
[    5.356688] raid6: int64x8  gen()  4326 MB/s
[    5.526696] raid6: int64x8  xor()  1573 MB/s
[    5.698063] raid6: neonx1   gen() 11178 MB/s
[    5.776741] pciehp 0000:00:02.5:pcie004: Failed to check link status
[    5.777386] pciehp 0000:00:02.6:pcie004: Failed to check link status
[    5.866686] raid6: neonx1   xor()  3166 MB/s
[    6.056696] raid6: neonx2   gen() 17353 MB/s
[    6.226690] raid6: neonx2   xor()  4151 MB/s
[    6.396684] raid6: neonx4   gen() 16891 MB/s
[    6.566693] raid6: neonx4   xor()  4304 MB/s
[    6.736685] raid6: neonx8   gen() 10445 MB/s
[    6.906689] raid6: neonx8   xor()  4345 MB/s
[    6.907048] raid6: using algorithm neonx2 gen() 17353 MB/s
[    6.907475] raid6: .... xor() 4151 MB/s, rmw enabled
[    6.907879] raid6: using neon recovery algorithm
[    6.911472] async_tx: api initialized (async)
[    6.941773] xor: measuring software checksum speed
[    7.036688]    8regs     : 10316.800 MB/sec
[    7.136691]    8regs_prefetch: 10144.000 MB/sec
[    7.236687]    32regs    : 12870.400 MB/sec
[    7.336695]    32regs_prefetch: 13324.800 MB/sec
[    7.337101] xor: using function: 32regs_prefetch (13324.800 MB/sec)
[    7.423807] device-mapper: uevent: version 1.0.3
[    7.424713] device-mapper: ioctl: 4.39.0-ioctl (2018-04-03) initialised: dm-devel@redhat.com
[    7.484516] device-mapper: multipath round-robin: version 1.2.0 loaded
[    7.586115] RPC: Registered named UNIX socket transport module.
[    7.586694] RPC: Registered udp transport module.
[    7.587067] RPC: Registered tcp transport module.
[    7.587435] RPC: Registered tcp NFSv4.1 backchannel transport module.
[    7.938386] scsi host0: Virtio SCSI HBA
[    7.942259] scsi 0:0:0:0: Direct-Access     QEMU     QEMU HARDDISK    2.5+ PQ: 0 ANSI: 5
[[0;32m  OK  [0m] Started Show Plymouth Boot Screen.[    7.945417] scsi 0:0:0:3: Direct-Access     QEMU     QEMU HARDDISK    2.5+ PQ: 0 ANSI: 5

[[0;32m  OK  [0m] Started Forward Password Requests to Plymouth Directory Watch.
[[0;32m  OK  [0m] Reached target Paths.[    7.950546] scsi 0:0:0:2: Direct-Access     QEMU     QEMU HARDDISK    2.5+ PQ: 0 ANSI: 5

[[0;32m  OK  [0m] Reached target Local Encrypted Volumes.[    7.952770] 8021q: 802.1Q VLAN Support v1.8

[    7.954199] scsi 0:0:0:1: Direct-Access     QEMU     QEMU HARDDISK    2.5+ PQ: 0 ANSI: 5
[    7.972286] virtio_net virtio5 enp6s0: renamed from eth5
[    7.995209] sd 0:0:0:0: Power-on or device reset occurred
[    7.995888] sd 0:0:0:3: Power-on or device reset occurred
[    7.996428] sd 0:0:0:2: Power-on or device reset occurred
[    7.997018] sd 0:0:0:1: Power-on or device reset occurred
[    7.998122] sd 0:0:0:0: [sda] 629145600 512-byte logical blocks: (322 GB/300 GiB)
[    7.998155] sd 0:0:0:2: [sdc] 629145600 512-byte logical blocks: (322 GB/300 GiB)
[    7.998175] sd 0:0:0:3: [sdb] 629145600 512-byte logical blocks: (322 GB/300 GiB)
[    7.998212] sd 0:0:0:1: [sdd] 629145600 512-byte logical blocks: (322 GB/300 GiB)
[    7.998222] sd 0:0:0:3: [sdb] Write Protect is off
[    7.998303] sd 0:0:0:1: [sdd] Write Protect is off
[    7.998374] sd 0:0:0:3: [sdb] Write cache: enabled, read cache: enabled, doesn't support DPO or FUA
[    7.998408] sd 0:0:0:1: [sdd] Write cache: enabled, read cache: enabled, doesn't support DPO or FUA
[    7.999599] [drm] pci: virtio-gpu-pci detected at 0000:0d:00.0
[    7.999603] [drm] virgl 3d acceleration not supported by host
[    7.999965] sd 0:0:0:0: [sda] Write Protect is off
[    8.000425] [TTM] Zone  kernel: Available graphics memory: 33464448 kiB
[    8.000426] [TTM] Zone   dma32: Available graphics memory: 2097152 kiB
[    8.000427] [TTM] Initializing pool allocator
[    8.000431] [TTM] Initializing DMA pool allocator
[    8.000536] [drm] number of scanouts: 1
[    8.000540] [drm] number of cap sets: 0
[    8.000838] sd 0:0:0:2: [sdc] Write Protect is off
[    8.000900] [drm] Initialized virtio_gpu 0.1.0 0 for virtio10 on minor 0
[    8.002104] sd 0:0:0:3: [sdb] Attached SCSI disk
[    8.002362] sd 0:0:0:1: [sdd] Attached SCSI disk
[    8.003698] sd 0:0:0:0: [sda] Write cache: enabled, read cache: enabled, doesn't support DPO or FUA
[    8.004141] sd 0:0:0:2: [sdc] Write cache: enabled, read cache: enabled, doesn't support DPO or FUA
[    8.007888]  sda:
[    8.010534] sd 0:0:0:2: [sdc] Attached SCSI disk
[    8.015464] Console: switching to colour frame buffer device 128x48
[    8.016365] sd 0:0:0:0: [sda] Attached SCSI disk
[    8.022274] virtio_gpu virtio10: fb0: DRM emulated frame buffer device
[    8.025628] sd 0:0:0:0: Attached scsi generic sg0 type 0
[    8.026255] sd 0:0:0:3: Attached scsi generic sg1 type 0
[    8.027001] sd 0:0:0:2: Attached scsi generic sg2 type 0
[    8.027557] sd 0:0:0:1: Attached scsi generic sg3 type 0
[    8.077503] virtio_net virtio3 enp4s0: renamed from eth3
[[0;32m  OK  [0m] Started cancel waiting for multipath siblings of sdc.
[[0;32m  OK  [0m] Started cancel waiting for multipath siblings of sdb.
[[0;32m  OK  [0m] Started cancel waiting for multipath siblings of sda.
[[0;32m  OK  [0m] Started cancel waiting for multipath siblings of sdd.
[    8.156893] virtio_net virtio6 enp7s0: renamed from eth6
[    8.366927] virtio_net virtio4 enp5s0: renamed from eth4
[    8.647012] virtio_net virtio1 enp2s0: renamed from eth1
[    8.807215] virtio_net virtio0 enp1s0: renamed from eth0
[    8.877131] virtio_net virtio2 enp3s0: renamed from eth2
[    9.056931] virtio_net virtio7 enp8s0: renamed from eth7
[[0;32m  OK  [0m] Started udev Wait for Complete Device Initialization.
         Starting Device-Mapper Multipath Device Controller...
[[0;32m  OK  [0m] Started Device-Mapper Multipath Device Controller.
[[0;32m  OK  [0m] Reached target Local File Systems (Pre).
[[0;32m  OK  [0m] Reached target Local File Systems.
         Starting Create Volatile Files and Directories...
         Starting Open-iSCSI...
[[0;32m  OK  [0m] Started Open-iSCSI.
         Starting dracut initqueue hook...
[[0;32m  OK  [0m] Started Create Volatile Files and Directories.
[[0;32m  OK  [0m] Reached target System Initialization.
[[0;32m  OK  [0m] Reached target Basic System.
[    9.460689] IPv6: ADDRCONF(NETDEV_UP): enp1s0: link is not ready
[    9.463514] 8021q: adding VLAN 0 to HW filter on device enp1s0
[    9.477805] IPv6: ADDRCONF(NETDEV_UP): enp2s0: link is not ready
[    9.480414] 8021q: adding VLAN 0 to HW filter on device enp2s0
[    9.483633] IPv6: ADDRCONF(NETDEV_UP): enp3s0: link is not ready
[    9.486035] 8021q: adding VLAN 0 to HW filter on device enp3s0
[    9.489288] IPv6: ADDRCONF(NETDEV_UP): enp4s0: link is not ready
[    9.491736] 8021q: adding VLAN 0 to HW filter on device enp4s0
[    9.494926] IPv6: ADDRCONF(NETDEV_UP): enp5s0: link is not ready
[    9.497324] 8021q: adding VLAN 0 to HW filter on device enp5s0
[    9.500604] IPv6: ADDRCONF(NETDEV_UP): enp6s0: link is not ready
[    9.503070] 8021q: adding VLAN 0 to HW filter on device enp6s0
[    9.506254] IPv6: ADDRCONF(NETDEV_UP): enp7s0: link is not ready
[    9.508716] 8021q: adding VLAN 0 to HW filter on device enp7s0
[    9.511973] IPv6: ADDRCONF(NETDEV_UP): enp8s0: link is not ready
[    9.514444] 8021q: adding VLAN 0 to HW filter on device enp8s0
[    9.715991] dracut-initqueue[1062]: Warning: can't find installer main image path in .treeinfo
[    9.726599] dracut-initqueue[1062]:   % Total    % Received % Xferd  Average Speed   Time    Time     Time  Current
[    9.726779] dracut-initqueue[1062]:                                  Dload  Upload   Total   Spent    Left  Speed
[    9.727449] dracut-initqueue[1062]:   0     0    0     0    0     0      0      0 --:--:-- --:--:-- --:--:--     0  0     0    0     0    0     0      0      0 --:--:-- --:--:-- --:--:--     0
[    9.727544] dracut-initqueue[1062]: curl: (22) The requested URL returned error: 404 Not Found
[    9.730394] dracut-initqueue[1062]: Warning: Downloading 'http://10.20.0.2:8880/roller/centos/rollerweb/aarch64//images/install.img' failed!
[    9.743736] dracut-initqueue[1062]:   % Total    % Received % Xferd  Average Speed   Time    Time     Time  Current
[    9.743802] dracut-initqueue[1062]:                                  Dload  Upload   Total   Spent    Left  Speed
[   10.799381] dracut-initqueue[1062]:   0     0    0     0    0     0      0      0 --:--:-- --:--:-- --:--:--     0 12  681M   12 82.0M    0     0   262M      0  0:00:02 --:--:--  0:00:02  262M100  681M  100  681M    0     0   644M      0  0:00:01  0:00:01 --:--:--  645M
[   10.977309] loop: module loaded
[   12.138321] dracut-initqueue[1062]:   % Total    % Received % Xferd  Average Speed   Time    Time     Time  Current
[   12.138531] dracut-initqueue[1062]:                                  Dload  Upload   Total   Spent    Left  Speed
[   12.421139] dracut-initqueue[1062]:   0     0    0     0    0     0      0      0 --:--:-- --:--:-- --:--:--     0100 19685  100 19685    0     0  69804      0 --:--:-- --:--:-- --:--:-- 69804
[[0;32m  OK  [0m] Started dracut initqueue hook.
[[0;32m  OK  [0m] Reached target Remote File Systems (Pre).
[[0;32m  OK  [0m] Reached target Remote File Systems.
         Starting dracut pre-mount hook...
[[0;32m  OK  [0m] Started dracut pre-mount hook.
[[0;32m  OK  [0m] Reached target Initrd Root File System.
         Starting Reload Configuration from the Real Root...
         Stopping Device-Mapper Multipath Device Controller...
[[0;32m  OK  [0m] Started Reload Configuration from the Real Root.
         Starting dracut mount hook...
[[0;32m  OK  [0m] Reached target Initrd File Systems.
[[0;32m  OK  [0m] Reached target Initrd Default Target.
[[0;32m  OK  [0m] Stopped Device-Mapper Multipath Device Controller.
[   13.439735] EXT4-fs (dm-0): mounted filesystem with ordered data mode. Opts: (null)
[[0;32m  OK  [0m] Started dracut mount hook.
         Starting dracut pre-pivot and cleanup hook...
[   14.112801] dracut-pre-pivot[1504]: ln: failed to create symbolic link '/sysroot/boot/initramfs-4.18.0-147.5.1.es8_22.aarch64.img': File exists
[[0;32m  OK  [0m] Started dracut pre-pivot and cleanup hook.
         Starting Cleaning Up and Shutting Down Daemons...
         Starting Plymouth switch root service...
         Starting Setup Virtual Console...
[[0;32m  OK  [0m] Stopped cancel waiting for multipath siblings of sdc.
[[0;32m  OK  [0m] Stopped target Timers.
[[0;32m  OK  [0m] Stopped dracut pre-pivot and cleanup hook.
[[0;32m  OK  [0m] Stopped cancel waiting for multipath siblings of sda.
[[0;32m  OK  [0m] Stopped cancel waiting for multipath siblings of sdd.
[[0;32m  OK  [0m] Stopped target Initrd Default Target.
[[0;32m  OK  [0m] Stopped target Remote File Systems.
[[0;32m  OK  [0m] Stopped target Remote File Systems (Pre).
[[0;32m  OK  [0m] Stopped target Initrd Root Device.
[[0;32m  OK  [0m] Stopped cancel waiting for multipath siblings of sdb.
[[0;32m  OK  [0m] Stopped dracut mount hook.
[[0;32m  OK  [0m] Stopped dracut pre-mount hook.
[[0;32m  OK  [0m] Stopped dracut initqueue hook.
         Stopping Open-iSCSI...
[[0;32m  OK  [0m] Stopped target Basic System.
[[0;32m  OK  [0m] Stopped target System Initialization.
[[0;32m  OK  [0m] Stopped Apply Kernel Variables.
[[0;32m  OK  [0m] Stopped udev Wait for Complete Device Initialization.
[[0;32m  OK  [0m] Stopped target Local Encrypted Volumes.
[[0;32m  OK  [0m] Stopped target Swap.
[[0;32m  OK  [0m] Stopped Create Volatile Files and Directories.
[[0;32m  OK  [0m] Stopped target Local File Systems.
[[0;32m  OK  [0m] Stopped target Local File Systems (Pre).
[[0;32m  OK  [0m] Stopped target Sockets.
[[0;32m  OK  [0m] Stopped target Paths.
[[0;32m  OK  [0m] Stopped udev Coldplug all Devices.
[[0;32m  OK  [0m] Stopped dracut pre-trigger hook.
         Stopping udev Kernel Device Manager...
[[0;32m  OK  [0m] Stopped target Slices.
[[0;32m  OK  [0m] Stopped Open-iSCSI.
[[0;32m  OK  [0m] Started Setup Virtual Console.
         Stopping iSCSI UserSpace I/O driver...
[[0;32m  OK  [0m] Closed Open-iSCSI iscsid Socket.
[[0;32m  OK  [0m] Stopped iSCSI UserSpace I/O driver.
[[0;32m  OK  [0m] Started Cleaning Up and Shutting Down Daemons.
[[0;32m  OK  [0m] Closed Open-iSCSI iscsiuio Socket.
[[0;32m  OK  [0m] Started Plymouth switch root service.
[[0;32m  OK  [0m] Stopped udev Kernel Device Manager.
[[0;32m  OK  [0m] Stopped Create Static Device Nodes in /dev.
[[0;32m  OK  [0m] Stopped Create list of required static device nodes for the current kernel.
[[0;32m  OK  [0m] Stopped dracut pre-udev hook.
[[0;32m  OK  [0m] Stopped dracut cmdline hook.
         Stopping Hardware RNG Entropy Gatherer Daemon...
[[0;32m  OK  [0m] Closed udev Control Socket.
[[0;32m  OK  [0m] Closed udev Kernel Socket.
         Starting Cleanup udevd DB...
[[0;32m  OK  [0m] Stopped Hardware RNG Entropy Gatherer Daemon.
[[0;32m  OK  [0m] Started Cleanup udevd DB.
[[0;32m  OK  [0m] Reached target Switch Root.
         Starting Switch Root...
[   15.298967] systemd: 23 output lines suppressed due to ratelimiting
[   15.831567] SELinux:  Disabled at runtime.
[   15.956742] audit: type=1404 audit(1653426232.000:2): enforcing=0 old_enforcing=0 auid=4294967295 ses=4294967295 enabled=0 old-enabled=1 lsm=selinux res=1
[   16.064261] systemd[1]: systemd 239 (239-45.es8_5) running in system mode. (+PAM +AUDIT +SELINUX +IMA -APPARMOR +SMACK +SYSVINIT +UTMP +LIBCRYPTSETUP +GCRYPT +GNUTLS +ACL +XZ +LZ4 +SECCOMP +BLKID +ELFUTILS +KMOD +IDN2 -IDN +PCRE2 default-hierarchy=legacy)
[   16.068411] systemd[1]: Detected virtualization kvm.
[   16.070235] systemd[1]: Detected architecture arm64.

Welcome to [0;32mEasyStack Cloud Linux 8.4 (AltArch)[0m!

[   16.090296] systemd[1]: Initializing machine ID from KVM UUID.
[   16.902678] systemd[1]: systemd-journald.service: Succeeded.
[   16.905204] systemd[1]: initrd-switch-root.service: Succeeded.
[   16.907481] systemd[1]: Stopped Switch Root.
[[0;32m  OK  [0m] Stopped Switch Root.[   16.909571] systemd[1]: systemd-journald.service: Service has no hold-off time (RestartSec=0), scheduling restart.

[   16.911954] systemd[1]: systemd-journald.service: Scheduled restart job, restart counter is at 1.
[   16.914427] systemd[1]: Stopped Journal Service.
[[0;32m  OK  [0m] Stopped Journal Service.
         Starting Journal Service...
[[0;32m  OK  [0m] Stopped Plymouth switch root service.
[[0;32m  OK  [0m] Listening on RPCbind Server Activation Socket.
[[0;32m  OK  [0m] Created slice system-anaconda\x2dtmux.slice.
[[0;32m  OK  [0m] Listening on Device-mapper event daemon FIFOs.
[[0;32m  OK  [0m] Listening on multipathd control socket.
         Mounting POSIX Message Queue File System...
[[0;32m  OK  [0m] Listening on udev Control Socket.
         Starting Create list of required static device nodes for the current kernel...
[[0;32m  OK  [0m] Set up automount Arbitrary Executable File Formats File System Automount Point.
         Starting Remount Root and Kernel File Systems...
[[0;32m  OK  [0m] Listening on udev Kernel Socket.
         Starting udev Coldplug all Devices...
[[0;32m  OK  [0m] Created slice system-sshd\x2dkeygen.slice.
[[0;32m  OK  [0m] Listening on Process Core Dump Socket.
         Mounting Huge Pages File System...
         Starting Apply Kernel Variables...
         Mounting Kernel Debug File System...
[[0;32m  OK  [0m] Created slice User and Session Slice.
[[0;32m  OK  [0m] Reached target Slices.
[[0;32m  OK  [0m] Stopped target Switch Root.
[[0;32m  OK  [0m] Stopped target Initrd Root File System.
         Starting Setup Virtual Console...
[[0;32m  OK  [0m] Stopped target Initrd File Systems.
[[0;32m  OK  [0m] Reached target Swap.
[[0;32m  OK  [0m] Reached target RPC Port Mapper.
[[0;32m  OK  [0m] Reached target Paths.
[[0;32m  OK  [0m] Reached target Local Encrypted Volumes.
[[0;32m  OK  [0m] Listening on LVM2 poll daemon socket.
[[0;32m  OK  [0m] Listening on initctl Compatibility Named Pipe.
         Mounting Temporary Directory (/tmp)...
[[0;32m  OK  [0m] Started Journal Service.
[[0;32m  OK  [0m] Mounted POSIX Message Queue File System.
[[0;32m  OK  [0m] Started Create list of required static device nodes for the current kernel.
[[0;32m  OK  [0m] Started Remount Root and Kernel File Systems.
[[0;32m  OK  [0m] Mounted Huge Pages File System.
[[0;32m  OK  [0m] Started Apply Kernel Variables.
[[0;32m  OK  [0m] Mounted Kernel Debug File System.
[[0;32m  OK  [0m] Mounted Temporary Directory (/tmp).
         Starting Rebuild Hardware Database...
         Starting Load/Save Random Seed...
         Starting Create System Users...
         Starting Flush Journal to Persistent Storage...
[[0;32m  OK  [0m] Started udev Coldplug all Devices.
         Starting udev Wait for Complete Device Initialization...
[[0;32m  OK  [0m] Started Load/Save Random Seed.
[[0;32m  OK  [0m] Started Create System Users.
         Starting Create Static Device Nodes in /dev...
[[0;32m  OK  [0m] Started Flush Journal to Persistent Storage.
[[0;32m  OK  [0m] Started Create Static Device Nodes in /dev.
[[0;32m  OK  [0m] Started Setup Virtual Console.
[[0;32m  OK  [0m] Started Rebuild Hardware Database.
         Starting udev Kernel Device Manager...
[[0;32m  OK  [0m] Started udev Kernel Device Manager.
[[0;32m  OK  [0m] Started udev Wait for Complete Device Initialization.
         Starting Device-Mapper Multipath Device Controller...
[[0;32m  OK  [0m] Started Device-Mapper Multipath Device Controller.
[[0;32m  OK  [0m] Reached target Local File Systems (Pre).
[[0;32m  OK  [0m] Reached target Local File Systems.
         Starting Create Volatile Files and Directories...
         Starting Restore /run/initramfs on shutdown...
         Starting Tell Plymouth To Write Out Runtime Data...
         Starting Rebuild Journal Catalog...
[[0;32m  OK  [0m] Started Restore /run/initramfs on shutdown.
[[0;32m  OK  [0m] Started Create Volatile Files and Directories.
         Starting Update UTMP about System Boot/Shutdown...
[[0;32m  OK  [0m] Started Rebuild Journal Catalog.
         Starting Update is Completed...
[[0;32m  OK  [0m] Started Update UTMP about System Boot/Shutdown.
[[0;32m  OK  [0m] Started Update is Completed.
[[0;32m  OK  [0m] Started Tell Plymouth To Write Out Runtime Data.
[[0;32m  OK  [0m] Reached target System Initialization.
[[0;32m  OK  [0m] Started daily update of the root trust anchor for DNSSEC.
[[0;32m  OK  [0m] Reached target Timers.
[[0;32m  OK  [0m] Listening on Open-iSCSI iscsid Socket.
[[0;32m  OK  [0m] Listening on Open-iSCSI iscsiuio Socket.
[[0;32m  OK  [0m] Listening on D-Bus System Message Bus Socket.
[[0;32m  OK  [0m] Reached target Sockets.
[[0;32m  OK  [0m] Reached target Basic System.
         Starting Hardware RNG Entropy Gatherer Wake threshold service...
         Starting Anaconda NetworkManager configuration...
         Starting Hold until boot process finishes up...
         Starting pre-anaconda logging service...
         Starting Terminate Plymouth Boot Screen...
         Starting Service enabling compressing RAM with zRam...
         Starting Login Service...
         Starting OpenSSH ecdsa Server Key Generation...
         Starting OpenSSH rsa Server Key Generation...
         Starting OpenSSH ed25519 Server Key Generation...
[[0;32m  OK  [0m] Started Hardware RNG Entropy Gatherer Wake threshold service.
[[0;32m  OK  [0m] Started Anaconda NetworkManager configuration.
[[0;32m  OK  [0m] Started Service enabling compressing RAM with zRam.
         Starting Network Manager...
[[0;32m  OK  [0m] Started Hardware RNG Entropy Gatherer Daemon.
         Unmounting /mnt/sysimage/boot...
         Unmounting /mnt/sysimage/sys...
[[0;32m  OK  [0m] Removed slice system-anaconda\x2dtmux.slice.
         Stopping Anaconda...
[[0;32m  OK  [0m] Stopped Hardware RNG Entropy Gatherer Wake threshold service.
[[0;32m  OK  [0m] Unmounted /mnt/sysroot/sys.
[[0;32m  OK  [0m] Unmounted /mnt/sysroot/boot.
[[0;32m  OK  [0m] Stopped Authorization Manager.
[[0;32m  OK  [0m] Stopped Anaconda.
[[0;32m  OK  [0m] Stopped target Anaconda System Services.
[[0;32m  OK  [0m] Stopped Terminate Plymouth Boot Screen.
         Stopping OpenSSH server daemon...
         Stopping Login Service...
[[0;32m  OK  [0m] Stopped Hold until boot process finishes up.
         Stopping System Logging Service...
         Stopping RHSM dbus service...
         Stopping Service enabling compressing RAM with zRam...
[[0;32m  OK  [0m] Stopped OpenSSH server daemon.
[[0;32m  OK  [0m] Stopped RHSM dbus service.
[[0;32m  OK  [0m] Stopped Disk Manager.
[[0;32m  OK  [0m] Started Show Plymouth Reboot Screen.
[[0;32m  OK  [0m] Stopped target sshd-keygen.target.
[[0;32m  OK  [0m] Stopped Service enabling compressing RAM with zRam.
[[0;32m  OK  [0m] Unmounted RPC Pipe File System.
[[0;32m  OK  [0m] Unmounted /mnt/sysroot/sys/firmware/efi/efivars.
[[0;32m  OK  [0m] Unmounted /mnt/sysimage/dev/pts.
[[0;32m  OK  [0m] Unmounted /mnt/sysroot/run.
[[0;32m  OK  [0m] Unmounted /mnt/sysroot/dev/shm.
         Unmounting /mnt/sysroot/dev...
         Unmounting /mnt/sysimage/dev...
[[0;32m  OK  [0m] Unmounted /mnt/sysimage/dev.
[[0;32m  OK  [0m] Unmounted /mnt/sysroot/boot/efi.
[[0;32m  OK  [0m] Unmounted /mnt/sysimage/proc.
[[0;32m  OK  [0m] Unmounted /mnt/sysimage/sys.
[[0;32m  OK  [0m] Unmounted /mnt/sysimage/boot.
         Unmounting /mnt/sysimage...
[[0;32m  OK  [0m] Unmounted /mnt/sysroot/dev.
         Unmounting /mnt/sysroot...
[[0;32m  OK  [0m] Unmounted /mnt/sysroot.
[[0;32m  OK  [0m] Stopped LVM event activation on device 8:4.
[[0;32m  OK  [0m] Stopped LVM event activation on device 8:5.
[[0;32m  OK  [0m] Removed slice system-lvm2\x2dpvscan.slice.
[[0;32m  OK  [0m] Unmounted /mnt/sysimage.
[[0;32m  OK  [0m] Stopped target Local File Systems (Pre).
         Stopping Device-Mapper Multipath Device Controller...
[[0;32m  OK  [0m] Stopped Device-Mapper Multipath Device Controller.
[[0;32m  OK  [0m] Stopped Login Service.
[[0;32m  OK  [0m] Stopped System Logging Service.
[[0;32m  OK  [0m] Stopped target Network is Online.
[[0;32m  OK  [0m] Stopped target Network.
         Stopping Network Name Resolution...
[[0;32m  OK  [0m] Stopped Network Manager Wait Online.
         Stopping Network Manager...
[[0;32m  OK  [0m] Stopped Network Name Resolution.
[[0;32m  OK  [0m] Stopped Network Manager.
         Stopping D-Bus System Message Bus...
[[0;32m  OK  [0m] Stopped D-Bus System Message Bus.
[[0;32m  OK  [0m] Stopped NTP client/server.
[[0;32m  OK  [0m] Stopped target Basic System.
[[0;32m  OK  [0m] Stopped Forward Password Requests to Plymouth Directory Watch.
[[0;32m  OK  [0m] Stopped target Slices.
[[0;32m  OK  [0m] Removed slice User and Session Slice.
[[0;32m  OK  [0m] Stopped target Sockets.
[[0;32m  OK  [0m] Closed D-Bus System Message Bus Socket.
[[0;32m  OK  [0m] Closed Open-iSCSI iscsid Socket.
[[0;32m  OK  [0m] Closed Open-iSCSI iscsiuio Socket.
[[0;32m  OK  [0m] Stopped target Paths.
         Unmounting Temporary Directory (/tmp)...
[[0;32m  OK  [0m] Stopped target System Initialization.
         Stopping Update UTMP about System Boot/Shutdown...
[[0;32m  OK  [0m] Stopped Create Static Device Nodes in /dev.
         Stopping Load/Save Random Seed...
[[0;32m  OK  [0m] Stopped Update is Completed.
[[0;32m  OK  [0m] Stopped Apply Kernel Variables.
[[0;32m  OK  [0m] Stopped target Local Encrypted Volumes.
[[0;32m  OK  [0m] Stopped Rebuild Hardware Database.
[[0;32m  OK  [0m] Stopped Rebuild Journal Catalog.
[[0;32m  OK  [0m] Stopped Update UTMP about System Boot/Shutdown.
[[0;32m  OK  [0m] Stopped Load/Save Random Seed.
[[0;32m  OK  [0m] Stopped Create Volatile Files and Directories.
[[0;32m  OK  [0m] Stopped Create System Users.
[[0;32m  OK  [0m] Stopped Remount Root and Kernel File Systems.
[[0;32m  OK  [0m] Unmounted Temporary Directory (/tmp).
[[0;32m  OK  [0m] Reached target Unmount All Filesystems.
[[0;32m  OK  [0m] Stopped target Swap.
[[0;32m  OK  [0m] Reached target Shutdown.
[[0;32m  OK  [0m] Reached target Final Step.
         Starting Reboot...
[  642.502922] Kernel panic - not syncing: Attempted to kill init! exitcode=0x00007f00
[  642.502922] 
[  642.504675] CPU: 1 PID: 1 Comm: shutdown Not tainted 4.18.0-147.5.1.es8_22.aarch64 #1
[  642.505769] Hardware name: QEMU KVM Virtual Machine, BIOS 0.0.0 02/06/2015
[  642.506790] Call trace:
[  642.507494]  dump_backtrace+0x0/0x188
[  642.508279]  show_stack+0x24/0x30
[  642.509086]  dump_stack+0x90/0xb4
[  642.509862]  panic+0x12c/0x294
[  642.510596]  do_exit+0xa48/0xa78
[  642.511340]  do_group_exit+0x3c/0xd0
[  642.512126]  __arm64_sys_exit_group+0x24/0x28
[  642.512973]  el0_svc_handler+0xa0/0x128
[  642.513766]  el0_svc+0x8/0xc
[  642.514480] SMP: stopping secondary CPUs
[  642.515372] Kernel Offset: disabled
[  642.516117] CPU features: 0x00,22208238
[  642.516914] Memory Limit: none
[  642.517641] ---[ end Kernel panic - not syncing: Attempted to kill init! exitcode=0x00007f00
[  642.517641]  ]---
