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
#include "libc/fmt/itoa.h"
#include "libc/intrin/cmpxchg.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/lockcmpxchgp.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/stackframe.h"
#include "libc/nexgen32e/threaded.h"
#include "libc/runtime/stack.h"
#include "libc/runtime/symbols.internal.h"

#define MAX_NESTING 512

/**
 * @fileoverview Plain-text function call logging.
 *
 * Able to log ~2 million function calls per second, which is mostly
 * bottlenecked by system call overhead. Log size is reasonable if piped
 * into gzip.
 */

void ftrace_hook(void);

_Alignas(64) int ftrace_lock;

static struct Ftrace {
  int skew;
  int stackdigs;
  int64_t lastaddr;
} g_ftrace;

static privileged inline int GetNestingLevelImpl(struct StackFrame *frame) {
  int nesting = -2;
  while (frame) {
    ++nesting;
    frame = frame->next;
  }
  return MAX(0, nesting);
}

static privileged inline int GetNestingLevel(struct StackFrame *frame) {
  int nesting;
  nesting = GetNestingLevelImpl(frame);
  if (nesting < g_ftrace.skew) g_ftrace.skew = nesting;
  nesting -= g_ftrace.skew;
  return MIN(MAX_NESTING, nesting);
}

static privileged inline void ReleaseFtraceLock(void) {
  int zero = 0;
  __atomic_store(&ftrace_lock, &zero, __ATOMIC_RELAXED);
}

static privileged inline bool AcquireFtraceLock(void) {
  int me, owner;
  unsigned tries;
  if (!__threaded) {
    return _cmpxchg(&ftrace_lock, 0, -1);
  } else {
    for (tries = 0, me = gettid();;) {
      owner = 0;
      if (_lockcmpxchgp(&ftrace_lock, &owner, me)) {
        return true;
      }
      if (owner == -1) {
        // avoid things getting weird after first clone() call transition
        return false;
      }
      if (owner == me) {
        // we ignore re-entry into ftrace. while the code and build config
        // is written to make re-entry highly unlikely, it's impossible to
        // guarantee. there's also the possibility of asynchronous signals
        return false;
      }
      if (++tries & 7) {
        __builtin_ia32_pause();
      } else {
        sched_yield();
      }
    }
  }
}

/**
 * Prints name of function being called.
 *
 * We insert CALL instructions that point to this function, in the
 * prologues of other functions. We assume those functions behave
 * according to the System Five NexGen32e ABI.
 */
privileged void ftracer(void) {
  long stackuse;
  struct StackFrame *frame;
  if (AcquireFtraceLock()) {
    frame = __builtin_frame_address(0);
    frame = frame->next;
    if (frame->addr != g_ftrace.lastaddr) {
      stackuse = (intptr_t)GetStackAddr(0) + GetStackSize() - (intptr_t)frame;
      kprintf("%rFUN %5P %'13T %'*ld %*s%t\n", g_ftrace.stackdigs, stackuse,
              GetNestingLevel(frame) * 2, "", frame->addr);
      g_ftrace.lastaddr = frame->addr;
    }
    ReleaseFtraceLock();
  }
}

textstartup int ftrace_install(void) {
  if (GetSymbolTable()) {
    g_ftrace.lastaddr = -1;
    g_ftrace.stackdigs = LengthInt64Thousands(GetStackSize());
    g_ftrace.skew = GetNestingLevelImpl(__builtin_frame_address(0));
    return __hook(ftrace_hook, GetSymbolTable());
  } else {
    kprintf("error: --ftrace failed to open symbol table\n");
    return -1;
  }
}
