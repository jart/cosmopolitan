/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/calls.h"
#include "libc/mem/copyfd.internal.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/testlib.h"

STATIC_YOINK("zip_uri_support");

/**
 * Extracts zip asset to filesystem.
 *
 * @param zip is name of asset in zip executable central directory
 * @param to is local filesystem path to which it's extracted
 * @param mode is file mode used for `to`
 */
void testlib_extract(const char *zip, const char *to, int mode) {
  int fdin, fdout;
  ASSERT_NE(-1, (fdin = open(zip, O_RDONLY)));
  ASSERT_NE(-1, (fdout = creat(to, mode)));
  ASSERT_NE(-1, _copyfd(fdin, fdout, -1));
  ASSERT_NE(-1, close(fdout));
  ASSERT_NE(-1, close(fdin));
}
