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
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/sigbits.h"
#include "libc/calls/strace.internal.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/lockcmpxchg.h"
#include "libc/log/backtrace.internal.h"
#include "libc/log/gdb.h"
#include "libc/log/internal.h"
#include "libc/log/libfatal.internal.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/stackframe.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/pc.internal.h"

/**
 * @fileoverview Abnormal termination handling & GUI debugging.
 * @see libc/onkill.c
 */

STATIC_YOINK("strerror_wr"); /* for kprintf %m */
STATIC_YOINK("strsignal");   /* for kprintf %G */

static const char kGregOrder[17] forcealign(1) = {
    13, 11, 8, 14, 12, 9, 10, 15, 16, 0, 1, 2, 3, 4, 5, 6, 7,
};

static const char kGregNames[17][4] forcealign(1) = {
    "R8",  "R9",  "R10", "R11", "R12", "R13", "R14", "R15", "RDI",
    "RSI", "RBP", "RBX", "RDX", "RAX", "RCX", "RSP", "RIP",
};

static const char kCpuFlags[12] forcealign(1) = "CVPRAKZSTIDO";
static const char kFpuExceptions[6] forcealign(1) = "IDZOUP";

int kCrashSigs[7];
struct sigaction g_oldcrashacts[7];

relegated static void ShowFunctionCalls(ucontext_t *ctx) {
  struct StackFrame *bp;
  struct StackFrame goodframe;
  if (!ctx->uc_mcontext.rip) {
    kprintf("%s is NULL can't show backtrace\n", "RIP");
  } else {
    goodframe.next = (struct StackFrame *)ctx->uc_mcontext.rbp;
    goodframe.addr = ctx->uc_mcontext.rip;
    bp = &goodframe;
    ShowBacktrace(2, bp);
  }
}

relegated static char *AddFlag(char *p, int b, const char *s) {
  if (b) {
    p = __stpcpy(p, s);
  } else {
    *p = 0;
  }
  return p;
}

relegated static char *DescribeCpuFlags(char *p, int flags, int x87sw,
                                        int mxcsr) {
  unsigned i;
  for (i = 0; i < ARRAYLEN(kCpuFlags); ++i) {
    if (flags & 1) {
      *p++ = ' ';
      *p++ = kCpuFlags[i];
      *p++ = 'F';
    }
    flags >>= 1;
  }
  for (i = 0; i < ARRAYLEN(kFpuExceptions); ++i) {
    if ((x87sw | mxcsr) & (1 << i)) {
      *p++ = ' ';
      *p++ = kFpuExceptions[i];
      *p++ = 'E';
    }
  }
  p = AddFlag(p, x87sw & FPU_SF, " SF");
  p = AddFlag(p, x87sw & FPU_C0, " C0");
  p = AddFlag(p, x87sw & FPU_C1, " C1");
  p = AddFlag(p, x87sw & FPU_C2, " C2");
  p = AddFlag(p, x87sw & FPU_C3, " C3");
  return p;
}

relegated static void ShowGeneralRegisters(ucontext_t *ctx) {
  int64_t x;
  const char *s;
  size_t i, j, k;
  long double st;
  char *p, buf[128];
  p = buf;
  kprintf("\n");
  for (i = 0, j = 0, k = 0; i < ARRAYLEN(kGregNames); ++i) {
    if (j > 0) *p++ = ' ';
    if (!(s = kGregNames[(unsigned)kGregOrder[i]])[2]) *p++ = ' ';
    p = __stpcpy(p, s), *p++ = ' ';
    p = __fixcpy(p, ctx->uc_mcontext.gregs[(unsigned)kGregOrder[i]], 64);
    if (++j == 3) {
      j = 0;
      if (ctx->uc_mcontext.fpregs) {
        memcpy(&st, (char *)&ctx->uc_mcontext.fpregs->st[k], sizeof(st));
      } else {
        bzero(&st, sizeof(st));
      }
      p = __stpcpy(p, " ST(");
      p = __uintcpy(p, k++);
      p = __stpcpy(p, ") ");
      x = st * 1000;
      if (x < 0) x = -x, *p++ = '-';
      p = __uintcpy(p, x / 1000), *p++ = '.';
      p = __uintcpy(p, x % 1000);
      *p = 0;
      kprintf("%s\n", buf);
      p = buf;
    }
  }
  DescribeCpuFlags(
      p, ctx->uc_mcontext.eflags,
      ctx->uc_mcontext.fpregs ? ctx->uc_mcontext.fpregs->swd : 0,
      ctx->uc_mcontext.fpregs ? ctx->uc_mcontext.fpregs->mxcsr : 0);
  kprintf("%s\n", buf);
}

relegated static void ShowSseRegisters(ucontext_t *ctx) {
  size_t i;
  char *p, buf[128];
  if (ctx->uc_mcontext.fpregs) {
    kprintf("\n");
    for (i = 0; i < 8; ++i) {
      p = buf;
      if (i >= 10) {
        *p++ = i / 10 + '0';
        *p++ = i % 10 + '0';
      } else {
        *p++ = i + '0';
        *p++ = ' ';
      }
      *p++ = ' ';
      p = __fixcpy(p, ctx->uc_mcontext.fpregs->xmm[i + 0].u64[1], 64);
      p = __fixcpy(p, ctx->uc_mcontext.fpregs->xmm[i + 0].u64[0], 64);
      p = __stpcpy(p, " XMM");
      if (i + 8 >= 10) {
        *p++ = (i + 8) / 10 + '0';
        *p++ = (i + 8) % 10 + '0';
      } else {
        *p++ = (i + 8) + '0';
        *p++ = ' ';
      }
      *p++ = ' ';
      p = __fixcpy(p, ctx->uc_mcontext.fpregs->xmm[i + 8].u64[1], 64);
      p = __fixcpy(p, ctx->uc_mcontext.fpregs->xmm[i + 8].u64[0], 64);
      *p = 0;
      kprintf("XMM%s\n", buf);
    }
  }
}

void ShowCrashReportHook(int, int, int, struct siginfo *, ucontext_t *);

relegated void ShowCrashReport(int err, int sig, struct siginfo *si,
                               ucontext_t *ctx) {
  int i;
  char *p;
  char host[64];
  struct utsname names;
  static char buf[4096];
  if (weaken(ShowCrashReportHook)) {
    ShowCrashReportHook(2, err, sig, si, ctx);
  }
  names.sysname[0] = 0;
  names.release[0] = 0;
  names.version[0] = 0;
  names.nodename[0] = 0;
  __stpcpy(host, "unknown");
  gethostname(host, sizeof(host));
  uname(&names);
  p = buf;
  errno = err;
  kprintf("\n%serror%s: Uncaught %G (%s) on %s pid %d\n"
          "  %s\n"
          "  %m\n"
          "  %s %s %s %s\n",
          !__nocolor ? "\e[30;101m" : "", !__nocolor ? "\e[0m" : "", sig,
          (ctx && (ctx->uc_mcontext.rsp >= GetStaticStackAddr(0) &&
                   ctx->uc_mcontext.rsp <= GetStaticStackAddr(0) + PAGESIZE))
              ? "Stack Overflow"
              : GetSiCodeName(sig, si->si_code),
          host, getpid(), program_invocation_name, names.sysname, names.version,
          names.nodename, names.release);
  if (ctx) {
    kprintf("\n");
    ShowFunctionCalls(ctx);
    ShowGeneralRegisters(ctx);
    ShowSseRegisters(ctx);
  }
  kprintf("\n");
  PrintMemoryIntervals(2, &_mmi);
  /* PrintSystemMappings(2); */
  if (__argv) {
    for (i = 0; i < __argc; ++i) {
      if (!__argv[i]) continue;
      if (IsAsan() && !__asan_is_valid(__argv[i], 1)) continue;
      kprintf("%s ", __argv[i]);
    }
  }
  kprintf("\n");
}

relegated static void RestoreDefaultCrashSignalHandlers(void) {
  size_t i;
  sigset_t ss;
  sigemptyset(&ss);
  sigprocmask(SIG_SETMASK, &ss, NULL);
  for (i = 0; i < ARRAYLEN(kCrashSigs); ++i) {
    if (kCrashSigs[i]) sigaction(kCrashSigs[i], &g_oldcrashacts[i], NULL);
  }
}

static wontreturn relegated noinstrument void __minicrash(int sig,
                                                          struct siginfo *si,
                                                          ucontext_t *ctx,
                                                          const char *kind) {
  kprintf("\n"
          "\n"
          "CRASHED %s WITH %G\n"
          "%s\n"
          "RIP %x\n"
          "RSP %x\n"
          "RBP %x\n"
          "\n",
          kind, sig, __argv[0], ctx ? ctx->uc_mcontext.rip : 0,
          ctx ? ctx->uc_mcontext.rsp : 0, ctx ? ctx->uc_mcontext.rbp : 0);
  __restorewintty();
  _Exit(119);
}

/**
 * Crashes in a developer-friendly human-centric way.
 *
 * We first try to launch GDB if it's an interactive development
 * session. Otherwise we show a really good crash report, sort of like
 * Python, that includes filenames and line numbers. Many editors, e.g.
 * Emacs, will even recognize its syntax for quick hopping to the
 * failing line. That's only possible if the the .com.dbg file is in the
 * same folder. If the concomitant debug binary can't be found, we
 * simply print addresses which may be cross-referenced using objdump.
 *
 * This function never returns, except for traps w/ human supervision.
 */
relegated noinstrument void __oncrash(int sig, struct siginfo *si,
                                      ucontext_t *ctx) {
  intptr_t rip;
  int gdbpid, err, st, ft;
  static bool noreentry, notpossible;
  st = __strace, __strace = 0;
  ft = g_ftrace, g_ftrace = 0;
  if (_lockcmpxchg(&noreentry, false, true)) {
    if (!__vforked) {
      rip = ctx ? ctx->uc_mcontext.rip : 0;
      err = errno;
      if ((gdbpid = IsDebuggerPresent(true))) {
        DebugBreak();
      } else if (__nocolor || g_isrunningundermake) {
        gdbpid = -1;
      } else if (!IsTiny() && IsLinux() && FindDebugBinary() && !__isworker) {
        RestoreDefaultCrashSignalHandlers();
        gdbpid = AttachDebugger(
            ((sig == SIGTRAP || sig == SIGQUIT) &&
             (rip >= (intptr_t)&_base && rip < (intptr_t)&_etext))
                ? rip
                : 0);
      }
      if (!(gdbpid > 0 && (sig == SIGTRAP || sig == SIGQUIT))) {
        __restore_tty();
        ShowCrashReport(err, sig, si, ctx);
        __restorewintty();
        _Exit(128 + sig);
      }
    } else {
      __minicrash(sig, si, ctx, "WHILE VFORKED");
    }
  } else if (sig == SIGTRAP) {
    /* chances are IsDebuggerPresent() confused strace w/ gdb */
    g_ftrace = ft;
    __strace = st;
    return;
  } else if (_lockcmpxchg(&notpossible, false, true)) {
    __minicrash(sig, si, ctx, "WHILE CRASHING");
  } else {
    for (;;) {
      asm("ud2");
    }
  }
  noreentry = false;
  ++g_ftrace;
}
