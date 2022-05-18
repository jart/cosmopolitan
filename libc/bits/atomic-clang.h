#ifndef COSMOPOLITAN_LIBC_BITS_ATOMIC_CLANG_H_
#define COSMOPOLITAN_LIBC_BITS_ATOMIC_CLANG_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § atomics » clang                                           ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

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

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_ATOMIC_CLANG_H_ */
