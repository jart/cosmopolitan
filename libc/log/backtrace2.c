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
#include "libc/alg/alg.h"
#include "libc/alg/bisectcarleft.h"
#include "libc/bits/bits.h"
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/calls/hefty/spawn.h"
#include "libc/conv/conv.h"
#include "libc/dce.h"
#include "libc/fmt/fmt.h"
#include "libc/nexgen32e/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"

#define kBacktraceMaxFrames 128
#define kBacktraceBufSize   ((kBacktraceMaxFrames - 1) * (16 + 1))

static char *formataddress(FILE *f, const struct SymbolTable *st, intptr_t addr,
                           char *out, unsigned size, bool symbolic) {
  int64_t addend;
  const char *name;
  const struct Symbol *symbol;
  if (st->count && ((intptr_t)addr >= (intptr_t)&_base &&
                    (intptr_t)addr <= (intptr_t)&_end && symbolic)) {
    symbol = &st->symbols[bisectcarleft((const int32_t(*)[2])st->symbols,
                                        st->count, addr - st->addr_base - 1)];
    addend = addr - st->addr_base - symbol->addr_rva;
    name = &st->name_base[symbol->name_rva];
    snprintf(out, size, "%s%c%#x", name, addend >= 0 ? '+' : '-', abs(addend));
  } else {
    snprintf(out, size, "%p", addr);
  }
  return out;
}

static int printbacktraceusingsymbols(FILE *f, const struct StackFrame *bp,
                                      char buf[hasatleast kBacktraceBufSize]) {
  size_t gi;
  intptr_t addr;
  struct Garbages *garbage;
  struct SymbolTable *symbols;
  const struct StackFrame *frame;
  if ((symbols = getsymboltable())) {
    garbage = weaken(__garbage);
    gi = garbage ? garbage->i : 0;
    for (frame = bp; frame; frame = frame->next) {
      addr = frame->addr;
      if (addr == weakaddr("__gc")) {
        do {
          --gi;
        } while ((addr = garbage->p[gi].ret) == weakaddr("__gc"));
      }
      fprintf(f, "%p %p %s\n", frame, addr,
              formataddress(f, symbols, addr, buf, kBacktraceBufSize, true));
    }
    return 0;
  } else {
    return -1;
  }
}

static int printbacktraceusingaddr2line(
    FILE *f, const struct StackFrame *bp,
    char buf[hasatleast kBacktraceBufSize],
    char *argv[hasatleast kBacktraceMaxFrames]) {
  ssize_t got;
  intptr_t addr;
  size_t i, j, gi;
  int rc, pid, tubes[3];
  struct Garbages *garbage;
  const struct StackFrame *frame;
  const char *addr2line, *debugbin, *p1, *p2, *p3;
  if (!(debugbin = finddebugbinary()) ||
      !(addr2line = emptytonull(firstnonnull(
            getenv("ADDR2LINE"), firstnonnull(commandv("addr2line"), ""))))) {
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
  if ((pid = spawnve(0, tubes, addr2line, argv, environ)) == -1) return -1;
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
        fwrite(p1, 1, p2 - p1, f);
        got -= p3 - p1;
        p1 += p3 - p1;
      } else {
        fwrite(p1, 1, got, f);
        break;
      }
    }
  }
  close(tubes[1]);
  if (waitpid(pid, &rc, 0) == -1) return -1;
  if (WEXITSTATUS(rc) != 0) return -1;
  return 0;
}

static noinline int printbacktrace(FILE *f, const struct StackFrame *bp,
                                   char *argv[hasatleast kBacktraceMaxFrames],
                                   char buf[hasatleast kBacktraceBufSize]) {
  if (!IsTiny()) {
    if (printbacktraceusingaddr2line(f, bp, buf, argv) != -1) {
      return 0;
    }
  }
  return printbacktraceusingsymbols(f, bp, buf);
}

void showbacktrace(FILE *f, const struct StackFrame *bp) {
  static bool noreentry;
  char *argv[kBacktraceMaxFrames];
  char buf[kBacktraceBufSize];
  if (!noreentry) {
    noreentry = true;
    printbacktrace(f, bp, argv, buf);
    noreentry = 0;
  }
}
