#ifndef COSMOPOLITAN_LIBC_MEM_GC_H_
#define COSMOPOLITAN_LIBC_MEM_GC_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void *_gc(void *);
void *_defer(void *, void *);
void __defer(void *, void *, void *);
void _gclongjmp(void *, int) dontthrow wontreturn;
void _gc_free(void *);

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define _gc(THING) _defer((void *)_gc_free, (void *)(THING))
#define _defer(FN, ARG)                           \
  ({                                              \
    autotype(ARG) Arg = (ARG);                    \
    /* prevent weird opts like tail call */       \
    asm volatile("" : "+g"(Arg) : : "memory");    \
    __defer(__builtin_frame_address(0), FN, Arg); \
    asm volatile("" : "+g"(Arg) : : "memory");    \
    Arg;                                          \
  })
#endif /* defined(__GNUC__) && !defined(__STRICT_ANSI__) */

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_MEM_GC_H_ */
