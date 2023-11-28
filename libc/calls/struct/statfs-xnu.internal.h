#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_STATFS_XNU_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_STATFS_XNU_INTERNAL_H_
#include "libc/calls/struct/fsid.h"
COSMOPOLITAN_C_START_

struct statfs_xnu {
  uint32_t f_bsize;         /* fundamental file system block size */
  int32_t f_iosize;         /* optimal transfer block size */
  uint64_t f_blocks;        /* total data blocks in file system */
  uint64_t f_bfree;         /* free blocks in fs */
  uint64_t f_bavail;        /* free blocks avail to non-superuser */
  uint64_t f_files;         /* total file nodes in file system */
  uint64_t f_ffree;         /* free file nodes in fs */
  fsid_t f_fsid;            /* file system id */
  uint32_t f_owner;         /* user that mounted the filesystem */
  uint32_t f_type;          /* type of filesystem */
  uint32_t f_flags;         /* copy of mount exported flags */
  uint32_t f_fssubtype;     /* fs sub-type (flavor) */
  char f_fstypename[16];    /* fs type name */
  char f_mntonname[4096];   /* directory on which mounted */
  char f_mntfromname[4096]; /* mounted filesystem */
  uint32_t f_flags_ext;     /* extended flags */
  uint32_t f_reserved[7];   /* For future use */
};

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_STATFS_XNU_INTERNAL_H_ */
