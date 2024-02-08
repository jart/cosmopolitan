#ifndef COSMOPOLITAN_LIBC_INTRIN_KPRINTF_H_
#define COSMOPOLITAN_LIBC_INTRIN_KPRINTF_H_
#ifdef _COSMO_SOURCE

#define klog         __klog
#define kprintf      __kprintf
#define ksnprintf    __ksnprintf
#define kvprintf     __kvprintf
#define kvsnprintf   __kvsnprintf
#define kloghandle   __kloghandle
#define kisdangerous __kisdangerous
#define uprintf      __uprintf
#define uvprintf     __uvprintf

#if !(__ASSEMBLER__ + __LINKER__ + 0)

COSMOPOLITAN_C_START_

void kprintf(const char *, ...) libcesque;
size_t ksnprintf(char *, size_t, const char *, ...) libcesque;
void kvprintf(const char *, va_list) libcesque;
size_t kvsnprintf(char *, size_t, const char *, va_list) libcesque;

bool32 kisdangerous(const void *) libcesque;

void klog(const char *, size_t) libcesque;
void _klog_serial(const char *, size_t) libcesque;
long kloghandle(void) libcesque;

void uprintf(const char *, ...) libcesque;
void uvprintf(const char *, va_list) libcesque;

#ifndef TINY
#define KINFOF(FMT, ...)                                         \
  do {                                                           \
    uprintf("\r\e[35m%s:%d: " FMT "\e[0m\n", __FILE__, __LINE__, \
            ##__VA_ARGS__);                                      \
  } while (0)
#define KWARNF(FMT, ...)                                                  \
  do {                                                                    \
    uprintf("\r\e[94;49mwarn: %s:%d: " FMT "\e[0m\n", __FILE__, __LINE__, \
            ##__VA_ARGS__);                                               \
  } while (0)
#else
#define KINFOF(FMT, ...) ((void)0)
#define KWARNF(FMT, ...) ((void)0)
#endif
#define KDIEF(FMT, ...)                                                     \
  do {                                                                      \
    kprintf("\r\e[30;101mfatal: %s:%d: " FMT "\e[0m\n", __FILE__, __LINE__, \
            ##__VA_ARGS__);                                                 \
    for (;;) asm volatile("cli\n\thlt");                                    \
  } while (0)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* _COSMO_SOURCE */
#endif /* COSMOPOLITAN_LIBC_INTRIN_KPRINTF_H_ */
