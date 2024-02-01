/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/likely.h"
#include "libc/str/str.h"

static const char kUtf8Dispatch[] = {
    0, 0, 1, 1, 1, 1, 1, 1,  // 0300 utf8-2
    1, 1, 1, 1, 1, 1, 1, 1,  // 0310
    1, 1, 1, 1, 1, 1, 1, 1,  // 0320
    1, 1, 1, 1, 1, 1, 1, 1,  // 0330
    2, 3, 3, 3, 3, 3, 3, 3,  // 0340 utf8-3
    3, 3, 3, 3, 3, 3, 3, 3,  // 0350
    4, 5, 5, 5, 5, 0, 0, 0,  // 0360 utf8-4
    0, 0, 0, 0, 0, 0, 0, 0,  // 0370
};

/**
 * Returns true if text is utf-8.
 *
 *     isutf8 n=0                  1 nanoseconds
 *     isutf8 n=5                661 ps/byte          1,476 mb/s
 *     isutf8 ascii n=22851       26 ps/byte             35 GB/s
 *     isutf8 unicode n=3193     543 ps/byte          1,795 mb/s
 *
 * This function considers all ASCII characters including NUL to be
 * valid UTF-8. The conditions for something not being valid are:
 *
 * - Incorrect sequencing of 0300 (FIRST) and 0200 (CONT) chars
 * - Thompson-Pike varint sequence not encodable as UTF-16
 * - Overlong UTF-8 encoding
 *
 * @param size if -1 implies strlen
 */
bool32 isutf8(const void *data, size_t size) {
  long c;
  const char *p, *e;
  if (size == -1) size = data ? strlen(data) : 0;
  p = data;
  e = p + size;
  while (p < e) {
#if defined(__x86_64__) && !defined(__chibicc__)
    typedef char xmm_t __attribute__((__vector_size__(16), __aligned__(16)));
    if (!((intptr_t)p & 15)) {
      for (;;) {
        unsigned m;
        if ((m = __builtin_ia32_pmovmskb128(*(xmm_t *)p >= (xmm_t){0}) ^
                 0xffff)) {
          m = __builtin_ctzll(m);
          p += m;
          break;
        } else if ((p += 16) >= e) {
          break;
        }
      }
      if (p >= e) {
        return true;
      }
    }
#endif
    if (LIKELY((c = *p++ & 255) < 0200)) continue;
    if (UNLIKELY(c < 0300)) return false;
    switch (kUtf8Dispatch[c - 0300]) {
      case 0:
        return false;
      case 1:
        if (p < e && (*p & 0300) == 0200) {
          ++p;
          break;
        } else {
          return false;  // missing cont
        }
      case 2:
        if (p < e && (*p & 0377) < 0240) {
          return false;  // overlong
        }
        // fallthrough
      case 3:
        if (p + 2 <= e &&             //
            (p[0] & 0300) == 0200 &&  //
            (p[1] & 0300) == 0200) {  //
          p += 2;
          break;
        } else {
          return false;  // missing cont
        }
      case 4:
        if (p < e && (*p & 0377) < 0220) {
          return false;  // overlong
        }
        // fallthrough
      case 5:
        if (p + 3 <= e &&                         //
            (((uint32_t)(p[+2] & 0377) << 030 |   //
              (uint32_t)(p[+1] & 0377) << 020 |   //
              (uint32_t)(p[+0] & 0377) << 010 |   //
              (uint32_t)(p[-1] & 0377) << 000) &  //
             0xC0C0C000) == 0x80808000) {         //
          p += 3;
          break;
        } else {
          return false;  // missing cont
        }
      default:
        __builtin_unreachable();
    }
  }
  return true;
}
