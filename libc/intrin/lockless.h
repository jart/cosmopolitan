#ifndef COSMOPOLITAN_LIBC_INTRIN_LOCKLESS_H_
#define COSMOPOLITAN_LIBC_INTRIN_LOCKLESS_H_
#include "libc/atomic.h"
#include "libc/intrin/atomic.h"
COSMOPOLITAN_C_START_

// lockless memory transactions
//
// - one writer
// - many readers
// - generation is monotonic
// - even numbers mean memory is ready
// - odd numbers mean memory is actively being changed
// - always use acquire semantics inside your read transaction
//
// let's say you want to be able to atomically read and write to 128-bit
// values, but you've only got a 64-bit system. if you expect that it'll
// frequently written, then you should use a mutex. but if you expect it
// to be frequently read and rarely written, then it's possible to do it
// without a mutex; in fact you don't even need the x86 lock instruction
// prefix; all that is required is a series of carefully ordered mov ops
// which are designed to exploit the strong ordering of the architecture

static inline unsigned lockless_write_begin(atomic_uint* genptr) {
  unsigned gen = atomic_load_explicit(genptr, memory_order_acquire);
  atomic_store_explicit(genptr, gen + 1, memory_order_release);
  return gen;
}

static inline void lockless_write_end(atomic_uint* genptr, unsigned gen) {
  atomic_store_explicit(genptr, gen + 2, memory_order_release);
}

static inline unsigned lockless_read_begin(atomic_uint* genptr) {
  return atomic_load_explicit(genptr, memory_order_acquire);
}

static inline bool lockless_read_end(atomic_uint* genptr, unsigned* want) {
  unsigned gen1 = *want;
  unsigned gen2 = atomic_load_explicit(genptr, memory_order_acquire);
  unsigned is_being_actively_changed = gen1 & 1;
  unsigned we_lost_race_with_writers = gen1 ^ gen2;
  if (!(is_being_actively_changed | we_lost_race_with_writers))
    return true;
  *want = gen2;
  return false;
}

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_INTRIN_LOCKLESS_H_ */
