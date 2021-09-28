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
#include "libc/calls/sigbits.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/termios.h"
#include "libc/calls/struct/utsname.h"
#include "libc/calls/termios.h"
#include "libc/calls/ucontext.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/intrin/asan.internal.h"
#include "libc/log/backtrace.internal.h"
#include "libc/log/color.internal.h"
#include "libc/log/gdb.h"
#include "libc/log/internal.h"
#include "libc/log/libfatal.internal.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/bsr.h"
#include "libc/nexgen32e/stackframe.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/pc.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/nr.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/termios.h"

/**
 * @fileoverview Abnormal termination handling & GUI debugging.
 * @see libc/onkill.c
 */

static const char kGregOrder[17] forcealign(1) = {
    13, 11, 8, 14, 12, 9, 10, 15, 16, 0, 1, 2, 3, 4, 5, 6, 7,
};

static const char kGregNames[17][4] forcealign(1) = {
    "R8",  "R9",  "R10", "R11", "R12", "R13", "R14", "R15", "RDI",
    "RSI", "RBP", "RBX", "RDX", "RAX", "RCX", "RSP", "RIP",
};

static const char kCpuFlags[12] forcealign(1) = "CVPRAKZSTIDO";
static const char kFpuExceptions[6] forcealign(1) = "IDZOUP";

/* <SYNC-LIST>: showcrashreports.c, oncrashthunks.S, oncrash.c */
int kCrashSigs[8];
struct sigaction g_oldcrashacts[8];
static const char kCrashSigNames[8][5] forcealign(1) = {
    "QUIT",  //
    "FPE",   //
    "ILL",   //
    "SEGV",  //
    "TRAP",  //
    "ABRT",  //
    "BUS",   //
    "PIPE",  //
};
/* </SYNC-LIST>: showcrashreports.c, oncrashthunks.S, oncrash.c */

relegated static const char *TinyStrSignal(int sig) {
  size_t i;
  for (i = 0; i < ARRAYLEN(kCrashSigs); ++i) {
    if (kCrashSigs[i] && sig == kCrashSigs[i]) {
      return kCrashSigNames[i];
    }
  }
  return "???";
}

relegated static void ShowFunctionCalls(int fd, ucontext_t *ctx) {
  struct StackFrame *bp;
  struct StackFrame goodframe;
  write(fd, "\n", 1);
  if (ctx && ctx->uc_mcontext.rip && ctx->uc_mcontext.rbp) {
    goodframe.next = (struct StackFrame *)ctx->uc_mcontext.rbp;
    goodframe.addr = ctx->uc_mcontext.rip;
    bp = &goodframe;
    ShowBacktrace(fd, bp);
  }
}

relegated static char *AddFlag(char *p, int b, const char *s) {
  if (b) p = stpcpy(p, s);
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

relegated static char *ShowGeneralRegisters(char *p, ucontext_t *ctx) {
  int64_t x;
  const char *s;
  size_t i, j, k;
  long double st;
  *p++ = '\n';
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
      p = __uintcpy(p, x % 1000), *p++ = '\n';
    }
  }
  return DescribeCpuFlags(
      p, ctx->uc_mcontext.gregs[REG_EFL],
      ctx->uc_mcontext.fpregs ? ctx->uc_mcontext.fpregs->swd : 0,
      ctx->uc_mcontext.fpregs ? ctx->uc_mcontext.fpregs->mxcsr : 0);
}

relegated static char *ShowSseRegisters(char *p, ucontext_t *ctx) {
  size_t i;
  if (ctx->uc_mcontext.fpregs) {
    p = __stpcpy(p, "\n\n");
    for (i = 0; i < 8; ++i) {
      p = __stpcpy(p, "XMM");
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
      *p++ = '\n';
    }
  }
  return p;
}

relegated static void ShowMemoryMappings(int outfd) {
  ssize_t rc;
  int c, infd;
  char buf[64];
  if (!IsTiny()) {
    PrintMemoryIntervals(outfd, &_mmi);
    if ((infd = open("/proc/self/maps", O_RDONLY)) != -1) {
      while ((rc = read(infd, buf, sizeof(buf))) > 0) {
        __write(buf, rc);
      }
    }
    close(infd);
  }
}

void ShowCrashReportHook(int, int, int, struct siginfo *, ucontext_t *);

relegated void ShowCrashReport(int err, int fd, int sig, struct siginfo *si,
                               ucontext_t *ctx) {
  int i;
  char *p;
  bool colorful;
  char hostname[64];
  struct utsname names;
  static char buf[4096];
  if (weaken(ShowCrashReportHook)) {
    ShowCrashReportHook(err, fd, sig, si, ctx);
  }
  colorful = cancolor();
  __stpcpy(hostname, "unknown");
  gethostname(hostname, sizeof(hostname));
  p = buf;
  p = __stpcpy(p, "\n");
  if (colorful) p = __stpcpy(p, "\e[30;101m");
  p = __stpcpy(p, "error");
  if (colorful) p = __stpcpy(p, "\e[0m");
  p = __stpcpy(p, ": Uncaught SIG");
  p = __stpcpy(p, TinyStrSignal(sig));
  if (si) {
    p = __stpcpy(p, " (");
    p = __stpcpy(p, GetSiCodeName(sig, si->si_code));
    p = __stpcpy(p, ")");
  }
  p = __stpcpy(p, " on ");
  p = __stpcpy(p, hostname);
  p = __stpcpy(p, " pid ");
  p = __intcpy(p, __getpid());
  p = __stpcpy(p, "\n  ");
  p = __stpcpy(p, program_invocation_name);
  p = __stpcpy(p, "\n  ");
  p = __stpcpy(p, strerror(err));
  *p++ = '\n';
  if (uname(&names) != -1) {
    p = __stpcpy(p, "  ");
    p = __stpcpy(p, names.sysname), *p++ = ' ';
    p = __stpcpy(p, names.nodename), *p++ = ' ';
    p = __stpcpy(p, names.release), *p++ = ' ';
    p = __stpcpy(p, names.version), *p++ = '\n';
  }
  __write(buf, p - buf);
  ShowFunctionCalls(fd, ctx);
  if (ctx) {
    p = buf;
    p = ShowGeneralRegisters(p, ctx);
    p = ShowSseRegisters(p, ctx);
    *p++ = '\n';
    __write(buf, p - buf);
  }
  p = buf;
  *p++ = '\n';
  ShowMemoryMappings(fd);
  __write(buf, p - buf);
  if (__argv) {
    for (i = 0; i < __argc; ++i) {
      if (!__argv[i]) continue;
      if (IsAsan() && !__asan_is_valid(__argv[i], 1)) continue;
      __write(__argv[i], strlen(__argv[i]));
      __write(" ", 1);
    }
  }
  __write("\n", 1);
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
relegated void __oncrash(int sig, struct siginfo *si, ucontext_t *ctx) {
  intptr_t rip;
  int gdbpid, err;
  static bool once;
  err = errno;
  if (once) _exit(119);
  once = true;
  rip = ctx ? ctx->uc_mcontext.rip : 0;
  if ((gdbpid = IsDebuggerPresent(true))) {
    DebugBreak();
  } else if (IsTerminalInarticulate() || IsRunningUnderMake()) {
    gdbpid = -1;
  } else if (FindDebugBinary()) {
    RestoreDefaultCrashSignalHandlers();
    gdbpid =
        attachdebugger(((sig == SIGTRAP || sig == SIGQUIT) &&
                        (rip >= (intptr_t)&_base && rip < (intptr_t)&_etext))
                           ? rip
                           : 0);
  }
  if (gdbpid > 0 && (sig == SIGTRAP || sig == SIGQUIT)) return;
  __restore_tty(1);
  ShowCrashReport(err, STDERR_FILENO, sig, si, ctx);
  exit(128 + sig);
  unreachable;
}
