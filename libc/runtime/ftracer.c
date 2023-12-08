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
#include "libc/macros.internal.h"
#include "libc/nexgen32e/stackframe.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#include "libc/thread/tls2.internal.h"

/**
 * @fileoverview Plain-text function call logging.
 *
 * Able to log ~2 million function calls per second, which is mostly
 * bottlenecked by system call overhead. Log size is reasonable if piped
 * into gzip.
 */

#define MAX_NESTING 512

#ifdef __x86_64__
#define DETOUR_SKEW 2
#elif defined(__aarch64__)
#define DETOUR_SKEW 8
#endif

static struct CosmoFtrace g_ftrace;

__funline int GetNestingLevelImpl(struct StackFrame *frame) {
  int nesting = -2;
  while (frame) {
    ++nesting;
    frame = frame->next;
  }
  return MAX(0, nesting);
}

__funline int GetNestingLevel(struct CosmoFtrace *ft, struct StackFrame *sf) {
  int nesting;
  nesting = GetNestingLevelImpl(sf);
  if (nesting < ft->ft_skew) ft->ft_skew = nesting;
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
  sf = __builtin_frame_address(0);
  st = (uintptr_t)__argv - sizeof(uintptr_t);
  if (__ftrace <= 0) return;
  if (__tls_enabled) {
    tib = __get_tls_privileged();
    if (tib->tib_ftrace <= 0) return;
    ft = &tib->tib_ftracer;
    if ((char *)sf >= tib->tib_sigstack_addr &&
        (char *)sf <= tib->tib_sigstack_addr + tib->tib_sigstack_size) {
      st = (uintptr_t)tib->tib_sigstack_addr + tib->tib_sigstack_size;
    } else if ((pt = (struct PosixThread *)tib->tib_pthread) &&
               pt->pt_attr.__stacksize) {
      st = (uintptr_t)pt->pt_attr.__stackaddr + pt->pt_attr.__stacksize;
    }
  } else {
    ft = &g_ftrace;
  }
  stackuse = st - (intptr_t)sf;
  if (_cmpxchg(&ft->ft_once, false, true)) {
    ft->ft_lastaddr = -1;
    ft->ft_skew = GetNestingLevelImpl(sf);
  }
  if (_cmpxchg(&ft->ft_noreentry, false, true)) {
    sf = sf->next;
    fn = sf->addr + DETOUR_SKEW;
    if (fn != ft->ft_lastaddr) {
      kprintf("%rFUN %6P %6H %'18T %'*ld %*s%t\n", ftrace_stackdigs, stackuse,
              GetNestingLevel(ft, sf) * 2, "", fn);
      ft->ft_lastaddr = fn;
    }
    ft->ft_noreentry = false;
  }
}
