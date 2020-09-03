#ifndef COSMOPOLITAN_LIBC_MEM_HOOK_HOOK_H_
#define COSMOPOLITAN_LIBC_MEM_HOOK_HOOK_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern void (*hook$free)(void *);
extern void *(*hook$malloc)(size_t);
extern void *(*hook$calloc)(size_t, size_t);
extern void *(*hook$memalign)(size_t, size_t);
extern void *(*hook$realloc)(void *, size_t);
extern void *(*hook$realloc_in_place)(void *, size_t);
extern void *(*hook$valloc)(size_t);
extern void *(*hook$pvalloc)(size_t);
extern size_t (*hook$malloc_usable_size)(const void *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_MEM_HOOK_HOOK_H_ */
