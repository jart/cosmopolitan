#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_IOVEC_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_IOVEC_INTERNAL_H_
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/mem/alloca.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int64_t sys_preadv(int, struct iovec *, int, int64_t, int64_t) _Hide;
int64_t sys_pwritev(int, const struct iovec *, int, int64_t, int64_t) _Hide;
int64_t sys_readv(int32_t, const struct iovec *, int32_t) _Hide;
int64_t sys_vmsplice(int, const struct iovec *, int64_t, uint32_t) _Hide;
int64_t sys_writev(int32_t, const struct iovec *, int32_t) _Hide;
size_t __iovec_size(const struct iovec *, size_t) _Hide;
ssize_t WritevUninterruptible(int, struct iovec *, int);
ssize_t sys_read_nt(struct Fd *, const struct iovec *, size_t, ssize_t) _Hide;
ssize_t sys_readv_metal(struct Fd *, const struct iovec *, int) _Hide;
ssize_t sys_readv_nt(struct Fd *, const struct iovec *, int) _Hide;
ssize_t sys_readv_serial(struct Fd *, const struct iovec *, int) _Hide;
ssize_t sys_write_nt(int, const struct iovec *, size_t, ssize_t) _Hide;
ssize_t sys_writev_metal(struct Fd *, const struct iovec *, int) _Hide;
ssize_t sys_writev_nt(int, const struct iovec *, int) _Hide;
ssize_t sys_writev_serial(struct Fd *, const struct iovec *, int) _Hide;
ssize_t sys_send_nt(int, const struct iovec *, size_t, uint32_t) _Hide;
ssize_t sys_sendto_nt(int, const struct iovec *, size_t, uint32_t, void *,
                      uint32_t) _Hide;

const char *DescribeIovec(char[300], ssize_t, const struct iovec *, int);
#define DescribeIovec(x, y, z) DescribeIovec(alloca(300), x, y, z)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_IOVEC_INTERNAL_H_ */
