#ifndef COSMOPOLITAN_LIBC_INTRIN_SPINLOCK_H_
#define COSMOPOLITAN_LIBC_INTRIN_SPINLOCK_H_
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § spinlocks                                                 ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│─╝
  fast tiny inline synchronization routines */

#ifdef TINY
#define _spinlock(lock) _spinlock_tiny(lock)
#else
#define _spinlock(lock) _spinlock_cooperative(lock)
#endif

#define _spunlock(lock) __atomic_store_n(lock, 0, __ATOMIC_RELAXED)

#define _seizelock(lock, value)                     \
  ({                                                \
    autotype(lock) __lock = (lock);                 \
    typeof(*__lock) __x = (value);                  \
    __atomic_store(__lock, &__x, __ATOMIC_RELEASE); \
  })

#define _spinlock_tiny(lock)  \
  ({                          \
    while (_trylock(lock)) {  \
      __builtin_ia32_pause(); \
    }                         \
    0;                        \
  })

#define _spinlock_cooperative(lock)                  \
  ({                                                 \
    char __x;                                        \
    unsigned __tries = 0;                            \
    char *__lock = (lock);                           \
    for (;;) {                                       \
      __atomic_load(__lock, &__x, __ATOMIC_RELAXED); \
      if (!__x && !_trylock(__lock)) {               \
        break;                                       \
      } else if (++__tries & 7) {                    \
        __builtin_ia32_pause();                      \
      } else {                                       \
        _spinlock_yield();                           \
      }                                              \
    }                                                \
    0;                                               \
  })

#define _spinlock_cooperative_(lock)                 \
  ({                                                 \
    char __x;                                        \
    volatile int __i;                                \
    unsigned __tries = 0;                            \
    char *__lock = (lock);                           \
    for (;;) {                                       \
      __atomic_load(__lock, &__x, __ATOMIC_RELAXED); \
      if (!__x && !_trylock(__lock)) {               \
        break;                                       \
      } else if (__tries < 7) {                      \
        for (__i = 0; __i != 1 << __tries; __i++) {  \
        }                                            \
        __tries++;                                   \
      } else {                                       \
        _spinlock_yield();                           \
      }                                              \
    }                                                \
    0;                                               \
  })

#define _trylock(lock) __atomic_test_and_set(lock, __ATOMIC_SEQ_CST)

void _spinlock_yield(void);

#endif /* COSMOPOLITAN_LIBC_INTRIN_SPINLOCK_H_ */
