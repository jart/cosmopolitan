#ifndef COSMOPOLITAN_LIBC_STR_TPDECODE_H_
#define COSMOPOLITAN_LIBC_STR_TPDECODE_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int tpdecode(const char *, wint_t *) paramsnonnull((1)) libcesque;

#ifndef __STRICT_ANSI__
#define tpdecode(S, OUT) __tpdecode(S, OUT)
forceinline int __tpdecode(const char *s, wint_t *out) {
  int ax;
  if (0 <= *s && *s <= 0x7f) {
    *out = *s;
    return 1;
  }
  asm("call\ttpdecode"
      : "=a"(ax), "=m"(*(char(*)[6])s)
      : "D"(s), "S"(out)
      : "cc");
  return ax;
}
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STR_TPDECODE_H_ */
