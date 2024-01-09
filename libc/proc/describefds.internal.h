#ifndef COSMOPOLITAN_LIBC_PROC_DESCRIBEFDS_INTERNAL_H_
#define COSMOPOLITAN_LIBC_PROC_DESCRIBEFDS_INTERNAL_H_
#include "libc/calls/struct/fd.internal.h"
#include "libc/nt/struct/startupinfo.h"
COSMOPOLITAN_C_START_

bool __is_cloexec(const struct Fd *) libcesque;
void __undescribe_fds(int64_t, int64_t *, uint32_t) libcesque;
char *__describe_fds(const struct Fd *, size_t, struct NtStartupInfo *, int64_t,
                     int64_t **, uint32_t *) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_PROC_DESCRIBEFDS_INTERNAL_H_ */
