#ifndef COSMOPOLITAN_LIBC_INTRIN_SPINLOCK_H_
#define COSMOPOLITAN_LIBC_INTRIN_SPINLOCK_H_
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/lockcmpxchg.h"
#include "libc/log/backtrace.internal.h"
#include "libc/log/log.h"
#include "libc/runtime/symbols.internal.h"

#if IsModeDbg() && !defined(_SPINLOCK_DEBUG)
#define _SPINLOCK_DEBUG
#endif

#if defined(_SPINLOCK_DEBUG)
#define _spinlock(lock)        _spinlock_ndebug(lock)
#define _spinlock_ndebug(lock) _spinlock_optimistic(lock)
#elif defined(TINY)
#define _spinlock(lock)        _spinlock_tiny(lock)
#define _spinlock_ndebug(lock) _spinlock_tiny(lock)
#else
#define _spinlock(lock)        _spinlock_optimistic(lock)
#define _spinlock_ndebug(lock) _spinlock_optimistic(lock)
#endif

#define _spunlock(lock) __atomic_clear(lock, __ATOMIC_RELAXED)

#define _trylock(lock) __atomic_test_and_set(lock, __ATOMIC_SEQ_CST)

#define _seizelock(lock)                        \
  do {                                          \
    typeof(*(lock)) x = 1;                      \
    __atomic_store(lock, &x, __ATOMIC_RELEASE); \
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

#define _spinlock_debug(lock)                                                  \
  do {                                                                         \
    typeof(*(lock)) me, owner;                                                 \
    unsigned long warntries = 10000000;                                        \
    me = gettid();                                                             \
    if (!_lockcmpxchg(lock, 0, me)) {                                          \
      __atomic_load(lock, &owner, __ATOMIC_RELAXED);                           \
      if (owner == me) {                                                       \
        kprintf("%s:%d: warning: possible re-entry on %s in %s()\n", __FILE__, \
                __LINE__, #lock, __FUNCTION__);                                \
      }                                                                        \
      while (!_lockcmpxchg(lock, 0, me)) {                                     \
        if (!--warntries) {                                                    \
          warntries = -1;                                                      \
          kprintf("%s:%d: warning: possible deadlock on %s in %s()\n",         \
                  __FILE__, __LINE__, #lock, __FUNCTION__);                    \
        }                                                                      \
        __builtin_ia32_pause();                                                \
      }                                                                        \
    }                                                                          \
  } while (0)

#endif /* COSMOPOLITAN_LIBC_INTRIN_SPINLOCK_H_ */
