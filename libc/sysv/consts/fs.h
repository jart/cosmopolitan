#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_FS_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_FS_H_

#define FS_IOC_GETFLAGS   0x80086601
#define FS_IOC_SETFLAGS   0x40086602
#define FS_IOC_GETVERSION 0x80087601
#define FS_IOC_SETVERSION 0x40087602
#define FS_IOC_FIEMAP     0xc020660b
#define FS_IOC_FSGETXATTR 0x801c581f
#define FS_IOC_FSSETXATTR 0x401c5820
#define FS_IOC_GETFSLABEL 0x81009431
#define FS_IOC_SETFSLABEL 0x41009432

#define FS_FL_USER_VISIBLE    0x0003DFFF /* user visible flags */
#define FS_FL_USER_MODIFIABLE 0x000380FF /* user modifiable flags */
#define FS_SECRM_FL           0x00000001 /* secure deletion */
#define FS_UNRM_FL            0x00000002 /* undelete */
#define FS_COMPR_FL           0x00000004 /* compress */
#define FS_SYNC_FL            0x00000008 /* synchronous */
#define FS_IMMUTABLE_FL       0x00000010
#define FS_APPEND_FL          0x00000020 /* append-only */
#define FS_NODUMP_FL          0x00000040
#define FS_NOATIME_FL         0x00000080
#define FS_DIRTY_FL           0x00000100
#define FS_COMPRBLK_FL        0x00000200
#define FS_NOCOMP_FL          0x00000400
#define FS_ENCRYPT_FL         0x00000800 /* encrypted file */
#define FS_BTREE_FL           0x00001000
#define FS_INDEX_FL           0x00001000 /* hash-indexed directory */
#define FS_IMAGIC_FL          0x00002000
#define FS_JOURNAL_DATA_FL    0x00004000
#define FS_NOTAIL_FL          0x00008000
#define FS_DIRSYNC_FL         0x00010000
#define FS_TOPDIR_FL          0x00020000
#define FS_HUGE_FILE_FL       0x00040000
#define FS_EXTENT_FL          0x00080000

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_FS_H_ */
