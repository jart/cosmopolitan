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
#include "tool/plinko/lib/plinko.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/errno.h"
#include "libc/intrin/likely.h"
#include "libc/intrin/strace.internal.h"
#include "libc/log/countbranch.h"
#include "libc/log/countexpr.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/arch.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sig.h"
#include "libc/time/clockstonanos.internal.h"
#include "third_party/getopt/getopt.internal.h"
#include "tool/build/lib/case.h"
#include "tool/plinko/lib/char.h"
#include "tool/plinko/lib/error.h"
#include "tool/plinko/lib/gc.h"
#include "tool/plinko/lib/histo.h"
#include "tool/plinko/lib/index.h"
#include "tool/plinko/lib/print.h"
#include "tool/plinko/lib/printf.h"
#include "tool/plinko/lib/stack.h"
#include "tool/plinko/lib/trace.h"
#include "tool/plinko/lib/tree.h"

STATIC_STACK_SIZE(0x100000);

#define PUTS(f, s) write(f, s, strlen(s))

#define DISPATCH(ea, tm, r, p1, p2) \
  GetDispatchFn(LO(ea))(ea, tm, r, p1, p2, GetShadow(LO(ea)))

static void Unwind(int S) {
  int s;
  dword t;
  while (sp > S) {
    s = --sp & MASK(STACK);
    if ((t = g_stack[s])) {
      g_stack[s] = 0;
      cx = ~HI(t);
    }
  }
}

static void Backtrace(int S) {
  int i;
  dword f;
  for (i = 0; sp > S && i < STACK; ++i) {
    f = Pop();
    Fprintf(2, "%d %p%n", ~HI(f), LO(f));
    g_stack[sp & MASK(STACK)] = 0;
  }
}

forceinline bool ShouldIgnoreGarbage(int A) {
  static unsigned cadence;
  if (DEBUG_GARBAGE) return false;
  if (!(++cadence & AVERSIVENESS)) return false;
  return true;
}

static inline bool ShouldPanicAboutGarbage(int A) {
  return false;
}

static inline bool ShouldAbort(int A) {
  return cx <= A + BANE / STACK * 3;  // hacked thrice permitted memory
}

static relegated dontinline int ErrorExpr(void) {
  Raise(kError);
}

static int Order(int x, int y) {
  if (x < y) return -1;
  if (x > y) return +1;
  return 0;
}

static int Append(int x, int y) {
  if (!x) return y;
  return Cons(Car(x), Append(Cdr(x), y));
}

static int ReconstructAlist(int a) {
  int r;
  for (r = 0; a < 0; a = Cdr(a)) {
    if (Car(a) == kClosure) {
      return Reverse(r, a);
    }
    if (Caar(a) < 0) {
      r = Reverse(r, a);
    } else if (!Assoc(Caar(a), r)) {
      r = Cons(Car(a), r);
    }
  }
  return Reverse(r, 0);
}

static bool AtomEquals(int x, const char *s) {
  dword t;
  do {
    if (!*s) return false;
    t = Get(x);
    if (LO(t) != *s++) return false;  // xxx: ascii
  } while ((x = HI(t)) != TERM);
  return !*s;
}

static pureconst int LastCons(int x) {
  while (Cdr(x)) x = Cdr(x);
  return x;
}

static pureconst int LastChar(int x) {
  dword e;
  do e = Get(x);
  while ((x = HI(e)) != TERM);
  return LO(e);
}

forceinline pureconst bool IsClosure(int x) {
  return x < 0 && Car(x) == kClosure;
}

forceinline pureconst bool IsQuote(int x) {
  return x < 0 && Car(x) == kQuote;
}

static int Quote(int x) {
  if (IsClosure(x)) return x;
  if (IsPrecious(x)) return x;
  return List(kQuote, x);
}

static int QuoteList(int x) {
  if (!x) return x;
  return Cons(Quote(Car(x)), QuoteList(Cdr(x)));
}

static int GetAtom(const char *s) {
  int x, y;
  ax = y = TERM;
  x = *s++ & 255;
  if (*s) y = GetAtom(s);
  return Intern(x, y);
}

static int Gensym(void) {
  char B[16], t;
  static unsigned g;
  unsigned a, b, x, n;
  n = 0;
  x = g++;
  B[n++] = L'G';
  do B[n++] = L'0' + (x & 7);
  while ((x >>= 3));
  B[n] = 0;
  for (a = 1, b = n - 1; a < b; ++a, --b) {
    t = B[a];
    B[a] = B[b];
    B[b] = t;
  }
  return GetAtom(B);
}

static nosideeffect bool Member(int v, int x) {
  while (x) {
    if (x > 0) return v == x;
    if (v == Car(x)) return true;
    x = Cdr(x);
  }
  return false;
}

static int GetBindings(int x, int a) {
  int r, b;
  for (r = 0; x < 0; x = Cdr(x)) {
    if ((b = Assoc(Car(x), a))) {
      r = Cons(b, r);
    } else {
      Error("could not find dependency %S in %p", Car(x), a);
    }
  }
  return r;
}

static int Lambda(int e, int a, dword p1, dword p2) {
  int u;
  if (p1) a = Alist(LO(p1), HI(p1), a);
  if (p2) a = Alist(LO(p2), HI(p2), a);
  if (DEBUG_CLOSURE || logc) {
    u = FindFreeVariables(e, 0, 0);
    a = GetBindings(u, a);
  }
  return Enclose(e, a);
}

static int Function(int e, int a, dword p1, dword p2) {
  int u;
  if (e < 0 && Car(e) == kLambda) e = Lambda(e, a, p1, p2);
  if (e >= 0 || Car(e) != kClosure) Error("not a closure");
  a = Cddr(e);
  e = Cadr(e);
  u = FindFreeVariables(e, 0, 0);
  a = GetBindings(u, a);
  return Enclose(e, a);
}

static int Simplify(int e, int a) {
  return Function(e, a, 0, 0);
}

static int PrintFn(int x) {
  int y;
  DCHECK_LT(x, TERM);
  y = Car(x);
  while ((x = Cdr(x))) {
    if (!quiet) {
      Print(1, y);
      PrintSpace(1);
    }
    y = Car(x);
  }
  if (!quiet) {
    Print(1, y);
    PrintNewline(1);
  }
  return y;
}

static int PprintFn(int x) {
  int y, n;
  DCHECK_LT(x, TERM);
  n = 0;
  y = Car(x);
  while ((x = Cdr(x))) {
    if (!quiet) {
      n += Print(1, y);
      n += PrintSpace(1);
    }
    y = Car(x);
  }
  if (!quiet) {
    PrettyPrint(1, y, n);
    PrintNewline(1);
  }
  Flush(1);
  return y;
}

static relegated struct T DispatchRetImpossible(dword ea, dword tm, dword r) {
  Fprintf(2, "ERROR: \e[7;31mIMPOSSIBLE RETURN\e[0m NO %d%n");
  Raise(LO(ea));
}

static relegated struct T DispatchTailImpossible(dword ea, dword tm, dword r,
                                                 dword p1, dword p2) {
  Fprintf(2, "ERROR: \e[7;31mIMPOSSIBLE TAIL\e[0m NO %d%n");
  Raise(LO(ea));
}

static struct T DispatchRet(dword ea, dword tm, dword r) {
  return (struct T){LO(ea)};
}

static struct T DispatchLeave(dword ea, dword tm, dword r) {
  Pop();
  return (struct T){LO(ea)};
}

static struct T DispatchLeaveGc(dword ea, dword tm, dword r) {
  int A, e;
  e = LO(ea);
  A = GetFrameCx();
  if (e < A && cx < A && UNLIKELY(!ShouldIgnoreGarbage(A))) {
    e = MarkSweep(A, e);
  }
  Pop();
  return (struct T){e};
}

static struct T DispatchLeaveTmcGc(dword ea, dword tm, dword r) {
  int A, e;
  A = GetFrameCx();
  e = Reverse(LO(tm), LO(ea));
  if (!ShouldIgnoreGarbage(A)) {
    e = MarkSweep(A, e);
  }
  Pop();
  return (struct T){e};
}

RetFn *const kRet[] = {
    DispatchRet,            //
    DispatchRetImpossible,  //
    DispatchRetImpossible,  //
    DispatchRetImpossible,  //
    DispatchLeave,          //
    DispatchLeaveGc,        //
    DispatchRetImpossible,  //
    DispatchLeaveTmcGc,     //
};

struct T DispatchTail(dword ea, dword tm, dword r, dword p1, dword p2) {
  return DISPATCH(ea, tm, r, p1, p2);
}

struct T DispatchTailGc(dword ea, dword tm, dword r, dword p1, dword p2) {
  int A;
  struct Gc *G;
  A = GetFrameCx();
  if (cx < A && UNLIKELY(!ShouldIgnoreGarbage(A))) {
    if (ShouldPanicAboutGarbage(A)) {
      if (!ShouldAbort(A)) {
        ea = MAKE(LO(ea), ReconstructAlist(HI(ea)));
      } else {
        Raise(kCycle);
      }
    }
    G = NewGc(A);
    Mark(G, LO(ea));
    Mark(G, HI(ea));
    Mark(G, HI(p1));
    Mark(G, HI(p2));
    Census(G);
    p1 = MAKE(LO(p1), Relocate(G, HI(p1)));
    p2 = MAKE(LO(p2), Relocate(G, HI(p2)));
    ea = MAKE(Relocate(G, LO(ea)), Relocate(G, HI(ea)));
    Sweep(G);
  }
  return DISPATCH(ea, tm, r, p1, p2);
}

struct T DispatchTailTmcGc(dword ea, dword tm, dword r, dword p1, dword p2) {
  int A;
  struct Gc *G;
  A = GetFrameCx();
  if (UNLIKELY(!ShouldIgnoreGarbage(A))) {
    if (ShouldPanicAboutGarbage(A)) {
      if (!ShouldAbort(A)) {
        ea = MAKE(LO(ea), ReconstructAlist(HI(ea)));
      } else {
        Raise(kCycle);
      }
    }
    G = NewGc(A);
    Mark(G, LO(tm));
    Mark(G, LO(ea));
    Mark(G, HI(ea));
    Mark(G, HI(p1));
    Mark(G, HI(p2));
    Census(G);
    p1 = MAKE(LO(p1), Relocate(G, HI(p1)));
    p2 = MAKE(LO(p2), Relocate(G, HI(p2)));
    ea = MAKE(Relocate(G, LO(ea)), Relocate(G, HI(ea)));
    tm = MAKE(Relocate(G, LO(tm)), Relocate(G, HI(tm)));
    Sweep(G);
  }
  return DISPATCH(ea, tm, r, p1, p2);
}

struct T DispatchNil(dword ea, dword tm, dword r, dword p1, dword p2, dword d) {
  return Ret(0, tm, r);  // 𝑥 ⟹ ⊥
}

struct T DispatchTrue(dword ea, dword tm, dword r, dword p1, dword p2,
                      dword d) {
  return Ret(1, tm, r);  // 𝑥 ⟹ ⊤
}

struct T DispatchPrecious(dword ea, dword tm, dword r, dword p1, dword p2,
                          dword d) {
  return Ret(ea, tm, r);  // 𝑘 ⟹ 𝑘
}

struct T DispatchIdentity(dword ea, dword tm, dword r, dword p1, dword p2,
                          dword d) {
  return Ret(ea, tm, r);  // e.g. (⅄ (λ 𝑥 𝑦) 𝑎) ⟹ (⅄ (λ 𝑥 𝑦) 𝑎)
}

struct T DispatchShortcut(dword ea, dword tm, dword r, dword p1, dword p2,
                          dword d) {
  return Ret(MAKE(HI(d), 0), tm, r);
}

struct T DispatchLookup(dword ea, dword tm, dword r, dword p1, dword p2,
                        dword d) {
  int e, a, kv;
  e = LO(ea);
  a = HI(ea);
  DCHECK(!IsPrecious(e));
  DCHECK_GT(e, 0);
  DCHECK_LE(a, 0);
  if (LO(p1) == LO(ea)) return Ret(MAKE(HI(p1), 0), tm, r);
  if (LO(p2) == LO(ea)) return Ret(MAKE(HI(p2), 0), tm, r);
  if ((kv = Assoc(e, a))) {
    return Ret(MAKE(Cdr(kv), 0), tm, r);  // (eval 𝑘 (…(𝑘 𝑣)…)) ⟹ 𝑣
  } else {
    Error("crash variable %S%n", e);
  }
}

struct T DispatchQuote(dword ea, dword tm, dword r, dword p1, dword p2,
                       dword d) {
  return Ret(MAKE(HI(d), 0), tm, r);  // (Ω 𝑥) ⟹ 𝑥
}

struct T DispatchAtom(dword ea, dword tm, dword r, dword p1, dword p2,
                      dword d) {
  int x = FasterRecurse(HI(d), HI(ea), p1, p2);
  return Ret(MAKE(x >= 0, 0), tm, r);  // (α (𝑥 . 𝑦)) ⟹ ⊥, (α 𝑘) ⟹ ⊤
}

struct T DispatchCar(dword ea, dword tm, dword r, dword p1, dword p2, dword d) {
  int x = FasterRecurse(HI(d), HI(ea), p1, p2);
  return Ret(MAKE(Head(x), 0), tm, r);  // (⍅ (𝑥 . 𝑦)) ⟹ 𝑥
}

struct T DispatchCdr(dword ea, dword tm, dword r, dword p1, dword p2, dword d) {
  int x = FasterRecurse(HI(d), HI(ea), p1, p2);
  return Ret(MAKE(Tail(x), 0), tm, r);  // (⍆ (𝑥 . 𝑦)) ⟹ 𝑦
}

struct T DispatchEq(dword ea, dword tm, dword r, dword p1, dword p2, dword d) {
  int x = FasterRecurse(ARG1(LO(ea)), HI(ea), p1, p2);
  int y = FasterRecurse(HI(d), HI(ea), p1, p2);
  return Ret(MAKE(x == y, 0), tm, r);  // (≡ 𝑥 𝑥) ⟹ ⊤, (≡ 𝑥 𝑦) ⟹ ⊥
}

struct T DispatchCmp(dword ea, dword tm, dword r, dword p1, dword p2, dword d) {
  int x = FasterRecurse(ARG1(LO(ea)), HI(ea), p1, p2);
  int y = FasterRecurse(HI(d), HI(ea), p1, p2);
  return Ret(MAKE(Cmp(x, y), 0), tm, r);  // (≷ 𝑥 𝑦) ⟹ (⊥) | ⊥ | ⊤
}

struct T DispatchOrder(dword ea, dword tm, dword r, dword p1, dword p2,
                       dword d) {
  int x = FasterRecurse(ARG1(LO(ea)), HI(ea), p1, p2);
  int y = FasterRecurse(HI(d), HI(ea), p1, p2);
  return Ret(MAKE(Order(x, y), 0), tm, r);  // (⊙ 𝑥 𝑦) ⟹ (⊥) | ⊥ | ⊤
}

struct T DispatchCons(dword ea, dword tm, dword r, dword p1, dword p2,
                      dword d) {
  int x;
  if (cx < cHeap) cHeap = cx;
  x = Car(Cdr(LO(ea)));
  x = FasterRecurse(x, HI(ea), p1, p2);
  if (!HI(d)) return Ret(MAKE(Cons(x, 0), 0), tm, r);
  if (~r & NEED_POP) {
    r |= NEED_POP;
    Push(LO(ea));
  }
  r |= NEED_GC | NEED_TMC;
  tm = MAKE(Cons(x, LO(tm)), 0);
  return TailCall(MAKE(HI(d), HI(ea)), tm, r, p1, p2);  // (ℶ x 𝑦) ↩ (tm 𝑥′ . 𝑦)
}

struct T DispatchLambda(dword ea, dword tm, dword r, dword p1, dword p2,
                        dword d) {
  // (eval (𝑘 𝑥 𝑦) 𝑎) ⟹ (⅄ (𝑘 𝑥 𝑦) . 𝑎)
  SetFrame(r, LO(ea));
  r |= NEED_GC;
  return Ret(MAKE(Lambda(LO(ea), HI(ea), p1, p2), 0), tm, r);
}

struct T DispatchCond(dword ea, dword tm, dword r, dword p1, dword p2,
                      dword d) {
  int y, z, c = HI(d);
  if (r & NEED_POP) {
    Repush(LO(ea));
  }
  do {
    if (!Cdr(c) && !Cdr(Car(c))) {
      // (ζ …(𝑝)) ↩ 𝑝
      return TailCall(MAKE(Car(Car(c)), HI(ea)), tm, r, p1, p2);
    }
    if (~r & NEED_POP) {
      r |= NEED_POP;
      Push(LO(ea));
    }
    if ((y = FasterRecurse(Car(Car(c)), HI(ea), p1, p2))) {
      if ((z = Cdr(Car(c))) < 0) {
        // (ζ …(𝑝 𝑏)…) ↩ 𝑏 if 𝑝′
        return TailCall(MAKE(Car(z), HI(ea)), tm, r, p1, p2);
      } else {
        // (ζ …(𝑝)…) ⟹ 𝑝′ if 𝑝′
        return Ret(MAKE(y, 0), tm, r);
      }
    }
  } while ((c = Cdr(c)));
  return Ret(MAKE(c, 0), tm, r);  // (ζ) ⟹ ⊥
}

struct T DispatchIf(dword ea, dword tm, dword r, dword p1, dword p2, dword d) {
  return TailCall(
      MAKE(Get(LO(ea) + 4 + !FasterRecurse(Get(LO(ea) + 3), HI(ea), p1, p2)),
           HI(ea)),
      tm, r, p1, p2);
}

struct T DispatchPrinc(dword ea, dword tm, dword r, dword p1, dword p2,
                       dword d) {
  bool b;
  int e;
  e = LO(ea);
  SetFrame(r, e);
  b = literally;
  literally = true;
  e = recurse(MAKE(Head(Tail(e)), HI(ea)), p1, p2);
  Print(1, e);
  literally = b;
  return Ret(MAKE(e, 0), tm, r);
}

struct T DispatchFlush(dword ea, dword tm, dword r, dword p1, dword p2,
                       dword d) {
  SetFrame(r, LO(ea));
  Flush(1);
  return Ret(MAKE(kIgnore0, 0), tm, r);
}

struct T DispatchPrint(dword ea, dword tm, dword r, dword p1, dword p2,
                       dword d) {
  int a, f, x, A;
  f = LO(ea);
  a = HI(ea);
  SetFrame(r, f);
  for (;;) {
    f = Cdr(f);
    if (!Cdr(f)) {
      if (quiet) {
        return TailCall(MAKE(Car(f), a), tm, r, p1, p2);
      } else {
        x = recurse(MAKE(Car(f), a), p1, p2);
        Print(1, x);
        PrintNewline(1);
        return Ret(MAKE(x, 0), tm, r);
      }
    }
    if (!quiet) {
      A = cx;
      x = recurse(MAKE(Car(f), a), p1, p2);
      Print(1, x);
      PrintSpace(1);
      MarkSweep(A, 0);
    }
  }
}

struct T DispatchPprint(dword ea, dword tm, dword r, dword p1, dword p2,
                        dword d) {
  int a, f, x, n, A;
  f = LO(ea);
  a = HI(ea);
  SetFrame(r, f);
  for (n = 0;;) {
    f = Cdr(f);
    if (!Cdr(f)) {
      if (quiet) {
        return TailCall(MAKE(Car(f), a), tm, r, p1, p2);
      } else {
        x = recurse(MAKE(Car(f), a), p1, p2);
        PrettyPrint(1, x, n);
        PrintNewline(1);
        return Ret(MAKE(x, 0), tm, r);
      }
    }
    if (!quiet) {
      A = cx;
      x = recurse(MAKE(Car(f), a), p1, p2);
      n += Print(1, x);
      n += PrintSpace(1);
      MarkSweep(A, 0);
    }
  }
}

struct T DispatchPrintheap(dword ea, dword tm, dword r, dword p1, dword p2,
                           dword d) {
  int x, A;
  SetFrame(r, LO(ea));
  if (Cdr(LO(ea))) {
    A = cx;
    x = recurse(MAKE(Cadr(LO(ea)), HI(ea)), p1, p2);
    PrintHeap(A);
  } else {
    PrintHeap(0);
    x = 0;
  }
  return Ret(x, tm, r);
}

struct T DispatchGc(dword ea, dword tm, dword r, dword p1, dword p2, dword d) {
  int A, e;
  SetFrame(r, LO(ea));
  A = GetFrameCx();
  e = recurse(MAKE(HI(d), HI(ea)), p1, p2);
  if (e < A && cx < A && !ShouldIgnoreGarbage(A)) {
    e = MarkSweep(A, e);
  }
  return Ret(MAKE(e, 0), tm, r);
}

struct T DispatchProgn(dword ea, dword tm, dword r, dword p1, dword p2,
                       dword d) {
  int A, y, x = HI(d);
  for (;;) {
    y = Car(x);
    x = Cdr(x);
    if (!x) {
      if (r & NEED_POP) {
        Repush(y);
      }
      return TailCall(MAKE(y, HI(ea)), tm, r, p1, p2);  // (progn ⋯ 𝑥) ↩ 𝑥
    }
    A = cx;
    recurse(MAKE(y, HI(ea)), p1, p2);  // evaluate for effect
    MarkSweep(A, 0);
  }
}

struct T DispatchGensym(dword ea, dword tm, dword r, dword p1, dword p2,
                        dword d) {
  return Ret(MAKE(Gensym(), 0), tm, r);
}

struct T DispatchQuiet(dword ea, dword tm, dword r, dword p1, dword p2,
                       dword d) {
  SAVE(quiet, true);
  ea = MAKE(recurse(MAKE(Cadr(LO(ea)), HI(ea)), p1, p2), 0);
  RESTORE(quiet);
  return Ret(ea, tm, r);
}

struct T DispatchTrace(dword ea, dword tm, dword r, dword p1, dword p2,
                       dword d) {
  START_TRACE;
  ea = MAKE(recurse(MAKE(Cadr(LO(ea)), HI(ea)), p1, p2), 0);
  END_TRACE;
  return Ret(ea, tm, r);
}

struct T DispatchFtrace(dword ea, dword tm, dword r, dword p1, dword p2,
                        dword d) {
  ftrace_install();
  ftrace_enabled(+1);
  ea = MAKE(recurse(MAKE(Cadr(LO(ea)), HI(ea)), p1, p2), 0);
  ftrace_enabled(-1);
  return Ret(ea, tm, r);
}

struct T DispatchBeta(dword ea, dword tm, dword r, dword p1, dword p2,
                      dword d) {
  SetFrame(r, LO(ea));
  r |= NEED_GC;
  return Ret(MAKE(Simplify(recurse(MAKE(HI(d), HI(ea)), p1, p2), HI(ea)), 0),
             tm, r);
}

struct T DispatchFunction(dword ea, dword tm, dword r, dword p1, dword p2,
                          dword d) {
  // (eval (𝑓 (𝑘 𝑥 𝑦)) 𝑎) ⟹ (⅄ (𝑘 𝑥 𝑦) . prune 𝑎 wrt (𝑘 𝑥 𝑦))
  SetFrame(r, LO(ea));
  r |= NEED_GC;
  return Ret(
      MAKE(Function(recurse(MAKE(HI(d), HI(ea)), p1, p2), HI(ea), p1, p2), 0),
      tm, r);
}

struct T DispatchIgnore0(dword ea, dword tm, dword r, dword p1, dword p2,
                         dword d) {
  return Ret(MAKE(kIgnore0, 0), tm, r);
}

struct T DispatchIgnore1(dword ea, dword tm, dword r, dword p1, dword p2,
                         dword d) {
  int x = recurse(MAKE(Car(Cdr(LO(ea))), HI(ea)), p1, p2);
  return Ret(MAKE(List(kIgnore, x), 0), tm, r);
}

struct T DispatchExpand(dword ea, dword tm, dword r, dword p1, dword p2,
                        dword d) {
  int x;
  SetFrame(r, LO(ea));
  r |= NEED_GC;
  x = HI(d);
  x = recurse(MAKE(x, HI(ea)), p1, p2);
  return Ret(MAKE(expand(x, HI(ea)), 0), tm, r);
}

static int GrabArgs(int x, int a, dword p1, dword p2) {
  if (x >= 0) return x;
  return Cons(recurse(MAKE(Car(x), a), p1, p2), GrabArgs(Cdr(x), a, p1, p2));
}

struct T DispatchError(dword ea, dword tm, dword r, dword p1, dword p2,
                       dword d) {
  int e, x;
  e = LO(ea);
  SetFrame(r, e);
  r |= NEED_GC;
  x = GrabArgs(Cdr(e), HI(ea), p1, p2);
  Raise(Cons(Car(e), x));
}

struct T DispatchExit(dword ea, dword tm, dword r, dword p1, dword p2,
                      dword d) {
  longjmp(exiter, 1);
}

struct T DispatchRead(dword ea, dword tm, dword r, dword p1, dword p2,
                      dword d) {
  return Ret(MAKE(Read(0), 0), tm, r);
}

struct T DispatchFuncall(dword ea, dword tm, dword r, dword p1, dword p2,
                         dword d) {
  int a, b, e, f, t, u, y, p, z;
  e = LO(ea);
  a = HI(ea);
  DCHECK_LT(e, 0);
  SetFrame(r, e);
  r |= NEED_GC;
  f = Car(e);
  z = Cdr(e);
  y = HI(d) ? HI(d) : FasterRecurse(f, a, p1, p2);
Delegate:
  if (y < 0) {
    t = Car(y);
    if (t == kClosure) {
      // (eval ((⅄ (λ 𝑥 𝑦) 𝑏) 𝑧) 𝑎) ↩ (eval ((λ 𝑥 𝑦) 𝑧) 𝑏)
      y = Cdr(y);  // ((λ 𝑥 𝑦) 𝑏)
      u = Cdr(y);  //          𝑏
      y = Car(y);  //  (λ 𝑥 𝑦)
      t = Car(y);  //   λ
    } else {
      u = a;
    }
    p = Car(Cdr(y));
    b = Car(Cdr(Cdr(y)));
    if (t == kLambda) {
      if (!(p > 0 && b < 0 && Cdr(b) == p)) {
        struct Binding bz = bind_(p, z, a, u, p1, p2);
        return TailCall(MAKE(b, bz.u), tm, r, bz.p1, 0);
      } else {
        // fast path ((lambda 𝑣 (𝑦 . 𝑣)) 𝑧) ↩ (𝑦′ 𝑧)
        y = recurse(MAKE(Car(b), u), 0, 0);
        goto Delegate;
      }
    } else if (t == kMacro) {
      // (eval ((ψ 𝑥 𝑦) 𝑥) 𝑎) ↩ (eval (eval 𝑦 ((𝑥ᵢ 𝑥ᵢ) 𝑎)) 𝑎)
      return TailCall(MAKE(eval(b, pairlis(p, Exlis(z, a), u)), a), tm, r, 0,
                      0);
    }
  } else if (y > 1 && y != f && IsPrecious(y)) {
    // unplanned builtin calls
    // e.g. ((cond (p car) (cdr)) x)
    return TailCall(MAKE(Cons(y, z), a), tm, r, p1, p2);
  }
  React(e, y, kFunction);
}

struct T DispatchCall1(dword ea, dword tm, dword r, dword p1, dword p2,
                       dword d) {
  int b, e, u, y, p;
  e = LO(ea);
  DCHECK_LT(e, 0);
  SetFrame(r, e);
  y = HI(d);
  // (eval ((⅄ (λ 𝑥 𝑦) 𝑏) 𝑧) 𝑎) ↩ (eval ((λ 𝑥 𝑦) 𝑧) 𝑏)
  y = Cdr(y);  // ((λ 𝑥 𝑦) 𝑏)
  u = Cdr(y);  //          𝑏
  y = Car(y);  //  (λ 𝑥 𝑦)
  p = Car(Cdr(y));
  b = Car(Cdr(Cdr(y)));
  return TailCall(MAKE(b, u), tm, r,
                  MAKE(Car(p), FasterRecurse(Car(Cdr(LO(ea))), HI(ea), p1, p2)),
                  0);
}

struct T DispatchCall2(dword ea, dword tm, dword r, dword p1, dword p2,
                       dword d) {
  int b, e, u, y, p;
  e = LO(ea);
  DCHECK_LT(e, 0);
  SetFrame(r, e);
  y = HI(d);
  // (eval ((⅄ (λ 𝑥 𝑦) 𝑏) 𝑧) 𝑎) ↩ (eval ((λ 𝑥 𝑦) 𝑧) 𝑏)
  y = Cdr(y);  // ((λ 𝑥 𝑦) 𝑏)
  u = Cdr(y);  //          𝑏
  y = Car(y);  //  (λ 𝑥 𝑦)
  p = Car(Cdr(y));
  b = Car(Cdr(Cdr(y)));
  return TailCall(
      MAKE(b, u), tm, r,
      MAKE(Car(p), FasterRecurse(Car(Cdr(LO(ea))), HI(ea), p1, p2)),
      MAKE(Car(Cdr(p)), FasterRecurse(Car(Cdr(Cdr(LO(ea)))), HI(ea), p1, p2)));
}

struct T DispatchLet1(dword ea, dword tm, dword r, dword p1, dword p2,
                      dword d) {
  // Fast path DispatchFuncall() for ((λ (𝑣) 𝑦) 𝑧₀) expressions
  // HI(d) contains ((𝑣) 𝑦)
  if (UNLIKELY(trace))
    Fprintf(2, "%J╟─%s[%p @ %d] δ %'Rns%n", "DispatchLet1", LO(ea), LO(ea));
  int v = Car(Car(HI(d)));
  int y = Car(Cdr(HI(d)));
  int z = FasterRecurse(Car(Cdr(LO(ea))), HI(ea), p1, p2);
  int a = HI(ea);
  if (!LO(p1) || LO(p1) == v) {
    p1 = MAKE(v, z);
  } else if (!LO(p2) || LO(p2) == v) {
    p2 = MAKE(v, z);
  } else {
    a = Alist(LO(p2), HI(p2), a);
    p2 = p1;
    p1 = MAKE(v, z);
  }
  return TailCall(MAKE(y, a), tm, r, p1, p2);
}

int Eval(int e, int a) {
  return ((ForceIntTailDispatchFn *)GetDispatchFn(e))(MAKE(e, a), 0, 0, 0, 0,
                                                      GetShadow(e));
}

static void ResetStats(void) {
  cHeap = cx;
  cGets = 0;
  cSets = 0;
}

static void PrintStats(long usec) {
  Fprintf(2,
          ";; heap    %'16ld  nsec    %'16ld%n"
          ";; gets    %'16ld  sets    %'16ld%n"
          ";; atom    %'16ld  frez    %'16ld%n",
          -cHeap - -cFrost, usec, cGets, cSets, cAtoms, -cFrost);
}

static wontreturn int Exit(void) {
  exit(0 <= fails && fails <= 255 ? fails : 255);
}

static wontreturn void PrintUsage(void) {
  PUTS(!!fails + 1, "Usage: ");
  PUTS(!!fails + 1, program_invocation_name);
  PUTS(!!fails + 1, " [-MNSacdfgqstz?h] <input.lisp >errput.lisp\n\
  -d  dump global defines, on success\n\
  -s  print statistics upon each eval\n\
  -z  uses alternative unicode glyphs\n\
  -f  print log of all function calls\n\
  -S  avoid pretty printing most case\n\
  -c  dont conceal transitive closure\n\
  -a  log name bindings in the traces\n\
  -t  hyper verbose jump table traces\n\
  -M  enable tracing of macro expands\n\
  -N  disable define name substitutes\n\
  -g  will log garbage collector pass\n\
  -q  makes (print) and (pprint) noop\n\
");
  Exit();
}

int Plinko(int argc, char *argv[]) {
  int S, x;
  bool trace;
  uint64_t t1, t2;
  tick = kStartTsc;
#ifndef NDEBUG
  ShowCrashReports();
#endif
  signal(SIGPIPE, SIG_DFL);

  depth = -1;
  trace = false;
  while ((x = getopt(argc, argv, "MNSacdfgqstz?h")) != -1) {
    switch (x) {
      CASE(L'd', dump = true);
      CASE(L's', stats = true);
      CASE(L'z', symbolism = true);
      CASE(L'f', ftrace = true);
      CASE(L'S', simpler = true);
      CASE(L'c', logc = true);
      CASE(L'a', loga = true);
      CASE(L't', trace = true);
      CASE(L'g', gtrace = true);
      CASE(L'q', quiet = true);
      CASE(L'M', mtrace = true);
      CASE(L'N', noname = true);
      CASE(L'?', PrintUsage());
      CASE(L'h', PrintUsage());
      default:
        ++fails;
        PrintUsage();
    }
  }

  if (sys_arch_prctl(ARCH_SET_FS, 0x200000000000) == -1 ||
      sys_arch_prctl(ARCH_SET_GS, (intptr_t)DispatchPlan) == -1) {
    fputs("error: ", stderr);
    fputs(strerror(errno), stderr);
    fputs("\nyour operating system doesn't allow you change both "
          "the %fs and %gs registers\nin your processor. that's a shame, "
          "since they're crucial for performance.\n",
          stderr);
    exit(1);
  }

  if (mmap((void *)0x200000000000,
           ROUNDUP((TERM + 1) * sizeof(g_mem[0]), FRAMESIZE),
           PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED, -1,
           0) == MAP_FAILED ||
      mmap((void *)(0x200000000000 +
                    (BANE & (BANE | MASK(BANE))) * sizeof(g_mem[0])),
           (BANE & (BANE | MASK(BANE))) * sizeof(g_mem[0]),
           PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED, -1,
           0) == MAP_FAILED ||
      mmap((void *)0x400000000000,
           ROUNDUP((TERM + 1) * sizeof(g_mem[0]), FRAMESIZE),
           PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED, -1,
           0) == MAP_FAILED ||
      mmap((void *)(0x400000000000 +
                    (BANE & (BANE | MASK(BANE))) * sizeof(g_mem[0])),
           (BANE & (BANE | MASK(BANE))) * sizeof(g_mem[0]),
           PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED, -1,
           0) == MAP_FAILED) {
    fputs("error: ", stderr);
    fputs(strerror(errno), stderr);
    fputs("\nyour operating system doesn't allow you to allocate\n"
          "outrageous amounts of overcommit memory, which is a shame, since\n"
          "the pml4t feature in your processor was intended to give you that\n"
          "power since it's crucial for sparse data applications and lisp.\n"
          "for instance, the way racket works around this problem is by\n"
          "triggering thousands of segmentation faults as part of normal\n"
          "operation\n",
          stderr);
    exit(1);
  }

  g_mem = (void *)0x200000000000;

  inputs = argv + optind;
  if (*inputs) {
    close(0);
    DCHECK_NE(-1, open(*inputs++, O_RDONLY));
  }

  eval = Eval;
  bind_ = Bind;
  evlis = Evlis;
  expand = Expand;
  recurse = Recurse;
  pairlis = Pairlis;
  kTail[0] = DispatchTail;
  kTail[1] = DispatchTailImpossible;
  kTail[2] = DispatchTailImpossible;
  kTail[3] = DispatchTailImpossible;
  kTail[4] = DispatchTail;
  kTail[5] = DispatchTailGc;
  kTail[6] = DispatchTailImpossible;
  kTail[7] = DispatchTailTmcGc;
  if (trace) EnableTracing();

  cx = -1;
  cFrost = cx;
  Setup();
  cFrost = cx;

  if (!setjmp(exiter)) {
    for (;;) {
      S = sp;
      DCHECK_EQ(0, S);
      DCHECK_EQ(cx, cFrost);
      if (!(x = setjmp(crash))) {
        x = Read(0);
        x = expand(x, globals);
        if (stats) ResetStats();
        if (x < 0 && Car(x) == kDefine) {
          globals = Define(x, globals);
          cFrost = cx;
        } else {
          t1 = rdtsc();
          x = eval(x, globals);
          if (x < 0 && Car(x) == kIgnore) {
            MarkSweep(cFrost, 0);
          } else {
            Fprintf(1, "%p%n", x);
            MarkSweep(cFrost, 0);
            if (stats) {
              t2 = rdtsc();
              PrintStats(ClocksToNanos(t1, t2));
            }
          }
        }
      } else {
        x = ~x;
        ++fails;
        eval = Eval;
        expand = Expand;
        Fprintf(2, "?%p%s%n", x, cx == BANE ? " [HEAP OVERFLOW]" : "");
        Backtrace(S);
        Exit();
        Unwind(S);
        MarkSweep(cFrost, 0);
      }
    }
  }

#if HISTO_ASSOC
  PrintHistogram(2, "COMPARES PER ASSOC", g_assoc_histogram,
                 ARRAYLEN(g_assoc_histogram));
#endif
#if HISTO_GARBAGE
  PrintHistogram(2, "GC MARKS PER COLLECTION", g_gc_marks_histogram,
                 ARRAYLEN(g_gc_marks_histogram));
  PrintHistogram(2, "GC DISCARDS PER COLLECTION", g_gc_discards_histogram,
                 ARRAYLEN(g_gc_discards_histogram));
  PrintHistogram(2, "GC MARKS / DISCARDS FOR DENSE COLLECTIONS",
                 g_gc_dense_histogram, ARRAYLEN(g_gc_dense_histogram));
  PrintHistogram(2, "GC DISCARDS / MARKS FOR SPARSE COLLECTIONS",
                 g_gc_sparse_histogram, ARRAYLEN(g_gc_sparse_histogram));
  PrintHistogram(2, "GC LOP", g_gc_lop_histogram, ARRAYLEN(g_gc_lop_histogram));
#endif

  if (dump && !fails) {
    DumpDefines(kDefine, globals, Reverse(ordglob, 0));
  }
  Exit();
}
