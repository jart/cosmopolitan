/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/macros.h"
#include "libc/nexgen32e/stackframe.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

/**
 * @fileoverview plain-text function call logging
 */

#define MAX_NESTING 512

#ifdef __x86_64__
#define DETOUR_SKEW 2
#elif defined(__aarch64__)
#define DETOUR_SKEW 8
#endif

static struct CosmoFtrace g_ftrace;

__funline int GetNestingLevelImpl(struct StackFrame *frame) {
  int nesting = -1;
  while (frame && !kisdangerous(frame)) {
    ++nesting;
    frame = frame->next;
  }
  return MAX(0, nesting);
}

__funline int GetNestingLevel(struct CosmoFtrace *ft, struct StackFrame *sf) {
  int nesting;
  nesting = GetNestingLevelImpl(sf);
  if (nesting < ft->ft_skew)
    ft->ft_skew = nesting;
  nesting -= ft->ft_skew;
  return MIN(MAX_NESTING, nesting);
}

/**
 * Prints name of function being called.
 *
 * Whenever a function is called, ftrace_hook() will be called from the
 * function prologue which saves the parameter registers and calls this
 * function, which is responsible for logging the function call.
 *
 * @see ftrace_install()
 */
privileged void ftracer(void) {
  long stackuse;
  uintptr_t fn, st;
  struct CosmoTib *tib;
  struct StackFrame *sf;
  struct CosmoFtrace *ft;
  struct PosixThread *pt;

  // get interesting values
  sf = __builtin_frame_address(0);
  st = (uintptr_t)__argv - sizeof(uintptr_t);
  if (__ftrace <= 0)
    return;

  // determine top of stack
  // main thread won't consider kernel provided argblock
  if (__tls_enabled) {
    tib = __get_tls_privileged();
    if (tib->tib_ftrace <= 0)
      return;
    ft = &tib->tib_ftracer;
    pt = (struct PosixThread *)tib->tib_pthread;
    if (pt != &_pthread_static) {
      if ((char *)sf >= tib->tib_sigstack_addr &&
          (char *)sf <= tib->tib_sigstack_addr + tib->tib_sigstack_size) {
        st = (uintptr_t)tib->tib_sigstack_addr + tib->tib_sigstack_size;
      } else if (pt && pt->pt_attr.__stacksize) {
        st = (uintptr_t)pt->pt_attr.__stackaddr + pt->pt_attr.__stacksize;
      }
    }
  } else {
    ft = &g_ftrace;
  }

  // estimate stack pointer of hooked function
  uintptr_t usp = (uintptr_t)sf;
  usp += sizeof(struct StackFrame);  // overhead of this function
#if defined(__x86_64__)
  usp += 8;       // ftrace_hook() stack aligning
  usp += 8 * 8;   // ftrace_hook() pushed 8x regs
  usp += 8 * 16;  // ftrace_hook() pushed 8x xmms
#elif defined(__aarch64__)
  usp += 384;  // overhead of ftrace_hook()
#else
#error "unsupported architecture"
#endif

  // determine how much stack hooked function is using
  stackuse = st - usp;

  // log function call
  //
  //     FUN $PID $TID $STARTNANOS $STACKUSE $SYMBOL
  //
  if (!ft->ft_once) {
    ft->ft_lastaddr = -1;
    ft->ft_skew = GetNestingLevelImpl(sf);
    ft->ft_once = true;
  }
  sf = sf->next;
  fn = sf->addr + DETOUR_SKEW;
  if (fn != ft->ft_lastaddr) {
    kprintf("%rFUN %6P %6H %'18T %'*ld %*s%t\n", ftrace_stackdigs, stackuse,
            GetNestingLevel(ft, sf) * 2, "", fn);
    ft->ft_lastaddr = fn;
  }
}
