/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "tool/plinko/lib/histo.h"
#include "tool/plinko/lib/index.h"
#include "tool/plinko/lib/plinko.h"
#include "tool/plinko/lib/tree.h"

/**
 * Performs lookup of binding.
 *
 * We perform lookups into an association list, e.g.
 *
 *       ┌───┬───┐ ┌───┬───┐ ┌───┬───┐
 *       │ 𝑘₀│ 𝑣₀│ │ 𝑘₁│ 𝑣₁│ │ 𝑘₂│ 𝑣₂│
 *       └─┬─┴───┘ └─┬─┴───┘ └─┬─┴───┘
 *       ┌─│─┬───┐ ┌─│─┬───┐ ┌─│─┬───┐
 *     𝑎=│ ┴ │ ├───┤ ┴ │ ├───┤ ┴ │ 0 │
 *       └───┴───┘ └───┴───┘ └───┴───┘
 *
 * For example, if 𝑘₁ is queried then we return:
 *
 *     ┌───┬───┐
 *     │ 𝑘₁│ 𝑣₁│
 *     └───┴───┘
 *
 * However there's a twist: the association is allowed to turn into a
 * red-black tree. That would look like this:
 *
 *       ┌───┬───┐ ┌───┬───┐
 *       │ 𝑘₀│ 𝑣₀│ │ 𝑘₁│ 𝑣₁│
 *       └─┬─┴───┘ └─┬─┴───┘
 *       ┌─│─┬───┐ ┌─│─┬───┐ ┌───┬───┐
 *     𝑎=│ ┴ │ ├───┤ ┴ │ ├───│ ┬ │ 𝑐 │
 *       └───┴───┘ └───┴───┘ └─│─┴───┘
 *                           ┌─┴─┬───┐
 *                           │ ┬ │ ┬ │
 *                           └─│─┴─│─┘
 *                      ┌───┬──┴┐ ┌┴──┬───┐
 *                      │ 𝑘₂│ 𝑣₂│ │ L │ R │
 *                      └───┴───┘ └───┴───┘
 *
 * We're able to tell a tree node apart from an association list node
 * because we make the assumption that 𝑘ᵢ is an atom. On the other hand
 * 𝑣ᵢ can be anything.
 *
 * @param k is an atom
 * @param a is an association list and/or red-black tree
 * @return entry cons cell or 0 if not found
 */
int Assoc(int k, int a) {
  int i, j, e, c, r;
  DCHECK_GE(k, 0);
  i = a;
  r = 0;
  if (k && k != 1) {
    for (c = 0; i;) {
      e = Head(i);
      i = Cdr(i);
      j = Head(e);
      ++c;
      if (j == k) {
        r = e;
        break;
      } else if (j < 0 && (j =
#if HISTO_ASSOC
                               GetTreeCount(k, e, &c)
#else
                               GetTree(k, e)
#endif
                               )) {
        r = Ent(j);
        break;
      }
    }
#if HISTO_ASSOC
    HISTO(g_assoc_histogram, c);
#endif
  }
  return r;
}
