#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_STATFS_NETBSD_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_STATFS_NETBSD_H_
#include "libc/calls/struct/fsid.h"
COSMOPOLITAN_C_START_

struct statfs_netbsd {
  int16_t f_type;         /* type of file system */
  uint16_t f_oflags;      /* deprecated copy of mount flags */
  int64_t f_bsize;        /* fundamental file system block size */
  int64_t f_iosize;       /* optimal transfer block size */
  int64_t f_blocks;       /* total data blocks in file system */
  int64_t f_bfree;        /* free blocks in fs */
  int64_t f_bavail;       /* free blocks avail to non-superuser */
  int64_t f_files;        /* total file nodes in file system */
  int64_t f_ffree;        /* free file nodes in fs */
  fsid_t f_fsid;          /* file system id */
  uint32_t f_owner;       /* user that mounted the file system */
  int64_t f_flags;        /* copy of mount flags */
  int64_t f_syncwrites;   /* count of sync writes since mount */
  int64_t f_asyncwrites;  /* count of async writes since mount */
  int64_t f_spare[1];     /* spare for later */
  char f_fstypename[16];  /* fs type name */
  char f_mntonname[90];   /* directory on which mounted */
  char f_mntfromname[90]; /* mounted file system */
};

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_STATFS_NETBSD_H_ */
