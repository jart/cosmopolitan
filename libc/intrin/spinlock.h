#ifndef COSMOPOLITAN_LIBC_INTRIN_SPINLOCK_H_
#define COSMOPOLITAN_LIBC_INTRIN_SPINLOCK_H_

#ifdef TINY
#define _spinlock(lock)                         \
  do {                                          \
    while (__sync_lock_test_and_set(lock, 1)) { \
      __builtin_ia32_pause();                   \
    }                                           \
  } while (0)
#else
#define _spinlock(lock)                               \
  do {                                                \
    for (;;) {                                        \
      typeof(*(lock)) x;                              \
      __atomic_load(lock, &x, __ATOMIC_RELAXED);      \
      if (!x && !__sync_lock_test_and_set(lock, 1)) { \
        break;                                        \
      } else {                                        \
        __builtin_ia32_pause();                       \
      }                                               \
    }                                                 \
  } while (0)
#endif

#define _spunlock(lock) __sync_lock_release(lock)

#endif /* COSMOPOLITAN_LIBC_INTRIN_SPINLOCK_H_ */
