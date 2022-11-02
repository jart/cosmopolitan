#ifndef COSMOPOLITAN_LIBC_INTRIN_DIRECTMAP_H_
#define COSMOPOLITAN_LIBC_INTRIN_DIRECTMAP_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct ProtectNt {
  uint32_t flags1;
  uint32_t flags2;
};

struct DirectMap {
  void *addr;
  int64_t maphandle;
};

struct DirectMap sys_mmap(void *, size_t, int, int, int, int64_t);
struct DirectMap sys_mmap_nt(void *, size_t, int, int, int, int64_t);
struct DirectMap sys_mmap_metal(void *, size_t, int, int, int, int64_t);
int sys_munmap_metal(void *, size_t);
int __prot2nt(int, int);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_DIRECTMAP_H_ */
