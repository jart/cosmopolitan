#ifndef COSMOPOLITAN_LIBC_MEM_HOOK_H_
#define COSMOPOLITAN_LIBC_MEM_HOOK_H_
COSMOPOLITAN_C_START_

extern void (*hook_free)(void *);
extern void *(*hook_malloc)(size_t);
extern void *(*hook_calloc)(size_t, size_t);
extern void *(*hook_memalign)(size_t, size_t);
extern void *(*hook_realloc)(void *, size_t);
extern void *(*hook_realloc_in_place)(void *, size_t);
extern int (*hook_malloc_trim)(size_t);
extern size_t (*hook_malloc_usable_size)(void *);
extern size_t (*hook_bulk_free)(void *[], size_t);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_MEM_HOOK_H_ */
