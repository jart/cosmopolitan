#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IOVEC_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IOVEC_H_
COSMOPOLITAN_C_START_

struct NtIovec {
  uint32_t len;
  char *buf;
};

void DescribeIovNt(const struct NtIovec *, uint32_t, ssize_t);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IOVEC_H_ */
