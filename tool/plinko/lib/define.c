/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Copying of this file is authorized only if (1) you are Justine Tunney, or    │
│ (2) you make absolutely no changes to your copy.                             │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES     │
│ WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF             │
│ MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR      │
│ ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES       │
│ WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN        │
│ ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF      │
│ OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/log/check.h"
#include "tool/plinko/lib/cons.h"
#include "tool/plinko/lib/gc.h"
#include "tool/plinko/lib/index.h"
#include "tool/plinko/lib/plinko.h"
#include "tool/plinko/lib/print.h"
#include "tool/plinko/lib/printf.h"
#include "tool/plinko/lib/tree.h"

void GetName(int *x) {
  int t;
  if (*x < 0) {
    if ((t = GetTree(*x, revglob))) {
      *x = Val(Ent(t));
    }
  }
}

int Define(int e, int a) {
  struct Gc *G;
  int k, v, x, r, o;
  DCHECK_EQ(kDefine, Car(e));
  if (Cdr(e) >= 0) Error("bad define: %S", e);
  if (Cadr(e) <= 0) Error("scheme define: %S", e);
  if (Cddr(e) >= 0 || Caddr(e) == kLambda) {
    /*
     * compatibility with sectorlisp friendly branch, e.g.
     *
     *   (DEFINE 𝑘 . 𝑣)
     *   (DEFINE 𝑘 . (LAMBDA ⋯))
     *
     * are equivalent to the following
     *
     *   (DEFINE 𝑘 (QUOTE 𝑣))
     *   (DEFINE 𝑘 (QUOTE (LAMBDA ⋯)))
     */
    e = Cdr(e);
    k = Car(e);
  } else if (!Cdddr(e)) {
    k = Cadr(e);
    v = Caddr(e);
    x = eval(v, a);
    e = Cons(k, x);
  } else {
    Error("too many args: %S", e);
  }
  a = Cons(PutTree(e, Car(a), 0), 0);
  r = PutTree(Cons(Cdr(e), Car(e)), revglob, 0);
  o = Cons(k, ordglob);
  G = NewGc(cFrost);
  Mark(G, a);
  Mark(G, r);
  Mark(G, o);
  Census(G);
  a = Relocate(G, a);
  r = Relocate(G, r);
  o = Relocate(G, o);
  Sweep(G);
  revglob = r;
  ordglob = o;
  return a;
}

/**
 * Prints stuff declared by Define().
 *
 * The output is primarily intended for describing the internal machine
 * state, with a focus on readability. If closures are printed, with -c
 * then each nugget can be copied back into the machine on its own, b/c
 * each definition is a transitive closure that totally defines all its
 * dependencies. However it's quite verbose, so the normal mode is just
 * readable and can be finessed back into a good program with some work
 * although one thing that's really cool is it peels away macros in the
 * hope it can help demystify amazingly dense recreational abstractions
 *
 * @param t is symbol to use for DEFINE
 * @param a is an association list and/or red-black tree
 * @param o is ordering of names
 * @see Assoc() for documentation on structure of 𝑎
 */
int DumpDefines(int t, int a, int o) {
  int e;
  bool nn;
  nn = noname;
  noname = true;
  for (; o; o = Cdr(o)) {
    e = Assoc(Car(o), a);
    Printf("%n%p%n", Cons(t, Cons(Car(e), Cons(Cdr(e), -0))));
  }
  noname = nn;
  return 0;
}
