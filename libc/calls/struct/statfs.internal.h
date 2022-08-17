#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_STATFS_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_STATFS_INTERNAL_H_
#include "libc/calls/struct/statfs-meta.internal.h"
#include "libc/calls/struct/statfs.h"
#include "libc/mem/alloca.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int sys_statfs(const char *, union statfs_meta *);
int sys_fstatfs(int, union statfs_meta *);

const char *DescribeStatfs(char[300], int, const struct statfs *);
#define DescribeStatfs(rc, sf) DescribeStatfs(alloca(300), rc, sf)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_STATFS_INTERNAL_H_ */
