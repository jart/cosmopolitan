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
#include "libc/calls/hefty/ntspawn.h"
#include "libc/fmt/conv.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/str/oldutf16.internal.h"
#include "libc/str/str.h"
#include "libc/str/tpdecode.internal.h"
#include "libc/sysv/errfuns.h"

/**
 * Copies sorted environment variable block for Windows.
 *
 * This is designed to meet the requirements of CreateProcess().
 *
 * @param envp is an a NULL-terminated array of UTF-8 strings
 * @return freshly allocated lpEnvironment or NULL w/ errno
 */
textwindows char16_t *mkntenvblock(char *const envp[]) {
  wint_t wc;
  size_t i, j, bi, bn;
  char16_t *bp, cbuf[2];
  unsigned consumed, produced;
  bi = 0;
  bn = 8;
  bp = NULL;
  if ((envp = sortenvp(envp)) && (bp = calloc(bn, sizeof(char16_t)))) {
    for (i = 0; envp[i]; ++i) {
      for (j = 0;; j += consumed) {
        consumed = abs(tpdecode(&envp[i][j], &wc));
        produced = abs(pututf16(cbuf, ARRAYLEN(cbuf), wc, false));
        if (CONCAT(&bp, &bi, &bn, cbuf, produced) == -1) goto error;
        if (!wc) break;
      }
    }
    ++bi;
    if (bi > ENV_MAX) {
      e2big();
      goto error;
    }
    free(envp);
    return bp;
  }
error:
  free(envp);
  free(bp);
  return NULL;
}
