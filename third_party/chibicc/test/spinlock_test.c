#include "third_party/chibicc/test/test.h"

#define SPINLOCK(lock)                                \
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

#define SPUNLOCK(lock) __sync_lock_release(lock)

////////////////////////////////////////////////////////////////////////////////

#define SPINLOCK2(lock)                                           \
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

#define SPUNLOCK2(lock) __sync_lock_release(lock)

////////////////////////////////////////////////////////////////////////////////

_Alignas(64) char lock;

main() {
  int x, y;

  ASSERT(0, lock);
  SPINLOCK(&lock);
  ASSERT(1, lock);
  SPUNLOCK(&lock);
  ASSERT(0, lock);

  ASSERT(0, lock);
  SPINLOCK2(&lock);
  ASSERT(1, lock);
  SPUNLOCK2(&lock);
  ASSERT(0, lock);

  x = 0;
  y = 7;
  ASSERT(0, x);
  ASSERT(7, y);
  __atomic_store(&x, &y, __ATOMIC_RELAXED);
  ASSERT(7, x);
  ASSERT(7, y);

  x = 0;
  y = 7;
  ASSERT(0, x);
  ASSERT(7, y);
  __atomic_store(&x, &y, __ATOMIC_SEQ_CST);
  ASSERT(7, x);
  ASSERT(7, y);

  x = 5;
  y = __atomic_test_and_set(&x, __ATOMIC_SEQ_CST);
  ASSERT(1, x);
  ASSERT(5, y);

  x = 5;
  __atomic_clear(&x, __ATOMIC_SEQ_CST);
  ASSERT(0, x);

  //
}
