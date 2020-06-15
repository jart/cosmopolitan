/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/sigbits.h"
#include "libc/calls/struct/utsname.h"
#include "libc/calls/ucontext.h"
#include "libc/dce.h"
#include "libc/fmt/fmt.h"
#include "libc/log/gdb.h"
#include "libc/log/internal.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/sig.h"

STATIC_YOINK("ftoa");
STATIC_YOINK("ntoa");
STATIC_YOINK("stoa");

/**
 * @fileoverview Abnormal termination handling & GUI debugging.
 * @see libc/onkill.c
 */

struct siginfo;

aligned(1) const char kGregOrder[17] = {13, 11, 8, 14, 12, 9, 10, 15, 16,
                                        0,  1,  2, 3,  4,  5, 6,  7};
aligned(1) const char kGregNames[17][4] = {
    "R8",  "R9",  "R10", "R11", "R12", "R13", "R14", "R15", "RDI",
    "RSI", "RBP", "RBX", "RDX", "RAX", "RCX", "RSP", "RIP"};
aligned(1) const char kGodHatesFlags[12][2] = {
    "CF", "VF", "PF", "RF", "AF", "KF", "ZF", "SF", "TF", "IF", "DF", "OF"};
aligned(1) const char kCrashSigNames[8][5] = {"QUIT", "FPE",  "ILL", "SEGV",
                                              "TRAP", "ABRT", "BUS"};

int kCrashSigs[8];
struct sigaction g_oldcrashacts[8];

relegated static const char *tinystrsignal(int sig) {
  size_t i;
  for (i = 0; i < ARRAYLEN(kCrashSigs); ++i) {
    if (kCrashSigs[i] && sig == kCrashSigs[i]) {
      return kCrashSigNames[i];
    }
  }
  return "???";
}

relegated static void showfunctioncalls(FILE *f, ucontext_t *ctx) {
  fputc('\n', f);
  struct StackFrame *bp;
  struct StackFrame goodframe;
  if (ctx && ctx->uc_mcontext.rip && ctx->uc_mcontext.rbp) {
    goodframe.next = (struct StackFrame *)ctx->uc_mcontext.rbp;
    goodframe.addr = ctx->uc_mcontext.rip;
    bp = &goodframe;
    showbacktrace(f, bp);
  }
}

relegated static void describecpuflags(FILE *f, unsigned efl) {
  size_t i;
  for (i = 0; i < ARRAYLEN(kGodHatesFlags); ++i) {
    if (efl & 1) {
      fputc(' ', f);
      fputc(kGodHatesFlags[i][0], f);
      fputc(kGodHatesFlags[i][1], f);
    }
    efl >>= 1;
  }
  (fprintf)(f, " %s%d\n", "IOPL", efl & 3);
}

relegated static void showgeneralregisters(FILE *f, ucontext_t *ctx) {
  size_t i, j, k;
  long double st;
  fputc('\n', f);
  for (i = 0, j = 0, k = 0; i < ARRAYLEN(kGregNames); ++i) {
    if (j > 0) {
      fputc(' ', f);
    }
    (fprintf)(f, "%-3s %016lx", kGregNames[(unsigned)kGregOrder[i]],
              ctx->uc_mcontext.gregs[(unsigned)kGregOrder[i]]);
    if (++j == 3) {
      j = 0;
      memcpy(&st, (char *)&ctx->fpustate.st[k], sizeof(st));
      (fprintf)(f, " %s(%zu) %Lf", "ST", k, st);
      ++k;
      fputc('\r', f);
      fputc('\n', f);
    }
  }
  fflush(stderr);
  describecpuflags(f, ctx->uc_mcontext.gregs[REG_EFL]);
}

relegated static void showsseregisters(FILE *f, ucontext_t *ctx) {
  size_t i;
  fputc('\n', f);
  for (i = 0; i < 8; ++i) {
    (fprintf)(f, VEIL("r", "%s%-2zu %016lx%016lx %s%-2d %016lx%016lx\n"), "XMM",
              i + 0, ctx->fpustate.xmm[i + 0].u64[0],
              ctx->fpustate.xmm[i + 0].u64[1], "XMM", i + 8,
              ctx->fpustate.xmm[i + 8].u64[0], ctx->fpustate.xmm[i + 8].u64[1]);
  }
}

relegated static void showmemorymappings(FILE *f) {
  int c;
  FILE *f2;
  if (!IsTiny()) {
    showmappings(f);
    if (IsLinux()) {
      if ((f2 = fopen("/proc/self/maps", "r"))) {
        while ((c = fgetc(f2)) != -1) {
          if (fputc(c, f) == -1) break;
        }
      }
      fclose(f2);
    }
  }
}

relegated static void showcrashreport(int err, FILE *f, int sig,
                                      ucontext_t *ctx) {
  struct utsname names;
  (fprintf)(f, VEIL("r", "\n%serror%s: Uncaught SIG%s\n  %s\n  %s\n"), RED2,
            RESET, tinystrsignal(sig), getauxval(AT_EXECFN), strerror(err));
  if (uname(&names) != -1) {
    (fprintf)(f, VEIL("r", "  %s %s %s %s\n"), names.sysname, names.nodename,
              names.release, names.version);
  }
  showfunctioncalls(f, ctx);
  if (ctx) {
    showgeneralregisters(f, ctx);
    showsseregisters(f, ctx);
  }
  fputc('\n', f);
  memsummary(f);
  showmemorymappings(f);
}

relegated static void restoredefaultcrashsignalhandlers(void) {
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
relegated void oncrash(int sig, struct siginfo *si, ucontext_t *ctx) {
  intptr_t rip;
  int gdbpid, err;
  static bool once;
  err = errno;
  g_runstate |= RUNSTATE_BROKEN;
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
    restoredefaultcrashsignalhandlers();
    gdbpid =
        attachdebugger(((sig == SIGTRAP || sig == SIGQUIT) &&
                        (rip >= (intptr_t)&_base && rip < (intptr_t)&_etext))
                           ? rip
                           : 0);
  }
  if (gdbpid > 0 && (sig == SIGTRAP || sig == SIGQUIT)) return;
  showcrashreport(err, stderr, sig, ctx);
  quick_exit(128 + sig);
  unreachable;
}
