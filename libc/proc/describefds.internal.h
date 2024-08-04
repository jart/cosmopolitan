#ifndef COSMOPOLITAN_LIBC_PROC_DESCRIBEFDS_INTERNAL_H_
#define COSMOPOLITAN_LIBC_PROC_DESCRIBEFDS_INTERNAL_H_
#include "libc/intrin/fds.h"
#include "libc/nt/struct/startupinfo.h"
COSMOPOLITAN_C_START_

#define CURSOR_ADDRESS_FLAG 0x4000000000000000

bool __is_cloexec(const struct Fd *) libcesque;
void __undescribe_fds(int64_t, int64_t *, uint32_t) libcesque;
char *__describe_fds(const struct Fd *, size_t, struct NtStartupInfo *, int64_t,
                     int64_t **, uint32_t *) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_PROC_DESCRIBEFDS_INTERNAL_H_ */
