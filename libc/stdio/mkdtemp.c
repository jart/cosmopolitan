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
#include "libc/errno.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/temp.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

/**
 * Creates temporary directory, e.g.
 *
 *     char path[PATH_MAX];
 *     snprintf(path, sizeof(path), "%s%s.XXXXXX",
 *              kTmpPath, program_invocation_short_name);
 *     printf("%s\n", mkdtemp(path));
 *     rmdir(path);
 *
 * @param template must end with XXXXXX which is replaced with
 *     nondeterministic base36 random data
 * @return pointer to template on success, or NULL w/ errno
 * @raise EINVAL if template didn't end with XXXXXX
 */
char *mkdtemp(char *template) {
  unsigned x;
  int i, j, n;
  if ((n = strlen(template)) >= 6 && !memcmp(template + n - 6, "XXXXXX", 6)) {
    for (i = 0; i < 10; ++i) {
      x = _rand64();
      for (j = 0; j < 6; ++j) {
        template[n - 6 + j] = "0123456789abcdefghijklmnopqrstuvwxyz"[x % 36];
        x /= 36;
      }
      if (!mkdir(template, 0700)) {
        return template;
      }
      if (errno != EEXIST) {
        break;
      }
    }
    for (j = 0; j < 6; ++j) {
      template[n - 6 + j] = 'X';
    }
  } else {
    einval();
  }
  return 0;
}
