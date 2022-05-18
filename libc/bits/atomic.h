#ifndef COSMOPOLITAN_LIBC_BITS_ATOMIC_H_
#define COSMOPOLITAN_LIBC_BITS_ATOMIC_H_
#include "libc/atomic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/**
 * @fileoverview Cosmopolitan C11 Atomics Library
 *
 *   - Forty-two different ways to say MOV.
 *   - Fourteen different ways to say XCHG.
 *   - Twenty different ways to say LOCK CMPXCHG.
 *
 * It's a lower level programming language than assembly!
 *
 * @see libc/atomic.h
 */

#define memory_order         int
#define memory_order_relaxed 0
#define memory_order_consume 1
#define memory_order_acquire 2
#define memory_order_release 3
#define memory_order_acq_rel 4
#define memory_order_seq_cst 5

#define ATOMIC_VAR_INIT(value)   (value)
#define atomic_is_lock_free(obj) ((void)(obj), sizeof(obj) <= sizeof(void *))

#define atomic_flag      atomic_bool
#define ATOMIC_FLAG_INIT ATOMIC_VAR_INIT(0)
#define atomic_flag_test_and_set_explicit(x, order) \
  atomic_exchange_explicit(x, 1, order)
#define atomic_flag_clear_explicit(x, order) atomic_store_explicit(x, 0, order)

#define atomic_compare_exchange_strong(pObject, pExpected, desired) \
  atomic_compare_exchange_strong_explicit(                          \
      pObject, pExpected, desired, memory_order_seq_cst, memory_order_seq_cst)
#define atomic_compare_exchange_weak(pObject, pExpected, desired) \
  atomic_compare_exchange_weak_explicit(                          \
      pObject, pExpected, desired, memory_order_seq_cst, memory_order_seq_cst)
#define atomic_exchange(pObject, desired) \
  atomic_exchange_explicit(pObject, desired, memory_order_seq_cst)
#define atomic_fetch_add(pObject, operand) \
  atomic_fetch_add_explicit(pObject, operand, memory_order_seq_cst)
#define atomic_fetch_and(pObject, operand) \
  atomic_fetch_and_explicit(pObject, operand, memory_order_seq_cst)
#define atomic_fetch_or(pObject, operand) \
  atomic_fetch_or_explicit(pObject, operand, memory_order_seq_cst)
#define atomic_fetch_sub(pObject, operand) \
  atomic_fetch_sub_explicit(pObject, operand, memory_order_seq_cst)
#define atomic_fetch_xor(pObject, operand) \
  atomic_fetch_xor_explicit(pObject, operand, memory_order_seq_cst)
#define atomic_load(pObject) atomic_load_explicit(pObject, memory_order_seq_cst)
#define atomic_store(pObject, desired) \
  atomic_store_explicit(pObject, desired, memory_order_seq_cst)
#define atomic_flag_test_and_set(x) \
  atomic_flag_test_and_set_explicit(x, memory_order_seq_cst)
#define atomic_flag_clear(x) atomic_flag_clear_explicit(x, memory_order_seq_cst)

#if defined(__CLANG_ATOMIC_BOOL_LOCK_FREE)
#include "libc/bits/atomic-clang.h"
#elif (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 407
#include "libc/bits/atomic-gcc47.h"
#else
#include "libc/bits/atomic-gcc.h"
#endif

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_ATOMIC_H_ */
