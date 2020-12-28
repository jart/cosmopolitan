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
#include "libc/macros.h"
#include "libc/nexgen32e/stackframe.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/memtrack.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sig.h"

STATIC_YOINK("ftoa");
STATIC_YOINK("ntoa");
STATIC_YOINK("stoa");

/**
 * @fileoverview Abnormal termination handling & GUI debugging.
 * @see libc/onkill.c
 */

struct siginfo;

static const char kGregOrder[17] forcealign(1) = {
    13, 11, 8, 14, 12, 9, 10, 15, 16, 0, 1, 2, 3, 4, 5, 6, 7,
};

static const char kGregNames[17][4] forcealign(1) = {
    "R8",  "R9",  "R10", "R11", "R12", "R13", "R14", "R15", "RDI",
    "RSI", "RBP", "RBX", "RDX", "RAX", "RCX", "RSP", "RIP",
};

static const char kGodHatesFlags[12] forcealign(1) = "CVPRAKZSTIDO";
static const char kCrashSigNames[8][5] forcealign(1) = {
    "QUIT", "FPE", "ILL", "SEGV", "TRAP", "ABRT", "BUS"};

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

relegated static void DescribeCpuFlags(int fd, unsigned flags) {
  unsigned i;
  char buf[64], *p;
  p = buf;
  for (i = 0; i < ARRAYLEN(kGodHatesFlags); ++i) {
    if (flags & 1) {
      *p++ = ' ';
      *p++ = kGodHatesFlags[i];
      *p++ = 'F';
    }
    flags >>= 1;
  }
  p = stpcpy(p, " IOPL");
  *p++ = '0' + (flags & 3);
  write(fd, buf, p - buf);
}

relegated static void ShowGeneralRegisters(int fd, ucontext_t *ctx) {
  size_t i, j, k;
  long double st;
  write(fd, "\r\n", 2);
  for (i = 0, j = 0, k = 0; i < ARRAYLEN(kGregNames); ++i) {
    if (j > 0) write(fd, " ", 1);
    (dprintf)(fd, "%-3s %016lx", kGregNames[(unsigned)kGregOrder[i]],
              ctx->uc_mcontext.gregs[(unsigned)kGregOrder[i]]);
    if (++j == 3) {
      j = 0;
      memcpy(&st, (char *)&ctx->fpustate.st[k], sizeof(st));
      (dprintf)(fd, " %s(%zu) %Lf", "ST", k, st);
      ++k;
      write(fd, "\r\n", 2);
    }
  }
  DescribeCpuFlags(fd, ctx->uc_mcontext.gregs[REG_EFL]);
}

relegated static void ShowSseRegisters(int fd, ucontext_t *ctx) {
  size_t i;
  write(fd, "\r\n\r\n", 4);
  for (i = 0; i < 8; ++i) {
    (dprintf)(fd, VEIL("r", "%s%-2zu %016lx%016lx %s%-2d %016lx%016lx\r\n"),
              "XMM", i + 0, ctx->fpustate.xmm[i + 0].u64[0],
              ctx->fpustate.xmm[i + 0].u64[1], "XMM", i + 8,
              ctx->fpustate.xmm[i + 8].u64[0], ctx->fpustate.xmm[i + 8].u64[1]);
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
  struct utsname names;
  (dprintf)(fd, VEIL("r", "\r\n%serror%s: Uncaught SIG%s\r\n  %s\r\n  %s\r\n"),
            RED2, RESET, TinyStrSignal(sig), getauxval(AT_EXECFN),
            strerror(err));
  if (uname(&names) != -1) {
    (dprintf)(fd, VEIL("r", "  %s %s %s %s\r\n"), names.sysname, names.nodename,
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
  for (i = 0; i < g_argc; ++i) {
    write(fd, g_argv[i], strlen(g_argv[i]));
    write(fd, "\r\n", 2);
  }
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
  if (once) abort();
  once = true;
  /* TODO(jart): Needs translation for ucontext_t and possibly siginfo_t. */
  if (IsFreebsd() || IsOpenbsd()) ctx = NULL;
  rip = ctx ? ctx->uc_mcontext.rip : 0;
  if ((gdbpid = IsDebuggerPresent(true))) {
    DebugBreak();
  } else if (isterminalinarticulate() || isrunningundermake()) {
    gdbpid = -1;
  } else {
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
