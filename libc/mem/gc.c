/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/nexgen32e/stackframe.h"
#include "libc/runtime/gc.h"
#include "libc/runtime/runtime.h"

/**
 * Frees memory when function returns.
 *
 * This garbage collector overwrites the return address on the stack so
 * that the RET instruction calls a trampoline which calls free(). It's
 * loosely analogous to Go's defer keyword rather than a true cycle gc.
 *
 *     const char *s = _gc(strdup("hello"));
 *     puts(s);
 *
 * This macro is equivalent to:
 *
 *      _defer(free, ptr)
 *
 * @warning do not return a gc()'d pointer
 * @warning do not realloc() with gc()'d pointer
 * @warning be careful about static keyword due to impact of inlining
 * @note you should use -fno-omit-frame-pointer
 */
void *(_gc)(void *thing) {
  struct StackFrame *frame;
  frame = __builtin_frame_address(0);
  __deferer(frame->next, _weakfree, thing);
  return thing;
}

/**
 * Calls fn(arg) when function returns.
 *
 * This garbage collector overwrites the return address on the stack so
 * that the RET instruction calls a trampoline which calls free(). It's
 * loosely analogous to Go's defer keyword rather than a true cycle gc.
 *
 * @warning do not return a gc()'d pointer
 * @warning do not realloc() with gc()'d pointer
 * @warning be careful about static keyword due to impact of inlining
 * @note you should use -fno-omit-frame-pointer
 */
void *(_defer)(void *fn, void *arg) {
  struct StackFrame *frame;
  frame = __builtin_frame_address(0);
  __deferer(frame->next, fn, arg);
  return arg;
}
