#ifndef COSMOPOLITAN_LIBC_FMT_FMT_INTERNAL_H_
#define COSMOPOLITAN_LIBC_FMT_FMT_INTERNAL_H_

#define PRINTF_NTOA_BUFFER_SIZE 144

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define __FMT_PUT(C)              \
  do {                            \
    char Buf[1] = {C};            \
    if (out(Buf, arg, 1) == -1) { \
      return -1;                  \
    }                             \
  } while (0)

int __fmt_pad(int (*)(const char *, void *, size_t), void *,
              unsigned long) _Hide;
int __fmt_stoa(int (*)(const char *, void *, size_t), void *, void *,
               unsigned long, unsigned long, unsigned long, unsigned char,
               unsigned char) _Hide;
int __fmt_ntoa(int (*)(const char *, void *, size_t), void *, va_list,
               unsigned char, unsigned long, unsigned long, unsigned long,
               unsigned char, const char *) _Hide;
int __fmt_dtoa(int (*)(const char *, void *, size_t), void *, int, int, int,
               int, int, bool, char, unsigned char, const char *, va_list);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_FMT_FMT_INTERNAL_H_ */
