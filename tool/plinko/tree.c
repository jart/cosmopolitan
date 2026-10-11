#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include "tree.h"
#include "index.h"

int Nod(int E, int L, int R, int C) {
#ifndef NDEBUG
  assert(L <= 0);
  assert(R <= 0);
  assert(!(C & ~1));
#endif
  return Cons(Cons(E, (L | R) ? Cons(L, R) : 0), C);
}

#if DEBUG_TREE
static void CheckTreeImpl(int N) {
  int p, L, R;
  if (N >= 0)
    Error(L"N is atom: %S", N);
  if (Car(N) >= 0)
    Error(L"Car(N) is an atom: %S", N);
  if (Cdr(N) & ~1)
    Error(L"Cdr(N) is non-bool: %S", N);
  if ((L = Lit(N))) {
    if ((p = Cmp(Key(Ent(L)), Key(Ent(N)))) != -1) {
      Error(L"Cmp(Key(L), Key(N)) != -1%n"
            L"Result = %d%n"
            L"Key(L) = %p%n"
            L"Key(N) = %p",
            p, Key(Ent(L)), Key(Ent(N)));
    }
    if (Red(N) && Red(L)) {
      Error(L"left node and its parent are both red%n%T", N);
    }
    CheckTreeImpl(L);
  }
  if ((R = Rit(N))) {
    if ((p = Cmp(Key(Ent(R)), Key(Ent(N)))) != +1) {
      Error(L"Cmp(Key(R), Key(N)) != +1%n"
            L"Result = %d%n"
            L"Key(R) = %p%n"
            L"Key(N) = %p",
            p, Key(Ent(R)), Key(Ent(N)));
    }
    if (Red(N) && Red(R)) {
      Error(L"right node and its parent are both red%n%T", N);
    }
    CheckTreeImpl(R);
  }
}
#endif

static int CheckTree(int N) {
#if DEBUG_TREE
  if (N) {
    if (Red(N))
      Error(L"tree root is red%n%T", N);
    CheckTreeImpl(N);
  }
#endif
  return N;
}

static int BalTree(int E, int L, int R, int C) {
  // Chris Okasaki "Red-Black Trees in a Functional Setting";
  // Functional Pearls, Cambridge University Press, Jan 1993.
  int LL, LR, RL, RR;
  if (!C) {
    LL = Lit(L);
    LR = Rit(L);
    RL = Lit(R);
    RR = Rit(R);
    if (Red(L) && Red(LR)) {
      // Degenerate Case No. 1
      // Complete the Triforce
      //
      //        Z
      //       ╱ ╲             𝐘
      //      𝐗   d           ╱ ╲
      //     ╱ ╲       →     X   Z
      //    a   𝐘           ╱ ╲ ╱ ╲
      //       ╱ ╲         a  b c  d
      //      b   c
      //
      // ((Z ((X a (Y b . c) . t) . t) . d)) →
      // ((Y ((X a . b)) (Z c . d)) . t)
      L = Nod(Ent(L), LL, Lit(LR), 0);
      R = Nod(E, Rit(LR), R, 0);
      E = Ent(LR);
      C = 1;
    } else if (Red(L) && Red(LL)) {
      // Degenerate Case No. 2
      // Complete the Triforce
      //
      //          Z
      //         ╱ ╲           𝐘
      //        𝐘   d         ╱ ╲
      //       ╱ ╲      →    X   Z
      //      𝐗   c         ╱ ╲ ╱ ╲
      //     ╱ ╲           a  b c  d
      //    a   b
      //
      // ((Z ((Y ((X a . b) . t) . c) . t) . d)) →
      // ((Y ((X a . b)) (Z c . d)) . t)
      R = Nod(E, LR, R, 0);
      E = Ent(L);
      L = Bkn(LL);
      C = 1;
    } else if (Red(R) && Red(RR)) {
      // Degenerate Case No. 3
      // Complete the Triforce
      //
      //     X
      //    ╱ ╲               𝐘
      //   a   𝐘             ╱ ╲
      //      ╱ ╲     →     X   Z
      //     b   𝐙         ╱ ╲ ╱ ╲
      //        ╱ ╲       a  b c  d
      //       c   d
      //
      // ((X a (Y b (Z c . d) . t) . t)) →
      // ((Y ((X a . b)) (Z c . d)) . t)
      L = Nod(E, L, RL, 0);
      E = Ent(R);
      R = Bkn(RR);
      C = 1;
    } else if (Red(R) && Red(RL)) {
      // Degenerate Case No. 4
      // Complete the Triforce
      //
      //       X
      //      ╱ ╲             𝐘
      //     a   𝐙           ╱ ╲
      //        ╱ ╲    →    X   Z
      //       𝐘   d       ╱ ╲ ╱ ╲
      //      ╱ ╲         a  b c  d
      //     b   c
      //
      // ((X a (Z ((Y b . c) . t) . d) . t)) →
      // ((Y ((X a . b)) (Z c . d)) . t)
      L = Nod(E, L, Lit(RL), 0);
      R = Nod(Ent(R), Rit(RL), RR, 0);
      E = Ent(RL);
      C = 1;
    }
  }
  return Nod(E, L, R, C);
}

static int InsTree(int E, int N, int KEEP) {
  int P, L, R;
  if (N) {
    P = Cmp(Key(E), Key(Ent(N)));
    if (P < 0) {
      if ((L = InsTree(E, Lit(N), KEEP)) > 0)
        return L;  // rethrow
      if (L != Lit(N))
        N = BalTree(Ent(N), L, Rit(N), Tail(N));
    } else if (P > 0) {
      if ((R = InsTree(E, Rit(N), KEEP)) > 0)
        return R;  // rethrow
      if (R != Rit(N))
        N = BalTree(Ent(N), Lit(N), R, Tail(N));
    } else if (KEEP < 0 || (!KEEP && !Equal(Val(E), Val(Ent(N))))) {
      N = Cons(Cons(E, Chl(N)), Red(N));
    } else if (KEEP > 1) {
      N = KEEP;  // throw
    }
  } else {
    N = Cons(Cons(E, 0), 1);
  }
  return N;
}

/**
 * Inserts entry into red-black tree.
 *
 *        DICTIONARY NODE           SET NODE          ATOM SET NODE
 *
 *           ┌───┬───┐              ┌───┬───┐          ┌───┬───┐
 *           │ ┬ │ 𝑐 │              │ ┬ │ 𝑐 │          │ ┬ │ 𝑐 │
 *           └─│─┴───┘              └─│─┴───┘          └─│─┴───┘
 *           ┌─┴─┬───┐              ┌─┴─┬───┐          ┌─┴─┬───┐
 *           │ ┬ │ ┬ │              │ ┬ │ ┬ │          │ 𝑣 │ ┬ │
 *           └─│─┴─│─┘              └─│─┴─│─┘          └───┴─│─┘
 *      ┌───┬──┴┐ ┌┴──┬───┐    ┌───┬──┴┐ ┌┴──┬───┐         ┌─┴─┬───┐
 *      │ 𝑥 │ 𝑦 │ │ L │ R │    │ 𝑥 │ ⊥ │ │ L │ R │         │ L │ R │
 *      └───┴───┘ └───┴───┘    └───┴───┘ └───┴───┘         └───┴───┘
 *
 * @param E is entry which may be
 *     -  (𝑥 . 𝑦) where 𝑥 is the key and 𝑦 is arbitrary tag-along content
 *     -  𝑣 for memory-efficient sets of atoms
 * @param N is red-black tree node which should look like
 *     -  ⊥ is an tree or atom set with zero elements
 *     -  (((𝑥 ⋯) . (𝑙 . 𝑟)) . ⊥) is a black node a.k.a. (((𝑥 ⋯) 𝑙 . 𝑟))
 *     -  (((𝑥 ⋯) . (𝑙 . 𝑟)) . ⊤) is a red node   a.k.a. (((𝑥 ⋯) 𝑙 . 𝑟) . ⊤)
 *     -  ((𝑣 𝑙 . 𝑟)) a memory-efficient black node for an atom set
 *     -  ((𝑣 𝑙 . 𝑟)) is functionally equivalent to (((𝑣) 𝑙 . 𝑟))
 *     -  ((𝑣 ⊥ . ⊥)) is an atom set with a single element
 *     -  ((𝑣)) is functionally equivalent to ((𝑣 ⊥ . ⊥)) or ((𝑣 . (⊥ . ⊥)) . ⊥)
 *     -  𝑣 is crash therefore (((⋯) 𝑣)) and ⊥(((⋯) ⊥ . 𝑣)) are crash
 *     -  (𝑣) is crash, first element must be a cons cell
 *     -  ((⋯) . 𝑥) is crash if 𝑥 isn't ⊤ or ⊥
 * @param KEEP may be
 *     - -1 to replace existing entries always
 *     -  0 to replace existing entries if values are non-equal
 *     -  1 to return N if
 *     - >1 specifies arbitrary tombstone to return if key exists
 * @return ((𝑒 𝑙 . 𝑟) . 𝑐) if found where 𝑒 can be 𝑣 or (𝑥 . 𝑦)
 *     - or KEEP if not found and KEEP > 1
 *     - or ⊥ if not found
 */
int PutTree(int E, int N, int KEEP) {
  assert(N <= 0);
  assert(Car(N) <= 0);
  assert(KEEP >= -1);
  return CheckTree(Bkn(InsTree(E, N, KEEP)));
}

/**
 * Finds node in binary tree.
 * @return ((𝑒 𝑙 . 𝑟) . 𝑐) if found, otherwise 0
 */
int GetTree(int k, int N) {
  int p;
  while (N) {
    p = Cmp(k, Key(Ent(N)));
    if (p < 0) {
      N = Lit(N);
    } else if (p > 0) {
      N = Rit(N);
    } else {
      break;
    }
  }
  return N;
}

int GetTreeCount(int k, int N, int *c) {
  int p;
  while (N) {
    ++*c;
    p = Cmp(k, Key(Ent(N)));
    if (p < 0) {
      N = Lit(N);
    } else if (p > 0) {
      N = Rit(N);
    } else {
      break;
    }
  }
  return N;
}
