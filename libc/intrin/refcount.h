#ifndef COSMOPOLITAN_LIBC_INTRIN_REFCOUNT_H_
#define COSMOPOLITAN_LIBC_INTRIN_REFCOUNT_H_

#define _incref(x) __atomic_fetch_add(x, 1, __ATOMIC_RELAXED)
#define _decref(x) __atomic_sub_fetch(x, 1, __ATOMIC_SEQ_CST)

#endif /* COSMOPOLITAN_LIBC_INTRIN_REFCOUNT_H_ */
