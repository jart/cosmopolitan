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

static void critbit0_clear_traverse(void *top) {
  unsigned char *p = top;
  if (1 & (intptr_t)p) {
    struct CritbitNode *q = (void *)(p - 1);
    critbit0_clear_traverse(q->child[0]);
    critbit0_clear_traverse(q->child[1]);
    free(q);
  } else {
    free(p);
  }
}

/**
 * Removes all items from 𝑡.
 * @param t tree
 * @note h/t djb and agl
 */
void critbit0_clear(struct critbit0 *t) {
  if (t->root) {
    critbit0_clear_traverse(t->root);
    t->root = NULL;
  }
  t->count = 0;
}
