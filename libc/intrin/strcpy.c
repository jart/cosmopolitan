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
#include "libc/dce.h"
#include "libc/str/str.h"
#ifndef __aarch64__

typedef char xmm_u __attribute__((__vector_size__(16), __aligned__(1)));
typedef char xmm_t __attribute__((__vector_size__(16), __aligned__(16)));

/**
 * Copies bytes from 𝑠 to 𝑑 until a NUL is encountered.
 *
 * @param 𝑑 is destination memory
 * @param 𝑠 is a NUL-terminated string
 * @note 𝑑 and 𝑠 can't overlap
 * @return original dest
 * @asyncsignalsafe
 */
__vex char *strcpy(char *d, const char *s) {
  size_t i = 0;
#if defined(__x86_64__) && !defined(__chibicc__)
  for (; (uintptr_t)(s + i) & 15; ++i) {
    if (!(d[i] = s[i])) {
      return d;
    }
  }
  for (;;) {
    xmm_t z = {0};
    xmm_t v = *(xmm_t *)(s + i);
    if (!__builtin_ia32_pmovmskb128(v == z)) {
      *(xmm_u *)(d + i) = v;
      i += 16;
    } else {
      break;
    }
  }
#endif
  for (;;) {
    if (!(d[i] = s[i])) {
      return d;
    }
    ++i;
  }
}

#endif /* __aarch64__ */
