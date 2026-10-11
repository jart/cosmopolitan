#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include "config.h"
#include "cons.h"
#include "gc.h"
#include "index.h"
#include "plinko.h"
#include "printf.h"
#include "trace.h"

static int Expander(int, int, int);

// expands each expression in list x. the local variables in v shadow
// global macros of the same name
static int ExpandList(int x, int a, int v) {
  if (x >= 0)
    return x;
  return Keep(x, Cons(Expander(Car(x), a, v), ExpandList(Cdr(x), a, v)));
}

int Exlis(int x, int a) {
  return ExpandList(x, a, 0);
}

// adds the variables bound by lambda parameters x to the list v
static int Shadows(int x, int v) {
  if (!x)
    return v;
  if (x > 0)
    return Cons(x, v);
  return Shadows(Car(x), Shadows(Cdr(x), v));
}

// expands (λ 𝑥 𝑦...) where the parameters 𝑥 shadow global macros
static int ExpandLambda(int e, int a, int v) {
  int x;
  if ((x = Cdr(e)) >= 0)
    return e;
  return Keep(
      e, Cons(Car(e), Cons(Car(x), ExpandList(Cdr(x), a, Shadows(Car(x), v)))));
}

// expands (ζ (𝑥 𝑦...)...) where each clause is a list of expressions
static int ExpandClauses(int x, int a, int v) {
  int c;
  if (x >= 0)
    return x;
  c = Car(x);
  return Keep(
      x, Cons(c < 0 ? ExpandList(c, a, v) : c, ExpandClauses(Cdr(x), a, v)));
}

// returns macro (𝑚 𝑥 𝑦) with environment in *u if f names one
static int GetMacro(int f, int a, int v, int *u) {
  if (f >= 0) {
    if (HasAtom(f, v))
      return 0;  // local variable shadows global
    if (!(f = Assoc(f, a)))
      return 0;
    if ((f = Cdr(f)) >= 0)
      return 0;
  }
  if (Car(f) == kClosure) {
    *u = Cddr(f);
    f = Cadr(f);
  } else {
    *u = a;
  }
  if (Head(f) != kMacro)
    return 0;
  return f;
}

/**
 * Performs macro expansion.
 *
 * Macros are applied to their arguments unexpanded, and whatever they
 * return is expanded again. That way a macro like backquote receives
 * its template as written. Otherwise we expand the subexpressions of
 * each form, taking into consideration that lambda parameters shadow
 * global definitions within their bodies.
 *
 * @param e is expression to expand
 * @param a is environment to look up global macros
 * @param v is list of local variables in scope
 */
static int Expander(int e, int a, int v) {
  int f, m, u, x, y, s;
  for (s = 0;;) {
    assert(e < TERM);
    assert(a <= 0);
    if (e >= 0)
      return e;
    if ((f = Car(e)) > 0) {
      if (f == kQuote || f == kClosure)
        return e;
      if (f == kLambda || f == kMacro)
        return ExpandLambda(e, a, v);
      if (f == kCond)
        return Keep(e, Cons(f, ExpandClauses(Cdr(e), a, v)));
      if (f == kTrace) {
        START_TRACE;
        x = Cadr(e);
        y = Expander(x, a, v);
        e = x == y ? e : List(Car(e), y);
        END_TRACE;
        return e;
      }
      if (HasAtom(f, s))
        return ExpandList(e, a, v);
    }
    if (!(m = GetMacro(f, a, v, &u)))
      return ExpandList(e, a, v);
    if (f > 0)
      s = Cons(f, s);
    if (mtrace)
      Fprintf(2, L"%I┌─Macro[%S]%n", e);
    e = eval(Caddr(m), pairlis(Cadr(m), Cdr(e), u));
    if (mtrace)
      Fprintf(2, L"%I└─%S%n", e);
  }
}

int Expand(int e, int a) {
  int r, A;
  A = cx;
  Push(List(kExpand, e));
  r = Keep(e, Expander(e, a, 0));
  Pop();
  r = MarkSweep(A, r);
  return r;
}
