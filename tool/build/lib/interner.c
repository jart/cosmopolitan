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
#include "libc/alg/arraylist.internal.h"
#include "libc/alg/arraylist2.internal.h"
#include "libc/bits/safemacros.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/str/str.h"
#include "libc/x/x.h"
#include "tool/build/lib/interner.h"

#define kInitialItems 16

struct InternerObject {
  struct Interner pool;
  size_t i, n;
  struct InternerHash {
    unsigned hash; /* 0 means empty */
    unsigned index;
  } * p;
};

static void rehash(struct InternerObject *it) {
  size_t i, j, n, step;
  struct InternerHash *p;
  n = it->n;
  p = it->p;
  it->p = xcalloc((it->n <<= 1), sizeof(struct InternerHash));
  for (i = 0; i < n; ++i) {
    if (!p[i].hash) continue;
    step = 0;
    do {
      j = (p[i].hash + step * ((step + 1) >> 1)) & (it->n - 1);
      step++;
    } while (it->p[j].hash);
    memcpy(&it->p[j], &p[i], sizeof(p[i]));
  }
  free_s(&p);
}

/**
 * Creates new interner.
 */
struct Interner *newinterner(void) {
  struct InternerObject *it;
  it = xcalloc(1, sizeof(*it));
  it->p = xcalloc((it->n = kInitialItems), sizeof(*it->p));
  it->pool.p = xcalloc((it->pool.n = kInitialItems), sizeof(*it->pool.p));
  return &it->pool;
}

/**
 * Destroys interner.
 */
void freeinterner(struct Interner *t) {
  struct InternerObject *it = (struct InternerObject *)t;
  if (it) {
    free(it->pool.p);
    free(it->p);
    free(it);
  }
}

/**
 * Returns number of unique items interned.
 */
size_t interncount(const struct Interner *t) {
  struct InternerObject *it = (struct InternerObject *)t;
  return it->i;
}

/**
 * Interns object.
 *
 * @return index into 𝑡→𝑝 holding equal item
 * @note use consistent size w/ non-string items
 */
size_t internobj(struct Interner *t, const void *data, size_t size) {
  char *item;
  unsigned hash;
  size_t i, step;
  struct InternerObject *it;
  step = 0;
  item = data;
  it = (struct InternerObject *)t;
  hash = max(1, crc32c(0, data, size));
  do {
    /* it is written that triangle probe halts iff i<n/2 && popcnt(n)==1 */
    i = (hash + step * ((step + 1) >> 1)) & (it->n - 1);
    if (it->p[i].hash == hash && it->p[i].index + size <= it->pool.n &&
        memcmp(item, &it->pool.p[it->p[i].index], size) == 0) {
      return it->p[i].index;
    }
    step++;
  } while (it->p[i].hash);
  if (++it->i == (it->n >> 1)) {
    rehash(it);
    step = 0;
    do {
      i = (hash + step * ((step + 1) >> 1)) & (it->n - 1);
      step++;
    } while (it->p[i].hash);
  }
  it->p[i].hash = hash;
  return (it->p[i].index =
              CONCAT(&it->pool.p, &it->pool.i, &it->pool.n, item, size));
}

/**
 * Interns string.
 *
 * The NUL-terminated string 𝑠 is concatenated to the relocatable
 * double-NUL terminated string list 𝑡→𝑝 with de-duplication and
 * preservation of insertion order.
 *
 * @return index into 𝑡→𝑝 holding string equal to 𝑠
 */
size_t intern(struct Interner *t, const char *s) {
  return internobj(t, s, strlen(s) + 1);
}
