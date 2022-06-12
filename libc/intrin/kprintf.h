#ifndef COSMOPOLITAN_LIBC_INTRIN_KPRINTF_H_
#define COSMOPOLITAN_LIBC_INTRIN_KPRINTF_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void kprintf(const char *, ...);
size_t ksnprintf(char *, size_t, const char *, ...);
void kvprintf(const char *, va_list);
size_t kvsnprintf(char *, size_t, const char *, va_list);
bool kisdangerous(const void *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_KPRINTF_H_ */
