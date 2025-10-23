## resize之后休眠恢复日志
```
[    1.828667] PM: Checking hibernation image partition UUID=71b066c5-7377-4a2e-be68-fb39c04186ea
[    1.828670] PM: Hibernation image not present or could not be loaded.
[    1.845260] IMA: No TPM chip found, activating TPM-bypass! (rc=-19)
[    2.339902] PM: Starting manual resume from disk
[    2.340442] PM: Hibernation image partition 253:1 present
[    2.340444] PM: Looking for hibernation image.
[    2.341219] PM: Image signature found, resuming
[    2.342748] PM: Preparing processes for restore.
[    2.344663] PM: Loading hibernation image.
[    2.344735] PM: Marking nosave pages: [mem 0x0009f000-0x000fffff]
[    2.344738] PM: Marking nosave pages: [mem 0xbffda000-0xffffffff]
[    2.345095] PM: Basic memory bitmaps created
[    2.356791] PM: Using 3 thread(s) for decompression.
PM: Loading and decompressing image data (139062 pages)...
[    2.358016] PM: Image mismatch: memory size
[    2.358483] PM: Read 556248 kbytes in 0.01 seconds (55624.80 MB/s)
[    2.359928] PM: Error -1 resuming
[    2.359931] PM: Failed to load hibernation image, recovering.
[    2.369813] PM: Basic memory bitmaps freed
[    2.371754] PM: Hibernation image not present or could not be loaded.
```
## 查看代码
在休眠恢复时，会检查，当前的内存大小和休眠之前是否一致，不一致则报错:
```cpp
static int check_header(struct swsusp_info *info)
{
    char *reason;

    reason = check_image_kernel(info);
    if (!reason && info->num_physpages != num_physpages)
        reason = "memory size";
    if (reason) {
        printk(KERN_ERR "PM: Image mismatch: %s\n", reason);
        return -EPERM;
    }
    return 0;
}
```

而在休眠时，会将`num_physpages`赋值。
```cpp
static int init_header(struct swsusp_info *info)
{
    memset(info, 0, sizeof(struct swsusp_info));
    info->num_physpages = num_physpages;
    info->image_pages = nr_copy_pages;
    info->pages = snapshot_get_image_size();
    info->size = info->pages;
    info->size <<= PAGE_SHIFT;
    return init_header_complete(info);
}
```

而该值，在系统引导时初始化, 负值为最大的pfn
```
setup_arch
=> num_physpages = max_pfn;
```
在内存热插拔时，会修改:
```
void __online_page_set_limits(struct page *page)
{
    unsigned long pfn = page_to_pfn(page);

    if (pfn >= num_physpages)
        num_physpages = pfn + 1;
}
```



## 调试
当前系统4G内存
```
[root@memsnapshot1 ~]# free -g
              total        used        free      shared  buff/cache   available
Mem:              3           0           2           0           0           3
Swap:             3           0           3
```
查看`num_physpages`
```
crash> p/x num_physpages
$1 = 0x140000
```

调整为8G
```
[root@memsnapshot1 ~]# free -g
              total        used        free      shared  buff/cache   available
Mem:              7           0           6           0           0           7
Swap:             3           0           3

crash> p/x num_physpages
$1 = 0x240000
```
重启/休眠后:
```
crash> p/x num_physpages
$1 = 0x240000
```
未改变, **不过可以确定跟热变配有关**

## panic

某次复现过程中出现了panic:
```
crash> bt
PID: 45361  TASK: ffffa091ede76300  CPU: 0   COMMAND: "sh"
 #0 [ffffa0917460bc30] machine_kexec at ffffffffb1e66294
 #1 [ffffa0917460bc90] __crash_kexec at ffffffffb1f22562
 #2 [ffffa0917460bd60] panic at ffffffffb257a8a3
 #3 [ffffa0917460bde0] acpi_hibernation_leave at ffffffffb22153a9
 #4 [ffffa0917460bdf0] hibernation_snapshot at ffffffffb1efa9c1
 #5 [ffffa0917460be18] hibernate at ffffffffb1efb273
 #6 [ffffa0917460be30] state_store at ffffffffb1ef8c44
 #7 [ffffa0917460be70] kobj_attr_store at ffffffffb2188612
 #8 [ffffa0917460be80] sysfs_kf_write at ffffffffb20db0b2
 #9 [ffffa0917460be90] kernfs_fop_write at ffffffffb20da69b
#10 [ffffa0917460bec8] vfs_write at ffffffffb204db50
#11 [ffffa0917460bf08] sys_write at ffffffffb204e92f
#12 [ffffa0917460bf50] system_call_fastpath at ffffffffb2593f92
    RIP: 00007f70a3ed3ba0  RSP: 00007fff6cf6ea50  RFLAGS: 00010206
    RAX: 0000000000000001  RBX: 0000000000000005  RCX: 0000000000000000
    RDX: 0000000000000005  RSI: 00007f70a47ff000  RDI: 0000000000000001
    RBP: 00007f70a47ff000   R8: 000000000000000a   R9: 00007f70a47f8740
    R10: 00007fff6cf6e5e0  R11: 0000000000000246  R12: 00007f70a41ac400
    R13: 0000000000000005  R14: 0000000000000001  R15: 0000000000000000
    ORIG_RAX: 0000000000000001  CS: 0033  SS: 002b
```

经定位代码:
```cpp
static void acpi_hibernation_leave(void)
{
    /*
     * If ACPI is not enabled by the BIOS and the boot kernel, we need to
     * enable it here.
     */
    acpi_enable();
    /* Reprogram control registers */
    acpi_leave_sleep_state_prep(ACPI_STATE_S4);
    /* Check the hardware signature */
    if (facs && s4_hardware_signature != facs->hardware_signature) {
        printk(KERN_EMERG "ACPI: Hardware changed while hibernated, "
            "cannot resume!\n");
        panic("ACPI S4 hardware signature mismatch");
    }
    /* Restore the NVS memory area */
    suspend_nvs_restore();
    /* Allow EC transactions to happen. */
    acpi_ec_unblock_transactions_early();
}
```
而 `s4_hardware_signature` 负值:

```sh
acpi_init
=> acpi_bus_init
   => acpi_sleep_init
      => acpi_sleep_hibernate_setup
         => acpi_get_table(ACPI_SIG_FACS, 1, (struct acpi_table_header **)&facs);
         => if (facs)
            => s4_hardware_signature = facs->hardware_signature;
```

只发生在启动，而在热变配重启后，该apci表会改变.

## 结论

经过几轮测试，发现热变配过程会引起很多问题。暂不支持热变配
