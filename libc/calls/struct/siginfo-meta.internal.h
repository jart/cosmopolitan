#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGINFO_META_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGINFO_META_INTERNAL_H_
#include "libc/calls/struct/siginfo-freebsd.internal.h"
#include "libc/calls/struct/siginfo-netbsd.internal.h"
#include "libc/calls/struct/siginfo-openbsd.internal.h"
#include "libc/calls/struct/siginfo-xnu.internal.h"
#include "libc/calls/struct/siginfo.h"
COSMOPOLITAN_C_START_

union siginfo_meta {
  struct siginfo linux;
  struct siginfo_xnu xnu;
  struct siginfo_freebsd freebsd;
  struct siginfo_openbsd openbsd;
  struct siginfo_netbsd netbsd;
};

void __siginfo2cosmo(struct siginfo *, const union siginfo_meta *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGINFO_META_INTERNAL_H_ */
