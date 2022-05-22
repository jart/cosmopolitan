#ifndef COSMOPOLITAN_LIBC_INTRIN_SPINLOCK_H_
#define COSMOPOLITAN_LIBC_INTRIN_SPINLOCK_H_
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/intrin/lockcmpxchg.h"
#include "libc/intrin/lockcmpxchgp.h"
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § spinlocks                                                 ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│─╝
  privileged unsophisticated locking subroutines */

#if IsModeDbg() && !defined(_SPINLOCK_DEBUG)
#define _SPINLOCK_DEBUG
#endif

#if defined(_SPINLOCK_DEBUG)
#define _spinlock(lock)        _spinlock_ndebug(lock)
#define _spinlock_ndebug(lock) _spinlock_cooperative(lock)
#define _trylock(lock)         _trylock_debug(lock)
#define _seizelock(lock)       _seizelock_impl(lock, gettid())
#elif defined(TINY)
#define _spinlock(lock)        _spinlock_tiny(lock)
#define _spinlock_ndebug(lock) _spinlock_tiny(lock)
#define _trylock(lock)         _trylock_inline(lock)
#define _seizelock(lock)       _seizelock_impl(lock, 1)
#else
#define _spinlock(lock)        _spinlock_cooperative(lock)
#define _spinlock_ndebug(lock) _spinlock_cooperative(lock)
#define _trylock(lock)         _trylock_inline(lock)
#define _seizelock(lock)       _seizelock_impl(lock, 1)
#endif

#define _trylock_inline(lock) __atomic_test_and_set(lock, __ATOMIC_SEQ_CST)

#define _trylock_debug(lock) \
  _trylock_debug_4(lock, #lock, __FILE__, __LINE__, __FUNCTION__)

#define _spinlock_debug(lock) \
  _spinlock_debug_4(lock, #lock, __FILE__, __LINE__, __FUNCTION__)

#define _spunlock(lock)                             \
  do {                                              \
    autotype(lock) __lock = (lock);                 \
    typeof(*__lock) __x = 0;                        \
    __atomic_store(__lock, &__x, __ATOMIC_RELAXED); \
  } while (0)

#define _seizelock_impl(lock, value)                \
  do {                                              \
    autotype(lock) __lock = (lock);                 \
    typeof(*__lock) __x = (value);                  \
    __atomic_store(__lock, &__x, __ATOMIC_RELEASE); \
  } while (0)

#define _spinlock_tiny(lock)          \
  do {                                \
    autotype(lock) __lock = (lock);   \
    while (_trylock_inline(__lock)) { \
      __builtin_ia32_pause();         \
    }                                 \
  } while (0)

#define _spinlock_cooperative(lock)                  \
  do {                                               \
    autotype(lock) __lock = (lock);                  \
    typeof(*__lock) __x;                             \
    unsigned __tries = 0;                            \
    for (;;) {                                       \
      __atomic_load(__lock, &__x, __ATOMIC_RELAXED); \
      if (!__x && !_trylock_inline(__lock)) {        \
        break;                                       \
      } else if (++__tries & 7) {                    \
        __builtin_ia32_pause();                      \
      } else {                                       \
        sched_yield();                               \
      }                                              \
    }                                                \
  } while (0)

int _trylock_debug_4(int *, const char *, const char *, int, const char *);
void _spinlock_debug_4(int *, const char *, const char *, int, const char *);

#endif /* COSMOPOLITAN_LIBC_INTRIN_SPINLOCK_H_ */
