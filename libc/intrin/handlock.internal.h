#ifndef COSMOPOLITAN_LIBC_INTRIN_HANDLOCK_INTERNAL_H_
#define COSMOPOLITAN_LIBC_INTRIN_HANDLOCK_INTERNAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void __hand_wipe(void);
void __hand_rlock(void);
void __hand_runlock(void);
void __hand_lock(void);
void __hand_unlock(void);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_HANDLOCK_INTERNAL_H_ */
