/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/mem/hook.internal.h"
#include "libc/mem/mem.h"
#include "third_party/dlmalloc/dlmalloc.h"

void *(*hook_realloc)(void *, size_t) = dlrealloc;

/**
 * Allocates / resizes / frees memory, e.g.
 *
 * Returns a pointer to a chunk of size n that contains the same data as
 * does chunk p up to the minimum of (n, p's size) bytes, or null if no
 * space is available.
 *
 * If p is NULL, then realloc() is equivalent to malloc().
 *
 * If p is not NULL and n is 0, then realloc() shrinks the allocation to
 * zero bytes. The allocation isn't freed and still continues to be a
 * uniquely allocated piece of memory. However it should be assumed that
 * zero bytes can be accessed, since that's enforced by `MODE=asan`.
 *
 * The returned pointer may or may not be the same as p. The algorithm
 * prefers extending p in most cases when possible, otherwise it employs
 * the equivalent of a malloc-copy-free sequence.
 *
 * Please note that p is NOT free()'d should realloc() fail, thus:
 *
 *     if ((p2 = realloc(p, n2))) {
 *       p = p2;
 *       ...
 *     } else {
 *       ...
 *     }
 *
 * if n is for fewer bytes than already held by p, the newly unused
 * space is lopped off and freed if possible.
 *
 * The old unix realloc convention of allowing the last-free'd chunk to
 * be used as an argument to realloc is not supported.
 *
 * @param p is address of current allocation or NULL
 * @param n is number of bytes needed
 * @return rax is result, or NULL w/ errno w/o free(p)
 * @see dlrealloc()
 */
void *realloc(void *p, size_t n) {
  return hook_realloc(p, n);
}
