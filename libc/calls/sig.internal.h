#ifndef COSMOPOLITAN_LIBC_CALLS_SIGNALS_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_SIGNALS_INTERNAL_H_
#include "libc/calls/struct/sigset.h"

#define __SIG_QUEUE_LENGTH        8
#define __SIG_POLLING_INTERVAL_MS 50

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

bool __sig_check(bool) hidden;
int __sig_add(int, int) hidden;
sigset_t __sig_mask(const sigset_t *) hidden;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_SIGNALS_INTERNAL_H_ */
