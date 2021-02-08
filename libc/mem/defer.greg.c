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
#include "libc/assert.h"
#include "libc/bits/likely.h"
#include "libc/calls/calls.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/gc.internal.h"
#include "libc/runtime/gc.h"
#include "libc/runtime/runtime.h"

forceinline bool PointerNotOwnedByParentStackFrame(struct StackFrame *frame,
                                                   struct StackFrame *parent,
                                                   void *ptr) {
  return !(((intptr_t)ptr > (intptr_t)frame) &&
           ((intptr_t)ptr < (intptr_t)parent));
}

/**
 * Adds destructor to garbage shadow stack.
 *
 * @param frame is passed automatically by wrapper macro
 * @param fn takes one argument
 * @param arg is passed to fn(arg)
 * @return arg
 */
void __defer(struct StackFrame *frame, void *fn, void *arg) {
  size_t n2;
  struct Garbage *p2;
  struct StackFrame *f2;
  if (!arg) return;
  f2 = __builtin_frame_address(0);
  assert(__garbage.n);
  assert(f2->next == frame);
  assert(PointerNotOwnedByParentStackFrame(f2, frame, arg));
  if (UNLIKELY(__garbage.i == __garbage.n)) {
    n2 = __garbage.n + (__garbage.n >> 1);
    p2 = malloc(n2 * sizeof(*__garbage.p));
    memcpy(p2, __garbage.p, __garbage.n * sizeof(*__garbage.p));
    if (__garbage.p != __garbage.initmem) free(__garbage.p);
    __garbage.p = p2;
    __garbage.n = n2;
  }
  __garbage.p[__garbage.i].frame = frame->next;
  __garbage.p[__garbage.i].fn = (intptr_t)fn;
  __garbage.p[__garbage.i].arg = (intptr_t)arg;
  __garbage.p[__garbage.i].ret = frame->addr;
  __garbage.i++;
  frame->addr = (intptr_t)__gc;
}
