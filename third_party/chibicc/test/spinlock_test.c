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

_Alignas(64) char lock;

main() {
  ASSERT(0, lock);
  SPINLOCK(&lock);
  ASSERT(1, lock);
  SPUNLOCK(&lock);
  ASSERT(0, lock);
}
