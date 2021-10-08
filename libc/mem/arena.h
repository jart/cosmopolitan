#ifndef COSMOPOLITAN_LIBC_MEM_ARENA_H_
#define COSMOPOLITAN_LIBC_MEM_ARENA_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void __arena_push(void);
void __arena_pop(void);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_MEM_ARENA_H_ */
