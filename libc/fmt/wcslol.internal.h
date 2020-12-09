#ifndef COSMOPOLITAN_LIBC_FMT_WCSLOL_H_
#define COSMOPOLITAN_LIBC_FMT_WCSLOL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define WCSLOL(STR, ENDPTR, OPTIONAL_BASE, MIN, MAX)      \
  ({                                                      \
    intmax_t res = wcstoimax(STR, ENDPTR, OPTIONAL_BASE); \
    if (res < MIN) return MIN;                            \
    if (res > MAX) return MAX;                            \
    res;                                                  \
  })

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_FMT_WCSLOL_H_ */
