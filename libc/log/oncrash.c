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
#include "libc/calls/sigbits.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/utsname.h"
#include "libc/calls/ucontext.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/log/backtrace.internal.h"
#include "libc/log/color.internal.h"
#include "libc/log/gdb.h"
#include "libc/log/internal.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/stackframe.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/memtrack.h"
#include "libc/runtime/pc.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sig.h"

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
static const char kCrashSigNames[8][5] forcealign(1) = {
    "QUIT", "FPE", "ILL", "SEGV", "TRAP", "ABRT", "BUS", "PIPE"};

hidden int kCrashSigs[8];
hidden struct sigaction g_oldcrashacts[8];

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
  write(fd, "\r\n", 2);
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

relegated static void DescribeCpuFlags(int fd, int flags, int x87sw,
                                       int mxcsr) {
  unsigned i;
  char buf[64], *p;
  p = buf;
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
  write(fd, buf, p - buf);
}

relegated static void ShowGeneralRegisters(int fd, ucontext_t *ctx) {
  size_t i, j, k;
  long double st;
  write(fd, "\r\n", 2);
  for (i = 0, j = 0, k = 0; i < ARRAYLEN(kGregNames); ++i) {
    if (j > 0) write(fd, " ", 1);
    dprintf(fd, "%-3s %016lx", kGregNames[(unsigned)kGregOrder[i]],
            ctx->uc_mcontext.gregs[(unsigned)kGregOrder[i]]);
    if (++j == 3) {
      j = 0;
      if (ctx->uc_mcontext.fpregs) {
        memcpy(&st, (char *)&ctx->uc_mcontext.fpregs->st[k], sizeof(st));
      } else {
        memset(&st, 0, sizeof(st));
      }
      dprintf(fd, " %s(%zu) %Lg", "ST", k, st);
      ++k;
      write(fd, "\r\n", 2);
    }
  }
  DescribeCpuFlags(
      fd, ctx->uc_mcontext.gregs[REG_EFL],
      ctx->uc_mcontext.fpregs ? ctx->uc_mcontext.fpregs->swd : 0,
      ctx->uc_mcontext.fpregs ? ctx->uc_mcontext.fpregs->mxcsr : 0);
}

relegated static void ShowSseRegisters(int fd, ucontext_t *ctx) {
  size_t i;
  if (ctx->uc_mcontext.fpregs) {
    write(fd, "\r\n\r\n", 4);
    for (i = 0; i < 8; ++i) {
      dprintf(fd, "%s%-2zu %016lx%016lx %s%-2d %016lx%016lx\r\n", "XMM", i + 0,
              ctx->uc_mcontext.fpregs->xmm[i + 0].u64[1],
              ctx->uc_mcontext.fpregs->xmm[i + 0].u64[0], "XMM", i + 8,
              ctx->uc_mcontext.fpregs->xmm[i + 8].u64[1],
              ctx->uc_mcontext.fpregs->xmm[i + 8].u64[0]);
    }
  }
}

relegated static void ShowMemoryMappings(int outfd) {
  ssize_t rc;
  int c, infd;
  char buf[64];
  if (!IsTiny()) {
    PrintMemoryIntervals(outfd, &_mmi);
    if ((infd = open("/proc/self/maps", O_RDONLY)) != -1) {
      while ((rc = read(infd, buf, sizeof(buf))) > 0) {
        write(outfd, buf, rc);
      }
    }
    close(infd);
  }
}

relegated static void ShowCrashReport(int err, int fd, int sig,
                                      ucontext_t *ctx) {
  int i;
  char hostname[64];
  struct utsname names;
  strcpy(hostname, "unknown");
  gethostname(hostname, sizeof(hostname));
  dprintf(fd, "\r\n%serror%s: Uncaught SIG%s on %s\r\n  %s\r\n  %s\r\n", RED2,
          RESET, TinyStrSignal(sig), hostname, getauxval(AT_EXECFN),
          strerror(err));
  if (uname(&names) != -1) {
    dprintf(fd, "  %s %s %s %s\r\n", names.sysname, names.nodename,
            names.release, names.version);
  }
  ShowFunctionCalls(fd, ctx);
  if (ctx) {
    ShowGeneralRegisters(fd, ctx);
    ShowSseRegisters(fd, ctx);
  }
  write(fd, "\r\n", 2);
  ShowMemoryMappings(fd);
  write(fd, "\r\n", 2);
  for (i = 0; i < __argc; ++i) {
    write(fd, __argv[i], strlen(__argv[i]));
    write(fd, " ", 1);
  }
  write(fd, "\r\n", 2);
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
  ShowCrashReport(err, STDERR_FILENO, sig, ctx);
  exit(128 + sig);
  unreachable;
}
