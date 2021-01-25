#ifndef COSMOPOLITAN_LIBC_BITS_LIKELY_H_
#define COSMOPOLITAN_LIBC_BITS_LIKELY_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

#define likely(expr)   __builtin_expect(!!(expr), 1)
#define unlikely(expr) __builtin_expect(!!(expr), 0)

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_LIKELY_H_ */
