#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGACTION_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGACTION_H_
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/ucontext.h"
#include "libc/dce.h"
#include "libc/sysv/consts/sig.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

typedef void (*sighandler_t)(int);
typedef void (*sigaction_f)(int, struct siginfo *,
                            void * /*struct ucontext **/);

struct sigaction { /* cosmo abi */
  union {
    sighandler_t sa_handler;
    sigaction_f sa_sigaction;
  };
  uint64_t sa_flags;
  void (*sa_restorer)(void);
  struct sigset sa_mask;
};

sighandler_t signal(int, sighandler_t);
int sigaction(int, const struct sigaction *, struct sigaction *);

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)

void _init_onntconsoleevent(void);
void _init_wincrash(void);
void _check_sigwinch();

#ifndef __SIGACTION_YOINK
#define __SIGACTION_YOINK(SIG)             \
  do {                                     \
    if (SupportsWindows()) {               \
      if (__builtin_constant_p(SIG)) {     \
        switch (SIG) {                     \
          case SIGINT:                     \
          case SIGQUIT:                    \
          case SIGHUP:                     \
          case SIGTERM:                    \
            YOINK(_init_onntconsoleevent); \
            break;                         \
          case SIGTRAP:                    \
          case SIGILL:                     \
          case SIGSEGV:                    \
          case SIGABRT:                    \
          case SIGFPE:                     \
            YOINK(_init_wincrash);         \
            break;                         \
          case SIGWINCH:                   \
            YOINK(_check_sigwinch);        \
            break;                         \
          default:                         \
            break;                         \
        }                                  \
      } else {                             \
        YOINK(_init_onntconsoleevent);     \
        YOINK(_init_wincrash);             \
        YOINK(_check_sigwinch);            \
      }                                    \
    }                                      \
  } while (0)
#endif

#define sigaction(SIG, ACT, OLD) \
  ({                             \
    __SIGACTION_YOINK(SIG);      \
    sigaction(SIG, (ACT), OLD);  \
  })

#define signal(SIG, HAND)   \
  ({                        \
    __SIGACTION_YOINK(SIG); \
    signal(SIG, HAND);      \
  })

#endif /* GNU && !ANSI */

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGACTION_H_ */
