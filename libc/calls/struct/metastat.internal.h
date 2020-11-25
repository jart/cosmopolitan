#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_METASTAT_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_METASTAT_H_
#ifndef __STRICT_ANSI__
#include "libc/calls/struct/stat.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

union metastat {
  struct stat linux;

  struct stat$xnu {
    int32_t st_dev;
    uint16_t st_mode, st_nlink;
    uint64_t st_ino;
    uint32_t st_uid, st_gid;
    int32_t st_rdev;
    struct timespec st_atim, st_mtim, st_ctim, st_birthtim;
    int64_t st_size, st_blocks;
    int32_t st_blksize;
    uint32_t st_flags, st_gen;
    int32_t st_lspare;
    int64_t st_qspare[2];
  } xnu;

  struct stat$freebsd {
    uint64_t st_dev, st_ino, st_nlink;
    uint16_t st_mode;
    int16_t st_padding0;
    uint32_t st_uid, st_gid;
    int32_t st_padding1;
    uint64_t st_rdev;
    struct timespec st_atim, st_mtim, st_ctim, st_birthtim;
    int64_t st_size, st_blocks;
    int32_t st_blksize;
    uint32_t st_flags;
    uint64_t st_gen;
    unsigned long st_spare[10];
  } freebsd;

  struct stat$openbsd {
    uint32_t st_mode;
    int32_t st_dev;
    uint64_t st_ino;
    uint32_t st_nlink, st_uid, st_gid;
    int32_t st_rdev;
    struct timespec st_atim, st_mtim, st_ctim;
    int64_t st_size, st_blocks;
    int32_t st_blksize;
    uint32_t st_flags, st_gen;
    struct timespec __st_birthtim;
  } openbsd;
};

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* !ANSI */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_METASTAT_H_ */
