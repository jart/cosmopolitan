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
#include "libc/runtime/runtime.h"
#include "tool/plinko/lib/plinko.h"

#ifdef __llvm__
dword cGets;
dword *g_mem;
#endif

unsigned short sp;

bool ftrace;
bool mtrace;
bool gtrace;
bool noname;
bool literally;
bool symbolism;

bool dump;     // -d causes globals to be printed at exit
bool trace;    // -t causes evaluator to print explanations
bool loga;     // -a flag causes -t to print massive environment
bool logc;     // -c flag causes -t to print jupiterian closures
bool quiet;    // tracks (quiet) state which suppresses (print)
bool stats;    // -s causes statistics to be printed after each evaluation
bool simpler;  // -S usually disables pretty printing so you can | cut -c-80

int cx;       // cons stack pointer
int ax;       // used by read atom interner
int dx;       // used by read for lookahead
int bp[4];    // buffer pointers for file descriptors
int pdp;      // used by print to prevent stack overflow
int depth;    // when tracing is enabled tracks trace depth
int fails;    // failure count to influence process exit code
int cHeap;    // statistical approximate of minimum cx during work
int cAtoms;   // statistical count of characters in atom hash tree
int cFrost;   // monotonic frostline of defined permanent cons cells
int globals;  // cons[rbtree;bool  0] of globally scoped definitions i.e. 𝑎
int revglob;  // reverse mapped rbtree of globals (informational printing)
int ordglob;  // the original defined order for all global definition keys

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

char g_buffer[4][512];
unsigned short g_depths[128][3];

dword tick;
dword cSets;
dword *g_dis;
EvalFn *eval;
BindFn *bind_;
char **inputs;
EvalFn *expand;
EvlisFn *evlis;
PairFn *pairlis;
TailFn *kTail[8];
RecurseFn *recurse;

int g_copy[256];
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
