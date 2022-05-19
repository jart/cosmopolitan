#ifndef COSMOPOLITAN_LIBC_INTRIN_SPINLOCK_H_
#define COSMOPOLITAN_LIBC_INTRIN_SPINLOCK_H_
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/intrin/lockcmpxchg.h"
#include "libc/intrin/lockcmpxchgp.h"

#if IsModeDbg() && !defined(_SPINLOCK_DEBUG)
#define _SPINLOCK_DEBUG
#endif

#if defined(_SPINLOCK_DEBUG)
#define _spinlock(lock)        _spinlock_ndebug(lock)
#define _spinlock_ndebug(lock) _spinlock_cooperative(lock)
#elif defined(TINY)
#define _spinlock(lock)        _spinlock_tiny(lock)
#define _spinlock_ndebug(lock) _spinlock_tiny(lock)
#else
#define _spinlock(lock)        _spinlock_cooperative(lock)
#define _spinlock_ndebug(lock) _spinlock_cooperative(lock)
#endif

#define _trylock(lock) __atomic_test_and_set(lock, __ATOMIC_SEQ_CST)

#define _spunlock(lock)                             \
  do {                                              \
    autotype(lock) __lock = (lock);                 \
    typeof(*__lock) __x = 0;                        \
    __atomic_store(__lock, &__x, __ATOMIC_RELAXED); \
  } while (0)

#define _seizelock(lock)                            \
  do {                                              \
    autotype(lock) __lock = (lock);                 \
    typeof(*__lock) __x = 1;                        \
    __atomic_store(__lock, &__x, __ATOMIC_RELEASE); \
  } while (0)

#define _spinlock_tiny(lock)        \
  do {                              \
    autotype(lock) __lock = (lock); \
    while (_trylock(__lock)) {      \
      __builtin_ia32_pause();       \
    }                               \
  } while (0)

#define _spinlock_cooperative(lock)                  \
  do {                                               \
    autotype(lock) __lock = (lock);                  \
    typeof(*__lock) __x;                             \
    int __tries = 0;                                 \
    for (;;) {                                       \
      __atomic_load(__lock, &__x, __ATOMIC_RELAXED); \
      if (!__x && !_trylock(__lock)) {               \
        break;                                       \
      } else if (++__tries & 7) {                    \
        __builtin_ia32_pause();                      \
      } else {                                       \
        sched_yield();                               \
      }                                              \
    }                                                \
  } while (0)

void _spinlock_debug_1(void *, const char *, const char *, int, const char *);
void _spinlock_debug_4(void *, const char *, const char *, int, const char *);

#define _spinlock_debug(lock)                                             \
  do {                                                                    \
    switch (sizeof(*(lock))) {                                            \
      case 1:                                                             \
        _spinlock_debug_1(lock, #lock, __FILE__, __LINE__, __FUNCTION__); \
        break;                                                            \
      case 4:                                                             \
        _spinlock_debug_4(lock, #lock, __FILE__, __LINE__, __FUNCTION__); \
        break;                                                            \
      default:                                                            \
        assert(!"unsupported size");                                      \
    }                                                                     \
  } while (0)

#endif /* COSMOPOLITAN_LIBC_INTRIN_SPINLOCK_H_ */
