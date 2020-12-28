/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/intrin/pcmpeqb.h"
#include "libc/intrin/pmovmskb.h"
#include "libc/str/str.h"

/**
 * Copies bytes from 𝑠 to 𝑑 until a NUL is encountered.
 *
 * @param 𝑑 is destination memory
 * @param 𝑠 is a NUL-terminated string
 * @note 𝑑 and 𝑠 can't overlap
 * @return pointer to nul byte
 * @see strcpy(), memccpy()
 * @asyncsignalsafe
 */
char *stpcpy(char *d, const char *s) {
  size_t i;
  uint8_t v1[16], v2[16], vz[16];
  i = 0;
  while (((uintptr_t)(s + i) & 15)) {
    if (!(d[i] = s[i])) {
      return d + i;
    }
    ++i;
  }
  for (;;) {
    memset(vz, 0, 16);
    memcpy(v1, s + i, 16);
    pcmpeqb(v2, v1, vz);
    if (!pmovmskb(v2)) {
      memcpy(d + i, v1, 16);
      i += 16;
    } else {
      break;
    }
  }
  for (;;) {
    if (!(d[i] = s[i])) {
      return d + i;
    }
    ++i;
  }
}
