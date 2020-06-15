#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_SYSTEMHANDLEENTRY_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_SYSTEMHANDLEENTRY_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtSystemHandleEntry {
  uint32_t OwnerPid;
  unsigned char ObjectType;
  unsigned char HandleFlags;
  unsigned short HandleValue;
  void *ObjectPointer;
  uint32_t AccessMask;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_SYSTEMHANDLEENTRY_H_ */
