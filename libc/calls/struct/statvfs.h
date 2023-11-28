#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_STATVFS_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_STATVFS_H_
COSMOPOLITAN_C_START_

struct statvfs {
  unsigned long f_bsize;   /* Filesystem block size */
  unsigned long f_frsize;  /* Fragment size */
  uint64_t f_blocks;       /* Size of fs in f_frsize units */
  uint64_t f_bfree;        /* Number of free blocks */
  uint64_t f_bavail;       /* Number of free blocks for unprivileged users */
  uint64_t f_files;        /* Number of inodes */
  uint64_t f_ffree;        /* Number of free inodes */
  uint64_t f_favail;       /* Number of free inodes for unprivileged users */
  unsigned long f_fsid;    /* Filesystem ID */
  unsigned long f_flag;    /* Mount flags */
  unsigned long f_namemax; /* Maximum filename length */
};

int statvfs(const char *, struct statvfs *);
int fstatvfs(int, struct statvfs *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_STATVFS_H_ */
