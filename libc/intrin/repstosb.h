#ifndef COSMOPOLITAN_LIBC_INTRIN_REPSTOSB_H_
#define COSMOPOLITAN_LIBC_INTRIN_REPSTOSB_H_
#ifdef _COSMO_SOURCE

forceinline void *repstosb(void *dest, unsigned char al, size_t cx) {
  unsigned char *di = (unsigned char *)dest;
  while (cx) *di++ = al, cx--;
  return di;
}

#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
#define repstosb(DI, AL, CX)                         \
  ({                                                 \
    void *Di = (DI);                                 \
    size_t Cx = (CX);                                \
    unsigned char Al = (AL);                         \
    asm("rep stosb %b5,(%0)"                         \
        : "=D"(Di), "=c"(Cx), "=m"(*(char(*)[Cx])Di) \
        : "0"(Di), "1"(Cx), "a"(Al));                \
    Di;                                              \
  })
#endif

#endif /* _COSMO_SOURCE */
#endif /* COSMOPOLITAN_LIBC_INTRIN_REPSTOSB_H_ */
