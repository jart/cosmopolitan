#ifndef COSMOPOLITAN_LIBC_FMT_PALANDPRINTF_H_
#define COSMOPOLITAN_LIBC_FMT_PALANDPRINTF_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int spacepad(int(int, void *), void *, unsigned long) hidden;
int ftoa(int(int, void *), void *, long double, unsigned long, unsigned long,
         unsigned long) hidden;
int stoa(int(int, void *), void *, void *, unsigned long, unsigned long,
         unsigned long, unsigned char, unsigned char) hidden;
int ntoa(int(int, void *), void *, va_list, unsigned char, unsigned long,
         unsigned long, unsigned long, unsigned long, const char *) hidden;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_FMT_PALANDPRINTF_H_ */
