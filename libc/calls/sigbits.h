#ifndef COSMOPOLITAN_LIBC_CALLS_SIGBITS_H_
#define COSMOPOLITAN_LIBC_CALLS_SIGBITS_H_
#include "libc/bits/bits.h"
#include "libc/bits/progn.h"
#include "libc/str/str.h"
#include "libc/calls/struct/sigset.h"
#include "libc/sysv/errfuns.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct sigset;

int sigaddset(struct sigset *, int) paramsnonnull();
int sigdelset(struct sigset *, int) paramsnonnull();
int sigemptyset(struct sigset *) paramsnonnull();
int sigfillset(struct sigset *) paramsnonnull();
int sigismember(const struct sigset *, int) paramsnonnull() nosideeffect;

#define sigemptyset(SET)                     \
  ({                                         \
    memset((SET), 0, sizeof(struct sigset)); \
    0;                                       \
  })

#define sigfillset(SET)                       \
  ({                                          \
    memset((SET), -1, sizeof(struct sigset)); \
    0;                                        \
  })

#define sigaddset(SET, SIG)                                               \
  ({                                                                      \
    int Sig = (SIG);                                                      \
    __IsValidSignal(Sig) ? PROGN(bts((SET)->sig, Sig - 1), 0) : einval(); \
  })

#define sigdelset(SET, SIG)                                               \
  ({                                                                      \
    int Sig = (SIG);                                                      \
    __IsValidSignal(Sig) ? PROGN(btr((SET)->sig, Sig - 1), 0) : einval(); \
  })

#define sigismember(SET, SIG)                                  \
  ({                                                           \
    int Sig = (SIG);                                           \
    __IsValidSignal(Sig) ? bt((SET)->sig, Sig - 1) : einval(); \
  })

#define __IsValidSignal(SIG) (1 <= (SIG) && (SIG) <= NSIG)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_SIGBITS_H_ */
