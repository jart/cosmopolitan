#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include <wchar.h>
#include "cons.h"
#include "plinko.h"

// interns an atom exactly how the reader would for this name
static int Atom(const wchar_t *s) {
  int n, y;
  n = wcslen(s);
  ax = y = TERM;
  while (n--)
    y = Intern(s[n], y);
  return y;
}

static int Program(int x, unsigned f) {
  SetShadow(x, MAKE(f, 0));
  return x;
}

static int Precious(const wchar_t *s) {
  return Program(Atom(s), DF(DispatchPrecious));
}

static int Lookup(const wchar_t *s) {
  return Program(Atom(s), DF(DispatchLookup));
}

void Setup(void) {
  int i;
  wchar_t name[2] = {0};
  SetShadow(-1, DF(DispatchPlan));
  SetShadow(0, DF(DispatchPrecious));
  SetShadow(+1, DF(DispatchPrecious));
  kEq = Precious(L"EQ");
  kGc = Precious(L"GC");
  kCmp = Precious(L"CMP");
  kCar = Precious(L"CAR");
  kCdr = Precious(L"CDR");
  kBeta = Precious(L"BETA");
  kAtom = Precious(L"ATOM");
  kCond = Precious(L"COND");
  kCons = Precious(L"CONS");
  kRead = Precious(L"READ");
  kDump = Precious(L"DUMP");
  kExit = Precious(L"EXIT");
  kFork = Precious(L"FORK");
  kQuote = Precious(L"QUOTE");
  kProgn = Precious(L"PROGN");
  kMacro = Precious(L"MACRO");
  kQuiet = Precious(L"QUIET");
  kError = Precious(L"ERROR");
  kTrace = Precious(L"TRACE");
  kPrint = Precious(L"PRINT");
  kPrinc = Precious(L"PRINC");
  kFlush = Precious(L"FLUSH");
  kOrder = Precious(L"ORDER");
  kGensym = Precious(L"GENSYM");
  kPprint = Precious(L"PPRINT");
  kIgnore = Precious(L"IGNORE");
  kMtrace = Precious(L"MTRACE");
  kFtrace = Precious(L"FTRACE");
  kGtrace = Precious(L"GTRACE");
  kLambda = Precious(L"LAMBDA");
  kDefine = Precious(L"DEFINE");
  kExpand = Precious(L"EXPAND");
  kClosure = Precious(L"CLOSURE");
  kPartial = Precious(L"PARTIAL");
  kFunction = Precious(L"FUNCTION");
  kIntegrate = Precious(L"INTEGRATE");
  kPrintheap = Precious(L"PRINTHEAP");
  kImpossible = Precious(L"IMPOSSIBLE");
  kComma = Lookup(L"COMMA_");
  kSplice = Lookup(L"SPLICE_");
  kBackquote = Lookup(L"BACKQUOTE_");
  kString = Lookup(L"STRING_");
  kSquare = Lookup(L"SQUARE_");
  kCurly = Lookup(L"CURLY_");
  kDefun = Lookup(L"DEFUN");
  kDefmacro = Lookup(L"DEFMACRO");
  kAppend = Lookup(L"APPEND");
  kOr = Lookup(L"OR");
  kAnd = Lookup(L"AND");
  kIntersection = Lookup(L"INTERSECTION");
  kList = Lookup(L"LIST");
  kMember = Lookup(L"MEMBER");
  kNot = Lookup(L"NOT");
  kReverse = Lookup(L"REVERSE");
  kSqrt = Lookup(L"SQRT");
  kSubset = Lookup(L"SUBSET");
  kSuperset = Lookup(L"SUPERSET");
  kBecause = Lookup(L"BECAUSE");
  kTherefore = Lookup(L"THEREFORE");
  kUnion = Lookup(L"UNION");
  kImplies = Lookup(L"IMPLIES");
  kYcombinator = Lookup(L"YCOMBINATOR");
  kNand = Lookup(L"NAND");
  kNor = Lookup(L"NOR");
  kXor = Lookup(L"XOR");
  kIff = Lookup(L"IFF");
  kCycle = Lookup(L"CYCLE");
  kTrench = Lookup(L"𝕋ℝ𝔼ℕℂℍ");
  kUnchanged = Lookup(L"ⁿ/ₐ");
  kIgnore0 = Program(ShareCons(Atom(L"IGNORE"), 0), DF(DispatchIgnore0));
  for (i = 0; i < 26; ++i) {
    name[0] = 'A' + i;
    kConsAlphabet[i] = Program(ShareCons(Atom(name), 0), DF(DispatchPlan));
  }
  for (i = 0; i < 26; ++i) {
    name[0] = 'A' + i;
    kAlphabet[i] = name[0] == 'T' ? 1 : Lookup(name);
  }
}
