#ifndef COSMOPOLITAN_LIBC_INTRIN_KMALLOC_H_
#define COSMOPOLITAN_LIBC_INTRIN_KMALLOC_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void *kmalloc(size_t) mallocesque attributeallocsize((1)) returnsaligned((8));

void __kmalloc_lock(void);
void __kmalloc_unlock(void);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_KMALLOC_H_ */
