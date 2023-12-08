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
    if (p[i] != ubytes[i]) {
      return 0;
    }
  }
  return allprefixed_traverse(top, callback, arg);
}
