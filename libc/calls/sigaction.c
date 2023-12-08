/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/struct/sigaction.h"
#include "ape/sections.internal.h"
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigaction.internal.h"
#include "libc/calls/struct/siginfo.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/calls/ucontext.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/strace.internal.h"
#include "libc/limits.h"
#include "libc/log/backtrace.internal.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/limits.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

#define SA_RESTORER 0x04000000

static void sigaction_cosmo2native(union metasigaction *sa) {
  void *handler;
  uint64_t flags;
  void *restorer;
  uint32_t masklo;
  uint32_t maskhi;
  if (!sa) return;
  flags = sa->cosmo.sa_flags;
  handler = sa->cosmo.sa_handler;
  restorer = sa->cosmo.sa_restorer;
  masklo = sa->cosmo.sa_mask;
  maskhi = sa->cosmo.sa_mask >> 32;
  if (IsLinux()) {
    sa->linux.sa_flags = flags;
    sa->linux.sa_handler = handler;
    sa->linux.sa_restorer = restorer;
    sa->linux.sa_mask[0] = masklo;
    sa->linux.sa_mask[1] = maskhi;
  } else if (IsXnuSilicon()) {
    sa->silicon.sa_flags = flags;
    sa->silicon.sa_handler = handler;
    sa->silicon.sa_mask[0] = masklo;
  } else if (IsXnu()) {
    sa->xnu_in.sa_flags = flags;
    sa->xnu_in.sa_handler = handler;
    sa->xnu_in.sa_restorer = restorer;
    sa->xnu_in.sa_mask[0] = masklo;
  } else if (IsFreebsd()) {
    sa->freebsd.sa_flags = flags;
    sa->freebsd.sa_handler = handler;
    sa->freebsd.sa_mask[0] = masklo;
    sa->freebsd.sa_mask[1] = maskhi;
    sa->freebsd.sa_mask[2] = 0;
    sa->freebsd.sa_mask[3] = 0;
  } else if (IsOpenbsd()) {
    sa->openbsd.sa_flags = flags;
    sa->openbsd.sa_handler = handler;
    sa->openbsd.sa_mask[0] = masklo;
  } else if (IsNetbsd()) {
    sa->netbsd.sa_flags = flags;
    sa->netbsd.sa_handler = handler;
    sa->netbsd.sa_mask[0] = masklo;
    sa->netbsd.sa_mask[1] = maskhi;
    sa->netbsd.sa_mask[2] = 0;
    sa->netbsd.sa_mask[3] = 0;
  }
}

static void sigaction_native2cosmo(union metasigaction *sa) {
  void *handler;
  uint64_t flags;
  void *restorer = 0;
  uint32_t masklo;
  uint32_t maskhi = 0;
  if (!sa) return;
  if (IsLinux()) {
    flags = sa->linux.sa_flags;
    handler = sa->linux.sa_handler;
    restorer = sa->linux.sa_restorer;
    masklo = sa->linux.sa_mask[0];
    maskhi = sa->linux.sa_mask[1];
  } else if (IsXnu()) {
    flags = sa->silicon.sa_flags;
    handler = sa->silicon.sa_handler;
    masklo = sa->silicon.sa_mask[0];
  } else if (IsXnu()) {
    flags = sa->xnu_out.sa_flags;
    handler = sa->xnu_out.sa_handler;
    masklo = sa->xnu_out.sa_mask[0];
  } else if (IsFreebsd()) {
    flags = sa->freebsd.sa_flags;
    handler = sa->freebsd.sa_handler;
    masklo = sa->freebsd.sa_mask[0];
    maskhi = sa->freebsd.sa_mask[1];
  } else if (IsOpenbsd()) {
    flags = sa->openbsd.sa_flags;
    handler = sa->openbsd.sa_handler;
    masklo = sa->openbsd.sa_mask[0];
  } else if (IsNetbsd()) {
    flags = sa->netbsd.sa_flags;
    handler = sa->netbsd.sa_handler;
    masklo = sa->netbsd.sa_mask[0];
    maskhi = sa->netbsd.sa_mask[1];
  } else {
    return;
  }
  sa->cosmo.sa_flags = flags;
  sa->cosmo.sa_handler = handler;
  sa->cosmo.sa_restorer = restorer;
  sa->cosmo.sa_mask = masklo | (uint64_t)maskhi << 32;
}

static int __sigaction(int sig, const struct sigaction *act,
                       struct sigaction *oldact) {
  _Static_assert(
      (sizeof(struct sigaction) >= sizeof(struct sigaction_linux) &&
       sizeof(struct sigaction) >= sizeof(struct sigaction_xnu_in) &&
       sizeof(struct sigaction) >= sizeof(struct sigaction_xnu_out) &&
       sizeof(struct sigaction) >= sizeof(struct sigaction_silicon) &&
       sizeof(struct sigaction) >= sizeof(struct sigaction_freebsd) &&
       sizeof(struct sigaction) >= sizeof(struct sigaction_openbsd) &&
       sizeof(struct sigaction) >= sizeof(struct sigaction_netbsd)),
      "sigaction cosmo abi needs tuning");
  int64_t arg4, arg5;
  int rc, rva, oldrva;
  sigaction_f sigenter;
  struct sigaction *ap, copy;
  if (IsMetal()) return enosys(); /* TODO: Signals on Metal */
  if (!(1 <= sig && sig <= _NSIG)) return einval();
  if (sig == SIGKILL || sig == SIGSTOP) return einval();
  if (IsAsan() && ((act && !__asan_is_valid(act, sizeof(*act))) ||
                   (oldact && !__asan_is_valid(oldact, sizeof(*oldact))))) {
    return efault();
  }
  if (!act) {
    rva = (int32_t)(intptr_t)SIG_DFL;
  } else if ((intptr_t)act->sa_handler < kSigactionMinRva) {
    rva = (int)(intptr_t)act->sa_handler;
  } else if ((intptr_t)act->sa_handler >=
                 (intptr_t)&__executable_start + kSigactionMinRva &&
             (intptr_t)act->sa_handler <
                 (intptr_t)&__executable_start + INT_MAX) {
    rva = (int)((uintptr_t)act->sa_handler - (uintptr_t)&__executable_start);
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

      if (IsLinux()) {
        if (!(ap->sa_flags & SA_RESTORER)) {
          ap->sa_flags |= SA_RESTORER;
          ap->sa_restorer = &__restore_rt;
        }
        if (__iswsl1()) {
          sigenter = __sigenter_wsl;
        } else {
          sigenter = ap->sa_sigaction;
        }
      } else if (IsXnu()) {
        ap->sa_restorer = (void *)&__sigenter_xnu;
        sigenter = __sigenter_xnu;
        // mitigate Rosetta signal handling strangeness
        // https://github.com/jart/cosmopolitan/issues/455
        ap->sa_flags |= SA_SIGINFO;
      } else if (IsXnu()) {
        sigenter = __sigenter_xnu;
        ap->sa_flags |= SA_SIGINFO;  // couldn't hurt
      } else if (IsNetbsd()) {
        sigenter = __sigenter_netbsd;
      } else if (IsFreebsd()) {
        sigenter = __sigenter_freebsd;
      } else if (IsOpenbsd()) {
        sigenter = __sigenter_openbsd;
      } else {
        return enosys();
      }
      if (rva < kSigactionMinRva) {
        ap->sa_sigaction = (void *)(intptr_t)rva;
      } else {
        ap->sa_sigaction = sigenter;
      }
      sigaction_cosmo2native((union metasigaction *)ap);
    } else {
      ap = NULL;
    }
    if (IsXnu()) {
      arg4 = (int64_t)(intptr_t)oldact; /* from go code */
      arg5 = 0;
    } else if (IsNetbsd()) {
      /* int __sigaction_sigtramp(int signum,
                                  const struct sigaction *nsa,
                                  struct sigaction *osa,
                                  const void *tramp,
                                  int vers); */
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
    if (!IsXnuSilicon()) {
      rc = sys_sigaction(sig, ap, oldact, arg4, arg5);
    } else {
      rc = _sysret(__syslib->__sigaction(sig, ap, oldact));
    }
    if (rc != -1) {
      sigaction_native2cosmo((union metasigaction *)oldact);
      if (oldact &&                         //
          oldact->sa_handler != SIG_DFL &&  //
          oldact->sa_handler != SIG_IGN &&  //
          (IsFreebsd() || IsOpenbsd() || IsNetbsd() || IsXnu())) {
        oldact->sa_handler =
            (sighandler_t)((uintptr_t)__executable_start + __sighandrvas[sig]);
      }
    }
  } else {
    if (oldact) {
      bzero(oldact, sizeof(*oldact));
      oldrva = __sighandrvas[sig];
      oldact->sa_mask = __sighandmask[sig];
      oldact->sa_flags = __sighandflags[sig];
      oldact->sa_sigaction =
          (sigaction_f)(oldrva < kSigactionMinRva
                            ? oldrva
                            : (uintptr_t)&__executable_start + oldrva);
    }
    rc = 0;
  }
  if (rc != -1 && !__vforked) {
    if (act) {
      __sighandrvas[sig] = rva;
      __sighandmask[sig] = act->sa_mask;
      __sighandflags[sig] = act->sa_flags;
      if (IsWindows() && __sig_ignored(sig)) {
        __sig_delete(sig);
      }
    }
  }
  return rc;
}

/**
 * Installs handler for kernel interrupt to thread, e.g.:
 *
 *     void GotCtrlC(int sig, siginfo_t *si, void *arg) {
 *       ucontext_t *ctx = arg;
 *     }
 *     struct sigaction sa = {.sa_sigaction = GotCtrlC,
 *                            .sa_flags = SA_RESETHAND|SA_RESTART|SA_SIGINFO};
 *     CHECK_NE(-1, sigaction(SIGINT, &sa, NULL));
 *
 * The following flags are supported across platforms:
 *
 * - `SA_SIGINFO`: Causes the `siginfo_t` and `ucontext_t` parameters to
 *   be passed. `void *ctx` actually refers to `struct ucontext *`.
 *   This not only gives you more information about the signal, but also
 *   allows your signal handler to change the CPU registers. That's
 *   useful for recovering from crashes. If you don't use this attribute,
 *   then signal delivery will go a little faster.
 *
 * - `SA_RESTART`: Enables BSD signal handling semantics. Normally i/o
 *   entrypoints check for pending signals to deliver. If one gets
 *   delivered during an i/o call, the normal behavior is to cancel the
 *   i/o operation and return -1 with EINTR in errno. If you use the
 *   `SA_RESTART` flag then that behavior changes, so that any function
 *   that's been annotated with @restartable will not return `EINTR` and
 *   will instead resume the i/o operation. This makes coding easier but
 *   it can be an anti-pattern if not used carefully, since poor usage
 *   can easily result in latency issues. It also requires one to do
 *   more work in signal handlers, so special care needs to be given to
 *   which C library functions are @asyncsignalsafe.
 *
 * - `SA_RESETHAND`: Causes signal handler to be single-shot. This means
 *   that, upon entry of delivery to a signal handler, it's reset to the
 *   `SIG_DFL` handler automatically. You may use the alias `SA_ONESHOT`
 *   for this flag, which means the same thing.
 *
 * - `SA_NODEFER`: Disables the reentrancy safety check on your signal
 *   handler. Normally that's a good thing, since for instance if your
 *   `SIGSEGV` signal handler happens to segfault, you're going to want
 *   your process to just crash rather than looping endlessly. But in
 *   some cases it's desirable to use `SA_NODEFER` instead, such as at
 *   times when you wish to `longjmp()` out of your signal handler and
 *   back into your program. This is only safe to do across platforms
 *   for non-crashing signals such as `SIGCHLD` and `SIGINT`. Crash
 *   handlers should use Xed instead to recover execution, because on
 *   Windows a `SIGSEGV` or `SIGTRAP` crash handler might happen on a
 *   separate stack and/or a separate thread. You may use the alias
 *   `SA_NOMASK` for this flag, which means the same thing.
 *
 * - `SA_NOCLDWAIT`: Changes `SIGCHLD` so the zombie is gone and you
 *   can't call `wait()` anymore; similar but may
 *   still deliver the SIGCHLD.
 *
 * - `SA_NOCLDSTOP`: Lets you set `SIGCHLD` handler that's only notified
 *   on exit/termination and not notified on `SIGSTOP`, `SIGTSTP`,
 *   `SIGTTIN`, `SIGTTOU`, or `SIGCONT`.
 *
 * Here's an example of the most professional way to handle signals in
 * an i/o event loop. It's generally a best practice to have signal
 * handlers do the fewest number of things possible. The trick is to
 * have your signals work hand-in-glove with the EINTR errno. This
 * obfuscates the need for having to worry about @asyncsignalsafe.
 *
 *     static volatile bool gotctrlc;
 *
 *     void OnCtrlC(int sig) {
 *       gotctrlc = true;
 *     }
 *
 *     int main() {
 *       size_t got;
 *       ssize_t rc;
 *       char buf[1];
 *       struct sigaction oldint;
 *       struct sigaction saint = {.sa_handler = GotCtrlC};
 *       if (sigaction(SIGINT, &saint, &oldint) == -1) {
 *         perror("sigaction");
 *         exit(1);
 *       }
 *       for (;;) {
 *         rc = read(0, buf, sizeof(buf));
 *         if (rc == -1) {
 *           if (errno == EINTR) {
 *             if (gotctrlc) {
 *               break;
 *             }
 *           } else {
 *             perror("read");
 *             exit(2);
 *           }
 *         }
 *         if (!(got = rc)) {
 *           break;
 *         }
 *         for (;;) {
 *           rc = write(1, buf, got);
 *           if (rc != -1) {
 *             assert(rc == 1);
 *             break;
 *           } else if (errno != EINTR) {
 *             perror("write");
 *             exit(3);
 *           }
 *         }
 *       }
 *       sigaction(SIGINT, &oldint, 0);
 *     }
 *
 * Please note that you can't do the above if you use SA_RESTART. Since
 * the purpose of SA_RESTART is to restart i/o operations whose docs say
 * that they're @restartable and read() is one such function. Here's
 * some even better news: if you don't install any signal handlers at
 * all, then your i/o calls will never be interrupted!
 *
 * Here's an example of the most professional way to recover from
 * `SIGSEGV`, `SIGFPE`, and `SIGILL`.
 *
 *     void ContinueOnCrash(void);
 *
 *     void SkipOverFaultingInstruction(struct ucontext *ctx) {
 *       struct XedDecodedInst xedd;
 *       xed_decoded_inst_zero_set_mode(&xedd, XED_MACHINE_MODE_LONG_64);
 *       xed_instruction_length_decode(&xedd, (void *)ctx->uc_mcontext.rip, 15);
 *       ctx->uc_mcontext.rip += xedd.length;
 *     }
 *
 *     void OnCrash(int sig, struct siginfo *si, void *vctx) {
 *       struct ucontext *ctx = vctx;
 *       SkipOverFaultingInstruction(ctx);
 *       ContinueOnCrash();  // reinstall here in case *rip faults
 *     }
 *
 *     void ContinueOnCrash(void) {
 *       struct sigaction sa = {.sa_handler = OnSigSegv,
 *                              .sa_flags = SA_SIGINFO | SA_RESETHAND};
 *       sigaction(SIGSEGV, &sa, 0);
 *       sigaction(SIGFPE, &sa, 0);
 *       sigaction(SIGILL, &sa, 0);
 *     }
 *
 *     int main() {
 *       ContinueOnCrash();
 *       // ...
 *     }
 *
 * You may also edit any other CPU registers during the handler. For
 * example, you can use the above technique so that division by zero
 * becomes defined to a specific value of your choosing!
 *
 * Please note that Xed isn't needed to recover from `SIGTRAP` which can
 * be raised at any time by embedding `DebugBreak()` or `asm("int3")` in
 * your program code. Your signal handler will automatically skip over
 * the interrupt instruction, assuming your signal handler returns.
 *
 * The important signals supported across all platforms are:
 *
 * - `SIGINT`: When you press Ctrl-C this signal gets broadcasted to
 *   your process session group. This is the normal way to terminate
 *   console applications.
 *
 * - `SIGQUIT`: When you press CTRL-\ this signal gets broadcasted to
 *   your process session group. This is the irregular way to kill an
 *   application in cases where maybe your `SIGINT` handler is broken
 *   although, Cosmopolitan Libc ShowCrashReports() should program it
 *   such as to attach a debugger to the process if possible, or else
 *   show a crash report. Also note that in New Technology you should
 *   press CTRL+BREAK rather than CTRL+\ to get this signal.
 *
 * - `SIGHUP`: This gets sent to your non-daemon processes when you
 *   close your terminal session.
 *
 * - `SIGTERM` is what the `kill` command sends by default. It's the
 *   choice signal for terminating daemons.
 *
 * - `SIGUSR1` and `SIGUSR2` can be anything you want. Their default
 *   action is to kill the process. By convention `SIGUSR1` is usually
 *   used by daemons to reload the config file.
 *
 * - `SIGCHLD` is sent when a process terminates and it takes a certain
 *   degree of UNIX mastery to address sanely.
 *
 * - `SIGALRM` is invoked by `setitimer()` and `alarm()`. It can be
 *   useful for interrupting i/o operations like `connect()`.
 *
 * - `SIGTRAP`: This happens when an INT3 instruction is encountered.
 *
 * - `SIGILL` happens on illegal instructions, e.g. `UD2`.
 *
 * - `SIGABRT` happens when you call `abort()`.
 *
 * - `SIGFPE` happens when you divide ints by zero, among other things.
 *
 * - `SIGSEGV` and `SIGBUS` indicate memory access errors and they have
 *   inconsistent semantics across platforms like FreeBSD.
 *
 * - `SIGWINCH` is sent when your terminal window is resized.
 *
 * - `SIGXCPU` and `SIGXFSZ` may be raised if you run out of resources,
 *   which can happen if your process, or the parent process that
 *   spawned your process, happened to call `setrlimit()`. Doing this is
 *   a wonderful idea.
 *
 * Signal handlers should avoid clobbering global variables like `errno`
 * because most signals are asynchronous, i.e. the signal handler might
 * be called at any assembly instruction. If something like a `SIGCHLD`
 * handler doesn't save / restore the `errno` global when calling wait,
 * then any i/o logic in the main program that checks `errno` will most
 * likely break. This is rare in practice, since systems usually design
 * signals to favor delivery from cancelation points before they block
 * however that's not guaranteed.
 *
 * @return 0 on success or -1 w/ errno
 * @see xsigaction() for a much better api
 * @asyncsignalsafe
 * @vforksafe
 */
int sigaction(int sig, const struct sigaction *act, struct sigaction *oldact) {
  int rc;
  if (sig == SIGKILL || sig == SIGSTOP) {
    rc = einval();
  } else {
    rc = __sigaction(sig, act, oldact);
  }
  STRACE("sigaction(%G, %s, [%s]) → %d% m", sig, DescribeSigaction(0, act),
         DescribeSigaction(rc, oldact), rc);
  return rc;
}
