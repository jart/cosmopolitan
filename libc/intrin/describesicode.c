/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/dce.h"
#include "libc/fmt/itoa.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"

static bool IsSiUser(int si_code) {
  if (!IsOpenbsd()) {
    return si_code == SI_USER;
  } else {
    return si_code <= 0;
  }
}

static void NameIt(char p[20], const char *s, int si_code) {
  p = stpcpy(p, s);
  FormatInt32(p, si_code);
}

/**
 * Returns symbolic name for siginfo::si_code value.
 */
const char *(DescribeSiCode)(char b[20], int sig, int si_code) {
  NameIt(b, "SI_", si_code);
  if (si_code == SI_QUEUE) {
    strcpy(b + 3, "QUEUE"); /* sent by sigqueue(2) */
  } else if (si_code == SI_TIMER) {
    strcpy(b + 3, "TIMER"); /* sent by setitimer(2) or clock_settime(2) */
  } else if (si_code == SI_TKILL) {
    strcpy(b + 3, "TKILL"); /* sent by tkill(2), etc. */
  } else if (si_code == SI_MESGQ) {
    strcpy(b + 3, "MESGQ"); /* sent by mq_notify(2) */
  } else if (si_code == SI_MESGQ) {
    strcpy(b + 3, "MESGQ"); /* aio completion */
  } else if (si_code == SI_ASYNCNL) {
    strcpy(b + 3, "ASYNCNL"); /* aio completion for dns; the horror */
  } else if (si_code == SI_NOINFO) {
    strcpy(b + 3, "NOINFO"); /* no signal specific info available */
  } else if (si_code == SI_KERNEL) {
    strcpy(b + 3, "KERNEL"); /* sent by kernel */
  } else if (IsSiUser(si_code)) {
    strcpy(b + 3, "USER"); /* sent by kill(2) i.e. from userspace */
  } else if (sig == SIGCHLD) {
    NameIt(b, "CLD_", si_code);
    if (si_code == CLD_EXITED) {
      strcpy(b + 4, "EXITED"); /* child exited */
    } else if (si_code == CLD_KILLED) {
      strcpy(b + 4, "KILLED"); /* child terminated w/o core */
    } else if (si_code == CLD_DUMPED) {
      strcpy(b + 4, "DUMPED"); /* child terminated w/ core */
    } else if (si_code == CLD_TRAPPED) {
      strcpy(b + 4, "TRAPPED"); /* traced child trapped */
    } else if (si_code == CLD_STOPPED) {
      strcpy(b + 4, "STOPPED"); /* child stopped */
    } else if (si_code == CLD_CONTINUED) {
      strcpy(b + 4, "CONTINUED"); /* stopped child continued */
    }
  } else if (sig == SIGTRAP) {
    NameIt(b, "TRAP_", si_code);
    if (si_code == TRAP_BRKPT) {
      strcpy(b + 5, "BRKPT"); /* process breakpoint */
    } else if (si_code == TRAP_TRACE) {
      strcpy(b + 5, "TRACE"); /* process trace trap */
    }
  } else if (sig == SIGSEGV) {
    NameIt(b, "SEGV_", si_code);
    if (si_code == SEGV_MAPERR) {
      strcpy(b + 5, "MAPERR"); /* address not mapped to object */
    } else if (si_code == SEGV_ACCERR) {
      strcpy(b + 5, "ACCERR"); /* invalid permissions for mapped object */
    } else if (si_code == SEGV_PKUERR) {
      strcpy(b + 5, "PKUERR"); /* FreeBSD: x86: PKU violation */
    }
  } else if (sig == SIGFPE) {
    NameIt(b, "FPE_???", si_code);
    if (si_code == FPE_INTDIV) {
      strcpy(b + 4, "INTDIV"); /* integer divide by zero */
    } else if (si_code == FPE_INTOVF) {
      strcpy(b + 4, "INTOVF"); /* integer overflow */
    } else if (si_code == FPE_FLTDIV) {
      strcpy(b + 4, "FLTDIV"); /* floating point divide by zero */
    } else if (si_code == FPE_FLTOVF) {
      strcpy(b + 4, "FLTOVF"); /* floating point overflow */
    } else if (si_code == FPE_FLTUND) {
      strcpy(b + 4, "FLTUND"); /* floating point underflow */
    } else if (si_code == FPE_FLTRES) {
      strcpy(b + 4, "FLTRES"); /* floating point inexact */
    } else if (si_code == FPE_FLTINV) {
      strcpy(b + 4, "FLTINV"); /* invalid floating point operation */
    } else if (si_code == FPE_FLTSUB) {
      strcpy(b + 4, "FLTSUB"); /* subscript out of range */
    }
  } else if (sig == SIGILL) {
    NameIt(b, "ILL_", si_code);
    if (si_code == ILL_ILLOPC) {
      strcpy(b + 4, "ILLOPC"); /* illegal opcode */
    } else if (si_code == ILL_ILLOPN) {
      strcpy(b + 4, "ILLOPN"); /* illegal operand */
    } else if (si_code == ILL_ILLADR) {
      strcpy(b + 4, "ILLADR"); /* illegal addressing mode */
    } else if (si_code == ILL_ILLTRP) {
      strcpy(b + 4, "ILLTRP"); /* illegal trap */
    } else if (si_code == ILL_PRVOPC) {
      strcpy(b + 4, "PRVOPC"); /* privileged opcode */
    } else if (si_code == ILL_PRVREG) {
      strcpy(b + 4, "PRVREG"); /* privileged register */
    } else if (si_code == ILL_COPROC) {
      strcpy(b + 4, "COPROC"); /* coprocessor error */
    } else if (si_code == ILL_BADSTK) {
      strcpy(b + 4, "BADSTK"); /* internal stack error */
    }
  } else if (sig == SIGBUS) {
    NameIt(b, "BUS_", si_code);
    if (si_code == BUS_ADRALN) {
      strcpy(b + 4, "ADRALN"); /* invalid address alignment */
    } else if (si_code == BUS_ADRERR) {
      strcpy(b + 4, "ADRERR"); /* non-existent physical address */
    } else if (si_code == BUS_OBJERR) {
      strcpy(b + 4, "OBJERR"); /* object specific hardware error */
    } else if (si_code == BUS_OOMERR) {
      strcpy(b + 4, "OOMERR"); /* FreeBSD */
    } else if (si_code == BUS_MCEERR_AR) {
      strcpy(b + 4, "MCEERR_AR"); /* Linux 2.6.32+ */
    } else if (si_code == BUS_MCEERR_AO) {
      strcpy(b + 4, "MCEERR_AO"); /* Linux 2.6.32+ */
    }
  } else if (sig == SIGIO) {
    NameIt(b, "POLL_", si_code);
    if (si_code == POLL_IN) {
      strcpy(b + 5, "IN"); /* data input available */
    } else if (si_code == POLL_OUT) {
      strcpy(b + 5, "OUT"); /* output buffer available */
    } else if (si_code == POLL_MSG) {
      strcpy(b + 5, "MSG"); /* input message available */
    } else if (si_code == POLL_ERR) {
      strcpy(b + 5, "ERR"); /* i/o error */
    } else if (si_code == POLL_PRI) {
      strcpy(b + 5, "PRI"); /* high priority input available */
    } else if (si_code == POLL_HUP) {
      strcpy(b + 5, "HUP"); /* device disconnected */
    }
  } else if (sig == SIGSYS) {
    NameIt(b, "SYS_", si_code);
    if (si_code == SYS_SECCOMP) {
      strcpy(b + 4, "SECCOMP");
    }
  }
  return b;
}
