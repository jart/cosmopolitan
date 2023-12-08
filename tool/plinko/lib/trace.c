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
#include "tool/plinko/lib/trace.h"
#include "tool/plinko/lib/plinko.h"
#include "tool/plinko/lib/printf.h"

void EnableTracing(void) {
  eval = EvalTrace;
  bind_ = BindTrace;
  evlis = EvlisTrace;
  pairlis = PairlisTrace;
  recurse = RecurseTrace;
  expand = mtrace ? ExpandTrace : Expand;
  kTail[0] = DispatchTailTrace;
  kTail[4] = DispatchTailTrace;
  kTail[5] = DispatchTailGcTrace;
  kTail[6] = DispatchTailTrace;
  kTail[7] = DispatchTailTmcGcTrace;
}

static inline int ShortenExpression(int e) {
  return e;
}

static bool AlwaysTrace(int e) {
  return true;
}
static bool LimitedTrace(int e) {
  return e < 0;
}

static relegated struct T TailTracer(dword ea, dword tm, dword r, dword p1,
                                     dword p2, TailFn *f) {
  if (depth < ARRAYLEN(g_depths)) {
    Fprintf(2, "%J╠═Tail[%p @ %d] p1=%S p2=%S δ %'Rns%n",
            ShortenExpression(LO(ea)), LO(ea), LO(p1), LO(p2));
  }
  return f(ea, tm, r, p1, p2);
}

relegated struct T DispatchTailTrace(dword ea, dword tm, dword r, dword p1,
                                     dword p2) {
  return TailTracer(ea, tm, r, p1, p2, DispatchTail);
}

relegated struct T DispatchTailGcTrace(dword ea, dword tm, dword r, dword p1,
                                       dword p2) {
  return TailTracer(ea, tm, r, p1, p2, DispatchTailGc);
}

relegated struct T DispatchTailTmcGcTrace(dword ea, dword tm, dword r, dword p1,
                                          dword p2) {
  int x;
  if (depth < ARRAYLEN(g_depths)) {
    x = LO(tm);
    x = x < 0 ? x : ~x;
    Fprintf(2, "%J╟⟿Cons[%p @ %d] δ %'Rns%n", x, x);
  }
  return TailTracer(ea, tm, r, p1, p2, DispatchTailTmcGc);
}

static relegated int Trace(int e, int a, EvalFn *f, bool p(int), const char *s,
                           const unsigned short c[5]) {
  int d, r, rp, S = sp;
  DCHECK_GE(depth, -1);
  d = depth;
  depth = MAX(d, 0);
  if (depth < ARRAYLEN(g_depths)) {
    if (p(e)) {
      if (loga) {
        Fprintf(2, "%I%c%c%s[e=%S @ %d; a=%S @ %d] δ %'Rns%n", c[0], c[1], s,
                ShortenExpression(e), e, a, a);
      } else {
        Fprintf(2, "%I%c%c%s[%S @ %d] δ %'Rns%n", c[0], c[1], s,
                ShortenExpression(e), e);
      }
    }
    g_depths[depth][0] = c[2];
    g_depths[depth][1] = L' ';
    g_depths[depth][2] = L' ';
  }
  ++depth;
  r = f(e, a);
  --depth;
  if (depth < ARRAYLEN(g_depths) && p(e)) {
    rp = r != e ? r : kUnchanged;
    if (sp == S) {
      Fprintf(2, "%I%c%c%p @ %d %'Rns%n", c[3], c[4], rp, r);
    } else {
      Fprintf(2, "%I%c%c%p @ %d %'Rns [STACK SKEW S=%d vs. sp=%d]%n", c[3],
              c[4], rp, r, S, sp);
    }
  }
  depth = d;
  return r;
}

relegated int RecurseTrace(dword ea, dword p1, dword p2) {
  int r;
  const char *s = "Recurse";
  const unsigned short c[5] = u"╔═║╚═";
  if (depth < ARRAYLEN(g_depths)) {
    if (loga) {
      Fprintf(2,
              "%I%c%c%s[LO(ea)=%S @ %d; HI(ea)=%S @ %d] p1=%S p2=%S δ %'Rns%n",
              c[0], c[1], s, ShortenExpression(LO(ea)), LO(ea), HI(ea), HI(ea),
              LO(p1), LO(p2));
    } else {
      Fprintf(2, "%I%c%c%s[%S @ %d] p1=%S p2=%S δ %'Rns%n", c[0], c[1], s,
              ShortenExpression(LO(ea)), LO(ea), LO(p1), LO(p2));
    }
    g_depths[depth][0] = c[2];
    g_depths[depth][1] = L' ';
    g_depths[depth][2] = L' ';
  }
  ++depth;
  r = Recurse(ea, p1, p2);
  --depth;
  if (depth < ARRAYLEN(g_depths)) {
    if (r != LO(ea)) {
      Fprintf(2, "%I%c%c%p @ %d δ %'Rns%n", c[3], c[4], r, r);
    } else {
      Fprintf(2, "%I%c%cⁿ/ₐ δ %'Rns%n", c[3], c[4]);
    }
  }
  return r;
}

relegated int EvlisTrace(int e, int a, dword p1, dword p2) {
  int r, d;
  const char *s = "Evlis";
  const unsigned short c[5] = u"╒─┆╘─";
  DCHECK_GE(depth, -1);
  d = depth;
  depth = MAX(d, 0);
  if (depth < ARRAYLEN(g_depths)) {
    if (loga) {
      Fprintf(2, "%I%c%c%s[e=%S @ %d; a=%S @ %d] δ %'Rns%n", c[0], c[1], s,
              ShortenExpression(e), e, a, a);
    } else {
      Fprintf(2, "%I%c%c%s[%S @ %d] δ %'Rns%n", c[0], c[1], s,
              ShortenExpression(e), e);
    }
    g_depths[depth][0] = c[2];
    g_depths[depth][1] = L' ';
    g_depths[depth][2] = L' ';
  }
  ++depth;
  r = Evlis(e, a, p1, p2);
  --depth;
  if (depth < ARRAYLEN(g_depths)) {
    if (r != e) {
      Fprintf(2, "%I%c%c%p @ %d δ %'Rns%n", c[3], c[4], r, r);
    } else {
      Fprintf(2, "%I%c%cⁿ/ₐ δ %'Rns%n", c[3], c[4]);
    }
  }
  depth = d;
  return r;
}

relegated int Trace3(int x, int y, int a, PairFn *f, const char *s,
                     const unsigned short c[5]) {
  int r;
  if (depth < ARRAYLEN(g_depths)) {
    if (loga) {
      Fprintf(2, "%I%c%c%s[x=%S; y=%S; a=%S] δ %'Rns%n", c[0], c[1], s,
              ShortenExpression(x), ShortenExpression(y), a);
    } else {
      Fprintf(2, "%I%c%c%s[x=%S; y=%S] δ %'Rns%n", c[0], c[1], s,
              ShortenExpression(x), ShortenExpression(y));
    }
    g_depths[depth][0] = c[2];
    g_depths[depth][1] = L' ';
    g_depths[depth][2] = L' ';
  }
  ++depth;
  r = f(x, y, a);
  --depth;
  return r;
}

relegated struct Binding BindTrace(int x, int y, int a, int u, dword p1,
                                   dword p2) {
  struct Binding r;
  if (depth < ARRAYLEN(g_depths)) {
    if (loga) {
      Fprintf(2, "%I%c%c%s[x=%S; y=%S; a=%S; u=%S] δ %'Rns%n", L'╒', L'─',
              "Bind", ShortenExpression(x), ShortenExpression(y), a, u);
    } else {
      Fprintf(2, "%I%c%c%s[x=%S; y=%S] δ %'Rns%n", L'╒', L'─', "Bind",
              ShortenExpression(x), ShortenExpression(y));
    }
    g_depths[depth][0] = L'┋';
    g_depths[depth][1] = L' ';
    g_depths[depth][2] = L' ';
  }
  ++depth;
  r = Bind(x, y, a, u, p1, p2);
  --depth;
  return r;
}

int EvalTrace(int e, int a) {
  return Trace(e, a, Eval, AlwaysTrace, "Eval", u"╔═║╚═");
}

int ExpandTrace(int e, int a) {
  return Trace(e, a, Expand, LimitedTrace, "Expand", u"┌─│└─");
}

int PairlisTrace(int x, int y, int a) {
  return Trace3(x, y, a, Pairlis, "Pairlis", u"╒─┊╘─");
}
