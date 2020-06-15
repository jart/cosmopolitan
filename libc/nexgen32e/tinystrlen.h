#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_TINYSTRLEN_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_TINYSTRLEN_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

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

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_TINYSTRLEN_H_ */
