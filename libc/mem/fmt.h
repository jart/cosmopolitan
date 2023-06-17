#ifndef COSMOPOLITAN_LIBC_MEM_FMT_H_
#define COSMOPOLITAN_LIBC_MEM_FMT_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int asprintf(char **, const char *, ...) printfesque(2)
    paramsnonnull((1, 2)) libcesque;
int vasprintf(char **, const char *, va_list) paramsnonnull() libcesque;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_MEM_FMT_H_ */
