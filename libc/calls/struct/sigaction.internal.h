#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGACTION_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGACTION_INTERNAL_H_
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/mem/alloca.h"
COSMOPOLITAN_C_START_

struct sigaction_linux {
  void *sa_handler;
  uint64_t sa_flags;
  void *sa_restorer;
  uint32_t sa_mask[2];
};

struct sigaction_freebsd {
  void *sa_handler;
  uint32_t sa_flags;
  uint32_t sa_mask[4];
};

struct sigaction_openbsd {
  void *sa_handler;
  uint32_t sa_mask[1];
  uint32_t sa_flags;
};

struct sigaction_netbsd {
  void *sa_handler;
  uint32_t sa_mask[4];
  uint32_t sa_flags;
};

struct sigaction_silicon {
  void *sa_handler;
  uint32_t sa_mask[1];
  uint32_t sa_flags;
};

struct sigaction_xnu_in {
  void *sa_handler;
  void *sa_restorer;
  uint32_t sa_mask[1];
  uint32_t sa_flags;
};

struct sigaction_xnu_out {
  void *sa_handler;
  uint32_t sa_mask[1];
  uint32_t sa_flags;
};

union metasigaction {
  struct sigaction cosmo;
  struct sigaction_linux linux;
  struct sigaction_freebsd freebsd;
  struct sigaction_openbsd openbsd;
  struct sigaction_netbsd netbsd;
  struct sigaction_silicon silicon;
  struct sigaction_xnu_in xnu_in;
  struct sigaction_xnu_out xnu_out;
};

void __sigenter_xnu(int, struct siginfo *, void *);
void __sigenter_wsl(int, struct siginfo *, void *);
void __sigenter_netbsd(int, struct siginfo *, void *);
void __sigenter_freebsd(int, struct siginfo *, void *);
void __sigenter_openbsd(int, struct siginfo *, void *);

const char *DescribeSigaction(char[256], int, const struct sigaction *);
#define DescribeSigaction(rc, sa) DescribeSigaction(alloca(256), rc, sa)

void _init_onntconsoleevent(void);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGACTION_INTERNAL_H_ */
