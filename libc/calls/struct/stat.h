#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_STAT_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_STAT_H_
#include "libc/calls/struct/timespec.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct stat {              /* cosmo abi */
  uint64_t st_dev;         /* 0: id of device with file */
  uint64_t st_ino;         /* 8: inode number in disk b-tree */
  uint64_t st_nlink;       /* 16: hard link count */
  uint32_t st_mode;        /* 24: octal file mask thing */
  uint32_t st_uid;         /* 28: user id of owner */
  uint32_t st_gid;         /* group id of owning group */
  uint32_t st_flags;       /* flags (bsd-only) */
  uint64_t st_rdev;        /* id of device if a special file */
  int64_t st_size;         /* bytes in file */
  int64_t st_blksize;      /* preferred chunking for underlying filesystem */
  int64_t st_blocks;       /* number of 512-byte pages allocated to file */
  struct timespec st_atim; /* access time */
  struct timespec st_mtim; /* modified time */
  struct timespec st_ctim; /* complicated time */
  struct timespec st_birthtim;
  uint64_t st_gen;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_STAT_H_ */
