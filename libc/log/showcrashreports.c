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
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigaltstack.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/log/internal.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/ss.h"

__static_yoink("zipos");                       // for symtab
__static_yoink("__die");                       // for backtracing
__static_yoink("ShowBacktrace");               // for backtracing
__static_yoink("GetSymbolTable");              // for backtracing
__static_yoink("PrintBacktraceUsingSymbols");  // for backtracing
__static_yoink("malloc_inspect_all");          // for asan memory origin
__static_yoink("GetSymbolByAddr");             // for asan memory origin

struct CrashHandler {
  int sig;
  struct sigaction old;
};

static inline void __oncrash(int sig, struct siginfo *si, void *arg) {
#ifdef __x86_64__
  __oncrash_amd64(sig, si, arg);
#elif defined(__aarch64__)
  __oncrash_arm64(sig, si, arg);
#else
  abort();
#endif
}

static void __got_sigquit(int sig, struct siginfo *si, void *arg) {
  write(2, "^\\", 2);
  __oncrash(sig, si, arg);
}
static void __got_sigfpe(int sig, struct siginfo *si, void *arg) {
  __oncrash(sig, si, arg);
}
static void __got_sigill(int sig, struct siginfo *si, void *arg) {
  __oncrash(sig, si, arg);
}
static void __got_sigsegv(int sig, struct siginfo *si, void *arg) {
  __oncrash(sig, si, arg);
}
static void __got_sigtrap(int sig, struct siginfo *si, void *arg) {
  __oncrash(sig, si, arg);
}
static void __got_sigabrt(int sig, struct siginfo *si, void *arg) {
  __oncrash(sig, si, arg);
}
static void __got_sigbus(int sig, struct siginfo *si, void *arg) {
  __oncrash(sig, si, arg);
}
static void __got_sigurg(int sig, struct siginfo *si, void *arg) {
  __oncrash(sig, si, arg);
}

static void RemoveCrashHandler(void *arg) {
  int e;
  struct CrashHandler *ch = arg;
  strace_enabled(-1);
  e = errno;
  sigaction(ch->sig, &ch->old, NULL);
  errno = e;
  free(ch);
  strace_enabled(+1);
}

static void InstallCrashHandler(int sig, sigaction_f thunk, int extraflags) {
  int e;
  struct sigaction sa;
  struct CrashHandler *ch;
  e = errno;
  if ((ch = malloc(sizeof(*ch)))) {
    ch->sig = sig;
    sa.sa_sigaction = thunk;
    sigfillset(&sa.sa_mask);
    sigdelset(&sa.sa_mask, SIGQUIT);
    sigdelset(&sa.sa_mask, SIGFPE);
    sigdelset(&sa.sa_mask, SIGILL);
    sigdelset(&sa.sa_mask, SIGSEGV);
    sigdelset(&sa.sa_mask, SIGTRAP);
    sigdelset(&sa.sa_mask, SIGABRT);
    sigdelset(&sa.sa_mask, SIGBUS);
    sigdelset(&sa.sa_mask, SIGURG);
    sa.sa_flags = SA_SIGINFO | SA_NODEFER | extraflags;
    if (!sigaction(sig, &sa, &ch->old)) {
      __cxa_atexit(RemoveCrashHandler, ch, 0);
    }
  }
  errno = e;
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
  int ef = 0;
  struct sigaltstack ss;
  _wantcrashreports = true;
  if (!IsWindows()) {
    ef = SA_ONSTACK;
    ss.ss_flags = 0;
    ss.ss_size = GetStackSize();
    // FreeBSD sigaltstack() will EFAULT if we use MAP_STACK here
    // OpenBSD sigaltstack() auto-applies MAP_STACK to the memory
    npassert((ss.ss_sp = _mapanon(GetStackSize())));
    npassert(!sigaltstack(&ss, 0));
  }
  InstallCrashHandler(SIGQUIT, __got_sigquit, ef);  // ctrl+\ aka ctrl+break
  InstallCrashHandler(SIGFPE, __got_sigfpe, ef);    // 1 / 0
  InstallCrashHandler(SIGILL, __got_sigill, ef);    // illegal instruction
  InstallCrashHandler(SIGSEGV, __got_sigsegv, ef);  // bad memory access
  InstallCrashHandler(SIGTRAP, __got_sigtrap, ef);  // bad system call
  InstallCrashHandler(SIGBUS, __got_sigbus, ef);    // misalign, mmap i/o failed
  InstallCrashHandler(SIGURG, __got_sigurg, ef);    // placeholder
  GetSymbolTable();
}
