#ifndef COSMOPOLITAN_LIBC_CALLS_NTMAGICPATHS_H_
#define COSMOPOLITAN_LIBC_CALLS_NTMAGICPATHS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct NtMagicPaths {
#define TAB(NAME, STRING) char NAME[sizeof(STRING)];
#include "libc/calls/ntmagicpaths.inc"
#undef TAB
};

hidden extern const struct NtMagicPaths kNtMagicPaths;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_NTMAGICPATHS_H_ */
