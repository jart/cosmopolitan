#ifndef COSMOPOLITAN_LIBC_CALLS_WINCRASH_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_WINCRASH_INTERNAL_H_
#include "libc/nt/struct/ntexceptionpointers.h"
#include "libc/nt/struct/overlapped.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

unsigned __wincrash_nt(struct NtExceptionPointers *);
struct NtOverlapped *_offset2overlap(int64_t, int64_t,
                                     struct NtOverlapped *) _Hide;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_WINCRASH_INTERNAL_H_ */
