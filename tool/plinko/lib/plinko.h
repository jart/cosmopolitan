#ifndef COSMOPOLITAN_TOOL_PLINKO_LIB_PLINKO_H_
#define COSMOPOLITAN_TOOL_PLINKO_LIB_PLINKO_H_
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/runtime/runtime.h"
#include "tool/plinko/lib/config.h"
#include "tool/plinko/lib/types.h"
COSMOPOLITAN_C_START_

#define LO(x)      (int)(x)
#define HI(x)      (int)((x) >> 32)
#define MASK(x)    ((x)-1u)
#define ROR(x, k)  ((unsigned)(x) >> k | ((unsigned)(x) << (32 - k)))
#define MAKE(l, h) (((unsigned)(l)) | (dword)(h) << 32)
#define SHAD(i)    g_dispatch[(i) & (BANE | MASK(BANE))]
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

#define PROG(d, k, s) \
  SetInput(s);        \
  k = Read(0);        \
  d(k)

#define GetFrameCx()        ~HI(GetCurrentFrame())
#define GetDispatchFn(x)    DecodeDispatchFn(GetShadow(x))
#define DecodeDispatchFn(x) ((DispatchFn *)(uintptr_t)(unsigned)(x))
#define GetShadow(i) \
  ((__seg_fs const dword *)((uintptr_t)g_mem))[(i) & (BANE | MASK(BANE))]

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
    DispatchLookup, DispatchBuiltin, DispatchFuncall, DispatchRecursive,
    DispatchYcombine, DispatchPrecious, DispatchIgnore0, DispatchAdd,
    DispatchShortcut, DispatchCar, DispatchCdr, DispatchAtom, DispatchEq,
    DispatchCmp, DispatchOrder, DispatchLambda, DispatchCond, DispatchCons,
    DispatchProgn, DispatchQuiet, DispatchTrace, DispatchFtrace,
    DispatchFunction, DispatchBeta, DispatchGensym, DispatchPrinc,
    DispatchPrintheap, DispatchGc, DispatchFlush, DispatchError, DispatchExit,
    DispatchRead, DispatchIdentity, DispatchLet, DispatchLet1, DispatchLet2,
    DispatchIgnore1, DispatchExpand, DispatchPprint, DispatchPrint,
    DispatchEnclosedLetegatinator, DispatchEnclosedLetegate, DispatchIf,
    DispatchCall1, DispatchCall2;

DispatchFn DispatchEnclosedLet, DispatchCaar, DispatchCadr, DispatchCdar,
    DispatchCddr, DispatchCaaar, DispatchCaadr, DispatchCadar, DispatchCaddr,
    DispatchCdaar, DispatchCdadr, DispatchCddar, DispatchCdddr, DispatchCaaaar,
    DispatchCaaadr, DispatchCaadar, DispatchCaaddr, DispatchCadaar,
    DispatchCadadr, DispatchCaddar, DispatchCadddr, DispatchCdaaar,
    DispatchCdaadr, DispatchCdadar, DispatchCdaddr, DispatchCddaar,
    DispatchCddadr, DispatchCdddar, DispatchCddddr;

#ifndef __llvm__
register dword cGets asm("r12");
register dword *g_mem asm("rbx");
#else
extern dword cGets;
extern dword *g_mem;
#endif

extern unsigned short sp;
extern bool loga;
extern bool logc;
extern bool dump;
extern bool quiet;
extern bool stats;
extern bool simpler;
extern bool trace;
extern bool ftrace;
extern bool mtrace;
extern bool gtrace;
extern bool noname;
extern bool literally;
extern bool symbolism;

extern int cHeap;
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
extern int bp[4];
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
extern char g_buffer[4][512];
extern unsigned short g_depths[128][3];

extern dword tick;
extern dword cSets;
extern dword *g_dis;
extern EvalFn *eval;
extern BindFn *bind_;
extern char **inputs;
extern EvalFn *expand;
extern EvlisFn *evlis;
extern PairFn *pairlis;
extern TailFn *kTail[8];
extern RecurseFn *recurse;

extern int g_copy[256];
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

bool HasAtom(int, int) nosideeffect;
bool IsConstant(int) pureconst;
bool IsYcombinator(int);
bool MakesClosures(int);
dword Plan(int, int, int);
int Assoc(int, int);
int Cmp(int, int);
int CountAtoms(int, int, int) nosideeffect;
int CountReferences(int, int, int);
int CountSimpleArguments(int) nosideeffect;
int CountSimpleParameters(int) nosideeffect;
int Define(int, int);
int DumpDefines(int, int, int);
int Desymbolize(int) pureconst;
int Enclose(int, int);
int Expand(int, int);
int FindFreeVariables(int, int, int);
int Intern(int, int);
int IsCar(int);
int IsCdr(int);
int IsDelegate(int);
int Plinko(int, char *[]);
int Preplan(int, int, int);
int Read(int);
int ReadByte(int);
int ReadChar(int);
int ReadSpaces(int);
int ReadString(int, unsigned);
int Reverse(int, int);
int Symbolize(int) pureconst;
struct qword IsIf(int);
void Flush(int);
void PlanFuncalls(int, dword, int);
void Setup(void);

forceinline dword Get(int i) {
#ifndef NDEBUG
  DCHECK_LT(i, TERM);
#endif
  ++cGets;
  return g_mem[i & (BANE | MASK(BANE))];
}

forceinline int Car(int i) {
#ifndef NDEBUG
  DCHECK_LE(i, 0);
#endif
  return LO(Get(i));
}

forceinline int Cdr(int i) {
#ifndef NDEBUG
  DCHECK_LE(i, 0);
#endif
  return HI(Get(i));
}

forceinline unsigned EncodeDispatchFn(DispatchFn *f) {
  DCHECK_LE((uintptr_t)f, UINT_MAX);
  return (uintptr_t)f;
}

forceinline pureconst bool IsPrecious(int x) {
  return LO(GetShadow(x)) == EncodeDispatchFn(DispatchPrecious);
}

forceinline pureconst bool IsVariable(int x) {
  return LO(GetShadow(x)) == EncodeDispatchFn(DispatchLookup);
}

forceinline nosideeffect void *Addr(int i) {
  return g_mem + (i & (BANE | MASK(BANE)));
}

forceinline struct T Ret(dword ea, dword tm, dword r) {
  return kRet[r](ea, tm, r);
}

static inline int FasterRecurse(int v, int a, dword p1, dword p2) {
  if (v == LO(p1)) return HI(p1);
  if (v == LO(p2)) return HI(p2);
  /* if (IsPrecious(v)) return v; */
  /* if (v < 0 && Car(v) == kQuote) return Car(Cdr(v)); */
  return recurse(MAKE(v, a), p1, p2);
}

forceinline int Recurse(dword ea, dword p1, dword p2) {
  return ((ForceIntTailDispatchFn *)GetDispatchFn(LO(ea)))(ea, 0, 0, p1, p2,
                                                           GetShadow(LO(ea)));
}

forceinline struct T TailCall(dword ea, dword tm, dword r, dword p1, dword p2) {
  return kTail[r](ea, tm, r, p1, p2);
}

static inline int Keep(int x, int y) {
  return Equal(x, y) ? x : y;
}

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_TOOL_PLINKO_LIB_PLINKO_H_ */
