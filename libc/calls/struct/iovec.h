#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_IOVEC_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_IOVEC_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct iovec {
  void *iov_base;
  size_t iov_len;
};

ssize_t preadv(int, struct iovec *, int, int64_t);
ssize_t pwritev(int, const struct iovec *, int, int64_t);
ssize_t vmsplice(int, const struct iovec *, int64_t, uint32_t);

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_IOVEC_H_ */
