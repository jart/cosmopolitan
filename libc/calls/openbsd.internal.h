#ifndef COSMOPOLITAN_LIBC_COMPAT_OPENBSD_H_
#define COSMOPOLITAN_LIBC_COMPAT_OPENBSD_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § system calls » openbsd                                    ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

typedef unsigned char u_char;

int pledge(const char *promises, const char *execpromises);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_COMPAT_OPENBSD_H_ */
