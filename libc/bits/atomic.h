#ifndef COSMOPOLITAN_LIBC_BITS_ATOMIC_H_
#define COSMOPOLITAN_LIBC_BITS_ATOMIC_H_
#include "libc/bits/bits.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/**
 * @fileoverview C11 version of The Cosmopolitan Atomics Library.
 *
 *   - Forty-two different ways to say MOV.
 *   - Fourteen different ways to say XCHG.
 *   - Twenty different ways to say LOCK CMPXCHG.
 *
 * Living proof high-level languages can be lower-level than assembly.
 */

#define memory_order         int
#define memory_order_relaxed 0
#define memory_order_consume 1
#define memory_order_acquire 2
#define memory_order_release 3
#define memory_order_acq_rel 4
#define memory_order_seq_cst 5

#define atomic_flag            struct AtomicFlag
#define atomic_flag_clear(PTR) atomic_store((PTR)->__cacheline, 0)
#define atomic_flag_test_and_set(PTR)        \
  ({                                         \
    uint32_t ax = 0;                         \
    lockcmpxchg((PTR)->__cacheline, &ax, 1); \
  })
#define atomic_init(PTR, VAL)                         atomic_store(PTR, VAL)
#define atomic_exchange(PTR, VAL)                     lockxchg(PTR, &(VAL))
#define atomic_compare_exchange_strong(X, Y, Z)       lockcmpxchg(X, Y, Z)
#define atomic_compare_exchange_weak(X, Y, Z)         lockcmpxchg(X, Y, Z)
#define atomic_load_explicit(PTR, ORDER)              atomic_load(PTR)
#define atomic_store_explicit(PTR, VAL, ORDER)        atomic_store(PTR, VAL)
#define atomic_flag_clear_explicit(PTR, ORDER)        atomic_store(PTR, 0)
#define atomic_exchange_explicit(PTR, VAL, ORDER)     lockxchg(PTR, &(VAL))
#define atomic_flag_test_and_set_explicit(PTR, ORDER) lockcmpxchg(PTR, 0, 1)
#define atomic_compare_exchange_strong_explicit(X, Y, Z, S, F) \
  lockcmpxchg(X, Y, Z)
#define atomic_compare_exchange_weak_explicit(X, Y, Z, S, F) \
  lockcmpxchg(X, Y, Z)

struct AtomicFlag {
  uint32_t __cacheline[16]; /* Intel V.O §9.4.6 */
} forcealign(64);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_ATOMIC_H_ */
