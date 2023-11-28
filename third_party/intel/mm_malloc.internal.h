#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _MM_MALLOC_H_INCLUDED
#define _MM_MALLOC_H_INCLUDED
#include "libc/mem/mem.h"
#ifndef __cplusplus
extern int _mm_posix_memalign(void **, size_t, size_t)
#else
extern "C" int _mm_posix_memalign(void **, size_t, size_t) throw()
#endif
    __asm__("posix_memalign");
static __inline void *_mm_malloc(size_t __size, size_t __alignment) {
  void *__ptr;
  if (__alignment == 1) return malloc(__size);
  if (__alignment == 2 || (sizeof(void *) == 8 && __alignment == 4))
    __alignment = sizeof(void *);
  if (_mm_posix_memalign(&__ptr, __alignment, __size) == 0)
    return __ptr;
  else
    return NULL;
}
static __inline void _mm_free(void *__ptr) {
  free(__ptr);
}
#endif
#endif
