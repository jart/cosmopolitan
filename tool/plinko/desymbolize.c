#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include "plinko.h"

#define CASE(OP, CODE) \
  case OP:             \
    CODE;              \
    break

int Desymbolize(int c) {
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
