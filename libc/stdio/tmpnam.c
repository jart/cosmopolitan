/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/dce.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

static char g_tmpnam[L_tmpnam];

/**
 * Generates temporary file name.
 *
 * @param buf may be null to generate `/tmp/tmpnam_XXXXXX` to a static
 *     buffer (it works on Windows) that is not thread safe; otherwise
 *     the caller must supply a buffer with `L_tmpnam` bytes available
 * @return unique generated filename, or null on failure; `buf` memory
 *     is only mutated on success
 */
char *tmpnam(char *buf) {
  char path[] = P_tmpdir "/tmpnam_XXXXXX";
  for (int t = 0; t < 100; ++t) {
    int w = _rand64();
    for (int i = 0; i < 6; ++i) {
      path[sizeof(path) - 1 - 6 + i] =
          "0123456789abcdefghikmnpqrstvwxyz"[w & 31];
      w >>= 5;
    }
    if (!fileexists(path)) {
      return strcpy(buf ? buf : g_tmpnam, path);
    }
  }
  return 0;
}
