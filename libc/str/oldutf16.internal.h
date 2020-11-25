#ifndef COSMOPOLITAN_LIBC_STR_OLDUTF16_H_
#define COSMOPOLITAN_LIBC_STR_OLDUTF16_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

unsigned getutf16(const char16_t *, wint_t *);
int pututf16(char16_t *, size_t, wint_t, bool);

#define pututf16(BUF, SIZE, CH, AWESOME) __pututf16(BUF, SIZE, CH, AWESOME)
#define getutf16(BUF, CHPTR)             __getutf16(BUF, CHPTR)

forceinline int __pututf16(char16_t *s, size_t size, wint_t wc,
                           bool32 awesome) {
  if (size >= 1 && (0x00 <= wc && wc <= 0xD7FF)) {
    if (wc >= 32 || !awesome) {
      s[0] = (char16_t)wc;
      return 1;
    } else if (size >= 2) {
      s[0] = 0xd800;
      s[1] = 0xdc00 | (char16_t)wc;
      return 2;
    }
  }
  int ax;
  asm("call\tpututf16"
      : "=a"(ax), "=m"(*(char(*)[size])s)
      : "D"(s), "S"(size), "d"(wc)
      : "cc");
  return ax;
}

forceinline unsigned __getutf16(const char16_t *s, wint_t *wc) {
  if ((0x00 <= s[0] && s[0] <= 0xD7FF)) {
    *wc = s[0];
    return 1;
  }
  unsigned ax;
  asm("call\tgetutf16"
      : "=a"(ax), "=m"(*wc)
      : "D"(s), "S"(wc), "m"(*s), "m"(*(char(*)[4])s)
      : "cc");
  return ax;
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STR_OLDUTF16_H_ */
