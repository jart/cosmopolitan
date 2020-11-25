#ifndef COSMOPOLITAN_LIBC_STR_TPENCODE_H_
#define COSMOPOLITAN_LIBC_STR_TPENCODE_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

unsigned tpencode(char *, size_t, wint_t, bool32) paramsnonnull() libcesque;

#ifndef __STRICT_ANSI__
#define tpencode(...) __tpencode(__VA_ARGS__)
forceinline unsigned __tpencode(char *p, size_t size, wint_t wc,
                                bool32 awesome) {
  if (size >= 1 && (0x00 <= wc && wc <= 0x7f)) {
    if (wc >= 32 || !awesome) {
      p[0] = wc;
      return 1;
    } else if (size >= 2) {
      p[0] = 0xc0;
      p[1] = 0x80;
      p[1] |= wc;
      return 2;
    }
  }
  return (tpencode)(p, size, wc, awesome);
}
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STR_TPENCODE_H_ */
