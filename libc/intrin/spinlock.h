#ifndef COSMOPOLITAN_LIBC_INTRIN_SPINLOCK_H_
#define COSMOPOLITAN_LIBC_INTRIN_SPINLOCK_H_

#ifdef TINY
#define _spinlock(lock)                                     \
  do {                                                      \
    while (__atomic_test_and_set(lock, __ATOMIC_SEQ_CST)) { \
      __builtin_ia32_pause();                               \
    }                                                       \
  } while (0)
#else
#define _spinlock(lock)                                           \
  do {                                                            \
    for (;;) {                                                    \
      typeof(*(lock)) x;                                          \
      __atomic_load(lock, &x, __ATOMIC_RELAXED);                  \
      if (!x && !__atomic_test_and_set(lock, __ATOMIC_SEQ_CST)) { \
        break;                                                    \
      } else {                                                    \
        __builtin_ia32_pause();                                   \
      }                                                           \
    }                                                             \
  } while (0)
#endif

#define _spunlock(lock) __atomic_clear(lock, __ATOMIC_RELAXED)

#define _seizelock(lock)                        \
  do {                                          \
    typeof(*(lock)) x = 1;                      \
    __atomic_store(lock, &x, __ATOMIC_SEQ_CST); \
  } while (0)

#endif /* COSMOPOLITAN_LIBC_INTRIN_SPINLOCK_H_ */
