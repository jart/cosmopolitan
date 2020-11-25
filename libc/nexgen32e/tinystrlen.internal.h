#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_TINYSTRLEN_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_TINYSTRLEN_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
#if !defined(__GNUC__) || defined(__STRICT_ANSI__)

int tinystrlen(const char *);
int tinystrnlen(const char *, size_t);
int tinystrlen16(const char16_t *);
int tinystrnlen16(const char16_t *, size_t);
int tinywcslen(const wchar_t *);
int tinywcsnlen(const wchar_t *, size_t);

#else

forceinline int tinystrlen(const char *s) {
  unsigned ax;
  asm("call\ttinystrlen" : "=a"(ax) : "D"(s), "m"(*(char(*)[PAGESIZE])s));
  return ax;
}

forceinline int tinystrnlen(const char *s, size_t n) {
  unsigned ax;
  asm("call\ttinystrnlen" : "=a"(ax) : "D"(s), "S"(n), "m"(*(char(*)[n])s));
  return ax;
}

forceinline int tinystrlen16(const char16_t *s) {
  unsigned ax;
  asm("call\ttinystrlen16" : "=a"(ax) : "D"(s), "m"(*(char16_t(*)[PAGESIZE])s));
  return ax;
}

forceinline int tinystrnlen16(const char16_t *s, size_t n) {
  unsigned ax;
  asm("call\ttinystrnlen16"
      : "=a"(ax)
      : "D"(s), "S"(n), "m"(*(char16_t(*)[n])s));
  return ax;
}

forceinline int tinywcslen(const wchar_t *s) {
  unsigned ax;
  asm("call\ttinywcslen" : "=a"(ax) : "D"(s), "m"(*(wchar_t(*)[PAGESIZE])s));
  return ax;
}

forceinline int tinywcsnlen(const wchar_t *s, size_t n) {
  unsigned ax;
  asm("call\ttinywcsnlen" : "=a"(ax) : "D"(s), "S"(n), "m"(*(wchar_t(*)[n])s));
  return ax;
}

#endif
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_TINYSTRLEN_H_ */
