#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include "plinko.h"

#ifdef PLINKO_MEMORY_GLOBALS
dword cGets;
dword *g_mem;
dword *g_shadow;
#endif

dword *g_heap_mem;
dword *g_heap_shadow;

// clang-format off
DispatchFn *const kDispatch[] = {
    0,
#define X(f) f,
    DISPATCH_FUNCTIONS(X)
#undef X
};
// clang-format on

wchar_t sp;

bool ftrace;
bool mtrace;
bool gtrace;
bool noname;
bool unplanned;  // -P disables the planner's optimized dispatches
bool literally;
bool symbolism;
bool wantrepl;

bool dump;     // -d causes globals to be printed at exit
bool trace;    // -t causes evaluator to print explanations
bool loga;     // -a flag causes -t to print massive environment
bool logc;     // -c flag causes -t to print jupiterian closures
bool quiet;    // tracks (quiet) state which suppresses (print)
bool stats;    // -s causes statistics to be printed after each evaluation
bool simpler;  // -S usually disables pretty printing so you can | cut -c-80

int cx;         // cons stack pointer
int ax;         // used by read atom interner
int dx = L' ';  // used by read for lookahead
int pdp;        // used by print to prevent stack overflow
int depth;      // when tracing is enabled tracks trace depth
int fails;      // failure count to influence process exit code
int cFloor;     // lowest cx possible before the heap needs to grow
int cHeap;      // statistical approximate of minimum cx during work
int cAtoms;     // statistical count of characters in atom hash tree
int cFrost;     // monotonic frostline of defined permanent cons cells
int globals;    // cons[rbtree;bool  0] of globally scoped definitions i.e. 𝑎
int revglob;    // reverse mapped rbtree of globals (informational printing)
int ordglob;    // the original defined order for all global definition keys

int kTrace;
int kMtrace;
int kFtrace;
int kGtrace;
int kEq;
int kGc;
int kCmp;
int kCar;
int kBackquote;
int kDefun;
int kDefmacro;
int kAppend;
int kBeta;
int kAnd;
int kCdr;
int kRead;
int kDump;
int kQuote;
int kProgn;
int kLambda;
int kDefine;
int kMacro;
int kQuiet;
int kSplice;
int kPrinc;
int kPrint;
int kPprint;
int kIgnore;
int kExpand;
int kCond;
int kAtom;
int kOr;
int kCons;
int kIntegrate;
int kString;
int kSquare;
int kCurly;
int kFork;
int kGensym;
int kTrench;
int kYcombinator;
int kBecause;
int kTherefore;
int kUnion;
int kImplies;
int kNand;
int kNor;
int kXor;
int kIff;
int kPartial;
int kError;
int kExit;
int kClosure;
int kFunction;
int kCycle;
int kFlush;
int kIgnore0;
int kComma;
int kIntersection;
int kList;
int kMember;
int kNot;
int kReverse;
int kSqrt;
int kSubset;
int kSuperset;
int kPrintheap;
int kImpossible;
int kUnchanged;
int kOrder;

jmp_buf crash;
jmp_buf exiter;

wchar_t g_depths[128][3];

dword tick;
dword cSets;
EvalFn *eval;
BindFn *bind_;
char **inputs;
EvalFn *expand;
EvlisFn *evlis;
PairFn *pairlis;
TailFn *kTail[8];
RecurseFn *recurse;

int g_print[256];
int kAlphabet[26];
dword g_stack[STACK];
int kConsAlphabet[26];

long g_assoc_histogram[12];
long g_gc_lop_histogram[30];
long g_gc_marks_histogram[30];
long g_gc_dense_histogram[30];
long g_gc_sparse_histogram[30];
long g_gc_discards_histogram[30];
