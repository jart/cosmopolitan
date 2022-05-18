#ifndef COSMOPOLITAN_LIBC_BITS_ATOMIC_GCC47_H_
#define COSMOPOLITAN_LIBC_BITS_ATOMIC_GCC47_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § atomics » gcc 4.7+                                        ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

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

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_ATOMIC_GCC47_H_ */
