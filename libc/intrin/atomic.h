#ifndef COSMOPOLITAN_LIBC_BITS_ATOMIC_H_
#define COSMOPOLITAN_LIBC_BITS_ATOMIC_H_
#include "libc/atomic.h"

/**
 * @fileoverview Cosmopolitan C11 Atomics Library
 *
 * - Forty-two different ways to say MOV.
 * - Fourteen different ways to say XCHG.
 * - Twenty different ways to say LOCK CMPXCHG.
 *
 * @see libc/atomic.h
 */

typedef int memory_order;

enum {
  memory_order_relaxed,
  memory_order_consume,
  memory_order_acquire,
  memory_order_release,
  memory_order_acq_rel,
  memory_order_seq_cst,
};

#define ATOMIC_VAR_INIT(...)     __VA_ARGS__
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

#define atomic_init(obj, value)    __c11_atomic_init(obj, value)
#define atomic_thread_fence(order) __c11_atomic_thread_fence(order)
#define atomic_signal_fence(order) __c11_atomic_signal_fence(order)
#define atomic_compare_exchange_strong_explicit(object, expected, desired, \
                                                success, failure)          \
  __c11_atomic_compare_exchange_strong(object, expected, desired, success, \
                                       failure)
#define atomic_compare_exchange_weak_explicit(object, expected, desired, \
                                              success, failure)          \
  __c11_atomic_compare_exchange_weak(object, expected, desired, success, \
                                     failure)
#define atomic_exchange_explicit(object, desired, order) \
  __c11_atomic_exchange(object, desired, order)
#define atomic_fetch_add_explicit(object, operand, order) \
  __c11_atomic_fetch_add(object, operand, order)
#define atomic_fetch_and_explicit(object, operand, order) \
  __c11_atomic_fetch_and(object, operand, order)
#define atomic_fetch_or_explicit(object, operand, order) \
  __c11_atomic_fetch_or(object, operand, order)
#define atomic_fetch_sub_explicit(object, operand, order) \
  __c11_atomic_fetch_sub(object, operand, order)
#define atomic_fetch_xor_explicit(object, operand, order) \
  __c11_atomic_fetch_xor(object, operand, order)
#define atomic_load_explicit(object, order) __c11_atomic_load(object, order)
#define atomic_store_explicit(object, desired, order) \
  __c11_atomic_store(object, desired, order)

#elif (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 407

#define atomic_init(obj, value)    ((void)(*(obj) = (value)))
#define atomic_thread_fence(order) __atomic_thread_fence(order)
#define atomic_signal_fence(order) __atomic_signal_fence(order)
#define atomic_compare_exchange_strong_explicit(pObject, pExpected, desired, \
                                                success, failure)            \
  __atomic_compare_exchange_n(pObject, pExpected, desired, 0, success, failure)
#define atomic_compare_exchange_weak_explicit(pObject, pExpected, desired, \
                                              success, failure)            \
  __atomic_compare_exchange_n(pObject, pExpected, desired, 1, success, failure)
#define atomic_exchange_explicit(pObject, desired, order) \
  __atomic_exchange_n(pObject, desired, order)
#define atomic_fetch_add_explicit(pObject, operand, order) \
  __atomic_fetch_add(pObject, operand, order)
#define atomic_fetch_and_explicit(pObject, operand, order) \
  __atomic_fetch_and(pObject, operand, order)
#define atomic_fetch_or_explicit(pObject, operand, order) \
  __atomic_fetch_or(pObject, operand, order)
#define atomic_fetch_sub_explicit(pObject, operand, order) \
  __atomic_fetch_sub(pObject, operand, order)
#define atomic_fetch_xor_explicit(pObject, operand, order) \
  __atomic_fetch_xor(pObject, operand, order)
#define atomic_load_explicit(pObject, order) __atomic_load_n(pObject, order)
#define atomic_store_explicit(pObject, desired, order) \
  __atomic_store_n(pObject, desired, order)

#elif (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 401

#define atomic_init(obj, value)    ((void)(*(obj) = (value)))
#define atomic_thread_fence(order) __sync_synchronize()
#define atomic_signal_fence(order) __asm__ volatile("" ::: "memory")
#define __atomic_apply_stride(object, operand) \
  (((__typeof__(*(object)))0) + (operand))
#define atomic_compare_exchange_strong_explicit(object, expected, desired,    \
                                                success_order, failure_order) \
  __extension__({                                                             \
    __typeof__(expected) __ep = (expected);                                   \
    __typeof__(*__ep) __e = *__ep;                                            \
    (void)(success_order);                                                    \
    (void)(failure_order);                                                    \
    (*__ep = __sync_val_compare_and_swap(object, __e, desired)) == __e;       \
  })
#define atomic_compare_exchange_weak_explicit(object, expected, desired,    \
                                              success_order, failure_order) \
  atomic_compare_exchange_strong_explicit(object, expected, desired,        \
                                          success_order, failure_order)
#if __has_builtin(__sync_swap)
#define atomic_exchange_explicit(object, desired, order) \
  ((void)(order), __sync_swap(object, desired))
#else
#define atomic_exchange_explicit(object, desired, order) \
  __extension__({                                        \
    __typeof__(object) __o = (object);                   \
    __typeof__(desired) __d = (desired);                 \
    (void)(order);                                       \
    __sync_synchronize();                                \
    __sync_lock_test_and_set(__o, __d);                  \
  })
#endif
#define atomic_fetch_add_explicit(object, operand, order) \
  ((void)(order),                                         \
   __sync_fetch_and_add(object, __atomic_apply_stride(object, operand)))
#define atomic_fetch_and_explicit(object, operand, order) \
  ((void)(order), __sync_fetch_and_and(object, operand))
#define atomic_fetch_or_explicit(object, operand, order) \
  ((void)(order), __sync_fetch_and_or(object, operand))
#define atomic_fetch_sub_explicit(object, operand, order) \
  ((void)(order),                                         \
   __sync_fetch_and_sub(object, __atomic_apply_stride(object, operand)))
#define atomic_fetch_xor_explicit(object, operand, order) \
  ((void)(order), __sync_fetch_and_xor(object, operand))
#define atomic_load_explicit(object, order) \
  ((void)(order), __sync_fetch_and_add(object, 0))
#define atomic_store_explicit(object, desired, order) \
  ((void)atomic_exchange_explicit(object, desired, order))

#elif defined(__GNUC__) && defined(__x86__) /* x86 with gcc 4.0 and earlier */

#define atomic_init(obj, value)    ((void)(*(obj) = (value)))
#define atomic_thread_fence(order) __asm__ volatile("mfence" ::: "memory")
#define atomic_signal_fence(order) __asm__ volatile("" ::: "memory")
#define atomic_compare_exchange_strong_explicit(object, expected, desired,    \
                                                success_order, failure_order) \
  __extension__({                                                             \
    char DidIt;                                                               \
    __typeof__(object) IfThing = (object);                                    \
    __typeof__(IfThing) IsEqualToMe = (expected);                             \
    __typeof__(*IfThing) ReplaceItWithMe = (desired), ax;                     \
    (void)(success_order);                                                    \
    (void)(failure_order);                                                    \
    __asm__ volatile("lock cmpxchg\t%3,(%1)\n\t"                              \
                     "setz\t%b0"                                              \
                     : "=q"(DidIt), "=r"(IfThing), "+a"(ax)                   \
                     : "r"(ReplaceItWithMe), "2"(*IsEqualToMe)                \
                     : "memory", "cc");                                       \
    *IsEqualToMe = ax;                                                        \
    DidIt;                                                                    \
  })
#define atomic_compare_exchange_weak_explicit(object, expected, desired,    \
                                              success_order, failure_order) \
  atomic_compare_exchange_strong_explicit(object, expected, desired,        \
                                          success_order, failure_order)
#define atomic_exchange_explicit(object, desired, order)                \
  __extension__({                                                       \
    __typeof__(object) __o = (object);                                  \
    __typeof__(*__o) __d = (desired);                                   \
    (void)(order);                                                      \
    __asm__ volatile("xchg\t%0,%1" : "=r"(__d), "+m"(*__o) : "0"(__d)); \
    __d;                                                                \
  })
#define atomic_fetch_add_explicit(object, operand, order)                    \
  __extension__({                                                            \
    __typeof__(object) __o = (object);                                       \
    __typeof__(*__o) __d = (desired);                                        \
    (void)(order);                                                           \
    __asm__ volatile("lock xadd\t%0,%1" : "=r"(__d), "+m"(*__o) : "0"(__d)); \
    __d;                                                                     \
  })
#define atomic_fetch_sub_explicit(object, operand, order) \
  atomic_fetch_add_explicit(object, -(operand), order)
#define atomic_load_explicit(object, order) \
  atomic_fetch_add_explicit(object, 0, order)
#define atomic_store_explicit(object, desired, order) \
  ((void)atomic_exchange_explicit(object, desired, order))

#else /* non-gcc or old gcc w/o x86 */
#error "atomic operations not supported with this compiler and/or architecture"
#endif

#endif /* COSMOPOLITAN_LIBC_BITS_ATOMIC_H_ */
