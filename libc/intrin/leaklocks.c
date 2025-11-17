/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2025 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/kprintf.h"
#include "libc/macros.h"
#include "libc/thread/tls.h"

static bool IsHoldingLocks(struct CosmoTib *tib) {
  for (int i = 0; i < ARRAYLEN(tib->tib_locks); ++i)
    if (tib->tib_locks[i])
      return true;
  return false;
}

/**
 * Aborts if any locks are held by calling thread.
 */
void AssertNoLocksAreHeld(void) {
  struct CosmoTib *tib = __get_tls();
  if (IsHoldingLocks(tib)) {
    kprintf("error: the following locks are held by this thread:\n");
    for (int i = 0; i < ARRAYLEN(tib->tib_locks); ++i)
      if (tib->tib_locks[i])
        kprintf("\t- %t\n", tib->tib_locks[i]);
    _Exit(74);
  }
}
