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
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/serialize.h"
#include "libc/stdio/rand.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"
#include "libc/temp.h"

/**
 * Creates temporary directory, e.g.
 *
 *     char path[] = "/tmp/foo.XXXXXX";
 *     mkdtemp(path);
 *     rmdir(path);
 *
 * @param template must end with XXXXXX which will be replaced
 *     with random text on success (and not modified on error)
 * @return pointer to template on success, or NULL w/ errno
 * @raise EINVAL if template didn't end with XXXXXX
 * @cancelationpoint
 */
char *mkdtemp(char *template) {
  int n;
  if ((n = strlen(template)) < 6 ||
      READ32LE(template + n - 6) != READ32LE("XXXX") ||
      READ16LE(template + n - 6 + 4) != READ16LE("XX")) {
    einval();
    return 0;
  }
  for (;;) {
    int x = _rand64();
    for (int i = 0; i < 6; ++i) {
      template[n - 6 + i] = "0123456789abcdefghikmnpqrstvwxyz"[x & 31];
      x >>= 5;
    }
    int e = errno;
    if (!mkdir(template, 0700)) {
      return template;
    } else if (errno != EEXIST) {
      memcpy(template + n - 6, "XXXXXX", 6);
      return 0;
    }
    errno = e;
  }
}
