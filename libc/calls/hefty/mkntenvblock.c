/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/alg/arraylist2.internal.h"
#include "libc/calls/hefty/ntspawn.h"
#include "libc/conv/conv.h"
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
