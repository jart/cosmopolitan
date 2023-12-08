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

void *(*hook_malloc)(size_t) = dlmalloc;

/**
 * Allocates uninitialized memory.
 *
 * Returns a pointer to a newly allocated chunk of at least n bytes, or
 * null if no space is available, in which case errno is set to ENOMEM
 * on ANSI C systems.
 *
 * If n is zero, malloc returns a minimum-sized chunk. (The minimum size
 * is 32 bytes on 64bit systems.) It should be assumed that zero bytes
 * are possible access, since that'll be enforced by `MODE=asan`.
 *
 * Note that size_t is an unsigned type, so calls with arguments that
 * would be negative if signed are interpreted as requests for huge
 * amounts of space, which will often fail. The maximum supported value
 * of n differs across systems, but is in all cases less than the
 * maximum representable value of a size_t.
 *
 * @param rdi is number of bytes needed, coerced to 1+
 * @return new memory, or NULL w/ errno
 */
void *malloc(size_t n) {
  return hook_malloc(n);
}
