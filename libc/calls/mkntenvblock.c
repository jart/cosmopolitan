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
#include "libc/alg/arraylist2.internal.h"
#include "libc/calls/ntspawn.h"
#include "libc/fmt/conv.h"
#include "libc/macros.h"
#include "libc/mem/alloca.h"
#include "libc/mem/mem.h"
#include "libc/str/oldutf16.internal.h"
#include "libc/str/str.h"
#include "libc/str/thompike.h"
#include "libc/str/tpdecode.internal.h"
#include "libc/str/utf16.h"
#include "libc/sysv/errfuns.h"

static int CompareStrings(const char *l, const char *r) {
  size_t i = 0;
  while (l[i] == r[i] && r[i]) ++i;
  return (l[i] & 0xff) - (r[i] & 0xff);
}

static void SortStrings(char **a, size_t n) {
  char *t;
  size_t i, j;
  for (i = 1; i < n; ++i) {
    for (t = a[i], j = i; j > 0 && CompareStrings(t, a[j - 1]) < 0; --j) {
      a[j] = a[j - 1];
    }
    a[j] = t;
  }
}

/**
 * Copies sorted environment variable block for Windows.
 *
 * This is designed to meet the requirements of CreateProcess().
 *
 * @param envp is an a NULL-terminated array of UTF-8 strings
 * @return freshly allocated lpEnvironment or NULL w/ errno
 */
textwindows int mkntenvblock(char16_t envvars[ARG_MAX], char *const envp[]) {
  axdx_t rc;
  uint64_t w;
  char **vars;
  wint_t x, y;
  size_t i, j, k, n, m;
  for (n = 0; envp[n];) n++;
  vars = alloca(n * sizeof(char *));
  memcpy(vars, envp, n * sizeof(char *));
  SortStrings(vars, n);
  for (k = i = 0; i < n; ++i) {
    j = 0;
    do {
      x = vars[i][j++] & 0xff;
      if (x >= 0200) {
        if (x < 0300) continue;
        m = ThomPikeLen(x);
        x = ThomPikeByte(x);
        while (--m) {
          if ((y = vars[i][j++] & 0xff)) {
            x = ThomPikeMerge(x, y);
          } else {
            x = 0;
            break;
          }
        }
      }
      w = EncodeUtf16(x);
      do {
        envvars[k++] = w & 0xffff;
        if (k == ARG_MAX) return e2big();
      } while ((w >>= 16));
    } while (x);
  }
  envvars[k] = u'\0';
  return 0;
}
