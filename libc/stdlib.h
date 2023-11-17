#ifndef COSMOPOLITAN_LIBC_STDLIB_H_
#define COSMOPOLITAN_LIBC_STDLIB_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

char *fcvt(double, int, int *, int *);
char *ecvt(double, int, int *, int *);
char *gcvt(double, int, char *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STDLIB_H_ */
