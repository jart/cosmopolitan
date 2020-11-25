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
#include "libc/alg/critbit0.h"
#include "libc/alg/internal.h"
#include "libc/str/str.h"

/**
 * Returns non-zero iff 𝑢 ∈ 𝑡.
 * @param t tree
 * @param u NUL-terminated string
 * @note h/t djb and agl
 */
bool critbit0_contains(struct critbit0 *t, const char *u) {
  const unsigned char *ubytes = (void *)u;
  const size_t ulen = strlen(u);
  unsigned char *p = t->root;
  if (!p) return 0;
  while (1 & (intptr_t)p) {
    struct CritbitNode *q = (void *)(p - 1);
    unsigned char c = 0;
    if (q->byte < ulen) c = ubytes[q->byte];
    const int direction = (1 + (q->otherbits | c)) >> 8;
    p = q->child[direction];
  }
  return 0 == strcmp(u, (const char *)p);
}
