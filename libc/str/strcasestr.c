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
#include "libc/str/str.h"
#include "libc/dce.h"
#include "libc/str/tab.internal.h"

typedef char xmm_t __attribute__((__vector_size__(16), __aligned__(16)));

/**
 * Searches for substring case-insensitively.
 *
 * @param haystack is the search area, as a NUL-terminated string
 * @param needle is the desired substring, also NUL-terminated
 * @return pointer to first substring within haystack, or NULL
 * @note this implementation goes fast in practice but isn't hardened
 *     against pathological cases, and therefore shouldn't be used on
 *     untrustworthy data
 * @asyncsignalsafe
 * @see strstr()
 */
__vex char *strcasestr(const char *haystack, const char *needle) {
#if defined(__x86_64__) && !defined(__chibicc__)
  char c;
  size_t i;
  unsigned k, m;
  const xmm_t *p;
  xmm_t v, n1, n2, z = {0};
  if (haystack == needle || !*needle) return (char *)haystack;
  c = *needle;
  n1 = (xmm_t){c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c};
  c = kToLower[c & 255];
  n2 = (xmm_t){c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c};
  for (;;) {
    k = (uintptr_t)haystack & 15;
    p = (const xmm_t *)((uintptr_t)haystack & -16);
    v = *p;
    m = __builtin_ia32_pmovmskb128((v == z) | (v == n1) | (v == n2));
    m >>= k;
    m <<= k;
    while (!m) {
      v = *++p;
      m = __builtin_ia32_pmovmskb128((v == z) | (v == n1) | (v == n2));
    }
    haystack = (const char *)p + __builtin_ctzl(m);
    for (i = 0;; ++i) {
      if (!needle[i]) return (/*unconst*/ char *)haystack;
      if (!haystack[i]) break;
      if (kToLower[needle[i] & 255] != kToLower[haystack[i] & 255]) break;
    }
    if (!*haystack++) break;
  }
  return 0;
#else
  size_t i;
  if (haystack == needle || !*needle) return (void *)haystack;
  for (;;) {
    for (i = 0;; ++i) {
      if (!needle[i]) return (/*unconst*/ char *)haystack;
      if (!haystack[i]) break;
      if (kToLower[needle[i] & 255] != kToLower[haystack[i] & 255]) break;
    }
    if (!*haystack++) break;
  }
  return 0;
#endif
}
