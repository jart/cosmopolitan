#ifndef NSYNC_ATOMIC_INTERNAL_H_
#define NSYNC_ATOMIC_INTERNAL_H_
#include "libc/intrin/atomic.h"
#include "libc/intrin/cmpxchg.h"
#include "third_party/nsync/atomic.h"
COSMOPOLITAN_C_START_

/* Atomic operations on nsync_atomic_uint32_ quantities
   CAS, load, and store.

   Normally, these are used only on nsync_atomic_uint32_ values, but on
   Linux they may be invoked on int values, because futexes operate on
   int values. A compile-time check in the futex code ensures that both
   int and nsync_atomic_uint32_ are 32 bits.

   Memory barriers:

           Operations with the suffixes _ACQ and _RELACQ ensure that the
           operation appears to complete before other memory operations
           subsequently performed by the same thread, as seen by other
           threads. (In the case of ATM_CAS_ACQ, this applies only if
           the operation returns a non-zero value.)

           Operations with the suffixes _REL and _RELACQ ensure that the
           operation appears to complete after other memory operations
           previously performed by the same thread, as seen by other
           threads. (In the case of ATM_CAS_REL, this applies only if
           the operation returns a non-zero value.)

   // Atomically,
   //   int ATM_CAS (nsync_atomic_uint32_ *p,
   //                uint32_t old_value, uint32_t new_value) {
   //		if (*p == old_value) {
   //			*p = new_value;
   //			return (some-non-zero-value);
   //		} else {
   //			return (0);
   //		}
   //	}
   // *_ACQ, *_REL, *_RELACQ variants are available,
   // with the barrier semantics described above.
   int ATM_CAS (nsync_atomic_uint32_ *p, uint32_t old_value,
                uint32_t new_value);

   // Atomically,
   //     uint32_t ATM_LOAD (nsync_atomic_uint32_ *p) { return (*p); }
   // A *_ACQ variant is available,
   // with the barrier semantics described above.
   uint32_t ATM_LOAD (nsync_atomic_uint32_ *p);

   // Atomically,
   //     void ATM_STORE (nsync_atomic_uint32_ *p, uint32_t value) {
   //       *p = value;
   //     }
   // A *_REL variant is available,
   // with the barrier semantics described above.
   void ATM_STORE (nsync_atomic_uint32_ *p, uint32_t value);
 */

static inline int atm_cas_nomb_u32_(nsync_atomic_uint32_ *p, uint32_t o,
                                    uint32_t n) {
  return atomic_compare_exchange_strong_explicit(NSYNC_ATOMIC_UINT32_PTR_(p),
                                                 &o, n, memory_order_relaxed,
                                                 memory_order_relaxed);
}

static inline int atm_cas_acq_u32_(nsync_atomic_uint32_ *p, uint32_t o,
                                   uint32_t n) {
  return atomic_compare_exchange_strong_explicit(NSYNC_ATOMIC_UINT32_PTR_(p),
                                                 &o, n, memory_order_acquire,
                                                 memory_order_relaxed);
}

static inline int atm_cas_rel_u32_(nsync_atomic_uint32_ *p, uint32_t o,
                                   uint32_t n) {
  return atomic_compare_exchange_strong_explicit(NSYNC_ATOMIC_UINT32_PTR_(p),
                                                 &o, n, memory_order_release,
                                                 memory_order_relaxed);
}

static inline int atm_cas_relacq_u32_(nsync_atomic_uint32_ *p, uint32_t o,
                                      uint32_t n) {
  return atomic_compare_exchange_strong_explicit(NSYNC_ATOMIC_UINT32_PTR_(p),
                                                 &o, n, memory_order_acq_rel,
                                                 memory_order_relaxed);
}

static inline int atm_cas_seqcst_u32_(nsync_atomic_uint32_ *p, uint32_t o,
                                      uint32_t n) {
  return atomic_compare_exchange_strong_explicit(NSYNC_ATOMIC_UINT32_PTR_(p),
                                                 &o, n, memory_order_seq_cst,
                                                 memory_order_relaxed);
}

#define ATM_CAS_HELPER_(barrier, p, o, n) \
  (atm_cas_##barrier##_u32_((p), (o), (n)))

#define ATM_CAS(p, o, n)        ATM_CAS_HELPER_(nomb, (p), (o), (n))
#define ATM_CAS_ACQ(p, o, n)    ATM_CAS_HELPER_(acq, (p), (o), (n))
#define ATM_CAS_REL(p, o, n)    ATM_CAS_HELPER_(rel, (p), (o), (n))
#define ATM_CAS_RELACQ(p, o, n) ATM_CAS_HELPER_(relacq, (p), (o), (n))
#define ATM_CAS_SEQCST(p, o, n) ATM_CAS_HELPER_(seqcst, (p), (o), (n))

/* Need a cast to remove "const" from some uses. */
#define ATM_LOAD(p)                                                          \
  (atomic_load_explicit((nsync_atomic_uint32_ *)NSYNC_ATOMIC_UINT32_PTR_(p), \
                        memory_order_relaxed))
#define ATM_LOAD_ACQ(p)                                                      \
  (atomic_load_explicit((nsync_atomic_uint32_ *)NSYNC_ATOMIC_UINT32_PTR_(p), \
                        memory_order_acquire))

#define ATM_STORE(p, v)                                    \
  (atomic_store_explicit(NSYNC_ATOMIC_UINT32_PTR_(p), (v), \
                         memory_order_relaxed))
#define ATM_STORE_REL(p, v)                                \
  (atomic_store_explicit(NSYNC_ATOMIC_UINT32_PTR_(p), (v), \
                         memory_order_release))

COSMOPOLITAN_C_END_
#endif /* NSYNC_ATOMIC_INTERNAL_H_ */
