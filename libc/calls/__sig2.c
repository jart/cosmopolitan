/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "ape/sections.internal.h"
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/ucontext.internal.h"
#include "libc/intrin/describebacktrace.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/log/libfatal.internal.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/context.h"
#include "libc/nt/struct/context.h"
#include "libc/nt/thread.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/thread/tls.h"

#ifdef __x86_64__

/**
 * Returns true if signal is ignored by default.
 */
textwindows bool __sig_is_ignored(int sig) {
  return sig == SIGURG ||   //
         sig == SIGCONT ||  //
         sig == SIGCHLD ||  //
         sig == SIGWINCH;
}

/**
 * Returns true if signal is so fatal it should dump core.
 */
textwindows bool __sig_is_core(int sig) {
  return sig == SIGSYS ||   //
         sig == SIGBUS ||   //
         sig == SIGSEGV ||  //
         sig == SIGQUIT ||  //
         sig == SIGTRAP ||  //
         sig == SIGXCPU ||  //
         sig == SIGXFSZ;
}

static inline textwindows int __sig_is_masked(int sig) {
  if (__tls_enabled) {
    return __get_tls()->tib_sigmask & (1ull << (sig - 1));
  } else {
    return __sig.mask & (1ull << (sig - 1));
  }
}

/**
 * Delivers signal to callback.
 *
 * @return true if `EINTR` should be raised
 */
bool __sig_deliver(int sigops, int sig, int sic, ucontext_t *ctx) {
  unsigned rva = __sighandrvas[sig];
  unsigned flags = __sighandflags[sig];

  // generate expensive data if needed
  ucontext_t uc;
  siginfo_t info;
  siginfo_t *infop;
  if (flags & SA_SIGINFO) {
    __repstosb(&info, 0, sizeof(info));
    info.si_signo = sig;
    info.si_code = sic;
    infop = &info;
    if (!ctx) {
      struct NtContext nc = {.ContextFlags = kNtContextAll};
      __repstosb(&uc, 0, sizeof(uc));
      GetThreadContext(GetCurrentThread(), &nc);
      _ntcontext2linux(&uc, &nc);
      ctx = &uc;
    }
  } else {
    infop = 0;
  }

  // save the thread's signal mask
  uint64_t oldmask;
  if (__tls_enabled) {
    oldmask = __get_tls()->tib_sigmask;
  } else {
    oldmask = __sig.mask;
  }
  if (ctx) {
    ctx->uc_sigmask = (sigset_t){{oldmask}};
  }

  // mask the signal that's being handled whilst handling
  if (!(flags & SA_NODEFER)) {
    if (__tls_enabled) {
      __get_tls()->tib_sigmask |= 1ull << (sig - 1);
    } else {
      __sig.mask |= 1ull << (sig - 1);
    }
  }

  STRACE("delivering %G", sig);
  ((sigaction_f)(__executable_start + rva))(sig, infop, ctx);

  if (ctx) {
    oldmask = ctx->uc_sigmask.__bits[0];
  }
  if (__tls_enabled) {
    __get_tls()->tib_sigmask = oldmask;
  } else {
    __sig.mask = oldmask;
  }
  if (flags & SA_RESETHAND) {
    __sighandrvas[sig] = (int32_t)(intptr_t)SIG_DFL;
  }

  if (!(sigops & kSigOpRestartable)) {
    return true;  // always send EINTR for wait4(), poll(), etc.
  } else if (flags & SA_RESTART) {
    STRACE("restarting syscall on %G", sig);
    return false;  // resume syscall for read(), write(), etc.
  } else {
    return true;  // default course is to raise EINTR
  }
}

/**
 * Handles signal.
 * @return true if `EINTR` should be raised
 */
textwindows bool __sig_handle(int sigops, int sig, int sic, ucontext_t *ctx) {
  if (__sighandrvas[sig] == (intptr_t)SIG_IGN ||
      (__sighandrvas[sig] == (intptr_t)SIG_DFL && __sig_is_ignored(sig))) {
    return false;
  }
  if (__sig_is_masked(sig)) {
    if (sigops & kSigOpUnmaskable) {
      goto DefaultAction;
    }
    if (__tls_enabled) {
      __get_tls()->tib_sigpending |= 1ull << (sig - 1);
    } else {
      __sig.pending |= 1ull << (sig - 1);
    }
    return false;
  }
  switch (__sighandrvas[sig]) {
    case (intptr_t)SIG_DFL:
    DefaultAction:
      if (!__sig_is_ignored(sig)) {
        uint32_t cmode;
        intptr_t hStderr;
        const char *signame;
        char *end, sigbuf[21], output[123];
        signame = strsignal_r(sig, sigbuf);
        STRACE("terminating due to uncaught %s", signame);
        if (__sig_is_core(sig)) {
          hStderr = GetStdHandle(kNtStdErrorHandle);
          if (GetConsoleMode(hStderr, &cmode)) {
            end = stpcpy(output, signame);
            end = stpcpy(end, " ");
            end = stpcpy(
                end,
                DescribeBacktrace(
                    ctx ? (struct StackFrame *)ctx->uc_mcontext.BP
                        : (struct StackFrame *)__builtin_frame_address(0)));
            end = stpcpy(end, "\n");
            WriteFile(hStderr, output, end - output, 0, 0);
          }
        }
        ExitProcess(sig);
      }
      // fallthrough
    case (intptr_t)SIG_IGN:
      return false;
    default:
      return __sig_deliver(sigops, sig, sic, ctx);
  }
}

static textwindows bool __sig_checkem(int sigops, uint64_t *pending) {
  bool res = false;
  for (int sig = 1; sig <= 64; ++sig) {
    if (*pending & (1ull << (sig - 1))) {
      *pending &= ~(1ull << (sig - 1));
      res |= __sig_handle(sigops, sig, SI_KERNEL, 0);
    }
  }
  return res;
}

/**
 * Checks for unblocked signals and delivers them on New Technology.
 * @return true if EINTR should be returned by caller
 * @note called from main thread
 * @threadsafe
 */
textwindows bool __sig_check(int sigops) {
  bool res = false;
  if (__tls_enabled) {
    res |= __sig_checkem(sigops, &__get_tls()->tib_sigpending);
  }
  return res | __sig_checkem(sigops, &__sig.pending);
}

#endif /* __x86_64__ */
