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
#include "libc/intrin/strace.h"
#include "libc/sysv/pib.h"

/**
 * Sets process-wide file mode creation mask.
 *
 * This value is used by functions like openat() and mkdirat() that make
 * new files. For example, if you say `mkdir("foo", 0777)` then it might
 * not actually create a file with rwxrwxrwx permission because the bits
 * were manipulated by the process-wide umask value. UNIX systems set it
 * to 022 usually, which means 0777 becomes 0755 (due to `0777 & ~0222`)
 *
 * @param newmask is new mask bits (only bits 0777 are considered)
 * @return previous mask
 * @note always succeeds
 */
unsigned umask(unsigned newmask) {
  int oldmask;
  if (!IsWindows()) {
    oldmask = sys_umask(newmask);
  } else {
    struct CosmoPib *pib = __get_pib();
    oldmask = pib->umask;
    pib->umask = newmask & 0777;
  }
  STRACE("umask(%#o) → %#o", newmask, oldmask);
  return oldmask;
}
