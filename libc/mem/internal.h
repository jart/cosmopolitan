#ifndef COSMOPOLITAN_LIBC_MEM_INTERNAL_H_
#define COSMOPOLITAN_LIBC_MEM_INTERNAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct CritbitNode {
  void *child[2];
  uint32_t byte;
  unsigned otherbits;
};

int PutEnvImpl(char *, bool) _Hide;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_MEM_INTERNAL_H_ */
