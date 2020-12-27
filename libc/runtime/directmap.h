#ifndef COSMOPOLITAN_LIBC_RUNTIME_DIRECTMAP_H_
#define COSMOPOLITAN_LIBC_RUNTIME_DIRECTMAP_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct DirectMap {
  void *addr;
  int64_t maphandle;
};

struct DirectMap __mmap(void *, size_t, unsigned, unsigned, int, int64_t);
struct DirectMap __mmap$nt(void *, size_t, unsigned, unsigned, int, int64_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_DIRECTMAP_H_ */
