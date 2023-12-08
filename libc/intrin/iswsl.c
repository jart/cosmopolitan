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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/likely.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"

#ifdef __x86_64__

#define GROWSDOWN 0x00000100
#define ANONYMOUS 0x00000020

/**
 * Returns true if host platform is WSL 1.0.
 */
bool __iswsl1(void) {
  static char res;
  if (res) return res & 1;
  if (!IsLinux()) return res = 2, false;
  int e = errno;
  unassert(__sys_mmap((void *)1, 4096, PROT_READ | PROT_WRITE,
                      MAP_FIXED | MAP_PRIVATE | ANONYMOUS | GROWSDOWN, -1, 0,
                      0) == MAP_FAILED);
  bool tmp = errno == ENOTSUP;
  errno = e;
  res = 2 | tmp;
  return tmp;
}

#endif /* __x86_64__ */
