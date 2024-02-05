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
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/asancodes.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"

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
  if ((p = mmap(0, GetStackSize(), PROT_READ | PROT_WRITE,
                MAP_ANONYMOUS |
                    (IsAarch64() && IsLinux() && IsQemuUser() ? MAP_PRIVATE
                                                              : MAP_STACK),
                -1, 0)) != MAP_FAILED) {
    if (IsAsan()) {
      __asan_poison(p + GetStackSize() - 16, 16, kAsanStackOverflow);
      __asan_poison(p, getauxval(AT_PAGESZ), kAsanStackOverflow);
    }
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
