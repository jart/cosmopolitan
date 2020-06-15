#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_OVERLAPPED_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_OVERLAPPED_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtOverlapped {
  uintptr_t Internal;
  uintptr_t InternalHigh;
  union {
    struct {
      uint32_t Offset;
      uint32_t OffsetHigh;
    };
    void *Pointer;
  };
  int64_t hEvent;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_OVERLAPPED_H_ */
