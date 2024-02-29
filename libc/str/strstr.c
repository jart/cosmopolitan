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

typedef char xmm_t __attribute__((__vector_size__(16), __aligned__(16)));

/**
 * Searches for substring.
 *
 * @param haystack is the search area, as a NUL-terminated string
 * @param needle is the desired substring, also NUL-terminated
 * @return pointer to first substring within haystack, or NULL
 * @note this implementation goes fast in practice but isn't hardened
 *     against pathological cases, and therefore shouldn't be used on
 *     untrustworthy data
 * @asyncsignalsafe
 * @see strcasestr()
 * @see memmem()
 */
__vex char *strstr(const char *haystack, const char *needle) {
#if defined(__x86_64__) && !defined(__chibicc__)
  size_t i;
  unsigned k, m;
  const xmm_t *p;
  xmm_t v, n, z = {0};
  if (haystack == needle || !*needle) return (char *)haystack;
  n = (xmm_t){*needle, *needle, *needle, *needle, *needle, *needle,
              *needle, *needle, *needle, *needle, *needle, *needle,
              *needle, *needle, *needle, *needle};
  for (;;) {
    k = (uintptr_t)haystack & 15;
    p = (const xmm_t *)((uintptr_t)haystack & -16);
    v = *p;
    m = __builtin_ia32_pmovmskb128((v == z) | (v == n));
    m >>= k;
    m <<= k;
    while (!m) {
      v = *++p;
      m = __builtin_ia32_pmovmskb128((v == z) | (v == n));
    }
    haystack = (const char *)p + __builtin_ctzl(m);
    for (i = 0;; ++i) {
      if (!needle[i]) return (/*unconst*/ char *)haystack;
      if (!haystack[i]) break;
      if (needle[i] != haystack[i]) break;
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
      if (needle[i] != haystack[i]) break;
    }
    if (!*haystack++) break;
  }
  return 0;
#endif
}
