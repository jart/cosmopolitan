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
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/gc.internal.h"
#include "libc/runtime/gc.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"

forceinline bool PointerNotOwnedByParentStackFrame(struct StackFrame *frame,
                                                   struct StackFrame *parent,
                                                   void *ptr) {
  return !(((intptr_t)ptr > (intptr_t)frame) &&
           ((intptr_t)ptr < (intptr_t)parent));
}

static void __garbage_destroy(void) {
  if (weaken(free)) {
    weaken(free)(__garbage.p);
  }
  bzero(&__garbage, sizeof(__garbage));
}

void __deferer(struct StackFrame *frame, void *fn, void *arg) {
  size_t n2;
  struct Garbage *p2;
  if (UNLIKELY(__garbage.i == __garbage.n)) {
    p2 = __garbage.p;
    n2 = __garbage.n + (__garbage.n >> 1);
    if (__garbage.p != __garbage.initmem) {
      if (!weaken(realloc)) return;
      if (!(p2 = weaken(realloc)(p2, n2 * sizeof(*p2)))) return;
    } else {
      if (!weaken(malloc)) return;
      if (!(p2 = weaken(malloc)(n2 * sizeof(*p2)))) return;
      memcpy(p2, __garbage.p, __garbage.n * sizeof(*p2));
      atexit(__garbage_destroy);
    }
    __garbage.p = p2;
    __garbage.n = n2;
  }
  __garbage.p[__garbage.i].frame = frame;
  __garbage.p[__garbage.i].fn = (intptr_t)fn;
  __garbage.p[__garbage.i].arg = (intptr_t)arg;
  __garbage.p[__garbage.i].ret = frame->addr;
  __garbage.i++;
  frame->addr = (intptr_t)__gc;
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
  struct StackFrame *f;
  if (!arg) return;
  f = __builtin_frame_address(0);
  assert(__garbage.n);
  assert(f->next == frame);
  assert(PointerNotOwnedByParentStackFrame(f, frame, arg));
  __deferer(frame, fn, arg);
}
