/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/complex.h"
#include "libc/stdio/stdio.h"
#include "libc/testlib/testlib.h"

TEST(complex, test) {
  char buf[128];
  complex double z, z1;
  sprintf(buf, "I = %.1f%+.1fi", creal(_Complex_I), cimag(_Complex_I));
  EXPECT_STREQ("I = 0.0+1.0i", buf);
  z1 = _Complex_I * _Complex_I;  // imaginary unit squared
  sprintf(buf, "I * I = %.1f%+.1fi", creal(z1), cimag(z1));
  EXPECT_STREQ("I * I = -1.0+0.0i", buf);
  z = 1.0 + 2.0 * _Complex_I;  // usual way to form a complex number pre-C11
  sprintf(buf, "z = %.1f%+.1fi", creal(z), cimag(z));
  EXPECT_STREQ("z = 1.0+2.0i", buf);
}
