#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_OVERLAPPED_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_OVERLAPPED_H_

struct NtOverlapped {
  uintptr_t Internal;
  uintptr_t InternalHigh;
  union {
    struct {
      uint32_t Offset;
      uint32_t OffsetHigh;
    };
    int64_t Pointer;
  };
  int64_t hEvent;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_OVERLAPPED_H_ */
