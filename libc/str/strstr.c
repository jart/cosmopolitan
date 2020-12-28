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
#include "libc/alg/alg.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/str/internal.h"
#include "libc/str/str.h"

/**
 * Searches for substring.
 *
 * @param haystack is the search area, as a NUL-terminated string
 * @param needle is the desired substring, also NUL-terminated
 * @return pointer to first substring within haystack, or NULL
 * @asyncsignalsafe
 * @see memmem()
 */
char *strstr(const char *haystack, const char *needle) {
  size_t i;
  if (needle[0]) {
    if (needle[1]) {
      for (;;) {
#if 0 /* todo: fix me */
        if (!((uintptr_t)haystack & 15) && X86_HAVE(SSE4_2) &&
            (((uintptr_t)needle + strlen(needle)) & 0xfff) <= 0xff0) {
          return strstr$sse42(haystack, needle);
        }
#endif
        for (i = 0;;) {
          if (!needle[i]) return haystack;
          if (!haystack[i]) break;
          if (needle[i] != haystack[i]) break;
          ++i;
        }
        if (!*haystack++) break;
      }
      return NULL;
    } else {
      return strchr(haystack, needle[0]);
    }
  } else {
    return haystack;
  }
}
