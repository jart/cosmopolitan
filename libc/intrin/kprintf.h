#ifndef COSMOPOLITAN_LIBC_INTRIN_KPRINTF_H_
#define COSMOPOLITAN_LIBC_INTRIN_KPRINTF_H_
#ifdef COSMO

#define kprintf      __kprintf
#define ksnprintf    __ksnprintf
#define kvprintf     __kvprintf
#define kvsnprintf   __kvsnprintf
#define kisdangerous __kisdangerous

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void kprintf(const char *, ...);
size_t ksnprintf(char *, size_t, const char *, ...);
void kvprintf(const char *, va_list);
size_t kvsnprintf(char *, size_t, const char *, va_list);
bool kisdangerous(const void *);

void kprintf(const char *, ...);
size_t ksnprintf(char *, size_t, const char *, ...);
void kvprintf(const char *, va_list);
size_t kvsnprintf(char *, size_t, const char *, va_list);
bool kisdangerous(const void *);
void _klog(const char *, size_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMO */
#endif /* COSMOPOLITAN_LIBC_INTRIN_KPRINTF_H_ */
