# kernel 
```
#define XFS_TRANSACTION         0x69
#define XFS_VOLUME              0x2
#define XFS_LOG                 0xaa
```
# 获取`logstart`
```
xfs_db> sb
xfs_db> p
magicnum = 0x58465342
blocksize = 4096
dblocks = 262144
rblocks = 0
rextents = 0
uuid = f7fe9fe5-8120-4119-ab30-8cc3ffc55978
logstart = 131078
```
# 计算所在的block
```
131078 * 4096 = 536895488 
```
获取数据, 截取lsn: [1,2 ,  1,6)
```
hexdump -s 536895488 xfs_test.img
20006000 edfe beba(大端, 高地址，存低字节) 0000 0100(cycle) 0000 0200(version) 0000 0002(len:0x200, 512)   ----record 1
20006010 [0000 0100 0000 0000](lsn : 1,0) [0000 0100 0000 0000](tail lsn : 1,0)
20006020 0000 0000(crc) ffff ffff(h_prev_block: ) 0000 0100(h_num_logops: 1) c0b0 d0d0(h_cyc_data)
20006030 0000 0000 0000 0000 0000 0000 0000 0000
*
20006120 0000 0000 0000 0000 0000 0000 0000 0100[h_fmt: 1 XLOG_FMT_LINUX_LE (小端) ]
20006130 [fef7 e59f 2081 1941 30ab c38c c5ff 7859](uuid)
20006140 0000 0080(size: 0x800 = 512 * 4, 4个blocksize) 0000 0000 0000 0000 0000 0000
20006150 0000 0000 0000 0000 0000 0000 0000 0000
*
20006200 0000 0100(tid) 0000 0800(len) 20(flags) aa(clientid) 0000(res2) 556e 0000
20006210 0000 0000 0000 0000 0000 0000 0000 0000
*
20006400 edfe beba 0000 0100(cycle) 0000 0200(version) 0000 0006(len: 512 * 3) ---record 2
20006410 0000 0100 0000 0200(lsn:1,2) 0000 0100 0000 0200(tail lsn : 1,2)
20006420 310a 0c8a(crc) 0000 0000(h_prev_block) 0000 0f00(h_num_logops: 15) d834 4f09(h_cycle_data)
20006430 0000 0000 0000 0000 0000 0000 0000 0000
*
20006520 0000 0000 0000 0000 0000 0000 0000 0100
20006530 fef7 e59f 2081 1941 30ab c38c c5ff 7859 [uuid]
20006540 0000 0080(size:0x800 = 512 *4) 0000 0000 0000 0000 0000 0000
20006550 0000 0000 0000 0000 0000 0000 0000 0000
*
20006600 (op0)0000 0100 (tid: 被替换为cycle number了) 0000 0000(len: 0) 01(flags: START_TRANS) 69(clientid: TRANS) 0000 (oh_rec2)  (op1)(tid)d834 4f09
20006610 0000 1000(len:16) 00(flags:none)69(clientid:TRANS) 0000(oh_rec2) 414e 5452(struct xfs_trans_header: magic: XFS_TRANS_HEADER_MAGIC) 0028  0000(th_type)
20006620 094f 34d8(tid) 000c 0000(th_num_items: 12, 15 - 3) (op2)d834 4f09(tid) 0000 1800(len: 24)
20006630 0069 0000(oh_rec2) (struct xfs_buf_log_format: buffer write) 123c(blf_type) 0002(blf_size) 3800(flags) 0001(blf_len) [0002 0000
20006640 0000 0000](blf_blkno: 2) [0001 0000](blf_map_size) [0001 0000](blf_data_map) (op3)d834 4f09(tid)
20006650 0000 8000(len:128) 00(flags:none)69(clientid:TRANS) 0000(oh_rec2) [4158 4947 0000 0100
20006660 0000 0000 0100 0000 0000 4000 0000 0300
20006670 0000 0100 0000 3c00 0000 8000 ffff ffff
20006680 ffff ffff ffff ffff ffff ffff ffff ffff
*
200066d0 ffff ffff ffff ffff](daddr 2 前128 bit) (op4)d834 4f09(tid) 0000 1800(len:24)
200066e0 00(flags:none)69(clientid:TRANS) 0000(oh_rec2) (struct xfs_buf_log_format : buffer_write)123c(blf_type) 0002(blf_size) 2000(flags) 0008(blf_len) [0020 0000
200066f0 0000 0000](blf_blkno: 32) [0001 0000](blf_map_size) [0001 0000](blf_data_map) (op5)d834 4f09(tid)
20006700 0000 8000(len:128) 00(flags:none)69(clientid:TRANS) 0000(oh_rec2) [4946 3342 0000 0100
20006710 ffff ffff ffff ffff 0000 0000 0000 2000
20006720 0000 0000 0000 0000 fef7 e59f 2081 1941
20006730 30ab c38c c5ff 7859 0000 0000 5f6c 1798
20006740 0000 8000 0000 3c40 ffff ffff ffff f0ff
20006750 0000 0000 0000 0000 0000 0000 0000 0000
*
20006780 0000 0000 0000 0000](daddr: 32 free_root)  (op6)d834 4f09(tid) 0000 1800
20006790 0069 0000(oh_rec2) (struct xfs_buf_log_format)123c 0002(blf_size) 2000(flags) 0008(blf_len) [0018 0000
200067a0 0000 0000](blk_blkno :32) [0001 0000](blf_map_size) [0001 0000](blf_data_map) (op7)d834 4f09(tid)
200067b0 0000 8000(len:128) 0069 0000(oh_rec2) [4149 3342 0000 0100
200067c0 ffff ffff ffff ffff 0000 0000 0000 1800
200067d0 0000 0000 0000 0000 fef7 e59f 2081 1941
200067e0 30ab c38c c5ff 7859 0000 0000 0156 7c15
200067f0 0000 8000 0000 3c40 ffff ffff ffff f0ff

20006800 0000 0100 0000 0000 0000 0000 0000 0000
20006810 0000 0000 0000 0000 0000 0000 0000 0000
*
20006830 0000 0000 0000 0000](daddr: 32 free_root) (op8)d834 4f09(tid)  0000 3800(len:56)
20006840 0069 0000(oh_rec2) (struct xfs_inode_log_format_64) 123b(ilf_type) 0003(ilf_size:3) [0003 0000](ilf_fields:XFS_ILOG_CORE, DDATA) 0000(ilf_asize) 0008(ilf_dsize)
20006850 [0000 0000](ilf_pad) [0083 0000 0000 0000](ilf_ino:128) [0000 0000
20006860 0000 0000 0000 0000 0000 0000](ilf_u) [0080 0000
20006870 0000 0000](ilf_blkno) [0020 0000](ilf_len:512) (0600 0000)[offset:600? : 1536!!!!我觉得是6] (op9)d834 4f09 (tid)
20006880 0000 b000(len:176) 0069 0000(oh_rec2) (op10)494e 41ed 0103 0000
20006890 0000 0000 0000 0000 0002 0000 0000 0000
200068a0 0000 0000 0000 0000 0511 a712 dc48 3519
200068b0 0511 a712 dc48 3519 0511 a712 dc48 3519
200068c0 0006 0000 0000 0000 0000 0000 0000 0000
200068d0 0000 0000 0000 0000 0000 0200 0000 0000
200068e0 0000 0000 75e6 8744 ffff ffff 0000 0000
200068f0 0002 0000 0000 0000 0000 0000 0000 0000
20006900 0008 0000 0000 0000 0000 0000 0000 0000
20006910 0000 0000 0000 0000 0511 a712 dc48 3519
20006920 0083 0000 0000 0000 fef7 e59f 2081 1941
20006930 30ab c38c c5ff 7859 d834 4f09 --tid(10) 0000 0800
20006940 0069 0000 0000 0000 8000 0000 d834 4f09 --tid(11)
20006950 0000 3800 0069 0000 123b 0003 0003 0000
20006960 0000 0014 0000 0000 0080 0000 0000 0000
20006970 0000 0000 0000 0000 0000 0000 0000 0000
20006980 0080 0000 0000 0000 0020 0000 0000 0000
20006990 d834 4f09 --tid(12) 0000 b000 0069 0000 494e --magic() 41ed
200069a0 0103 0000 0000 0000 0000 0000 0003 0000
200069b0 0000 0000 0000 0000 0000 0000 4e8b 002e
200069c0 dc48 3519 0511 a712 dc48 3519 0511 a712
200069d0 dc48 3519 0011 0000 0000 0000 0000 0000
200069e0 0000 0000 0000 0000 0000 0000 0000 0200
200069f0 0000 0000 0000 0000 0000 0000 ffff ffff
20006a00 0000 0100 0004 0000 0000 0000 0000 0000
20006a10 0000 0000 0008 0000 0000 0000 0000 0000
20006a20 0000 0000 0000 0000 0000 0000 5698 029a
20006a30 dc44 3519 0080 0000 0000 0000 fef7 e59f
20006a40 2081 1941 30ab c38c c5ff 7859 d834 4f09 --tid(13)
20006a50 0000 1400 0069 0000 0001 0000 8000 0003
20006a60 6560 6374 0002 0000 0083 0000 d834 4f09
20006a70 0000 0000 0269 0000 0000 0000 0000 0000
20006a80 0000 0000 0000 0000 0000 0000 0000 0000

-----结束了-----, 下一个log record
20006c00 edfe beba 0000 0100 0000 0200 0000 0002
```
# xfs 字段解释

## struct xlog_rec_header

```cpp
struct xlog_rec_header {
	__be32 h_cycle_data[XLOG_HEADER_CYCLE_SIZE / BBSIZE];
	...
	__be32 h_size;
}
```
* h_size: In-core log record size. This is somewhere between 16 and 256KiB, 
 with 32KiB being the default.
 ```cpp
#define XLOG_MIN_RECORD_BSIZE   (16*1024)       /* eventually 32k */
#define XLOG_BIG_RECORD_BSIZE   (32*1024)       /* 32k buffers */
#define XLOG_MAX_RECORD_BSIZE   (256*1024)
#define XLOG_HEADER_CYCLE_SIZE  (32*1024)       /* cycle data in header */
 ```
* h_fmt: 
```cpp
XLOG_FMT_UNKNOWN 	: Unknown. Perhaps this log is corrupt.
XLOG_FMT_LINUX_LE	:  Little-endian Linux.
XLOG_FMT_LINUX_BE	:  Big-endian Linux.
XLOG_FMT_IRIX_BE 	: Big-endian Irix.

#define XLOG_FMT_UNKNOWN  0
#define XLOG_FMT_LINUX_LE 1
#define XLOG_FMT_LINUX_BE 2
#define XLOG_FMT_IRIX_BE  3

//大小端, x86是小端
#ifdef XFS_NATIVE_HOST
#define XLOG_FMT XLOG_FMT_LINUX_BE
#else
#define XLOG_FMT XLOG_FMT_LINUX_LE
#endif


#ifdef __BIG_ENDIAN
#define XFS_NATIVE_HOST 1
#else
#undef XFS_NATIVE_HOST
#endif

```
* **h_cycle_data**: 
The first u32 of each log sector must contain the cycle number.
Since log item buffers are formatted without regard to this
requirement, the original contents of the first four bytes of
each sector in the log are copied into the corresponding element
of this array. After that, the first four bytes of those sectors
are stamped with the cycle number. This process is reversed at
recovery time. If there are more sectors in this log record than
there are slots in this array, the cycle data continues for as many
sectors are needed; each sector is formatted as type xlog_rec_ext_header.<br/><br/>
每个log sector的第一个u32 必须包含 cycle number。因为日志item buffer
被格式化为没用管这个需求，日志中的每个selector的前4个byte 被copy到该数组
中。在这之后，每个sector的前四byte被替换为 cycle number。该过程在recovery
时，被反转。如果这个日志记录中的扇区比这个数组中的槽多，那么cycle data将根
据需要的扇区数量继续进行;每个扇区格式化为xlog_rec_ext_header类型。

## struct xfs_trans_header
```cpp
typedef struct xfs_trans_header {
	uint th_magic;
	uint th_type;
	__int32_t th_tid;
	uint th_num_items;
} xfs_trans_header_t;
```
* **th_num_items** :
The number of operations appearing after this operation, not
including the commit operation. In effect, this tracks the
number of metadata change operations in this transaction.<br/>
在此op之后的op 数量，但是不包括commit operation。实际上，这将跟踪
此事务中元metadata change operations数量。

# XFS_BLK_DATAMAP_SIZE
```
#define XFS_BLF_DATAMAP_SIZE    ((XFS_MAX_BLOCKSIZE / XFS_BLF_CHUNK) / NBWORD)  (1 << 16  / 128 / (8 * 4)) = 16
#define XFS_MAX_BLOCKSIZE       (1 << XFS_MAX_BLOCKSIZE_LOG)
#define XFS_MAX_BLOCKSIZE_LOG   16
#define XFS_BLF_CHUNK           128
#define NBWORD                  (NBBY * sizeof(unsigned int))
#define NBBY    8
```

# XFS_ILOG_
```
#define XFS_ILOG_CORE   0x001   /* log standard inode fields */
#define XFS_ILOG_DDATA  0x002   /* log i_df.if_data */
```
