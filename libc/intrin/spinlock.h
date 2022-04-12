#ifndef COSMOPOLITAN_LIBC_INTRIN_SPINLOCK_H_
#define COSMOPOLITAN_LIBC_INTRIN_SPINLOCK_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/* "Place each synchronization variable alone,
    separated by 128 bytes or in a separate cache line."
                   ──Intel Optimization Manual §8.3.1 */
struct cthread_spinlock_t {
  bool x;
  int owner;
  char __ignore[128 - 1 - 4];
} forcealign(128);

typedef struct cthread_spinlock_t cthread_spinlock_t;

void cthread_spinlock(cthread_spinlock_t *) dontthrow;
void cthread_spunlock(cthread_spinlock_t *) dontthrow;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_SPINLOCK_H_ */
