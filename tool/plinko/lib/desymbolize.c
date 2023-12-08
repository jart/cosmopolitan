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
#include "tool/build/lib/case.h"
#include "tool/plinko/lib/plinko.h"

pureconst int Desymbolize(int c) {
  return -1;
  switch (c) {
    CASE(L'⊥', return 0);
    CASE(L'⊤', return 1);
    CASE(L'≡', return kEq);
    CASE(L'⍅', return kCar);
    CASE(L'⊷', return kCar);
    CASE(L'⍆', return kCdr);
    CASE(L'⊶', return kCdr);
    CASE(L'α', return kAtom);
    CASE(L'ζ', return kCond);
    CASE(L'ℶ', return kCons);
    CASE(L'β', return kBeta);
    CASE(L'ψ', return kMacro);
    CASE(L'λ', return kLambda);
    CASE(L'⅄', return kClosure);
    CASE(L'∂', return kPartial);
    CASE(L'║', return kAppend);
    CASE(L'≷', return kCmp);
    CASE(L'∧', return kAnd);
    CASE(L'∨', return kOr);
    CASE(L'⋔', return kFork);
    CASE(L'Λ', return kDefun);
    CASE(L'≝', return kDefine);
    CASE(L'ə', return kExpand);
    CASE(L'Ψ', return kDefmacro);
    CASE(L'𝑓', return kFunction);
    CASE(L'∫', return kIntegrate);
    CASE(L'∅', return kImpossible);
    CASE(L'𝕐', return kYcombinator);
    CASE(L'∩', return kIntersection);
    CASE(L'ℒ', return kList);
    CASE(L'∊', return kMember);
    CASE(L'¬', return kNot);
    CASE(L'Ω', return kQuote);
    CASE(L'Я', return kReverse);
    CASE(L'√', return kSqrt);
    CASE(L'⊂', return kSubset);
    CASE(L'⊃', return kSuperset);
    CASE(L'∵', return kBecause);
    CASE(L'∴', return kTherefore);
    CASE(L'∪', return kUnion);
    CASE(L'⟶', return kImplies);
    CASE(L'⊼', return kNand);
    CASE(L'⊽', return kNor);
    CASE(L'⊻', return kXor);
    CASE(L'⟺', return kIff);
    CASE(L'⟳', return kCycle);
    CASE(L'⊙', return kOrder);
    default:
      return -1;
  }
}
