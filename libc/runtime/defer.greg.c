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
#include "libc/alg/arraylist.h"
#include "libc/assert.h"
#include "libc/bits/bits.h"
#include "libc/calls/calls.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/gc.h"
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
