#ifndef COSMOPOLITAN_LIBC_BITS_TYPECHECK_H_
#define COSMOPOLITAN_LIBC_BITS_TYPECHECK_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

#define TYPECHECK(T, X)     \
  ({                        \
    T Lol1;                 \
    typeof(X) Lol2;         \
    (void)(&Lol1 == &Lol2); \
    X;                      \
  })

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_TYPECHECK_H_ */
