/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/cosmo.h"
#include "libc/errno.h"
#include "libc/runtime/stack.h"

/**
 * Allocates stack.
 *
 * The size of your returned stack is always GetStackSize().
 *
 * The bottom 4096 bytes of your stack can't be used, since it's always
 * reserved for a read-only guard page. With ASAN it'll be poisoned too.
 *
 * The top 16 bytes of a stack can't be used due to openbsd:stackbound
 * and those bytes are also poisoned under ASAN build modes.
 *
 * @return stack bottom address on success, or null w/ errno
 */
void *NewCosmoStack(void) {
  void *stackaddr;
  size_t stacksize = GetStackSize();
  size_t guardsize = GetGuardSize();
  errno_t err = cosmo_stack_alloc(&stacksize, &guardsize, &stackaddr);
  if (!err)
    return stackaddr;
  errno = err;
  return 0;
}

/**
 * Frees stack.
 *
 * @param stackaddr was allocated by NewCosmoStack()
 * @return 0 on success, or -1 w/ errno
 */
int FreeCosmoStack(void *stackaddr) {
  cosmo_stack_free(stackaddr, GetStackSize(), GetGuardSize());
  return 0;
}
