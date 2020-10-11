#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_STAT_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_STAT_H_
#include "libc/calls/struct/timespec.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct stat {               /* linux abi */
  int64_t st_dev;           /* 0: id of device with file */
  int64_t st_ino;           /* 8: inode number in disk b-tree */
  int64_t st_nlink;         /* 16: hard link count */
  int32_t st_mode;          /* 24: octal file mask thing */
  int32_t st_uid;           /* 28: user id of owner */
  int32_t st_gid;           /* group id of owning group */
  int32_t __pad;            /* ignore this */
  int64_t st_rdev;          /* id of device if a special file */
  int64_t st_size;          /* bytes in file */
  int64_t st_blksize;       /* preferred chunking for underlying filesystem */
  int64_t st_blocks;        /* number of 512-byte pages allocated to file */
  struct timespec st_atim;  /* access time (consider noatime) */
  struct timespec st_mtim;  /* modified time */
  struct timespec st_ctim;  /* complicated time */
  int64_t __future[3 + 10]; /* reserved for future use */
#define st_atime st_atim.tv_sec
#define st_mtime st_mtim.tv_sec
#define st_ctime st_ctim.tv_sec
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_STAT_H_ */
