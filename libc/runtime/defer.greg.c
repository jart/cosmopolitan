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
#include "libc/alg/arraylist.internal.h"
#include "libc/assert.h"
#include "libc/bits/bits.h"
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
  struct StackFrame *frame2;
  /*
   * To avoid an API requiring dlmalloc dependency, say:
   *   defer(free_s, &ptr_not_owned_by_current_frame)
   * Rather than:
   *   defer(weak(free), ptr)
   */
  if (!arg) return;
  frame2 = __builtin_frame_address(0);
  assert(frame2->next == frame);
  assert(PointerNotOwnedByParentStackFrame(frame2, frame, arg));
  if (append(&__garbage,
             (&(const struct Garbage){frame->next, (intptr_t)fn, (intptr_t)arg,
                                      frame->addr})) != -1) {
    atomic_store(&frame->addr, (intptr_t)&__gc);
  } else {
    abort();
  }
}
