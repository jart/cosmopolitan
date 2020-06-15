#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_MEMORYRANGEENTRY_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_MEMORYRANGEENTRY_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtMemoryRangeEntry {
  void *VirtualAddress;
  size_t NumberOfBytes;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_MEMORYRANGEENTRY_H_ */
