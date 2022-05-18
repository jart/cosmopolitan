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
#include "libc/bits/safemacros.internal.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/cmpxchg.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/spinlock.h"
#include "libc/log/libfatal.internal.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/nexgen32e/rdtscp.h"
#include "libc/nexgen32e/stackframe.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/time/clockstonanos.internal.h"

#define MAX_NESTING 512

/**
 * @fileoverview Plain-text function call logging.
 *
 * Able to log ~2 million function calls per second, which is mostly
 * bottlenecked by system call overhead. Log size is reasonable if piped
 * into gzip.
 */

void ftrace_hook(void);

_Alignas(64) char ftrace_lock;

static struct Ftrace {
  int skew;
  int stackdigs;
  int64_t lastaddr;
  uint64_t laststamp;
} g_ftrace;

static privileged int GetNestingLevelImpl(struct StackFrame *frame) {
  int nesting = -2;
  while (frame) {
    ++nesting;
    frame = frame->next;
  }
  return MAX(0, nesting);
}

static privileged int GetNestingLevel(struct StackFrame *frame) {
  int nesting;
  nesting = GetNestingLevelImpl(frame);
  if (nesting < g_ftrace.skew) g_ftrace.skew = nesting;
  nesting -= g_ftrace.skew;
  return MIN(MAX_NESTING, nesting);
}

/**
 * Prints name of function being called.
 *
 * We insert CALL instructions that point to this function, in the
 * prologues of other functions. We assume those functions behave
 * according to the System Five NexGen32e ABI.
 */
privileged void ftracer(void) {
  uint64_t stamp;
  size_t stackuse;
  struct StackFrame *frame;
  _spinlock_cooperative(&ftrace_lock);
  stamp = rdtsc();
  frame = __builtin_frame_address(0);
  frame = frame->next;
  if (frame->addr != g_ftrace.lastaddr) {
    stackuse = ROUNDUP((intptr_t)frame, GetStackSize()) - (intptr_t)frame;
    kprintf("%rFUN %5P %'13T %'*lu %*s%t\r\n", g_ftrace.stackdigs, stackuse,
            GetNestingLevel(frame) * 2, "", frame->addr);
    g_ftrace.laststamp = X86_HAVE(RDTSCP) ? rdtscp(0) : rdtsc();
    g_ftrace.lastaddr = frame->addr;
  }
  _spunlock(&ftrace_lock);
}

textstartup int ftrace_install(void) {
  if (GetSymbolTable()) {
    g_ftrace.lastaddr = -1;
    g_ftrace.laststamp = kStartTsc;
    g_ftrace.stackdigs = LengthInt64Thousands(GetStackSize());
    g_ftrace.skew = GetNestingLevelImpl(__builtin_frame_address(0));
    return __hook(ftrace_hook, GetSymbolTable());
  } else {
    kprintf("error: --ftrace failed to open symbol table\r\n");
    return -1;
  }
}
