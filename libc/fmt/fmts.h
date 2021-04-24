#ifndef COSMOPOLITAN_LIBC_FMT_FMTS_H_
#define COSMOPOLITAN_LIBC_FMT_FMTS_H_

#define PRINTF_NTOA_BUFFER_SIZE 144

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int __fmt_pad(int (*)(const char *, void *, size_t), void *,
              unsigned long) hidden;
int __fmt_stoa(int (*)(const char *, void *, size_t), void *, void *,
               unsigned long, unsigned long, unsigned long, unsigned char,
               unsigned char) hidden;
int __fmt_ntoa(int (*)(const char *, void *, size_t), void *, va_list,
               unsigned char, unsigned long, unsigned long, unsigned long,
               unsigned char, const char *) hidden;
char *__fmt_dtoa(double, int, int, int *, int *, char **) hidden;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_FMT_FMTS_H_ */
