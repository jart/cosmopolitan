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
#include "libc/alg/arraylist2.h"
#include "libc/calls/hefty/ntspawn.h"
#include "libc/conv/conv.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
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
  size_t block_i = 0;
  size_t block_n = 0;
  char16_t *block_p = NULL;
  size_t i, j;
  if (!(envp = sortenvp(envp))) goto error;
  const char16_t kNul = u'\0';
  for (i = 0; envp[i]; ++i) {
    unsigned consumed;
    for (j = 0;; j += consumed) {
      wint_t wc;
      char16_t cbuf[2];
      consumed = abs(tpdecode(&envp[i][j], &wc));
      if (CONCAT(&block_p, &block_i, &block_n, cbuf,
                 abs(pututf16(cbuf, ARRAYLEN(cbuf), wc, false))) == -1) {
        goto error;
      }
      if (!wc) break;
    }
  }
  if (APPEND(&block_p, &block_i, &block_n, &kNul) == -1) {
    goto error;
  }
  if (block_i > ENV_MAX) {
    e2big();
    goto error;
  }
  free(envp);
  return block_p;
error:
  free(envp);
  free(block_p);
  return NULL;
}
