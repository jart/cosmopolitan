#ifndef COSMOPOLITAN_LIBC_YIELD_H_
#define COSMOPOLITAN_LIBC_YIELD_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
#ifdef _COSMO_SOURCE

static inline void spin_yield(void) {
#if defined(__GNUC__) && defined(__aarch64__)
  __asm__ volatile("yield");
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
  __asm__ volatile("pause");
#endif
}

#endif /* _COSMO_SOURCE */
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_YIELD_H_ */
