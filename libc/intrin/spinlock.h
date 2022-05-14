#ifndef COSMOPOLITAN_LIBC_INTRIN_SPINLOCK_H_
#define COSMOPOLITAN_LIBC_INTRIN_SPINLOCK_H_
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/intrin/kprintf.h"
#include "libc/log/backtrace.internal.h"
#include "libc/log/log.h"

#if defined(_SPINLOCK_DEBUG)
#define _spinlock(lock) _spinlock_debug(lock)
#elif defined(TINY)
#define _spinlock(lock) _spinlock_tiny(lock)
#else
#define _spinlock(lock) _spinlock_optimistic(lock)
#endif

#define _spunlock(lock) __atomic_clear(lock, __ATOMIC_RELAXED)

#define _trylock(lock) __atomic_test_and_set(lock, __ATOMIC_SEQ_CST)

#define _seizelock(lock)                        \
  do {                                          \
    typeof(*(lock)) x = 1;                      \
    __atomic_store(lock, &x, __ATOMIC_SEQ_CST); \
  } while (0)

#define _spinlock_tiny(lock)  \
  do {                        \
    while (_trylock(lock)) {  \
      __builtin_ia32_pause(); \
    }                         \
  } while (0)

#define _spinlock_optimistic(lock)               \
  do {                                           \
    for (;;) {                                   \
      typeof(*(lock)) x;                         \
      __atomic_load(lock, &x, __ATOMIC_RELAXED); \
      if (!x && !_trylock(lock)) {               \
        break;                                   \
      } else {                                   \
        __builtin_ia32_pause();                  \
      }                                          \
    }                                            \
  } while (0)

#define _spinlock_debug(lock)                                           \
  do {                                                                  \
    typeof(*(lock)) me, owner;                                          \
    me = gettid();                                                      \
    if (_trylock(lock)) {                                               \
      __atomic_load(lock, &owner, __ATOMIC_RELAXED);                    \
      if (owner == me) {                                                \
        kprintf("%s:%d: warning: possible spinlock re-entry in %s()\n", \
                __FILE__, __LINE__, __FUNCTION__);                      \
        if (weaken(ShowBacktrace)) {                                    \
          weaken(ShowBacktrace)(2, 0);                                  \
        }                                                               \
      }                                                                 \
      _spinlock_optimistic(lock);                                       \
    }                                                                   \
    *lock = me;                                                         \
  } while (0)

#endif /* COSMOPOLITAN_LIBC_INTRIN_SPINLOCK_H_ */
