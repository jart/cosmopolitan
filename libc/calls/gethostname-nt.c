/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/macros.internal.h"
#include "libc/nt/enum/computernameformat.h"
#include "libc/nt/systeminfo.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

// Guarantees NUL-terminator, if zero is returned.
// Mutates on ENAMETOOLONG without nul-terminator.
textwindows int gethostname_nt(char *name, size_t len, int kind) {
  uint32_t nSize;
  char name8[256];
  char16_t name16[256];
  nSize = ARRAYLEN(name16);
  if (GetComputerNameEx(kind, name16, &nSize)) {
    tprecode16to8(name8, sizeof(name8), name16);
    if (memccpy(name, name8, '\0', len)) {
      return 0;
    } else {
      return enametoolong();
    }
    return 0;
  } else {
    return __winerr();
  }
}
