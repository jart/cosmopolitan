#ifndef LIBC_ISYSTEM_STDNORETURN_H_
#define LIBC_ISYSTEM_STDNORETURN_H_

#if !(__ASSEMBLER__ + __LINKER__ + 0)
#if __STDC_VERSION__ + 0 >= 201112
COSMOPOLITAN_C_START_

#define noreturn _Noreturn

COSMOPOLITAN_C_END_

#endif /* C11 */
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#endif
