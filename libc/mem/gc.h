#ifdef _COSMO_SOURCE
#ifndef COSMOPOLITAN_LIBC_MEM_GC_H_
#define COSMOPOLITAN_LIBC_MEM_GC_H_
COSMOPOLITAN_C_START_

libcesque void _gc_free(void *);
libcesque void __defer(void *, void *, void *);
libcesque void gclongjmp(void *, int) wontreturn;
#define gc(THING)  defer((void *)_gc_free, (void *)(THING))
#define _gc(THING) defer((void *)_gc_free, (void *)(THING))
#define defer(FN, ARG)                             \
  ({                                               \
    autotype(ARG) Arg = (ARG);                     \
    /* prevent weird opts like tail call */        \
    __asm__ volatile("" : "+g"(Arg) : : "memory"); \
    __defer(__builtin_frame_address(0), FN, Arg);  \
    __asm__ volatile("" : "+g"(Arg) : : "memory"); \
    Arg;                                           \
  })

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_MEM_GC_H_ */
#endif /* _COSMO_SOURCE */
