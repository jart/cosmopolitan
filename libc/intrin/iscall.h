#ifndef COSMOPOLITAN_LIBC_INTRIN_ISCALL_H_
#define COSMOPOLITAN_LIBC_INTRIN_ISCALL_H_
COSMOPOLITAN_C_START_

// returns true if `p` is preceded by x86 call instruction
// this is actually impossible to do but we'll do our best
int __is_call(const unsigned char *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_INTRIN_ISCALL_H_ */
