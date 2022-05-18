#ifndef COSMOPOLITAN_LIBC_BITS_ATOMIC_GCC_H_
#define COSMOPOLITAN_LIBC_BITS_ATOMIC_GCC_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § atomics » old gnu                                         ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define atomic_init(obj, value)    ((void)(*(obj) = (value)))
#define atomic_thread_fence(order) __sync_synchronize()
#define atomic_signal_fence(order) __asm__ volatile("" ::: "memory")
#define __atomic_apply_stride(object, operand) \
  (((__typeof__(__atomic_val(object)))0) + (operand))
#define atomic_compare_exchange_strong_explicit(object, expected, desired, \
                                                success, failure)          \
  __extension__({                                                          \
    __typeof__(expected) __ep = (expected);                                \
    __typeof__(*__ep) __e = *__ep;                                         \
    (void)(success);                                                       \
    (void)(failure);                                                       \
    (_Bool)((*__ep = __sync_val_compare_and_swap(object, __e, desired)) == \
            __e);                                                          \
  })
#define atomic_compare_exchange_weak_explicit(object, expected, desired,      \
                                              success, failure)               \
  atomic_compare_exchange_strong_explicit(object, expected, desired, success, \
                                          failure)
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
    __sync_lock_test_and_set(&__atomic_val(__o), __d);   \
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

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_ATOMIC_GCC_H_ */
