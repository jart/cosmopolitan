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
#include "libc/intrin/pshufd.h"
#include "libc/intrin/punpcklbw.h"
#include "libc/intrin/punpcklwd.h"
#include "libc/nexgen32e/hascharacter.internal.h"
#include "libc/str/str.h"

#define V(p) (void *)(p)

/**
 * Returns prefix length, consisting of chars not in reject.
 * a.k.a. Return index of first byte that's in charset.
 *
 * @param reject is nul-terminated character set
 * @see strspn(), strtok_r()
 * @asyncsignalsafe
 */
size_t strcspn(const char *s, const char *reject) {
  size_t i, n;
  unsigned m;
  char cv[16], sv[16];
  if ((n = strlen(reject)) < 16) {
    bzero(sv, 16);
    memcpy(sv, reject, n);
    for (i = 0;; ++i) {
      cv[0] = s[i];
      punpcklbw(V(cv), V(cv), V(cv));
      punpcklwd(V(cv), V(cv), V(cv));
      pshufd(V(cv), V(cv), 0);
      pcmpeqb(V(cv), V(cv), V(sv));
      if ((m = pmovmskb(V(cv)))) {
        break;
      }
    }
    return i;
  }
  for (i = 0; s[i]; ++i) {
    if (HasCharacter(s[i], reject)) {
      break;
    }
  }
  return i;
}
