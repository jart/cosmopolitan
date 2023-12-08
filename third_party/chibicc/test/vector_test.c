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
#include "third_party/chibicc/test/test.h"

typedef char byte16 __attribute__((__vector_size__(16)));
typedef float float4 __attribute__((__vector_size__(16)));
typedef float float4a1 __attribute__((__vector_size__(16), __aligned__(1)));
typedef float float4a16 __attribute__((__vector_size__(16), __aligned__(16)));
typedef double double2 __attribute__((__vector_size__(16)));
typedef double double2a1 __attribute__((__vector_size__(16), __aligned__(1)));
typedef double double2a16 __attribute__((__vector_size__(16), __aligned__(16)));

int main(void) {
  ASSERT(16, sizeof(float4));
  ASSERT(16, sizeof(float4a1));
  ASSERT(16, sizeof(float4a16));
  ASSERT(16, sizeof(double2));
  ASSERT(16, sizeof(double2a1));
  ASSERT(16, sizeof(double2a16));
  ASSERT(16, _Alignof(float4));
  ASSERT(1, _Alignof(float4a1));
  ASSERT(16, _Alignof(float4a16));
  ASSERT(16, _Alignof(double2));
  ASSERT(1, _Alignof(double2a1));
  ASSERT(16, _Alignof(double2a16));

  {
    float4 v1;
    float4 v2;
    float x[4] = {1, 2, 3, 4};
    float y[4] = {1, 1, 1, 1};
    memcpy(&v1, x, 16);
    memcpy(&v2, y, 16);
    v1 = v1 + v2;
    memcpy(x, &v1, 16);
    ASSERT(2, x[0]);
    // TODO(jart): fix me
    /* ASSERT(3, x[1]); */
    /* ASSERT(4, x[2]); */
    /* ASSERT(5, x[3]); */
  }

  {
    byte16 v;
    float x1[4] = {1, 2, 3, 4};
    float x2[4];
    memcpy(&v, x1, 16);
    __builtin_ia32_movntdq(x1, &v);
    memcpy(x2, &v, 16);
    ASSERT(1, x2[0]);
    // TODO(jart): fix me
    /* ASSERT(2, x[1]); */
  }

  return 0;
}
