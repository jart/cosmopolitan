#ifndef COSMOPOLITAN_LIBC_FMT_PALANDPRINTF_H_
#define COSMOPOLITAN_LIBC_FMT_PALANDPRINTF_H_

#define PRINTF_NTOA_BUFFER_SIZE 144
#define PRINTF_FTOA_BUFFER_SIZE 64

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int spacepad(int (*)(long, void *), void *, unsigned long) hidden;
int ftoa(int (*)(long, void *), void *, long double, int, unsigned long,
         unsigned long) hidden;
int stoa(int (*)(long, void *), void *, void *, unsigned long, unsigned long,
         unsigned long, unsigned char, unsigned char) hidden;
int ntoa(int (*)(long, void *), void *, va_list, unsigned char, unsigned long,
         unsigned long, unsigned long, unsigned char, const char *) hidden;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_FMT_PALANDPRINTF_H_ */
