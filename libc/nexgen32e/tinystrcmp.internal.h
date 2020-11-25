#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_TINYSTRCMP_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_TINYSTRCMP_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int tinystrcmp(const char *, const char *) nothrow nocallback
    paramsnonnull() nosideeffect;
int tinystrncmp(const char *, const char *, size_t) nothrow nocallback
    paramsnonnull() nosideeffect;

#define tinystrcmp(s1, s2)                                     \
  ({                                                           \
    int Res;                                                   \
    asm("call\ttinystrcmp"                                     \
        : "=a"(Res)                                            \
        : "D"(&(s1)[0]), "S"(&(s2)[0]), "m"(*(s1)), "m"(*(s2)) \
        : "cc");                                               \
    Res;                                                       \
  })

#define tinystrncmp(s1, s2, n)                                         \
  ({                                                                   \
    int Res;                                                           \
    asm("call\ttinystrncmp"                                            \
        : "=a"(Res)                                                    \
        : "D"(&(s1)[0]), "S"(&(s2)[0]), "d"(n), "m"(*(s1)), "m"(*(s2)) \
        : "cc");                                                       \
    Res;                                                               \
  })

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_TINYSTRCMP_H_ */
