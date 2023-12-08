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
#include "libc/assert.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigaltstack.h"
#include "libc/calls/struct/sigset.h"
#include "libc/intrin/leaky.internal.h"
#include "libc/log/internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"

#ifndef TINY
__static_yoink("zipos");                       // for symtab
__static_yoink("__die");                       // for backtracing
__static_yoink("ShowBacktrace");               // for backtracing
__static_yoink("GetSymbolTable");              // for backtracing
__static_yoink("PrintBacktraceUsingSymbols");  // for backtracing
__static_yoink("malloc_inspect_all");          // for asan memory origin
__static_yoink("GetSymbolByAddr");             // for asan memory origin
#endif

static void InstallCrashHandler(int sig, int flags) {
  struct sigaction sa;
  sigemptyset(&sa.sa_mask);
  sigaddset(&sa.sa_mask, SIGQUIT);
  sigaddset(&sa.sa_mask, SIGFPE);
  sigaddset(&sa.sa_mask, SIGILL);
  sigaddset(&sa.sa_mask, SIGSEGV);
  sigaddset(&sa.sa_mask, SIGTRAP);
  sigaddset(&sa.sa_mask, SIGBUS);
  sigaddset(&sa.sa_mask, SIGABRT);
  sa.sa_flags = SA_SIGINFO | flags;
#ifdef TINY
  sa.sa_sigaction = __minicrash;
#else
  GetSymbolTable();
  sa.sa_sigaction = __oncrash;
#endif
  unassert(!sigaction(sig, &sa, 0));
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
 */
void ShowCrashReports(void) {
  struct sigaltstack ss;
  static char crashstack[65536];
  FindDebugBinary();
  ss.ss_flags = 0;
  ss.ss_size = sizeof(crashstack);
  ss.ss_sp = crashstack;
  unassert(!sigaltstack(&ss, 0));
  InstallCrashHandler(SIGQUIT, 0);
#ifdef __x86_64__
  InstallCrashHandler(SIGTRAP, 0);
#else
  InstallCrashHandler(SIGTRAP, SA_RESETHAND);
#endif
  InstallCrashHandler(SIGFPE, SA_RESETHAND);
  InstallCrashHandler(SIGILL, SA_RESETHAND);
  InstallCrashHandler(SIGBUS, SA_RESETHAND);
  InstallCrashHandler(SIGABRT, SA_RESETHAND);
  InstallCrashHandler(SIGSEGV, SA_RESETHAND | SA_ONSTACK);
}

IGNORE_LEAKS(ShowCrashReports)
