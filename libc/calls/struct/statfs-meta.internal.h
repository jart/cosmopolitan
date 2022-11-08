#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_STATFS_META_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_STATFS_META_INTERNAL_H_
#include "libc/calls/struct/statfs-freebsd.internal.h"
#include "libc/calls/struct/statfs-linux.internal.h"
#include "libc/calls/struct/statfs-netbsd.internal.h"
#include "libc/calls/struct/statfs-openbsd.internal.h"
#include "libc/calls/struct/statfs-xnu.internal.h"
#include "libc/calls/struct/statfs.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

union statfs_meta {
  struct statfs_linux linux;
  struct statfs_xnu xnu;
  struct statfs_freebsd freebsd;
  struct statfs_openbsd openbsd;
  struct statfs_netbsd netbsd;
};

void statfs2cosmo(struct statfs *, const union statfs_meta *) _Hide;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_STATFS_META_INTERNAL_H_ */
