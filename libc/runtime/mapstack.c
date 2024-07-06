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
#include "libc/calls/calls.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"

#define MAP_ANON_OPENBSD  0x1000
#define MAP_STACK_OPENBSD 0x4000

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
  char *p;
  size_t n = GetStackSize();
  if ((p = mmap(0, n, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1,
                0)) != MAP_FAILED) {
    if (IsOpenbsd() && __sys_mmap(p, n, PROT_READ | PROT_WRITE,
                                  MAP_PRIVATE | MAP_FIXED | MAP_ANON_OPENBSD |
                                      MAP_STACK_OPENBSD,
                                  -1, 0, 0) != p)
      notpossible;
    if (mprotect(p, GetGuardSize(), PROT_NONE | PROT_GUARD))
      notpossible;
    return p;
  } else {
    return 0;
  }
}

/**
 * Frees stack.
 *
 * @param stk was allocated by NewCosmoStack()
 */
int FreeCosmoStack(void *stk) {
  return munmap(stk, GetStackSize());
}
