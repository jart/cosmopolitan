#ifndef _LINUX_F2FS_H
#define _LINUX_F2FS_H
#include <linux/ioctl.h>
#include <linux/types.h>

#define F2FS_IOCTL_MAGIC                0xf5
#define F2FS_IOC_START_ATOMIC_WRITE     _IO(F2FS_IOCTL_MAGIC, 1)
#define F2FS_IOC_COMMIT_ATOMIC_WRITE    _IO(F2FS_IOCTL_MAGIC, 2)
#define F2FS_IOC_START_VOLATILE_WRITE   _IO(F2FS_IOCTL_MAGIC, 3)
#define F2FS_IOC_RELEASE_VOLATILE_WRITE _IO(F2FS_IOCTL_MAGIC, 4)
#define F2FS_IOC_ABORT_ATOMIC_WRITE     _IO(F2FS_IOCTL_MAGIC, 5)
#define F2FS_IOC_GARBAGE_COLLECT        _IOW(F2FS_IOCTL_MAGIC, 6, __u32)
#define F2FS_IOC_WRITE_CHECKPOINT       _IO(F2FS_IOCTL_MAGIC, 7)
#define F2FS_IOC_DEFRAGMENT             _IOWR(F2FS_IOCTL_MAGIC, 8, struct f2fs_defragment)
#define F2FS_IOC_MOVE_RANGE             _IOWR(F2FS_IOCTL_MAGIC, 9, struct f2fs_move_range)
#define F2FS_IOC_FLUSH_DEVICE \
  _IOW(F2FS_IOCTL_MAGIC, 10, struct f2fs_flush_device)
#define F2FS_IOC_GARBAGE_COLLECT_RANGE \
  _IOW(F2FS_IOCTL_MAGIC, 11, struct f2fs_gc_range)
#define F2FS_IOC_GET_FEATURES            _IOR(F2FS_IOCTL_MAGIC, 12, __u32)
#define F2FS_IOC_SET_PIN_FILE            _IOW(F2FS_IOCTL_MAGIC, 13, __u32)
#define F2FS_IOC_GET_PIN_FILE            _IOR(F2FS_IOCTL_MAGIC, 14, __u32)
#define F2FS_IOC_PRECACHE_EXTENTS        _IO(F2FS_IOCTL_MAGIC, 15)
#define F2FS_IOC_RESIZE_FS               _IOW(F2FS_IOCTL_MAGIC, 16, __u64)
#define F2FS_IOC_GET_COMPRESS_BLOCKS     _IOR(F2FS_IOCTL_MAGIC, 17, __u64)
#define F2FS_IOC_RELEASE_COMPRESS_BLOCKS _IOR(F2FS_IOCTL_MAGIC, 18, __u64)
#define F2FS_IOC_RESERVE_COMPRESS_BLOCKS _IOR(F2FS_IOCTL_MAGIC, 19, __u64)
#define F2FS_IOC_SEC_TRIM_FILE \
  _IOW(F2FS_IOCTL_MAGIC, 20, struct f2fs_sectrim_range)
#define F2FS_IOC_GET_COMPRESS_OPTION \
  _IOR(F2FS_IOCTL_MAGIC, 21, struct f2fs_comp_option)
#define F2FS_IOC_SET_COMPRESS_OPTION \
  _IOW(F2FS_IOCTL_MAGIC, 22, struct f2fs_comp_option)
#define F2FS_IOC_DECOMPRESS_FILE      _IO(F2FS_IOCTL_MAGIC, 23)
#define F2FS_IOC_COMPRESS_FILE        _IO(F2FS_IOCTL_MAGIC, 24)
#define F2FS_IOC_START_ATOMIC_REPLACE _IO(F2FS_IOCTL_MAGIC, 25)

#define F2FS_IOC_SHUTDOWN         _IOR('X', 125, __u32)
#define F2FS_GOING_DOWN_FULLSYNC  0x0
#define F2FS_GOING_DOWN_METASYNC  0x1
#define F2FS_GOING_DOWN_NOSYNC    0x2
#define F2FS_GOING_DOWN_METAFLUSH 0x3
#define F2FS_GOING_DOWN_NEED_FSCK 0x4

#define F2FS_TRIM_FILE_DISCARD 0x1
#define F2FS_TRIM_FILE_ZEROOUT 0x2
#define F2FS_TRIM_FILE_MASK    0x3

struct f2fs_gc_range {
  __u32 sync;
  __u64 start;
  __u64 len;
};

struct f2fs_defragment {
  __u64 start;
  __u64 len;
};

struct f2fs_move_range {
  __u32 dst_fd;
  __u64 pos_in;
  __u64 pos_out;
  __u64 len;
};

struct f2fs_flush_device {
  __u32 dev_num;
  __u32 segments;
};

struct f2fs_sectrim_range {
  __u64 start;
  __u64 len;
  __u64 flags;
};

struct f2fs_comp_option {
  __u8 algorithm;
  __u8 log_cluster_size;
};

#endif /* _LINUX_F2FS_H */
