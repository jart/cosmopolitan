/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/mem/critbit0.h"
#include "libc/mem/internal.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"

/**
 * Removes 𝑢 from 𝑡.
 * @param t tree
 * @param u NUL-terminated string
 * @return true if 𝑡 was mutated
 * @note h/t djb and agl
 */
bool critbit0_delete(struct critbit0 *t, const char *u) {
  const unsigned char *ubytes = (void *)u;
  const size_t ulen = strlen(u);
  unsigned char *p = t->root;
  void **wherep = &t->root;
  void **whereq = 0;
  struct CritbitNode *q = 0;
  int direction = 0;
  if (!p) return false;
  while (1 & (intptr_t)p) {
    whereq = wherep;
    q = (void *)(p - 1);
    unsigned char c = 0;
    if (q->byte < ulen) c = ubytes[q->byte];
    direction = (1 + (q->otherbits | c)) >> 8;
    wherep = q->child + direction;
    p = *wherep;
  }
  if (0 != strcmp(u, (const char *)p)) return false;
  free(p), p = NULL;
  if (!whereq) {
    t->root = NULL;
    t->count = 0;
    return true;
  }
  *whereq = q->child[1 - direction];
  free(q), q = NULL;
  t->count--;
  return true;
}
