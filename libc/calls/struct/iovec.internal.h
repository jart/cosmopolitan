#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_IOVEC_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_IOVEC_INTERNAL_H_
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/mem/alloca.h"
COSMOPOLITAN_C_START_

ssize_t __robust_writev(int, struct iovec *, int);
int64_t sys_preadv(int, struct iovec *, int, int64_t, int64_t);
int64_t sys_pwritev(int, const struct iovec *, int, int64_t, int64_t);
int64_t sys_readv(int32_t, const struct iovec *, int32_t);
int64_t sys_vmsplice(int, const struct iovec *, int64_t, uint32_t);
int64_t sys_writev(int32_t, const struct iovec *, int32_t);
ssize_t sys_read_nt(int, const struct iovec *, size_t, int64_t);
ssize_t sys_readv_metal(int, const struct iovec *, int);
ssize_t sys_readv_nt(int, const struct iovec *, int);
ssize_t sys_readv_serial(int, const struct iovec *, int);
ssize_t sys_write_nt(int, const struct iovec *, size_t, ssize_t);
ssize_t sys_writev_metal(struct Fd *, const struct iovec *, int);
ssize_t sys_writev_nt(int, const struct iovec *, int);
ssize_t sys_writev_serial(struct Fd *, const struct iovec *, int);
ssize_t sys_send_nt(int, const struct iovec *, size_t, uint32_t);
ssize_t sys_sendto_nt(int, const struct iovec *, size_t, uint32_t, const void *,
                      uint32_t);

const char *DescribeIovec(char[300], ssize_t, const struct iovec *, int);
#define DescribeIovec(x, y, z) DescribeIovec(alloca(300), x, y, z)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_IOVEC_INTERNAL_H_ */
