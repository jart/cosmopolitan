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
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigaltstack.h"
#include "libc/log/internal.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/runtime/stack.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/ss.h"

STATIC_YOINK("__die");                       // for backtracing
STATIC_YOINK("ShowBacktrace");               // for backtracing
STATIC_YOINK("GetSymbolTable");              // for backtracing
STATIC_YOINK("PrintBacktraceUsingSymbols");  // for backtracing
STATIC_YOINK("malloc_inspect_all");          // for asan memory origin
STATIC_YOINK("__get_symbol_by_addr");        // for asan memory origin

extern const unsigned char __oncrash_thunks[8][11];
static struct sigaltstack g_oldsigaltstack;
static struct sigaction g_oldcrashacts[8];

static void InstallCrashHandlers(int extraflags) {
  int e;
  size_t i;
  struct sigaction sa;
  bzero(&sa, sizeof(sa));
  sa.sa_flags = SA_SIGINFO | SA_NODEFER | extraflags;
  sigfillset(&sa.sa_mask);
  for (i = 0; i < ARRAYLEN(kCrashSigs); ++i) {
    sigdelset(&sa.sa_mask, kCrashSigs[i]);
  }
  for (i = 0; i < ARRAYLEN(kCrashSigs); ++i) {
    if (kCrashSigs[i]) {
      sa.sa_sigaction = (sigaction_f)__oncrash_thunks[i];
      e = errno;
      sigaction(kCrashSigs[i], &sa, &g_oldcrashacts[i]);
      errno = e;
    }
  }
}

relegated void RestoreDefaultCrashSignalHandlers(void) {
  int e;
  size_t i;
  sigset_t ss;
  --__strace;
  sigemptyset(&ss);
  sigprocmask(SIG_SETMASK, &ss, NULL);
  for (i = 0; i < ARRAYLEN(kCrashSigs); ++i) {
    if (kCrashSigs[i]) {
      e = errno;
      sigaction(kCrashSigs[i], &g_oldcrashacts[i], NULL);
      errno = e;
    }
  }
  ++__strace;
}

static void FreeSigAltStack(void *p) {
  sigaltstack(&g_oldsigaltstack, 0);
  munmap(p, GetStackSize());
}

/**
 * Installs crash signal handlers.
 *
 * Normally, only functions calling die() will print backtraces. This
 * function may be called at program startup to install handlers that
 * will display similar information, for most types of crashes.
 *
 *   - Backtraces
 *   - CPU state printout
 *   - Automatic debugger attachment
 *
 * Another trick this function enables, is you can press CTRL+\ to open
 * the debugger GUI at any point while the program is running. It can be
 * useful, for example, if a program is caught in an infinite loop.
 *
 * @see callexitontermination()
 */
void ShowCrashReports(void) {
  char *sp;
  struct sigaltstack ss;
  /* <SYNC-LIST>: showcrashreports.c, oncrashthunks.S, oncrash.c */
  kCrashSigs[0] = SIGQUIT; /* ctrl+\ aka ctrl+break */
  kCrashSigs[1] = SIGFPE;  /* 1 / 0 */
  kCrashSigs[2] = SIGILL;  /* illegal instruction */
  kCrashSigs[3] = SIGSEGV; /* bad memory access */
  kCrashSigs[4] = SIGTRAP; /* bad system call */
  kCrashSigs[5] = SIGABRT; /* abort() called */
  kCrashSigs[6] = SIGBUS;  /* misaligned, noncanonical ptr, etc. */
  kCrashSigs[7] = SIGURG;  /* placeholder */
  /* </SYNC-LIST>: showcrashreports.c, oncrashthunks.S, oncrash.c */
  if (!IsWindows()) {
    bzero(&ss, sizeof(ss));
    ss.ss_flags = 0;
    ss.ss_size = GetStackSize();
    // FreeBSD sigaltstack() will EFAULT if we use MAP_STACK here
    // OpenBSD sigaltstack() auto-applies MAP_STACK to the memory
    if ((sp = mmap(0, GetStackSize(), PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)) != MAP_FAILED) {
      ss.ss_sp = sp;
      if (!sigaltstack(&ss, &g_oldsigaltstack)) {
        __cxa_atexit(FreeSigAltStack, ss.ss_sp, 0);
      } else {
        munmap(ss.ss_sp, GetStackSize());
      }
    }
    InstallCrashHandlers(SA_ONSTACK);
  } else {
    InstallCrashHandlers(0);
  }
  GetSymbolTable();
}
