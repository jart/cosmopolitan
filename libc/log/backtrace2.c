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
#include "libc/bits/safemacros.h"
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/calls/hefty/spawn.h"
#include "libc/dce.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/fmt.h"
#include "libc/log/backtrace.internal.h"
#include "libc/log/log.h"
#include "libc/nexgen32e/gc.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"

#define kBacktraceMaxFrames 128
#define kBacktraceBufSize   ((kBacktraceMaxFrames - 1) * (16 + 1))

static int PrintBacktraceUsingAddr2line(int fd, const struct StackFrame *bp) {
  ssize_t got;
  intptr_t addr;
  size_t i, j, gi;
  int rc, pid, tubes[3];
  struct Garbages *garbage;
  const struct StackFrame *frame;
  const char *debugbin, *p1, *p2, *p3, *addr2line;
  char buf[kBacktraceBufSize], *argv[kBacktraceMaxFrames];
  if (!(debugbin = FindDebugBinary()) || !(addr2line = GetAddr2linePath())) {
    return -1;
  }
  i = 0;
  j = 0;
  argv[i++] = "addr2line";
  argv[i++] = "-a"; /* filter out w/ shell script wrapper for old versions */
  argv[i++] = "-pCife";
  argv[i++] = debugbin;
  garbage = weaken(__garbage);
  gi = garbage ? garbage->i : 0;
  for (frame = bp; frame && i < kBacktraceMaxFrames - 1; frame = frame->next) {
    addr = frame->addr;
    if (addr == weakaddr("__gc")) {
      do {
        --gi;
      } while ((addr = garbage->p[gi].ret) == weakaddr("__gc"));
    }
    argv[i++] = &buf[j];
    j += snprintf(&buf[j], 17, "%#x", addr - 1) + 1;
  }
  argv[i++] = NULL;
  tubes[0] = STDIN_FILENO;
  tubes[1] = -1;
  tubes[2] = STDERR_FILENO;
  if ((pid = spawnve(0, tubes, addr2line, argv, environ)) == -1) {
    return -1;
  }
  while ((got = read(tubes[1], buf, kBacktraceBufSize)) > 0) {
    for (p1 = buf; got;) {
      /*
       * remove racist output from gnu tooling, that can't be disabled
       * otherwise, since it breaks other tools like emacs that aren't
       * equipped to ignore it, and what's most problematic is that
       * addr2line somehow manages to put the racism onto the one line
       * in the backtrace we actually care about.
       */
      if ((p2 = memmem(p1, got, " (discriminator ",
                       strlen(" (discriminator ") - 1)) &&
          (p3 = memchr(p2, '\n', got - (p2 - p1)))) {
        if (p3 > p2 && p3[-1] == '\r') --p3;
        write(fd, p1, p2 - p1);
        got -= p3 - p1;
        p1 += p3 - p1;
      } else {
        write(fd, p1, got);
        break;
      }
    }
  }
  close(tubes[1]);
  if (waitpid(pid, &rc, 0) == -1) return -1;
  if (WEXITSTATUS(rc) != 0) return -1;
  return 0;
}

static int PrintBacktrace(int fd, const struct StackFrame *bp) {
  if (!IsTiny()) {
    if (PrintBacktraceUsingAddr2line(fd, bp) != -1) {
      return 0;
    }
  }
  return PrintBacktraceUsingSymbols(fd, bp, GetSymbolTable());
}

void ShowBacktrace(int fd, const struct StackFrame *bp) {
  static bool noreentry;
  if (!bp) bp = __builtin_frame_address(0);
  if (!noreentry) {
    noreentry = true;
    PrintBacktrace(fd, bp);
    noreentry = 0;
  }
}
