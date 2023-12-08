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
#include "libc/str/str.h"
#include "tool/plinko/lib/cons.h"
#include "tool/plinko/lib/plinko.h"

static void SetInput(const char *s) {
  bp[0] = 0;
  dx = L' ';
  strcpy(stpcpy(g_buffer[0], s), " ");
}

static void Programme(int i, DispatchFn *f, int x) {
  SetShadow(i, MAKE(EncodeDispatchFn(f), x));
}

static void Program(int i, DispatchFn *f) {
  Programme(i, f, 0);
}

static void ProgramPrecious(int i) {
  Program(i, DispatchPrecious);
}

static void ProgramLookup(int i) {
  Program(i, DispatchLookup);
}

static void ProgramIgnore0(int i) {
  Program(i, DispatchIgnore0);
}

static void ProgramPlan(int i) {
  Program(i, DispatchPlan);
}

void Setup(void) {
  int i;
  char buf[4] = "(A)";
  SetShadow(-1, DF(DispatchPlan));
  SetShadow(0, DF(DispatchPrecious));
  SetShadow(+1, DF(DispatchPrecious));
  PROG(ProgramPrecious, kEq, "EQ");
  PROG(ProgramPrecious, kGc, "GC");
  PROG(ProgramPrecious, kCmp, "CMP");
  PROG(ProgramPrecious, kCar, "CAR");
  PROG(ProgramPrecious, kCdr, "CDR");
  PROG(ProgramPrecious, kBeta, "BETA");
  PROG(ProgramPrecious, kAtom, "ATOM");
  PROG(ProgramPrecious, kCond, "COND");
  PROG(ProgramPrecious, kCons, "CONS");
  PROG(ProgramPrecious, kRead, "READ");
  PROG(ProgramPrecious, kDump, "DUMP");
  PROG(ProgramPrecious, kExit, "EXIT");
  PROG(ProgramPrecious, kFork, "FORK");
  PROG(ProgramPrecious, kQuote, "QUOTE");
  PROG(ProgramPrecious, kProgn, "PROGN");
  PROG(ProgramPrecious, kMacro, "MACRO");
  PROG(ProgramPrecious, kQuiet, "QUIET");
  PROG(ProgramPrecious, kError, "ERROR");
  PROG(ProgramPrecious, kTrace, "TRACE");
  PROG(ProgramPrecious, kPrint, "PRINT");
  PROG(ProgramPrecious, kPrinc, "PRINC");
  PROG(ProgramPrecious, kFlush, "FLUSH");
  PROG(ProgramPrecious, kOrder, "ORDER");
  PROG(ProgramPrecious, kGensym, "GENSYM");
  PROG(ProgramPrecious, kPprint, "PPRINT");
  PROG(ProgramPrecious, kIgnore, "IGNORE");
  PROG(ProgramPrecious, kMtrace, "MTRACE");
  PROG(ProgramPrecious, kFtrace, "FTRACE");
  PROG(ProgramPrecious, kGtrace, "GTRACE");
  PROG(ProgramPrecious, kLambda, "LAMBDA");
  PROG(ProgramPrecious, kDefine, "DEFINE");
  PROG(ProgramPrecious, kExpand, "EXPAND");
  PROG(ProgramPrecious, kClosure, "CLOSURE");
  PROG(ProgramPrecious, kPartial, "PARTIAL");
  PROG(ProgramPrecious, kFunction, "FUNCTION");
  PROG(ProgramPrecious, kIntegrate, "INTEGRATE");
  PROG(ProgramPrecious, kPrintheap, "PRINTHEAP");
  PROG(ProgramPrecious, kImpossible, "IMPOSSIBLE");
  PROG(ProgramLookup, kComma, "COMMA_");
  PROG(ProgramLookup, kSplice, "SPLICE_");
  PROG(ProgramLookup, kBackquote, "BACKQUOTE_");
  PROG(ProgramLookup, kString, "STRING_");
  PROG(ProgramLookup, kSquare, "SQUARE_");
  PROG(ProgramLookup, kCurly, "CURLY_");
  PROG(ProgramLookup, kDefun, "DEFUN");
  PROG(ProgramLookup, kDefmacro, "DEFMACRO");
  PROG(ProgramLookup, kAppend, "APPEND");
  PROG(ProgramLookup, kOr, "OR");
  PROG(ProgramLookup, kAnd, "AND");
  PROG(ProgramLookup, kIntersection, "INTERSECTION");
  PROG(ProgramLookup, kList, "LIST");
  PROG(ProgramLookup, kMember, "MEMBER");
  PROG(ProgramLookup, kNot, "NOT");
  PROG(ProgramLookup, kReverse, "REVERSE");
  PROG(ProgramLookup, kSqrt, "SQRT");
  PROG(ProgramLookup, kSubset, "SUBSET");
  PROG(ProgramLookup, kSuperset, "SUPERSET");
  PROG(ProgramLookup, kBecause, "BECAUSE");
  PROG(ProgramLookup, kTherefore, "THEREFORE");
  PROG(ProgramLookup, kUnion, "UNION");
  PROG(ProgramLookup, kImplies, "IMPLIES");
  PROG(ProgramLookup, kYcombinator, "YCOMBINATOR");
  PROG(ProgramLookup, kNand, "NAND");
  PROG(ProgramLookup, kNor, "NOR");
  PROG(ProgramLookup, kXor, "XOR");
  PROG(ProgramLookup, kIff, "IFF");
  PROG(ProgramLookup, kCycle, "CYCLE");
  PROG(ProgramLookup, kTrench, "𝕋ℝ𝔼ℕℂℍ");
  PROG(ProgramLookup, kUnchanged, "ⁿ/ₐ");
  PROG(ProgramIgnore0, kIgnore0, "(IGNORE)");
  for (i = 0; i < 26; ++i, ++buf[1]) {
    PROG(ProgramPlan, kConsAlphabet[i], buf);
  }
  for (buf[0] = L'A', buf[1] = 0, i = 0; i < 26; ++i, ++buf[0]) {
    if (buf[0] != 'T') {
      PROG(ProgramLookup, kAlphabet[i], buf);
    } else {
      kAlphabet[i] = 1;
    }
  }
}
