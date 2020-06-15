#ifndef COSMOPOLITAN_LIBC_MEM_ALLOCA_H_
#define COSMOPOLITAN_LIBC_MEM_ALLOCA_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

#define alloca(size) __builtin_alloca(size)

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_MEM_ALLOCA_H_ */
