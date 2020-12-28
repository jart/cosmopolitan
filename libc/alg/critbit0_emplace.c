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
#include "libc/alg/critbit0.h"
#include "libc/alg/internal.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"

/**
 * Inserts 𝑢 into 𝑡 without copying.
 * @param t tree
 * @param u NUL-terminated string which must be 8+ byte aligned and
 *     becomes owned by the tree afterwards
 * @return true if 𝑡 was mutated
 * @note h/t djb and agl
 */
bool critbit0_emplace(struct critbit0 *t, char *u, size_t ulen) {
  unsigned char *p = t->root;
  if (!p) {
    t->root = u;
    t->count = 1;
    return true;
  }
  const unsigned char *const ubytes = (void *)u;
  while (1 & (intptr_t)p) {
    struct CritbitNode *q = (void *)(p - 1);
    unsigned char c = 0;
    if (q->byte < ulen) c = ubytes[q->byte];
    const int direction = (1 + (q->otherbits | c)) >> 8;
    p = q->child[direction];
  }
  uint32_t newbyte;
  uint32_t newotherbits;
  for (newbyte = 0; newbyte < ulen; ++newbyte) {
    if (p[newbyte] != ubytes[newbyte]) {
      newotherbits = p[newbyte] ^ ubytes[newbyte];
      goto different_byte_found;
    }
  }
  if (p[newbyte] != 0) {
    newotherbits = p[newbyte];
    goto different_byte_found;
  }
  return false;
different_byte_found:
  newotherbits |= newotherbits >> 1;
  newotherbits |= newotherbits >> 2;
  newotherbits |= newotherbits >> 4;
  newotherbits = (newotherbits & ~(newotherbits >> 1)) ^ 255;
  unsigned char c = p[newbyte];
  int newdirection = (1 + (newotherbits | c)) >> 8;
  struct CritbitNode *newnode = malloc(sizeof(struct CritbitNode));
  newnode->byte = newbyte;
  newnode->otherbits = newotherbits;
  newnode->child[1 - newdirection] = ubytes;
  void **wherep = &t->root;
  for (;;) {
    unsigned char *wp = *wherep;
    if (!(1 & (intptr_t)wp)) break;
    struct CritbitNode *q = (void *)(wp - 1);
    if (q->byte > newbyte) break;
    if (q->byte == newbyte && q->otherbits > newotherbits) break;
    unsigned char c2 = 0;
    if (q->byte < ulen) c2 = ubytes[q->byte];
    const int direction = (1 + (q->otherbits | c2)) >> 8;
    wherep = q->child + direction;
  }
  newnode->child[newdirection] = *wherep;
  *wherep = (void *)(1 + (char *)newnode);
  t->count++;
  return true;
}
