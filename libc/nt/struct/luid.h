#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_LUID_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_LUID_H_

struct NtLuid {
  uint32_t LowPart;
  int32_t HighPart;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_LUID_H_ */
