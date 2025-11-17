/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2025 Justine Alexandra Roberts Tunney                              │
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
#include "libc/atomic.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/ucontext.internal.h"
#include "libc/calls/ucontext.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/describebacktrace.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/weaken.h"
#include "libc/nexgen32e/stackframe.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"

// normally you want to put ShowCrashReports() at the start of main() to
// get nice crash reports. however sometimes when we build open source
// software like GCC using cosmo, it's not always easy to modify its
// main() functions, and the crash might happen too deep inside a
// process tree like `make` to pass flags like --strace to debug
//
// so what you can do here is something like this:
//
//     export KPRINTF_CRASH=1
//     o//examples/crashreport2
//
// that will cause the c runtime to install signal handlers for SIGABRT,
// SIGSEGV, SIGBUS, SIGILL, and SIGFPE that will print information about
// the crash via kprintf(). it furthermore *prevents* the compiled
// program from changing these signal handlers.

static void kprintf_crash_handle(int sig, siginfo_t *si, void *arg) {
  static atomic_bool once;
  if (atomic_exchange(&once, 1))
    for (;;)
      ;
  ucontext_t *ctx = arg;
  kprintf("error: %G cosmoaddr2line %s %lx %s\n", si->si_signo,
          _weaken(FindDebugBinary) ? _weaken(FindDebugBinary)()
                                   : program_invocation_name,
          ctx->uc_mcontext.PC,
          DescribeBacktrace((struct StackFrame *)ctx->uc_mcontext.BP));
  _Exit(44);
}

__attribute__((__constructor__(99))) void kprintf_crash_ctor(void) {
  int enable;
  if ((enable = !!getenv("KPRINTF_CRASH"))) {
    struct sigaction sa;
    sa.sa_sigaction = kprintf_crash_handle;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);

    // install signal handlers
    sigaction(SIGABRT, &sa, 0);
    sigaction(SIGSEGV, &sa, 0);
    sigaction(SIGBUS, &sa, 0);
    sigaction(SIGFPE, &sa, 0);
    sigaction(SIGILL, &sa, 0);

    // this tells sigaction to not allow signal handlers to change
    kprintf_crash = enable;
  }
}
