#ifndef COSMOPOLITAN_LIBC_PROC_DESCRIBEFDS_INTERNAL_H_
#define COSMOPOLITAN_LIBC_PROC_DESCRIBEFDS_INTERNAL_H_
#include "libc/calls/struct/fd.internal.h"
#include "libc/nt/struct/startupinfo.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

bool __is_cloexec(const struct Fd *);
void __undescribe_fds(int64_t, int64_t *, uint32_t);
char *__describe_fds(const struct Fd *, size_t, struct NtStartupInfo *, int64_t,
                     int64_t **, uint32_t *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_PROC_DESCRIBEFDS_INTERNAL_H_ */
