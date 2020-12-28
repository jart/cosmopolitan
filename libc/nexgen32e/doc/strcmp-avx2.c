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
#include "libc/bits/bits.h"
#include "libc/str/internal.h"

#define kVectorSize 32 /* x86+avx2 is 256-bit cpu */

typedef uint8_t uint8_v _Vector_size(kVectorSize);
typedef uint32_t vbitmask_t;

/**
 * Returns how many bytes the utf16 string would be as utf8.
 */
int strcmp$avx2(const char *s1, const char *s2) {
  if (s1 == s2) return 0;
  const unsigned char *p1 = (const unsigned char *)s1;
  const unsigned char *p2 = (const unsigned char *)s2;
  size_t i = -kVectorSize;
vLoop:
  i += kVectorSize;
bLoop:
  if (!IsPointerDangerous(p1 + i) && !IsPointerDangerous(p2 + i)) {
    unsigned char zf;
    vbitmask_t r1;
    uint8_v v1, v2;
    const uint8_v kZero = {0};
    asm(ZFLAG_ASM("vmovdqu\t%5,%2\n\t"     /* move because gcc problematic */
                  "vpcmpeqb\t%4,%2,%1\n\t" /* check for equality in p1 and p2 */
                  "vpcmpeqb\t%6,%2,%2\n\t" /* check for nul in p1 */
                  "vpandn\t%7,%1,%2\n\t" /* most complicated bitwise not ever */
                  "vpor\t%2,%1,%1\n\t"   /* check for nul in p2 */
                  "pmovmskb\t%1,%3\n\t"  /* turn 256 bits into 32 bits */
                  "bsf\t%3,%3")          /* find stop byte */
        : ZFLAG_CONSTRAINT(zf), "=x"(v1), "=x"(v2), "=r"(r1)
        : "m"(*(const uint8_v *)(p1 + i)), "m"(*(const uint8_v *)(p2 + i)),
          "x"(kZero), "m"(kVectorSize));
    if (zf) goto vLoop;
    return p1[i + r1] - p2[i + r1];
  } else {
    i += 1;
    int c;
    if (!(c = p1[i - 1] - p2[i - 1]) && p1[i - 1] + p1[i - 1] != 0) goto bLoop;
    return c;
  }
}
