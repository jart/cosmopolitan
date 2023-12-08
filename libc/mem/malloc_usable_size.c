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

size_t (*hook_malloc_usable_size)(void *) = dlmalloc_usable_size;

/**
 * Returns the number of bytes you can actually use in
 * an allocated chunk, which may be more than you requested
 * (although often not) due to alignment and minimum size
 * constraints.
 *
 * You can use this many bytes without worrying about overwriting
 * other allocated objects. This is not a particularly great
 * programming practice. malloc_usable_size can be more useful in
 * debugging and assertions, for example:
 *
 *     p = malloc(n)
 *     assert(malloc_usable_size(p) >= 256)
 *
 * @param p is address of allocation
 * @return total number of bytes
 * @see dlmalloc_usable_size()
 */
size_t malloc_usable_size(void *p) {
  return hook_malloc_usable_size(p);
}
