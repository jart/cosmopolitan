#ifndef COSMOPOLITAN_TOOL_PLINKO_LIB_PLINKO_H_
#define COSMOPOLITAN_TOOL_PLINKO_LIB_PLINKO_H_
#include <assert.h>
#include <limits.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <wchar.h>
#include "config.h"
#include "types.h"

#define LO(x)      (int)(x)
#define HI(x)      (int)((x) >> 32)
#define MASK(x)    ((x) - 1u)
#define ROR(x, k)  ((unsigned)(x) >> k | ((unsigned)(x) << (32 - k)))
#define MAKE(l, h) (((unsigned)(l)) | (dword)(h) << 32)
#define DF(f)      EncodeDispatchFn(f)
#define CAR(x)     LO(Get(x))
#define CDR(x)     HI(Get(x))
#define ARG1(e)    Cadr(e)
#define __(c)      (assert(IsUpper(c)), kAlphabet[c - 'A'])
#define _(s)       __(STRINGIFY(s)[0])

#define ZERO4 MAKE4(0, 0, 0, 0)
#define MAKE4(a, b, c, d)  \
  (struct qword) {         \
    MAKE(a, b), MAKE(c, d) \
  }

#define Equal(x, y) !Cmp(x, y)

#define RESTORE(s) s = save_##s
#define SAVE(s, x)        \
  typeof(s) save_##s = s; \
  s = x

#define GetFrameCx()        ~HI(GetCurrentFrame())
#define GetDispatchFn(x)    DecodeDispatchFn(GetShadow(x))
#define DecodeDispatchFn(x) kDispatch[(unsigned)(x)]
#define EncodeDispatchFn(f) ((unsigned)k##f)

#define GetShadow(i) g_shadow[i]

struct T {
  int res;
};

struct Binding {
  int u;
  dword p1;
};

typedef int EvalFn(int, int);
typedef int PairFn(int, int, int);
typedef int RecurseFn(dword, dword, dword);
typedef struct T RetFn(dword, dword, dword);
typedef int EvlisFn(int, int, dword, dword);
typedef struct Binding BindFn(int, int, int, int, dword, dword);
typedef struct T TailFn(dword, dword, dword, dword, dword);
typedef struct T DispatchFn(dword, dword, dword, dword, dword, dword);
typedef int ForceIntTailDispatchFn(dword, dword, dword, dword, dword, dword);

BindFn Bind, BindTrace;
RecurseFn RecurseTrace;
EvlisFn Evlis, EvlisTrace;
PairFn Pairlis, PairlisTrace;
EvalFn Eval, EvalTrace, ExpandTrace, Exlis;
TailFn DispatchTail, DispatchTailGc, DispatchTailTmcGc;
TailFn DispatchTailTrace, DispatchTailGcTrace, DispatchTailTmcGcTrace;

DispatchFn DispatchNil, DispatchTrue, DispatchPlan, DispatchQuote,
    DispatchLookup, DispatchFuncall, DispatchYcombine, DispatchPrecious,
    DispatchIgnore0, DispatchShortcut, DispatchCar, DispatchCdr, DispatchAtom,
    DispatchEq, DispatchCmp, DispatchOrder, DispatchLambda, DispatchCond,
    DispatchCons, DispatchProgn, DispatchQuiet, DispatchTrace, DispatchFtrace,
    DispatchFunction, DispatchBeta, DispatchGensym, DispatchPrinc,
    DispatchPrintheap, DispatchGc, DispatchFlush, DispatchError, DispatchExit,
    DispatchRead, DispatchIdentity, DispatchLet1, DispatchIgnore1,
    DispatchExpand, DispatchPprint, DispatchPrint, DispatchIf, DispatchCall1,
    DispatchCall2;

DispatchFn DispatchRecur, DispatchRecur1, DispatchRecur2;

DispatchFn DispatchCaar, DispatchCadr, DispatchCdar, DispatchCddr,
    DispatchCaaar, DispatchCaadr, DispatchCadar, DispatchCaddr, DispatchCdaar,
    DispatchCdadr, DispatchCddar, DispatchCdddr, DispatchCaaaar, DispatchCaaadr,
    DispatchCaadar, DispatchCaaddr, DispatchCadaar, DispatchCadadr,
    DispatchCaddar, DispatchCadddr, DispatchCdaaar, DispatchCdaadr,
    DispatchCdadar, DispatchCdaddr, DispatchCddaar, DispatchCddadr,
    DispatchCdddar, DispatchCddddr;

// every function that can be stored in a shadow cell. the shadow holds
// an index into kDispatch[] rather than the function address, so we can
// compare against dispatch functions using small immediates, and we'll
// never need to care where the program image got loaded in memory
#define DISPATCH_FUNCTIONS(X) \
  X(DispatchPlan)             \
  X(DispatchAtom)             \
  X(DispatchBeta)             \
  X(DispatchCaaaar)           \
  X(DispatchCaaadr)           \
  X(DispatchCaaar)            \
  X(DispatchCaadr)            \
  X(DispatchCaadar)           \
  X(DispatchCaaddr)           \
  X(DispatchCaar)             \
  X(DispatchCadaar)           \
  X(DispatchCadadr)           \
  X(DispatchCadar)            \
  X(DispatchCaddar)           \
  X(DispatchCadddr)           \
  X(DispatchCaddr)            \
  X(DispatchCadr)             \
  X(DispatchCall1)            \
  X(DispatchCall2)            \
  X(DispatchCar)              \
  X(DispatchCdaaar)           \
  X(DispatchCdaadr)           \
  X(DispatchCdaar)            \
  X(DispatchCdadar)           \
  X(DispatchCdaddr)           \
  X(DispatchCdadr)            \
  X(DispatchCdar)             \
  X(DispatchCddaar)           \
  X(DispatchCddadr)           \
  X(DispatchCddar)            \
  X(DispatchCdddar)           \
  X(DispatchCddddr)           \
  X(DispatchCdddr)            \
  X(DispatchCddr)             \
  X(DispatchCdr)              \
  X(DispatchCmp)              \
  X(DispatchCond)             \
  X(DispatchCons)             \
  X(DispatchEq)               \
  X(DispatchError)            \
  X(DispatchExit)             \
  X(DispatchExpand)           \
  X(DispatchFlush)            \
  X(DispatchFtrace)           \
  X(DispatchFuncall)          \
  X(DispatchFunction)         \
  X(DispatchGc)               \
  X(DispatchGensym)           \
  X(DispatchIdentity)         \
  X(DispatchIf)               \
  X(DispatchIgnore0)          \
  X(DispatchIgnore1)          \
  X(DispatchLambda)           \
  X(DispatchLet1)             \
  X(DispatchLookup)           \
  X(DispatchNil)              \
  X(DispatchOrder)            \
  X(DispatchPprint)           \
  X(DispatchPrecious)         \
  X(DispatchPrinc)            \
  X(DispatchPrint)            \
  X(DispatchPrintheap)        \
  X(DispatchProgn)            \
  X(DispatchQuiet)            \
  X(DispatchQuote)            \
  X(DispatchRead)             \
  X(DispatchRecur)            \
  X(DispatchRecur1)           \
  X(DispatchRecur2)           \
  X(DispatchShortcut)         \
  X(DispatchTrace)            \
  X(DispatchTrue)             \
  X(DispatchYcombine)

enum DispatchIndex {
  kDispatchNull,  // zero is unprogrammed, so calling it crashes loudly
#define X(f) k##f,
  DISPATCH_FUNCTIONS(X)
#undef X
};

extern DispatchFn *const kDispatch[];

#if defined(__aarch64__) && !defined(__llvm__)
register dword cGets __asm__("x26");
register dword *g_mem __asm__("x27");
register dword *g_shadow __asm__("x25");
#else
#define PLINKO_MEMORY_GLOBALS
extern dword cGets;
extern dword *g_mem;
extern dword *g_shadow;
#endif

// canonical copies of g_mem and g_shadow, since longjmp() restores the
// callee-saved registers they live in, which may predate a heap growth
extern dword *g_heap_mem;
extern dword *g_heap_shadow;

extern wchar_t sp;
extern bool loga;
extern bool logc;
extern bool dump;
extern bool quiet;
extern bool stats;
extern bool simpler;
extern bool unplanned;
extern bool trace;
extern bool ftrace;
extern bool mtrace;
extern bool gtrace;
extern bool noname;
extern bool literally;
extern bool symbolism;
extern bool wantrepl;

extern int cHeap;
extern int cFloor;
extern int cAtoms;
extern int cFrost;
extern int globals;
extern int revglob;
extern int ordglob;
extern int ax;
extern int cx;
extern int dx;
extern int ex;
extern int pdp;
extern int fails;
extern int depth;
extern int kTrace;
extern int kMtrace;
extern int kFtrace;
extern int kGtrace;
extern int kEq;
extern int kGc;
extern int kCmp;
extern int kCar;
extern int kBackquote;
extern int kDefun;
extern int kDefmacro;
extern int kAppend;
extern int kBeta;
extern int kAnd;
extern int kCdr;
extern int kRead;
extern int kDump;
extern int kQuote;
extern int kProgn;
extern int kLambda;
extern int kDefine;
extern int kMacro;
extern int kQuiet;
extern int kSplice;
extern int kPrinc;
extern int kPrint;
extern int kPprint;
extern int kIgnore;
extern int kExpand;
extern int kCond;
extern int kAtom;
extern int kOr;
extern int kCons;
extern int kIntegrate;
extern int kString;
extern int kSquare;
extern int kCurly;
extern int kFork;
extern int kGensym;
extern int kTrench;
extern int kYcombinator;
extern int kBecause;
extern int kTherefore;
extern int kUnion;
extern int kImplies;
extern int kNand;
extern int kNor;
extern int kXor;
extern int kIff;
extern int kPartial;
extern int kError;
extern int kExit;
extern int kClosure;
extern int kFunction;
extern int kCycle;
extern int kFlush;
extern int kIgnore0;
extern int kComma;
extern int kIntersection;
extern int kList;
extern int kMember;
extern int kNot;
extern int kReverse;
extern int kSqrt;
extern int kSubset;
extern int kSuperset;
extern int kPrintheap;
extern int kImpossible;
extern int kUnchanged;
extern int kOrder;

extern jmp_buf crash;
extern jmp_buf exiter;

extern RetFn *const kRet[8];
extern wchar_t g_depths[128][3];

extern dword tick;
extern dword cSets;
extern EvalFn *eval;
extern BindFn *bind_;
extern char **inputs;
extern EvalFn *expand;
extern EvlisFn *evlis;
extern PairFn *pairlis;
extern TailFn *kTail[8];
extern RecurseFn *recurse;

extern int g_print[256];
extern int kAlphabet[26];
extern dword g_stack[STACK];
extern int kConsAlphabet[26];

extern long g_assoc_histogram[12];
extern long g_gc_lop_histogram[30];
extern long g_gc_marks_histogram[30];
extern long g_gc_dense_histogram[30];
extern long g_gc_sparse_histogram[30];
extern long g_gc_discards_histogram[30];

bool HasAtom(int, int);
bool IsConstant(int);
bool IsYcombinator(int);
bool MakesClosures(int);
dword Plan(int, int, int);
int Assoc(int, int);
int AssocGlobal(int, int);
int Cmp(int, int);
int CountAtoms(int, int, int);
int CountReferences(int, int, int);
int CountSimpleArguments(int);
int CountSimpleParameters(int);
int Define(int, int);
int DumpDefines(int, int, int);
int Desymbolize(int);
int Enclose(int, int);
int Expand(int, int);
int FindFreeVariables(int, int, int);
int Intern(int, int);
int IsCar(int);
int IsCdr(int);
int Plinko(int, char *[]);
int Preplan(int, int, int);
int Read(void);
int ReadRune(void);
int ReadString(unsigned);
int Reverse(int, int);
int Symbolize(int);
struct qword IsIf(int);
void PlanFuncalls(int, dword, int);
void Setup(void);
void Grow(void);
void RestoreHeap(void);

static inline dword GetNanos(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec * 1000000000ul + ts.tv_nsec;
}

static inline dword Get(int i) {
#ifndef NDEBUG
  assert(i < TERM);
#endif
  ++cGets;
  return g_mem[i];
}

static inline int Car(int i) {
#ifndef NDEBUG
  assert(i <= 0);
#endif
  return LO(Get(i));
}

static inline int Cdr(int i) {
#ifndef NDEBUG
  assert(i <= 0);
#endif
  return HI(Get(i));
}

static inline bool IsPrecious(int x) {
  return LO(GetShadow(x)) == EncodeDispatchFn(DispatchPrecious);
}

static inline bool IsVariable(int x) {
  return LO(GetShadow(x)) == EncodeDispatchFn(DispatchLookup);
}

static inline struct T Ret(dword ea, dword tm, dword r) {
  return kRet[r](ea, tm, r);
}

static inline int FasterRecurse(int v, int a, dword p1, dword p2) {
  if (v == LO(p1))
    return HI(p1);
  if (v == LO(p2))
    return HI(p2);
  /* if (IsPrecious(v)) return v; */
  /* if (v < 0 && Car(v) == kQuote) return Car(Cdr(v)); */
  return recurse(MAKE(v, a), p1, p2);
}

static inline int Recurse(dword ea, dword p1, dword p2) {
  return ((ForceIntTailDispatchFn *)GetDispatchFn(LO(ea)))(ea, 0, 0, p1, p2,
                                                           GetShadow(LO(ea)));
}

static inline struct T TailCall(dword ea, dword tm, dword r, dword p1,
                                dword p2) {
  return kTail[r](ea, tm, r, p1, p2);
}

static inline int Keep(int x, int y) {
  return Equal(x, y) ? x : y;
}

#endif /* COSMOPOLITAN_TOOL_PLINKO_LIB_PLINKO_H_ */
