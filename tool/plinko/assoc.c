#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include "histo.h"
#include "index.h"
#include "plinko.h"
#include "tree.h"

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
  (void)c;
  assert(k >= 0);
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

/**
 * Performs lookup of global binding.
 *
 * Global definitions are the entries stored in red-black tree nodes of
 * the association list. This returns 0 if the atom is not defined, or
 * if it's shadowed by a local binding, which lets the planner decide at
 * the first evaluation which function a call will always refer to.
 *
 * @param k is an atom
 * @param a is an association list and/or red-black tree
 * @return entry cons cell or 0 if not a global
 */
int AssocGlobal(int k, int a) {
  int e, j;
  assert(k >= 0);
  if (!k || k == 1)
    return 0;
  for (; a; a = Cdr(a)) {
    e = Head(a);
    if ((j = Head(e)) == k)
      return 0;
    if (j < 0 && (j = GetTree(k, e)))
      return Ent(j);
  }
  return 0;
}
