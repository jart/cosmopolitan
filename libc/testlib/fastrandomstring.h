#ifndef COSMOPOLITAN_LIBC_TESTLIB_FASTRANDOMSTRING_H_
#define COSMOPOLITAN_LIBC_TESTLIB_FASTRANDOMSTRING_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

static inline const char *FastRandomString(void) {
  static unsigned long t;
  static union {
    unsigned long x;
    char b[sizeof(unsigned long)];
  } u;
  u.x = (t = (t * 0xdeaadead) + 0xdeaadead) & 0x7e7e7e7e7e7e7e;
  return u.b;
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_TESTLIB_FASTRANDOMSTRING_H_ */
