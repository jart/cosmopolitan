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
#include "libc/alg/alg.h"
#include "libc/alg/bisectcarleft.internal.h"
#include "libc/bits/safemacros.internal.h"
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/calls/sigbits.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/fmt.h"
#include "libc/fmt/itoa.h"
#include "libc/log/backtrace.internal.h"
#include "libc/log/libfatal.internal.h"
#include "libc/log/log.h"
#include "libc/nexgen32e/gc.internal.h"
#include "libc/runtime/gc.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/stdio/append.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sig.h"
#include "libc/x/x.h"

#define kBacktraceMaxFrames 128
#define kBacktraceBufSize   ((kBacktraceMaxFrames - 1) * (18 + 1))

static noasan int PrintBacktraceUsingAddr2line(int fd,
                                               const struct StackFrame *bp) {
  ssize_t got;
  intptr_t addr;
  size_t i, j, gi;
  int ws, pid, pipefds[2];
  struct Garbages *garbage;
  sigset_t chldmask, savemask;
  const struct StackFrame *frame;
  char *debugbin, *p1, *p2, *p3, *addr2line;
  char buf[kBacktraceBufSize], *argv[kBacktraceMaxFrames];
  if (IsOpenbsd()) return -1;
  if (IsWindows()) return -1;
  if (!(debugbin = FindDebugBinary())) return -1;
  if (!(addr2line = GetAddr2linePath())) return -1;
  i = 0;
  j = 0;
  argv[i++] = "addr2line";
  argv[i++] = "-a"; /* filter out w/ shell script wrapper for old versions */
  argv[i++] = "-pCife";
  argv[i++] = debugbin;
  garbage = weaken(__garbage);
  gi = garbage ? garbage->i : 0;
  for (frame = bp; frame && i < kBacktraceMaxFrames - 1; frame = frame->next) {
    if (!IsValidStackFramePointer(frame)) {
      return -1;
    }
    addr = frame->addr;
    if (addr == weakaddr("__gc")) {
      do {
        --gi;
      } while ((addr = garbage->p[gi].ret) == weakaddr("__gc"));
    }
    argv[i++] = buf + j;
    buf[j++] = '0';
    buf[j++] = 'x';
    j += uint64toarray_radix16(addr - 1, buf + j) + 1;
  }
  argv[i++] = NULL;
  sigemptyset(&chldmask);
  sigaddset(&chldmask, SIGCHLD);
  sigprocmask(SIG_BLOCK, &chldmask, &savemask);
  pipe(pipefds);
  if (!(pid = vfork())) {
    sigprocmask(SIG_SETMASK, &savemask, NULL);
    dup2(pipefds[1], 1);
    close(pipefds[0]);
    close(pipefds[1]);
    execvp(addr2line, argv);
    _exit(127);
  }
  close(pipefds[1]);
  while ((got = read(pipefds[0], buf, kBacktraceBufSize)) > 0) {
    p1 = buf;
    p3 = p1 + got;

    /*
     * Remove deep libc error reporting facilities from backtraces.
     *
     * For example, if the following shows up in Emacs:
     *
     *     40d097: __die at libc/log/die.c:33
     *     434daa: __asan_die at libc/intrin/asan.c:483
     *     435146: __asan_report_memory_fault at libc/intrin/asan.c:524
     *     435b32: __asan_report_store at libc/intrin/asan.c:719
     *     43472e: __asan_report_store1 at libc/intrin/somanyasan.S:118
     *     40c3a9: GetCipherSuite at net/https/getciphersuite.c:80
     *     4383a5: GetCipherSuite_test at test/net/https/getciphersuite.c:23
     *     ...
     *
     * Then it's unpleasant to need to press C-x C-n six times.
     */
#if 0
    while ((p2 = memchr(p1, '\n', p3 - p1))) {
      if (memmem(p1, p2 - p1, ": __asan_", 9) ||
          memmem(p1, p2 - p1, ": __die", 7)) {
        memmove(p1, p2 + 1, p3 - (p2 + 1));
        p3 -= p2 + 1 - p1;
      } else {
        p1 = p2 + 1;
        break;
      }
    }
#endif

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
        __write(p1, p2 - p1);
        got -= p3 - p1;
        p1 += p3 - p1;
      } else {
        __write(p1, got);
        break;
      }
    }
  }
  close(pipefds[0]);
  while (waitpid(pid, &ws, 0) == -1) {
    if (errno == EINTR) continue;
    return -1;
  }
  sigprocmask(SIG_SETMASK, &savemask, NULL);
  if (WIFEXITED(ws) && !WEXITSTATUS(ws)) {
    return 0;
  } else {
    return -1;
  }
}

static noasan int PrintBacktrace(int fd, const struct StackFrame *bp) {
  if (!IsTiny()) {
    if (PrintBacktraceUsingAddr2line(fd, bp) != -1) {
      return 0;
    }
  }
  return PrintBacktraceUsingSymbols(fd, bp, GetSymbolTable());
}

noasan void ShowBacktrace(int fd, const struct StackFrame *bp) {
#ifdef __FNO_OMIT_FRAME_POINTER__
  /* asan runtime depends on this function */
  static bool noreentry;
  ++g_ftrace;
  if (!bp) bp = __builtin_frame_address(0);
  if (!noreentry) {
    noreentry = true;
    PrintBacktrace(fd, bp);
    noreentry = false;
  }
  --g_ftrace;
#else
  __printf("ShowBacktrace() needs these flags to show C backtrace:\n"
           "\t-D__FNO_OMIT_FRAME_POINTER__\n"
           "\t-fno-omit-frame-pointer\n");
#endif
}
