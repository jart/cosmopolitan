#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_GC_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_GC_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct StackFrame;

struct Garbages {
  size_t i, n;
  struct Garbage {
    struct StackFrame *frame;
    intptr_t fn;
    intptr_t arg;
    intptr_t ret;
  } * p;
};

hidden extern struct Garbages g_garbage;

int64_t CollectGarbage(void) hidden;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_GC_H_ */
