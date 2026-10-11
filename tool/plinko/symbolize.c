#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include "plinko.h"

int Symbolize(int x) {
  if (literally)
    return -1;
  if (x == TERM)
    return -1;
  assert(x < TERM);
  switch (LO(Get(x))) {
    case L'A':
      if (x == kAtom)
        return L'α';
      if (x == kAnd)
        return L'∧';
      if (x == kAppend)
        return L'║';
      return -1;
    case L'B':
      if (x == kBeta)
        return L'β';
      if (x == kBecause)
        return L'∵';
      return -1;
    case L'C':
      if (x == kCar)
        return L'⍅';
      if (x == kCdr)
        return L'⍆';
      if (x == kClosure)
        return L'⅄';
      if (x == kCond)
        return L'ζ';
      if (x == kCons)
        return L'ℶ';
      if (x == kCmp)
        return L'≷';
      if (x == kCycle)
        return L'⟳';
      return -1;
    case L'D':
      if (x == kDefine)
        return L'≝';
      if (x == kDefmacro)
        return L'Ψ';
      if (x == kDefun)
        return L'Λ';
      return -1;
    case L'E':
      if (x == kEq)
        return L'≡';
      if (x == kExpand)
        return L'ə';
      return -1;
    case L'F':
      if (x == kFunction)
        return L'𝑓';
      if (x == kFork)
        return L'⋔';
      return -1;
    case L'P':
      if (x == kPartial)
        return L'∂';
      return -1;
    case L'I':
      if (x == kIff)
        return L'⟺';
      if (x == kImplies)
        return L'⟶';
      if (x == kIntegrate)
        return L'∫';
      if (x == kIntersection)
        return L'∩';
      return -1;
    case L'L':
      if (x == kLambda)
        return L'λ';
      if (x == kList)
        return L'ℒ';
      return -1;
    case L'M':
      if (x == kMacro)
        return L'ψ';
      if (x == kMember)
        return L'∊';
      return -1;
    case L'N':
      if (!x)
        return L'⊥';
      if (x == kNand)
        return L'⊼';
      if (x == kNor)
        return L'⊽';
      if (x == kNot)
        return L'¬';
      return -1;
    case L'O':
      if (x == kOr)
        return L'∨';
      if (x == kOrder)
        return L'⊙';
      return -1;
    case L'Q':
      if (x == kQuote)
        return L'Ω';
      return -1;
    case L'R':
      if (x == kReverse)
        return L'Я';
      return -1;
    case L'S':
      if (x == kSqrt)
        return L'√';
      if (x == kSubset)
        return L'⊂';
      if (x == kSuperset)
        return L'⊃';
      return -1;
    case L'T':
      if (x == 1)
        return L'⊤';
      if (x == kTherefore)
        return L'∴';
      return -1;
    case L'U':
      if (x == kUnion)
        return L'∪';
      if (x == kImpossible)
        return L'∅';
      return -1;
    case L'X':
      if (x == kXor)
        return L'⊻';
      return -1;
    case L'Y':
      if (x == kYcombinator)
        return L'𝕐';
      return -1;
    default:
      return -1;
  }
}
