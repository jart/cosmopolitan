/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/assert.h"
#include "libc/bits/bits.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/sigbits.h"
#include "libc/calls/struct/sigaction-freebsd.internal.h"
#include "libc/calls/struct/sigaction-linux.internal.h"
#include "libc/calls/struct/sigaction-netbsd.h"
#include "libc/calls/struct/sigaction-openbsd.internal.h"
#include "libc/calls/struct/sigaction-xnu.internal.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/typedef/sigaction_f.h"
#include "libc/calls/ucontext.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"

#define SA_RESTORER 0x04000000

#ifndef SWITCHEROO
#define SWITCHEROO(S1, S2, A, B, C, D)                     \
  do {                                                     \
    autotype((S2).A) a = (typeof((S2).A))(S1).A;           \
    autotype((S2).B) b = (typeof((S2).B))(S1).B;           \
    autotype((S2).C) c = (typeof((S2).C))(S1).C;           \
    typeof((S2).D) d;                                      \
    bzero(&d, sizeof(d));                                  \
    memcpy(&d, &((S1).D), MIN(sizeof(d), sizeof((S1).D))); \
    (S2).A = a;                                            \
    (S2).B = b;                                            \
    (S2).C = c;                                            \
    bzero(&((S2).D), sizeof((S2).D));                      \
    memcpy(&((S2).D), &d, MIN(sizeof(d), sizeof((S2).D))); \
  } while (0);
#endif

union metasigaction {
  struct sigaction cosmo;
  struct sigaction_linux linux;
  struct sigaction_freebsd freebsd;
  struct sigaction_openbsd openbsd;
  struct sigaction_netbsd netbsd;
  struct sigaction_xnu_in xnu_in;
  struct sigaction_xnu_out xnu_out;
};

void __sigenter_netbsd(int, void *, void *);
void __sigenter_freebsd(int, void *, void *);
void __sigenter_openbsd(int, void *, void *);

static void sigaction_cosmo2native(union metasigaction *sa) {
  if (!sa) return;
  switch (__hostos) {
    case LINUX:
      SWITCHEROO(sa->cosmo, sa->linux, sa_handler, sa_flags, sa_restorer,
                 sa_mask);
      break;
    case XNU:
      SWITCHEROO(sa->cosmo, sa->xnu_in, sa_handler, sa_flags, sa_restorer,
                 sa_mask);
      break;
    case FREEBSD:
      SWITCHEROO(sa->cosmo, sa->freebsd, sa_handler, sa_flags, sa_flags,
                 sa_mask);
      break;
    case OPENBSD:
      SWITCHEROO(sa->cosmo, sa->openbsd, sa_handler, sa_flags, sa_flags,
                 sa_mask);
      break;
    case NETBSD:
      SWITCHEROO(sa->cosmo, sa->netbsd, sa_handler, sa_flags, sa_flags,
                 sa_mask);
      break;
    default:
      break;
  }
}

static void sigaction_native2cosmo(union metasigaction *sa) {
  if (!sa) return;
  switch (__hostos) {
    case LINUX:
      SWITCHEROO(sa->linux, sa->cosmo, sa_handler, sa_flags, sa_restorer,
                 sa_mask);
      break;
    case XNU:
      SWITCHEROO(sa->xnu_out, sa->cosmo, sa_handler, sa_flags, sa_flags,
                 sa_mask);
      break;
    case FREEBSD:
      SWITCHEROO(sa->freebsd, sa->cosmo, sa_handler, sa_flags, sa_flags,
                 sa_mask);
      break;
    case OPENBSD:
      SWITCHEROO(sa->openbsd, sa->cosmo, sa_handler, sa_flags, sa_flags,
                 sa_mask);
      break;
    case NETBSD:
      SWITCHEROO(sa->netbsd, sa->cosmo, sa_handler, sa_flags, sa_flags,
                 sa_mask);
      break;
    default:
      break;
  }
}

/**
 * Installs handler for kernel interrupt, e.g.:
 *
 *     void GotCtrlC(int sig, siginfo_t *si, ucontext_t *ctx);
 *     struct sigaction sa = {.sa_sigaction = GotCtrlC,
 *                            .sa_flags = SA_RESETHAND|SA_RESTART|SA_SIGINFO};
 *     CHECK_NE(-1, sigaction(SIGINT, &sa, NULL));
 *
 * @see xsigaction() for a much better api
 * @asyncsignalsafe
 * @vforksafe
 */
int(sigaction)(int sig, const struct sigaction *act, struct sigaction *oldact) {
  _Static_assert((sizeof(struct sigaction) > sizeof(struct sigaction_linux) &&
                  sizeof(struct sigaction) > sizeof(struct sigaction_xnu_in) &&
                  sizeof(struct sigaction) > sizeof(struct sigaction_xnu_out) &&
                  sizeof(struct sigaction) > sizeof(struct sigaction_freebsd) &&
                  sizeof(struct sigaction) > sizeof(struct sigaction_openbsd) &&
                  sizeof(struct sigaction) > sizeof(struct sigaction_netbsd)),
                 "sigaction cosmo abi needs tuning");
  int64_t arg4, arg5;
  int rc, rva, oldrva;
  struct sigaction *ap, copy;
  if (IsMetal()) return enosys(); /* TODO: Signals on Metal */
  if (!(0 < sig && sig < NSIG)) return einval();
  if (sig == SIGKILL || sig == SIGSTOP) return einval();
  if (IsAsan() && ((act && !__asan_is_valid(act, sizeof(*act))) ||
                   (oldact && !__asan_is_valid(oldact, sizeof(*oldact))))) {
    return efault();
  }
  if (!act) {
    rva = (int32_t)(intptr_t)SIG_DFL;
  } else if ((intptr_t)act->sa_handler < kSigactionMinRva) {
    rva = (int)(intptr_t)act->sa_handler;
  } else if ((intptr_t)act->sa_handler >= (intptr_t)&_base + kSigactionMinRva &&
             (intptr_t)act->sa_handler < (intptr_t)&_base + INT_MAX) {
    rva = (int)((uintptr_t)act->sa_handler - (uintptr_t)&_base);
  } else {
    return efault();
  }
  if (__vforked && rva != (intptr_t)SIG_DFL && rva != (intptr_t)SIG_IGN) {
    return einval();
  }
  if (!IsWindows()) {
    if (act) {
      memcpy(&copy, act, sizeof(copy));
      ap = &copy;
      if (IsXnu()) {
        ap->sa_restorer = (void *)&__sigenter_xnu;
        ap->sa_handler = (void *)&__sigenter_xnu;
      } else if (IsLinux()) {
        if (!(ap->sa_flags & SA_RESTORER)) {
          ap->sa_flags |= SA_RESTORER;
          ap->sa_restorer = &__restore_rt;
        }
      } else if (IsNetbsd()) {
        ap->sa_sigaction = (sigaction_f)__sigenter_netbsd;
      } else if (IsFreebsd()) {
        ap->sa_sigaction = (sigaction_f)__sigenter_freebsd;
      } else if (IsOpenbsd()) {
        ap->sa_sigaction = (sigaction_f)__sigenter_openbsd;
      } else {
        return enosys();
      }
      sigaction_cosmo2native((union metasigaction *)ap);
    } else {
      ap = NULL;
    }
    if (IsXnu()) {
      arg4 = (int64_t)(intptr_t)oldact; /* from go code */
      arg5 = 0;
    } else if (IsNetbsd()) {
      if (ap) {
        arg4 = (int64_t)(intptr_t)&__restore_rt_netbsd;
        arg5 = 2; /* netbsd/lib/libc/arch/x86_64/sys/__sigtramp2.S */
      } else {
        arg4 = 0;
        arg5 = 0; /* netbsd/lib/libc/arch/x86_64/sys/__sigtramp2.S */
      }
    } else {
      arg4 = 8; /* or linux whines */
      arg5 = 0;
    }
    if ((rc = sys_sigaction(sig, ap, oldact, arg4, arg5)) != -1) {
      sigaction_native2cosmo((union metasigaction *)oldact);
    }
  } else {
    if (oldact) {
      bzero(oldact, sizeof(*oldact));
    }
    rc = 0;
  }
  if (rc != -1 && !__vforked) {
    if (oldact) {
      oldrva = __sighandrvas[sig];
      oldact->sa_sigaction = (sigaction_f)(
          oldrva < kSigactionMinRva ? oldrva : (intptr_t)&_base + oldrva);
    }
    if (act) {
      __sighandrvas[sig] = rva;
    }
  }
  return rc;
}
