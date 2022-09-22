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
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/cmpxchg.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/nopl.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/gettls.h"
#include "libc/nexgen32e/stackframe.h"
#include "libc/nexgen32e/threaded.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/stack.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/thread/thread.h"

#define MAX_NESTING 512

/**
 * @fileoverview Plain-text function call logging.
 *
 * Able to log ~2 million function calls per second, which is mostly
 * bottlenecked by system call overhead. Log size is reasonable if piped
 * into gzip.
 */

void ftrace_hook(void);

static int g_stackdigs;
static struct Ftrace g_ftrace;

static privileged inline int GetNestingLevelImpl(struct StackFrame *frame) {
  int nesting = -2;
  while (frame) {
    ++nesting;
    frame = frame->next;
  }
  return MAX(0, nesting);
}

static privileged inline int GetNestingLevel(struct Ftrace *ft,
                                             struct StackFrame *sf) {
  int nesting;
  nesting = GetNestingLevelImpl(sf);
  if (nesting < ft->skew) ft->skew = nesting;
  nesting -= ft->skew;
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
  long stackuse;
  struct Ftrace *ft;
  struct StackFrame *sf;
  if (__tls_enabled) {
    ft = (struct Ftrace *)(__get_tls_privileged() + 0x08);
  } else {
    ft = &g_ftrace;
  }
  if (_cmpxchg(&ft->once, false, true)) {
    ft->lastaddr = -1;
    ft->skew = GetNestingLevelImpl(__builtin_frame_address(0));
  }
  if (_cmpxchg(&ft->noreentry, false, true)) {
    sf = __builtin_frame_address(0);
    sf = sf->next;
    if (sf->addr != ft->lastaddr) {
      stackuse = GetStackAddr() + GetStackSize() - (intptr_t)sf;
      kprintf("%rFUN %6P %'13T %'*ld %*s%t\n", g_stackdigs, stackuse,
              GetNestingLevel(ft, sf) * 2, "", sf->addr);
      ft->lastaddr = sf->addr;
    }
    ft->noreentry = false;
  }
}

textstartup int ftrace_install(void) {
  if (GetSymbolTable()) {
    g_stackdigs = LengthInt64Thousands(GetStackSize());
    return __hook(ftrace_hook, GetSymbolTable());
  } else {
    kprintf("error: --ftrace failed to open symbol table\n");
    return -1;
  }
}
