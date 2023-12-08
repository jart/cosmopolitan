/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/calls.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/ucontext.internal.h"
#include "libc/calls/ucontext.h"
#include "libc/errno.h"
#include "libc/intrin/describebacktrace.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/log/internal.h"
#include "libc/log/log.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/str/str.h"

/**
 * Prints miniature crash report, e.g.
 *
 *     struct sigaction sa = {
 *         .sa_sigaction = __minicrash,
 *         .sa_flags = SA_SIGINFO | SA_RESETHAND,
 *     };
 *     sigaction(SIGSEGV, &sa, 0);
 *
 * This function may be called from a signal handler to print vital
 * information about the cause of a crash. Only vital number values
 * shall be printed. The `cosmoaddr2line` command may be copied and
 * pasted into the shell to obtain further details such as function
 * calls and source lines in the backtrace.
 *
 * This function may be used as a sigaction handler, so long as the
 * `SA_RESETHAND` flag is used. Using `SA_ONSTACK` is also a lovely
 * feature since sigaltstack() is needed to report stack overflows.
 *
 * This implementation is designed to be:
 *
 * 1. Reliable under broken runtime states
 * 2. Require only a few kB of stack
 * 3. Have minimal binary footprint
 *
 * @see __die() for crashing from normal code without aborting
 * @asyncsignalsafe
 * @vforksafe
 */
relegated dontinstrument void __minicrash(int sig, siginfo_t *si, void *arg) {
  char host[128];
  ucontext_t *ctx = arg;
  strcpy(host, "unknown");
  gethostname(host, sizeof(host));
  kprintf(
      "%serror: %s on %s pid %d tid %d got %G%s code %d addr %p%s\n"
      "cosmoaddr2line %s %lx %s\n",
      __nocolor ? "" : "\e[1;31m", program_invocation_short_name, host,
      getpid(), gettid(), sig,
      __is_stack_overflow(si, ctx) ? " (stack overflow)" : "", si->si_code,
      si->si_addr, __nocolor ? "" : "\e[0m", FindDebugBinary(),
      ctx ? ctx->uc_mcontext.PC : 0,
      DescribeBacktrace(ctx ? (struct StackFrame *)ctx->uc_mcontext.BP
                            : (struct StackFrame *)__builtin_frame_address(0)));
}
