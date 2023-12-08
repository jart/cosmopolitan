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
#include "libc/errno.h"
#include "libc/intrin/getauxval.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/auxv.h"

/**
 * Returns auxiliary value.
 *
 * @return auxiliary value or 0 if `key` not found
 * @see libc/sysv/consts.sh
 * @see System Five Application Binary Interface § 3.4.3
 * @error ENOENT when value not found
 * @asyncsignalsafe
 */
unsigned long getauxval(unsigned long key) {
  struct AuxiliaryValue x;
  x = __getauxval(key);
  if (key == AT_PAGESZ) {
    if (!x.isfound) {
#ifdef __aarch64__
      x.value = 16384;
#else
      x.value = 4096;
#endif
    }
    x.isfound = true;
  }
  if (x.isfound) {
    return x.value;
  } else {
    errno = ENOENT;
    return 0;
  }
}
