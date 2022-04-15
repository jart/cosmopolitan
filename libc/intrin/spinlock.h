#ifndef COSMOPOLITAN_LIBC_INTRIN_SPINLOCK_H_
#define COSMOPOLITAN_LIBC_INTRIN_SPINLOCK_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
#if (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 401 && \
    !defined(__STRICT_ANSI__)

#define _spinlock(lock)                                              \
  do {                                                               \
    for (;;) {                                                       \
      typeof(*(lock)) x;                                             \
      __atomic_load(lock, &x, __ATOMIC_RELAXED);                     \
      if (!x && !__sync_lock_test_and_set(lock, __ATOMIC_CONSUME)) { \
        break;                                                       \
      } else {                                                       \
        __builtin_ia32_pause();                                      \
      }                                                              \
    }                                                                \
  } while (0)

#define _spunlock(lock) __sync_lock_release(lock)

#endif /* GNU 4.1+ */
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_SPINLOCK_H_ */
