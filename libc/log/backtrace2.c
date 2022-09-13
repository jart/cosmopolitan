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
#include "libc/calls/internal.h"
#include "libc/calls/struct/rusage.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/fmt.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/promises.internal.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/log/backtrace.internal.h"
#include "libc/log/color.internal.h"
#include "libc/log/log.h"
#include "libc/mem/alg.h"
#include "libc/mem/bisectcarleft.internal.h"
#include "libc/mem/gc.internal.h"
#include "libc/nexgen32e/gc.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/stdio/append.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sig.h"
#include "libc/thread/tls.h"
#include "libc/x/x.h"

#define kBacktraceMaxFrames 128
#define kBacktraceBufSize   ((kBacktraceMaxFrames - 1) * (18 + 1))

static void ShowHint(const char *s) {
  kprintf("%snote: %s%s\n", SUBTLE, s, RESET);
}

static int PrintBacktraceUsingAddr2line(int fd, const struct StackFrame *bp) {
  bool ok;
  ssize_t got;
  intptr_t addr;
  size_t i, j, gi;
  int ws, pid, pipefds[2];
  struct Garbages *garbage;
  const struct StackFrame *frame;
  char *debugbin, *p1, *p2, *p3, *addr2line;
  char buf[kBacktraceBufSize], *argv[kBacktraceMaxFrames];

  if (!PLEDGED(STDIO) || !PLEDGED(EXEC) || !PLEDGED(EXEC)) {
    return -1;
  }

  if (!(debugbin = FindDebugBinary())) {
    return -1;
  }

  if (!(addr2line = GetAddr2linePath())) {
    if (IsLinux()) {
      ShowHint("can't find addr2line on path or in ADDR2LINE");
    }
    return -1;
  }

  // DWARF is a weak standard. Platforms that use LLVM or old GNU
  // usually can't be counted upon to print backtraces correctly.
  if (!IsLinux() && !IsWindows()) {
    ShowHint("won't print addr2line backtrace because probably llvm");
    return -1;
  }

  // backtrace_test.com failing on windows for some reason via runitd
  // don't want to pull in the high-level syscalls here anyway
  if (IsWindows()) {
    return -1;
  }

  // doesn't work on rhel5
  if (IsLinux() && !__is_linux_2_6_23()) {
    return -1;
  }

  i = 0;
  j = 0;
  argv[i++] = "addr2line";
  argv[i++] = "-a"; /* filter out w/ shell script wrapper for old versions */
  argv[i++] = "-pCife";
  argv[i++] = debugbin;
  garbage = __tls_enabled ? __get_tls()->tib_garbages : 0;
  gi = garbage ? garbage->i : 0;
  for (frame = bp; frame && i < kBacktraceMaxFrames - 1; frame = frame->next) {
    addr = frame->addr;
    if (addr == _weakaddr("__gc")) {
      do {
        --gi;
      } while ((addr = garbage->p[gi].ret) == _weakaddr("__gc"));
    }
    argv[i++] = buf + j;
    buf[j++] = '0';
    buf[j++] = 'x';
    j += uint64toarray_radix16(addr - 1, buf + j) + 1;
  }
  argv[i++] = NULL;
  if (sys_pipe2(pipefds, O_CLOEXEC) == -1) {
    return -1;
  }
  if ((pid = fork()) == -1) {
    sys_close(pipefds[0]);
    sys_close(pipefds[1]);
    return -1;
  }
  if (!pid) {
    sys_dup2(pipefds[1], 1);
    sys_execve(addr2line, argv, environ);
    _Exit(127);
  }
  sys_close(pipefds[1]);
  for (;;) {
    got = sys_read(pipefds[0], buf, kBacktraceBufSize);
    if (!got) break;
    if (got == -1 && errno == EINTR) {
      errno = 0;
      continue;
    }
    if (got == -1) {
      kprintf("error reading backtrace %m\n");
      break;
    }
    p1 = buf;
    p3 = p1 + got;
    /*
     * remove racist output from gnu tooling, that can't be disabled
     * otherwise, since it breaks other tools like emacs that aren't
     * equipped to ignore it, and what's most problematic is that
     * addr2line somehow manages to put the racism onto the one line
     * in the backtrace we actually care about.
     */
    for (got = p3 - buf, p1 = buf; got;) {
      if ((p2 = memmem(p1, got, " (discriminator ",
                       strlen(" (discriminator ") - 1)) &&
          (p3 = memchr(p2, '\n', got - (p2 - p1)))) {
        if (p3 > p2 && p3[-1] == '\r') --p3;
        write(2, p1, p2 - p1);
        got -= p3 - p1;
        p1 += p3 - p1;
      } else {
        write(2, p1, got);
        break;
      }
    }
  }
  sys_close(pipefds[0]);
  while (sys_wait4(pid, &ws, 0, 0) == -1) {
    if (errno == EINTR) continue;
    return -1;
  }
  if (WIFEXITED(ws) && !WEXITSTATUS(ws)) {
    return 0;
  } else {
    return -1;
  }
}

static int PrintBacktrace(int fd, const struct StackFrame *bp) {
  if (!IsTiny() && !__isworker) {
    if (PrintBacktraceUsingAddr2line(fd, bp) != -1) {
      return 0;
    }
  }
  return PrintBacktraceUsingSymbols(fd, bp, GetSymbolTable());
}

void ShowBacktrace(int fd, const struct StackFrame *bp) {
#ifdef __FNO_OMIT_FRAME_POINTER__
  /* asan runtime depends on this function */
  --__ftrace;
  --__strace;
  if (!bp) bp = __builtin_frame_address(0);
  PrintBacktrace(fd, bp);
  ++__strace;
  ++__ftrace;
#else
  (fprintf)(stderr, "ShowBacktrace() needs these flags to show C backtrace:\n"
                    "\t-D__FNO_OMIT_FRAME_POINTER__\n"
                    "\t-fno-omit-frame-pointer\n");
#endif
}
