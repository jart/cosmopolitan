#ifndef COSMOPOLITAN_LIBC_INTRIN_REPMOVSB_H_
#define COSMOPOLITAN_LIBC_INTRIN_REPMOVSB_H_
#ifdef _COSMO_SOURCE

forceinline void repmovsb(void **dest, const void **src, size_t cx) {
  char *di = (char *)*dest;
  const char *si = (const char *)*src;
  while (cx) *di++ = *si++, cx--;
  *dest = di, *src = si;
}

#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
#define repmovsb(DI, SI, CX)                                       \
  ({                                                               \
    void *Di = *(DI);                                              \
    const void *Si = *(SI);                                        \
    size_t Cx = (CX);                                              \
    asm("rep movsb"                                                \
        : "=D"(Di), "=S"(Si), "=c"(Cx), "=m"(*(char(*)[Cx])Di)     \
        : "0"(Di), "1"(Si), "2"(Cx), "m"(*(const char(*)[Cx])Si)); \
    *(DI) = Di, *(SI) = Si;                                        \
  })
#endif

#endif /* _COSMO_SOURCE */
#endif /* COSMOPOLITAN_LIBC_INTRIN_REPMOVSB_H_ */
