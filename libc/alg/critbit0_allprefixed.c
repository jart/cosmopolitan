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

static intptr_t allprefixed_traverse(unsigned char *top,
                                     intptr_t (*callback)(const char *, void *),
                                     void *arg) {
  if (1 & (intptr_t)top) {
    struct CritbitNode *q = (void *)(top - 1);
    for (int direction = 0; direction < 2; ++direction) {
      intptr_t rc = allprefixed_traverse(q->child[direction], callback, arg);
      if (rc) return rc;
    }
    return 0;
  }
  return callback((const char *)top, arg);
}

/**
 * Invokes callback for all items with prefix.
 *
 * @return 0 unless iteration was halted by CALLBACK returning
 *     nonzero, in which case that value is returned
 * @note h/t djb and agl
 */
intptr_t critbit0_allprefixed(struct critbit0 *t, const char *prefix,
                              intptr_t (*callback)(const char *elem, void *arg),
                              void *arg) {
  const unsigned char *ubytes = (void *)prefix;
  const size_t ulen = strlen(prefix);
  unsigned char *p = t->root;
  unsigned char *top = p;
  if (!p) return 0;
  while (1 & (intptr_t)p) {
    struct CritbitNode *q = (void *)(p - 1);
    unsigned char c = 0;
    if (q->byte < ulen) c = ubytes[q->byte];
    const int direction = (1 + (q->otherbits | c)) >> 8;
    p = q->child[direction];
    if (q->byte < ulen) top = p;
  }
  for (size_t i = 0; i < ulen; ++i) {
    if (p[i] != ubytes[i]) return 0;
  }
  return allprefixed_traverse(top, callback, arg);
}
