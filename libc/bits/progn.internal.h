#ifndef COSMOPOLITAN_LIBC_BITS_PROGN_H_
#define COSMOPOLITAN_LIBC_BITS_PROGN_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/**
 * Evaluates args, returning value of last one.
 *
 * This API comes from LISP.
 */
#define PROGN(...) ({ __VA_ARGS__; })

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_PROGN_H_ */
