#include "third_party/chibicc/test/test.h"

_Atomic(int) lock;

main() {
  int x;

  ASSERT(0, __atomic_exchange_n(&lock, 1, __ATOMIC_SEQ_CST));
  __atomic_load(&lock, &x, __ATOMIC_SEQ_CST);
  ASSERT(1, x);
  ASSERT(1, __atomic_exchange_n(&lock, 2, __ATOMIC_SEQ_CST));
  ASSERT(2, __atomic_exchange_n(&lock, 3, __ATOMIC_SEQ_CST));
  ASSERT(3, __atomic_load_n(&lock, __ATOMIC_SEQ_CST));
  __atomic_store_n(&lock, 0, __ATOMIC_SEQ_CST);
  ASSERT(0, __atomic_fetch_xor(&lock, 3, __ATOMIC_SEQ_CST));
  ASSERT(3, __atomic_fetch_xor(&lock, 1, __ATOMIC_SEQ_CST));
  ASSERT(2, __atomic_load_n(&lock, __ATOMIC_SEQ_CST));

  // CAS success #1
  x = 2;
  ASSERT(1, __atomic_compare_exchange_n(&lock, &x, 3, 0, __ATOMIC_SEQ_CST,
                                        __ATOMIC_SEQ_CST));
  ASSERT(2, x);
  ASSERT(3, lock);

  // CAS success #2
  x = 3;
  ASSERT(1, __atomic_compare_exchange_n(&lock, &x, 4, 0, __ATOMIC_SEQ_CST,
                                        __ATOMIC_SEQ_CST));
  ASSERT(3, x);
  ASSERT(4, lock);

  // CAS fail
  x = 3;
  ASSERT(0, __atomic_compare_exchange_n(&lock, &x, 7, 0, __ATOMIC_SEQ_CST,
                                        __ATOMIC_SEQ_CST));
  ASSERT(4, x);
  ASSERT(4, lock);

  // CAS success #3
  x = 4;
  ASSERT(1, __atomic_compare_exchange_n(&lock, &x, 31337, 0, __ATOMIC_SEQ_CST,
                                        __ATOMIC_SEQ_CST));
  ASSERT(4, x);
  ASSERT(31337, lock);

  // xadd
  ASSERT(31337, __atomic_fetch_add(&lock, 31337, __ATOMIC_SEQ_CST));
  ASSERT(62674, lock);
  ASSERT(62674, __atomic_fetch_sub(&lock, 31337, __ATOMIC_SEQ_CST));
  ASSERT(31337, lock);

  //
}
