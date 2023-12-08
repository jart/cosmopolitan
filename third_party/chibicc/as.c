/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/elf/def.h"
#include "libc/fmt/conv.h"
#include "libc/intrin/bsr.h"
#include "libc/intrin/popcnt.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/str/tab.internal.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/s.h"
#include "libc/x/x.h"
#include "libc/x/xasprintf.h"
#include "third_party/chibicc/file.h"
#include "third_party/gdtoa/gdtoa.h"
#include "libc/serialize.h"
#include "tool/build/lib/elfwriter.h"

#define OSZ  0x66
#define ASZ  0x67
#define REX  0x40  // byte
#define REXB 0x41  // src
#define REXX 0x42  // index
#define REXR 0x44  // dest
#define REXW 0x48  // quad

#define HASASZ   0x00010000
#define HASBASE  0x00020000
#define HASINDEX 0x00040000
#define ISRIP    0x00080000
#define ISREG    0x00100000

#define APPEND(L)                           \
  if (++L.n > L.c) {                        \
    L.c = L.n + 2 + (L.c >> 1);             \
    L.p = realloc(L.p, L.c * sizeof(*L.p)); \
  }

#define IS(P, N, S)  (N == sizeof(S) - 1 && !strncasecmp(P, S, sizeof(S) - 1))
#define MAX(X, Y)    ((Y) < (X) ? (X) : (Y))
#define READ128BE(S) ((uint128_t)READ64BE(S) << 64 | READ64BE((S) + 8))

struct As {
  int i;         // things
  int section;   // sections
  int previous;  // sections
  int inpath;    // strings
  int outpath;   // strings
  int counter;
  int pcrelative;
  bool inhibiterr;
  bool inhibitwarn;
  struct Ints {
    unsigned long n, c;
    int128_t *p;
  } ints;
  struct Floats {
    unsigned long n, c;
    long double *p;
  } floats;
  struct Slices {
    unsigned long n, c;
    struct Slice {
      unsigned long n, c;
      char *p;
    } * p;
  } slices;
  struct Sauces {
    unsigned long n, c;
    struct Sauce {
      unsigned path;  // strings
      unsigned line;  // 1-indexed
    } * p;
  } sauces;
  struct Things {
    unsigned long n, c;
    struct Thing {
      enum ThingType {
        TT_INT,
        TT_FLOAT,
        TT_SLICE,
        TT_PUNCT,
        TT_FORWARD,
        TT_BACKWARD,
      } t : 4;
      unsigned s : 28;  // sauces
      unsigned i;       // identity,ints,floats,slices
    } * p;
  } things;
  struct Sections {
    unsigned long n, c;
    struct Section {
      unsigned name;  // strings
      int flags;
      int type;
      int align;
      struct Slice binary;
    } * p;
  } sections;
  struct Symbols {
    unsigned long n, c;
    struct Symbol {
      bool isused;
      unsigned char stb;   // STB_*
      unsigned char stv;   // STV_*
      unsigned char type;  // STT_*
      unsigned name;       // slices
      unsigned section;    // sections
      long offset;
      long size;
      struct ElfWriterSymRef ref;
    } * p;
  } symbols;
  struct HashTable {
    unsigned i, n;
    struct HashEntry {
      unsigned h;
      unsigned i;
    } * p;
  } symbolindex;
  struct Labels {
    unsigned long n, c;
    struct Label {
      unsigned id;
      unsigned tok;     // things
      unsigned symbol;  // symbols
    } * p;
  } labels;
  struct Relas {
    unsigned long n, c;
    struct Rela {
      bool isdead;
      int kind;          // R_X86_64_{16,32,64,PC8,PC32,PLT32,GOTPCRELX,...}
      unsigned expr;     // exprs
      unsigned section;  // sections
      long offset;
      long addend;
    } * p;
  } relas;
  struct Exprs {
    unsigned long n, c;
    struct Expr {
      enum ExprKind {
        EX_INT,     // integer
        EX_SYM,     // things (then symbols after eval)
        EX_NEG,     // unary -
        EX_NOT,     // unary !
        EX_BITNOT,  // unary ~
        EX_ADD,     // +
        EX_SUB,     // -
        EX_MUL,     // *
        EX_DIV,     // /
        EX_REM,     // %
        EX_AND,     // &
        EX_OR,      // |
        EX_XOR,     // ^
        EX_SHL,     // <<
        EX_SHR,     // >>
        EX_EQ,      // ==
        EX_NE,      // !=
        EX_LT,      // <
        EX_LE,      // <=
      } kind;
      enum ExprMod {
        EM_NORMAL,
        EM_GOTPCREL,
        EM_DTPOFF,
        EM_TPOFF,
      } em;
      unsigned tok;
      int lhs;
      int rhs;
      int128_t x;
      bool isvisited;
      bool isevaluated;
    } * p;
  } exprs;
  struct Strings {
    unsigned long n, c;
    char **p;
  } strings, incpaths;
  struct SectionStack {
    unsigned long n, c;
    int *p;
  } sectionstack;
};

static const char kPrefixByte[30] = {
    0x67, 0x2E, 0x66, 0x3E, 0x26, 0x64, 0x65, 0xF0, 0xF3, 0xF3,
    0xF2, 0xF2, 0xF3, 0x40, 0x41, 0x44, 0x45, 0x46, 0x47, 0x48,
    0x49, 0x4C, 0x4D, 0x4E, 0x4F, 0x4A, 0x4B, 0x42, 0x43, 0x36,
};

static const char kPrefix[30][8] = {
    "addr32",   "cs",     "data16",  "ds",     "es",      "fs",
    "gs",       "lock",   "rep",     "repe",   "repne",   "repnz",
    "repz",     "rex",    "rex.b",   "rex.r",  "rex.rb",  "rex.rx",
    "rex.rxb",  "rex.w",  "rex.wb",  "rex.wr", "rex.wrb", "rex.wrx",
    "rex.wrxb", "rex.wx", "rex.wxb", "rex.x",  "rex.xb",  "ss",
};

static const char kSegmentByte[6] = {0x2E, 0x3E, 0x26, 0x64, 0x65, 0x36};
static const char kSegment[6][2] = {"cs", "ds", "es", "fs", "gs", "ss"};

/**
 * Context-sensitive register encoding information.
 *
 *   ┌rex
 *   │         ┌log₂size
 *   │         │  ┌reg
 *   ├──────┐  ├─┐├─┐
 * 0b0000000000000000
 */
static const struct Reg {
  char s[8];
  short reg;
  short rm;
  short base;
  short index;
} kRegs[] = /* clang-format off */ {
  {"ah",    4,                            4,                            -1,                  -1                 },
  {"al",    0,                            0,                            -1,                  -1                 },
  {"ax",    0 | 1<<3,                     0 | 1<<3,                     -1,                  -1                 },
  {"bh",    7,                            7,                            -1,                  -1                 },
  {"bl",    3,                            3,                            -1,                  -1                 },
  {"bp",    5 | 1<<3,                     5 | 1<<3,                     -1,                  -1                 },
  {"bpl",   5 | REX<<8,                   5 | REX<<8,                   -1,                  -1                 },
  {"bx",    3 | 1<<3,                     3 | 1<<3,                     -1,                  -1                 },
  {"ch",    5,                            5,                            -1,                  -1                 },
  {"cl",    1,                            1,                            -1,                  -1                 },
  {"cx",    1 | 1<<3,                     1 | 1<<3,                     -1,                  -1                 },
  {"dh",    6,                            6,                            -1,                  -1                 },
  {"di",    7 | 1<<3,                     7 | 1<<3,                     -1,                  -1                 },
  {"dil",   7 | REX<<8,                   7 | REX<<8,                   -1,                  -1                 },
  {"dl",    2,                            2,                            -1,                  -1                 },
  {"dx",    2 | 1<<3,                     2 | 1<<3,                     -1,                  -1                 },
  {"eax",   0 | 2<<3,                     0 | 2<<3,                     0 | 2<<3,            0 | 2<<3           },
  {"ebp",   5 | 2<<3,                     5 | 2<<3,                     5 | 2<<3,            5 | 2<<3           },
  {"ebx",   3 | 2<<3,                     3 | 2<<3,                     3 | 2<<3,            3 | 2<<3           },
  {"ecx",   1 | 2<<3,                     1 | 2<<3,                     1 | 2<<3,            1 | 2<<3           },
  {"edi",   7 | 2<<3,                     7 | 2<<3,                     7 | 2<<3,            7 | 2<<3           },
  {"edx",   2 | 2<<3,                     2 | 2<<3,                     2 | 2<<3,            2 | 2<<3           },
  {"eiz",   -1,                           -1,                           -1,                  4 | 2<<3           },
  {"esi",   6 | 2<<3,                     6 | 2<<3,                     6 | 2<<3,            6 | 2<<3           },
  {"esp",   4 | 2<<3,                     4 | 2<<3,                     4 | 2<<3,            4 | 2<<3           },
  {"mm0",   0 | 3<<3,                     0 | 3<<3,                     -1,                  -1                 },
  {"mm1",   1 | 3<<3,                     1 | 3<<3,                     -1,                  -1                 },
  {"mm2",   2 | 3<<3,                     2 | 3<<3,                     -1,                  -1                 },
  {"mm3",   3 | 3<<3,                     3 | 3<<3,                     -1,                  -1                 },
  {"mm4",   4 | 3<<3,                     4 | 3<<3,                     -1,                  -1                 },
  {"mm5",   5 | 3<<3,                     5 | 3<<3,                     -1,                  -1                 },
  {"mm6",   6 | 3<<3,                     6 | 3<<3,                     -1,                  -1                 },
  {"mm7",   7 | 3<<3,                     7 | 3<<3,                     -1,                  -1                 },
  {"mm8",   0 | 3<<3 | REXR<<8,           0 | 3<<3 | REXB<<8,           -1,                  -1                 },
  {"mm9",   1 | 3<<3 | REXR<<8,           1 | 3<<3 | REXB<<8,           -1,                  -1                 },
  {"r10",   2 | 3<<3 | REXR<<8 | REXW<<8, 2 | 3<<3 | REXB<<8 | REXW<<8, 2 | 3<<3 | REXB<<8,  2 | 3<<3 | REXX<<8 },
  {"r10b",  2 | REXR<<8,                  2 | REXB<<8,                  -1,                  -1                 },
  {"r10d",  2 | 2<<3 | REXR<<8,           2 | 2<<3 | REXB<<8,           2 | 2<<3 | REXB<<8,  2 | 2<<3 | REXX<<8 },
  {"r10w",  2 | 1<<3 | REXR<<8,           2 | 1<<3 | REXB<<8,           -1,                  -1                 },
  {"r11",   3 | 3<<3 | REXR<<8 | REXW<<8, 3 | 3<<3 | REXB<<8 | REXW<<8, 3 | 3<<3 | REXB<<8,  3 | 3<<3 | REXX<<8 },
  {"r11b",  3 | REXR<<8,                  3 | REXB<<8,                  -1,                  -1                 },
  {"r11d",  3 | 2<<3 | REXR<<8,           3 | 2<<3 | REXB<<8,           3 | 2<<3 | REXB<<8,  3 | 2<<3 | REXX<<8 },
  {"r11w",  3 | 1<<3 | REXR<<8,           3 | 1<<3 | REXB<<8,           -1,                  -1                 },
  {"r12",   4 | 3<<3 | REXR<<8 | REXW<<8, 4 | 3<<3 | REXB<<8 | REXW<<8, 4 | 3<<3 | REXB<<8,  4 | 3<<3 | REXX<<8 },
  {"r12b",  4 | REXR<<8,                  4 | REXB<<8,                  -1,                  -1                 },
  {"r12d",  4 | 2<<3 | REXR<<8,           4 | 2<<3 | REXB<<8,           4 | 2<<3 | REXB<<8,  4 | 2<<3 | REXX<<8 },
  {"r12w",  4 | 1<<3 | REXR<<8,           4 | 1<<3 | REXB<<8,           -1,                  -1                 },
  {"r13",   5 | 3<<3 | REXR<<8 | REXW<<8, 5 | 3<<3 | REXB<<8 | REXW<<8, 5 | 3<<3 | REXB<<8,  5 | 3<<3 | REXX<<8 },
  {"r13b",  5 | REXR<<8,                  5 | REXB<<8,                  -1,                  -1                 },
  {"r13d",  5 | 2<<3 | REXR<<8,           5 | 2<<3 | REXB<<8,           5 | 2<<3 | REXB<<8,  5 | 2<<3 | REXX<<8 },
  {"r13w",  5 | 1<<3 | REXR<<8,           5 | 1<<3 | REXB<<8,           -1,                  -1                 },
  {"r14",   6 | 3<<3 | REXR<<8 | REXW<<8, 6 | 3<<3 | REXB<<8 | REXW<<8, 6 | 3<<3 | REXB<<8,  6 | 3<<3 | REXX<<8 },
  {"r14b",  6 | REXR<<8,                  6 | REXB<<8,                  -1,                  -1                 },
  {"r14d",  6 | 2<<3 | REXR<<8,           6 | 2<<3 | REXB<<8,           6 | 2<<3 | REXB<<8,  6 | 2<<3 | REXX<<8 },
  {"r14w",  6 | 1<<3 | REXR<<8,           6 | 1<<3 | REXB<<8,           -1,                  -1                 },
  {"r15",   7 | 3<<3 | REXR<<8 | REXW<<8, 7 | 3<<3 | REXB<<8 | REXW<<8, 7 | 3<<3 | REXB<<8,  7 | 3<<3 | REXX<<8 },
  {"r15b",  7 | REXR<<8,                  7 | REXB<<8,                  -1,                  -1                 },
  {"r15d",  7 | 2<<3 | REXR<<8,           7 | 2<<3 | REXB<<8,           7 | 2<<3 | REXB<<8,  7 | 2<<3 | REXX<<8 },
  {"r15w",  7 | 1<<3 | REXR<<8,           7 | 1<<3 | REXB<<8,           -1,                  -1                 },
  {"r8",    0 | 3<<3 | REXR<<8 | REXW<<8, 0 | 3<<3 | REXB<<8 | REXW<<8, 0 | 3<<3 | REXB<<8,  0 | 3<<3 | REXX<<8 },
  {"r8b",   0 | REXR<<8,                  0 | REXB<<8,                  -1,                  -1                 },
  {"r8d",   0 | 2<<3 | REXR<<8,           0 | 2<<3 | REXB<<8,           0 | 2<<3 | REXB<<8,  0 | 2<<3 | REXX<<8 },
  {"r8w",   0 | 1<<3 | REXR<<8,           0 | 1<<3 | REXB<<8,           -1,                  -1                 },
  {"r9",    1 | 3<<3 | REXR<<8 | REXW<<8, 1 | 3<<3 | REXB<<8 | REXW<<8, 1 | 3<<3 | REXB<<8,  1 | 3<<3 | REXX<<8 },
  {"r9b",   1 | REXR<<8,                  1 | REXB<<8,                  -1,                  -1                 },
  {"r9d",   1 | 2<<3 | REXR<<8,           1 | 2<<3 | REXB<<8,           1 | 2<<3 | REXB<<8,  1 | 2<<3 | REXX<<8 },
  {"r9w",   1 | 1<<3 | REXR<<8,           1 | 1<<3 | REXB<<8,           -1,                  -1                 },
  {"rax",   0 | 3<<3 | REXW<<8,           0 | 3<<3 | REXW<<8,           0 | 3<<3,            0 | 3<<3           },
  {"rbp",   5 | 3<<3 | REXW<<8,           5 | 3<<3 | REXW<<8,           5 | 3<<3,            5 | 3<<3           },
  {"rbx",   3 | 3<<3 | REXW<<8,           3 | 3<<3 | REXW<<8,           3 | 3<<3,            3 | 3<<3           },
  {"rcx",   1 | 3<<3 | REXW<<8,           1 | 3<<3 | REXW<<8,           1 | 3<<3,            1 | 3<<3           },
  {"rdi",   7 | 3<<3 | REXW<<8,           7 | 3<<3 | REXW<<8,           7 | 3<<3,            7 | 3<<3           },
  {"rdx",   2 | 3<<3 | REXW<<8,           2 | 3<<3 | REXW<<8,           2 | 3<<3,            2 | 3<<3           },
  {"riz",   -1,                           -1,                           -1,                  4 | 3<<3           },
  {"rsi",   6 | 3<<3 | REXW<<8,           6 | 3<<3 | REXW<<8,           6 | 3<<3,            6 | 3<<3           },
  {"rsp",   4 | 3<<3 | REXW<<8,           4 | 3<<3 | REXW<<8,           4 | 3<<3,            4 | 3<<3           },
  {"si",    6 | 1<<3,                     6 | 1<<3,                     6 | 1<<3,            6 | 1<<3           },
  {"sil",   6 | REX<<8,                   6 | REX<<8,                   6 | REX<<8,          6 | REX<<8         },
  {"sp",    4 | 1<<3,                     4 | 1<<3,                     4 | 1<<3,            4 | 1<<3           },
  {"spl",   4 | REX<<8,                   4 | REX<<8,                   4 | REX<<8,          4 | REX<<8         },
  {"st",    0 | 4<<3,                     0 | 4<<3,                     -1,                  -1                 },
  {"st(0)", 0 | 4<<3,                     0 | 4<<3,                     -1,                  -1                 },
  {"st(1)", 1 | 4<<3,                     1 | 4<<3,                     -1,                  -1                 },
  {"st(2)", 2 | 4<<3,                     2 | 4<<3,                     -1,                  -1                 },
  {"st(3)", 3 | 4<<3,                     3 | 4<<3,                     -1,                  -1                 },
  {"st(4)", 4 | 4<<3,                     4 | 4<<3,                     -1,                  -1                 },
  {"st(5)", 5 | 4<<3,                     5 | 4<<3,                     -1,                  -1                 },
  {"st(6)", 6 | 4<<3,                     6 | 4<<3,                     -1,                  -1                 },
  {"st(7)", 7 | 4<<3,                     7 | 4<<3,                     -1,                  -1                 },
  {"xmm0",  0 | 4<<3,                     0 | 4<<3,                     -1,                  -1                 },
  {"xmm1",  1 | 4<<3,                     1 | 4<<3,                     -1,                  -1                 },
  {"xmm10", 2 | 4<<3 | REXR<<8,           2 | 4<<3 | REXB<<8,           -1,                  -1                 },
  {"xmm11", 3 | 4<<3 | REXR<<8,           3 | 4<<3 | REXB<<8,           -1,                  -1                 },
  {"xmm12", 4 | 4<<3 | REXR<<8,           4 | 4<<3 | REXB<<8,           -1,                  -1                 },
  {"xmm13", 5 | 4<<3 | REXR<<8,           5 | 4<<3 | REXB<<8,           -1,                  -1                 },
  {"xmm14", 6 | 4<<3 | REXR<<8,           6 | 4<<3 | REXB<<8,           -1,                  -1                 },
  {"xmm15", 7 | 4<<3 | REXR<<8,           7 | 4<<3 | REXB<<8,           -1,                  -1                 },
  {"xmm2",  2 | 4<<3,                     2 | 4<<3,                     -1,                  -1                 },
  {"xmm3",  3 | 4<<3,                     3 | 4<<3,                     -1,                  -1                 },
  {"xmm4",  4 | 4<<3,                     4 | 4<<3,                     -1,                  -1                 },
  {"xmm5",  5 | 4<<3,                     5 | 4<<3,                     -1,                  -1                 },
  {"xmm6",  6 | 4<<3,                     6 | 4<<3,                     -1,                  -1                 },
  {"xmm7",  7 | 4<<3,                     7 | 4<<3,                     -1,                  -1                 },
  {"xmm8",  0 | 4<<3 | REXR<<8,           0 | 4<<3 | REXB<<8,           -1,                  -1                 },
  {"xmm9",  1 | 4<<3 | REXR<<8,           1 | 4<<3 | REXB<<8,           -1,                  -1                 },
} /* clang-format on */;

long as_hashmap_hits;
long as_hashmap_miss;

static bool IsPunctMergeable(int c) {
  switch (c) {
    case ',':
    case ';':
    case '$':
      return false;
    default:
      return true;
  }
}

static char *PunctToStr(int p, char b[4]) {
  int c, i, j;
  bzero(b, 4);
  for (j = 0, i = 2; i >= 0; --i) {
    if ((c = (p >> (i * 8)) & 0xff)) {
      b[j++] = c;
    }
  }
  return b;
}

static void PrintSlice(struct Slice s) {
  fprintf(stderr, "%.*s\n", s.n, s.p);
}

static char *SaveString(struct Strings *l, char *p) {
  APPEND((*l));
  l->p[l->n - 1] = p;
  return p;
}

static int StrDup(struct As *a, const char *s) {
  SaveString(&a->strings, strdup(s));
  return a->strings.n - 1;
}

static int SliceDup(struct As *a, struct Slice s) {
  SaveString(&a->strings, strndup(s.p, s.n));
  return a->strings.n - 1;
}

static int AppendSauce(struct As *a, int path, int line) {
  if (!a->sauces.n || (line != a->sauces.p[a->sauces.n - 1].line ||
                       path != a->sauces.p[a->sauces.n - 1].path)) {
    APPEND(a->sauces);
    a->sauces.p[a->sauces.n - 1].path = path;
    a->sauces.p[a->sauces.n - 1].line = line;
  }
  return a->sauces.n - 1;
}

static void AppendExpr(struct As *a) {
  APPEND(a->exprs);
  bzero(a->exprs.p + a->exprs.n - 1, sizeof(*a->exprs.p));
  a->exprs.p[a->exprs.n - 1].tok = a->i;
  a->exprs.p[a->exprs.n - 1].lhs = -1;
  a->exprs.p[a->exprs.n - 1].rhs = -1;
}

static void AppendThing(struct As *a) {
  APPEND(a->things);
  bzero(a->things.p + a->things.n - 1, sizeof(*a->things.p));
}

static void AppendRela(struct As *a) {
  APPEND(a->relas);
  bzero(a->relas.p + a->relas.n - 1, sizeof(*a->relas.p));
}

static void AppendSlice(struct As *a) {
  APPEND(a->slices);
  bzero(a->slices.p + a->slices.n - 1, sizeof(*a->slices.p));
}

static int AppendSection(struct As *a, int name, int flags, int type) {
  int i;
  APPEND(a->sections);
  i = a->sections.n - 1;
  CHECK_LT(i, SHN_LORESERVE);
  a->sections.p[i].name = name;
  a->sections.p[i].flags = flags;
  a->sections.p[i].type = type;
  a->sections.p[i].align = 1;
  a->sections.p[i].binary.p = NULL;
  a->sections.p[i].binary.n = 0;
  return i;
}

static struct As *NewAssembler(void) {
  struct As *a = calloc(1, sizeof(struct As));
  AppendSlice(a);
  AppendSection(a, StrDup(a, ""), 0, SHT_NULL);
  AppendSection(a, StrDup(a, ".text"), SHF_ALLOC | SHF_EXECINSTR, SHT_PROGBITS);
  AppendSection(a, StrDup(a, ".data"), SHF_ALLOC | SHF_WRITE, SHT_PROGBITS);
  AppendSection(a, StrDup(a, ".bss"), SHF_ALLOC | SHF_WRITE, SHT_NOBITS);
  a->section = 1;
  return a;
}

static void FreeAssembler(struct As *a) {
  int i;
  for (i = 0; i < a->sections.n; ++i) free(a->sections.p[i].binary.p);
  for (i = 0; i < a->strings.n; ++i) free(a->strings.p[i]);
  for (i = 0; i < a->incpaths.n; ++i) free(a->incpaths.p[i]);
  free(a->ints.p);
  free(a->floats.p);
  free(a->slices.p);
  free(a->sauces.p);
  free(a->things.p);
  free(a->sections.p);
  free(a->symbols.p);
  free(a->symbolindex.p);
  free(a->labels.p);
  free(a->relas.p);
  free(a->exprs.p);
  free(a->strings.p);
  free(a->incpaths.p);
  free(a->sectionstack.p);
  free(a);
}

static void ReadFlags(struct As *a, int argc, char *argv[]) {
  int i;
  a->inpath = StrDup(a, "-");
  a->outpath = StrDup(a, "a.out");
  for (i = 1; i < argc; ++i) {
    if (!strcmp(argv[i], "-o")) {
      a->outpath = StrDup(a, argv[++i]);
    } else if (startswith(argv[i], "-o")) {
      a->outpath = StrDup(a, argv[i] + 2);
    } else if (!strcmp(argv[i], "-I")) {
      SaveString(&a->incpaths, strdup(argv[++i]));
    } else if (startswith(argv[i], "-I")) {
      SaveString(&a->incpaths, strdup(argv[i] + 2));
    } else if (!strcmp(argv[i], "-Z")) {
      a->inhibiterr = true;
    } else if (!strcmp(argv[i], "-W")) {
      a->inhibitwarn = true;
    } else if (argv[i][0] != '-') {
      a->inpath = StrDup(a, argv[i]);
    }
  }
}

static int ReadCharLiteral(struct Slice *buf, int c, char *p, int *i) {
  int x;
  if (c != '\\') return c;
  switch ((c = p[(*i)++])) {
    case 'a':
      return '\a';
    case 'b':
      return '\b';
    case 't':
      return '\t';
    case 'n':
      return '\n';
    case 'v':
      return '\v';
    case 'f':
      return '\f';
    case 'r':
      return '\r';
    case 'e':
      return 033;
    case 'x':
      if ((x = kHexToInt[p[*i] & 255]) != -1) {
        *i += 1, c = x;
        if ((x = kHexToInt[p[*i] & 255]) != -1) {
          *i += 1, c = c << 4 | x;
        }
      }
      return c;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
      c -= '0';
      if ('0' <= p[*i] && p[*i] <= '7') {
        c = c * 8 + (p[(*i)++] - '0');
        if ('0' <= p[*i] && p[*i] <= '7') {
          c = c * 8 + (p[(*i)++] - '0');
        }
      }
      return c;
    default:
      return c;
  }
}

static void PrintLocation(struct As *a) {
  fprintf(stderr,
          "%s:%d:: ", a->strings.p[a->sauces.p[a->things.p[a->i].s].path],
          a->sauces.p[a->things.p[a->i].s].line);
}

static wontreturn void Fail(struct As *a, const char *fmt, ...) {
  va_list va;
  PrintLocation(a);
  va_start(va, fmt);
  vfprintf(stderr, fmt, va);
  va_end(va);
  fputc('\n', stderr);
  __die();
}

static wontreturn void InvalidRegister(struct As *a) {
  Fail(a, "invalid register");
}

static char *FindInclude(struct As *a, const char *file) {
  int i;
  char *path;
  struct stat st;
  for (i = 0; i < a->incpaths.n; ++i) {
    path = xjoinpaths(a->incpaths.p[i], file);
    if (stat(path, &st) != -1 && S_ISREG(st.st_mode)) return path;
    free(path);
  }
  return NULL;
}

static void Tokenize(struct As *a, int path) {
  int c, i, line;
  char *p, *path2;
  struct Slice buf;
  bool bol, isfloat, isfpu;
  if (!fileexists(a->strings.p[path])) {
    fprintf(stderr, "%s: file not found\n", a->strings.p[path]);
    exit(1);
  }
  p = SaveString(&a->strings, read_file(a->strings.p[path]));
  p = skip_bom(p);
  canonicalize_newline(p);
  remove_backslash_newline(p);
  line = 1;
  bol = true;
  while ((c = *p)) {
    if (c == '/' && p[1] == '*') {
      for (i = 2; p[i]; ++i) {
        if (p[i] == '\n') {
          ++line;
          bol = true;
        } else {
          bol = false;
          if (p[i] == '*' && p[i + 1] == '/') {
            i += 2;
            break;
          }
        }
      }
      p += i;
      continue;
    }
    if (c == '#' || (c == '/' && bol) || (c == '/' && p[1] == '/')) {
      p = strchr(p, '\n');
      continue;
    }
    if (c == '\n') {
      AppendThing(a);
      a->things.p[a->things.n - 1].t = TT_PUNCT;
      a->things.p[a->things.n - 1].s = AppendSauce(a, path, line);
      a->things.p[a->things.n - 1].i = ';';
      ++p;
      bol = true;
      ++line;
      continue;
    }
    bol = false;
    if (c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\f' ||
        c == '\v') {
      ++p;
      continue;
    }
    if ((c & 0x80) || ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') ||
        c == '_' || c == '%' || c == '@' ||
        (c == '.' && !('0' <= p[1] && p[1] <= '9'))) {
      isfpu = false;
      for (i = 1;; ++i) {
        if (!((p[i] & 0x80) || ('a' <= p[i] && p[i] <= 'z') ||
              ('A' <= p[i] && p[i] <= 'Z') || ('0' <= p[i] && p[i] <= '9') ||
              p[i] == '.' || p[i] == '_' || p[i] == '$' ||
              (isfpu && (p[i] == '(' || p[i] == ')')))) {
          break;
        }
        if (i == 2 && p[i - 2] == '%' && p[i - 1] == 's' && p[i] == 't') {
          isfpu = true;
        }
      }
      if (i == 4 && !strncasecmp(p, ".end", 4)) break;
      AppendThing(a);
      a->things.p[a->things.n - 1].t = TT_SLICE;
      a->things.p[a->things.n - 1].s = AppendSauce(a, path, line);
      a->things.p[a->things.n - 1].i = a->slices.n;
      AppendSlice(a);
      a->slices.p[a->slices.n - 1].p = p;
      a->slices.p[a->slices.n - 1].n = i;
      p += i;
      continue;
    }
    if (('0' <= c && c <= '9') || (c == '.' && '0' <= p[1] && p[1] <= '9')) {
      isfloat = c == '.';
      if (c == '0' && p[1] != '.') {
        if (p[1] == 'x' || p[1] == 'X') {
          for (i = 2;; ++i) {
            if (!(('0' <= p[i] && p[i] <= '9') ||
                  ('a' <= p[i] && p[i] <= 'f') ||
                  ('A' <= p[i] && p[i] <= 'F'))) {
              break;
            }
          }
        } else if ((p[1] == 'b' || p[1] == 'B') &&
                   ('0' <= p[2] && p[2] <= '9')) {
          for (i = 2;; ++i) {
            if (!(p[i] == '0' || p[i] == '1')) break;
          }
        } else {
          for (i = 1;; ++i) {
            if (!('0' <= p[i] && p[i] <= '7')) break;
          }
        }
      } else {
        for (i = 1;; ++i) {
          if (('0' <= p[i] && p[i] <= '9') || p[i] == '-' || p[i] == '+') {
            continue;
          } else if (p[i] == '.' || p[i] == 'e' || p[i] == 'E' || p[i] == 'e') {
            isfloat = true;
            continue;
          }
          break;
        }
      }
      AppendThing(a);
      if (isfloat) {
        APPEND(a->floats);
        a->floats.p[a->floats.n - 1] = strtold(p, NULL);
        a->things.p[a->things.n - 1].i = a->floats.n - 1;
        a->things.p[a->things.n - 1].t = TT_FLOAT;
      } else {
        APPEND(a->ints);
        a->ints.p[a->ints.n - 1] = strtoumax(p, NULL, 0);
        a->things.p[a->things.n - 1].i = a->ints.n - 1;
        if (p[i] == 'f' || p[i] == 'F') {
          a->things.p[a->things.n - 1].t = TT_FORWARD;
          ++i;
        } else if (p[i] == 'b' || p[i] == 'B') {
          a->things.p[a->things.n - 1].t = TT_BACKWARD;
          ++i;
        } else {
          a->things.p[a->things.n - 1].t = TT_INT;
        }
      }
      a->things.p[a->things.n - 1].s = AppendSauce(a, path, line);
      p += i;
      continue;
    }
    if (c == '\'') {
      i = 1;
      c = p[i++];
      c = ReadCharLiteral(&buf, c, p, &i);
      if (p[i] == '\'') ++i;
      p += i;
      AppendThing(a);
      a->things.p[a->things.n - 1].t = TT_INT;
      a->things.p[a->things.n - 1].s = AppendSauce(a, path, line);
      a->things.p[a->things.n - 1].i = a->ints.n;
      APPEND(a->ints);
      a->ints.p[a->ints.n - 1] = c;
      continue;
    }
    if (c == '"') {
      bzero(&buf, sizeof(buf));
      for (i = 1; (c = p[i++]);) {
        if (c == '"') break;
        c = ReadCharLiteral(&buf, c, p, &i);
        APPEND(buf);
        buf.p[buf.n - 1] = c;
      }
      p += i;
      if (a->things.n && a->things.p[a->things.n - 1].t == TT_SLICE &&
          IS(a->slices.p[a->things.p[a->things.n - 1].i].p,
             a->slices.p[a->things.p[a->things.n - 1].i].n, ".include")) {
        APPEND(buf);
        buf.p[buf.n - 1] = '\0';
        --a->things.n;
        if ((path2 = FindInclude(a, buf.p))) {
          Tokenize(a, StrDup(a, path2));
          free(path2);
          free(buf.p);
        } else {
          Fail(a, "not found: %s", buf.p);
        }
      } else {
        SaveString(&a->strings, buf.p);
        AppendThing(a);
        a->things.p[a->things.n - 1].t = TT_SLICE;
        a->things.p[a->things.n - 1].s = AppendSauce(a, path, line);
        a->things.p[a->things.n - 1].i = a->slices.n;
        AppendSlice(a);
        a->slices.p[a->slices.n - 1] = buf;
      }
      continue;
    }
    if (IsPunctMergeable(c) && a->things.n &&
        a->things.p[a->things.n - 1].t == TT_PUNCT &&
        IsPunctMergeable(a->things.p[a->things.n - 1].i)) {
      a->things.p[a->things.n - 1].i = a->things.p[a->things.n - 1].i << 8 | c;
    } else {
      AppendThing(a);
      a->things.p[a->things.n - 1].t = TT_PUNCT;
      a->things.p[a->things.n - 1].s = AppendSauce(a, path, line);
      a->things.p[a->things.n - 1].i = c;
    }
    ++p;
  }
}

static int GetSymbol(struct As *a, int name) {
  struct HashEntry *p;
  unsigned i, j, k, n, m, h;
  if (!(h = crc32c(0, a->slices.p[name].p, a->slices.p[name].n))) h = 1;
  n = a->symbolindex.n;
  i = 0;
  if (n) {
    k = 0;
    for (;;) {
      i = (h + k + ((k + 1) >> 1)) & (n - 1);
      if (a->symbolindex.p[i].h == h &&
          a->slices.p[a->symbols.p[a->symbolindex.p[i].i].name].n ==
              a->slices.p[name].n &&
          !memcmp(a->slices.p[a->symbols.p[a->symbolindex.p[i].i].name].p,
                  a->slices.p[name].p, a->slices.p[name].n)) {
        ++as_hashmap_hits;
        return a->symbolindex.p[i].i;
      }
      if (!a->symbolindex.p[i].h) {
        break;
      } else {
        ++k;
        ++as_hashmap_miss;
      }
    }
  }
  if (++a->symbolindex.i >= (n >> 1)) {
    m = n ? n << 1 : 16;
    p = calloc(m, sizeof(struct HashEntry));
    for (j = 0; j < n; ++j) {
      if (a->symbolindex.p[j].h) {
        k = 0;
        do {
          i = (a->symbolindex.p[j].h + k + ((k + 1) >> 1)) & (m - 1);
          ++k;
        } while (p[i].h);
        p[i].h = a->symbolindex.p[j].h;
        p[i].i = a->symbolindex.p[j].i;
      }
    }
    k = 0;
    do {
      i = (h + k + ((k + 1) >> 1)) & (m - 1);
      ++k;
    } while (p[i].h);
    free(a->symbolindex.p);
    a->symbolindex.p = p;
    a->symbolindex.n = m;
  }
  APPEND(a->symbols);
  memset(a->symbols.p + a->symbols.n - 1, 0, sizeof(*a->symbols.p));
  a->symbolindex.p[i].h = h;
  a->symbolindex.p[i].i = a->symbols.n - 1;
  a->symbols.p[a->symbols.n - 1].name = name;
  return a->symbols.n - 1;
}

static void OnSymbol(struct As *a, int name) {
  int i = GetSymbol(a, name);
  if (a->symbols.p[i].section) {
    Fail(a, "already defined: %.*s", a->slices.p[name].n, a->slices.p[name].p);
  }
  a->symbols.p[i].section = a->section;
  a->symbols.p[i].offset = a->sections.p[a->section].binary.n;
  a->i += 2;
}

static void OnLocalLabel(struct As *a, int id) {
  int i;
  char *name;
  name = xasprintf(".Label.%d", a->counter++);
  SaveString(&a->strings, name);
  AppendSlice(a);
  a->slices.p[a->slices.n - 1].p = name;
  a->slices.p[a->slices.n - 1].n = strlen(name);
  i = GetSymbol(a, a->slices.n - 1);
  a->symbols.p[i].section = a->section;
  a->symbols.p[i].offset = a->sections.p[a->section].binary.n;
  APPEND(a->labels);
  a->labels.p[a->labels.n - 1].id = id;
  a->labels.p[a->labels.n - 1].tok = a->i;
  a->labels.p[a->labels.n - 1].symbol = i;
  a->i += 2;
}

static void SetSection(struct As *a, int section) {
  a->previous = a->section;
  a->section = section;
}

static bool IsInt(struct As *a, int i) {
  return a->things.p[i].t == TT_INT;
}
static bool IsFloat(struct As *a, int i) {
  return a->things.p[i].t == TT_FLOAT;
}
static bool IsSlice(struct As *a, int i) {
  return a->things.p[i].t == TT_SLICE;
}
static bool IsPunct(struct As *a, int i, int c) {
  return a->things.p[i].t == TT_PUNCT && a->things.p[i].i == c;
}
static bool IsForward(struct As *a, int i) {
  return a->things.p[i].t == TT_FORWARD;
}
static bool IsBackward(struct As *a, int i) {
  return a->things.p[i].t == TT_BACKWARD;
}
static bool IsComma(struct As *a) {
  return IsPunct(a, a->i, ',');
}
static bool IsSemicolon(struct As *a) {
  return IsPunct(a, a->i, ';');
}

static bool IsRegister(struct As *a, int i) {
  return IsSlice(a, i) && (a->slices.p[a->things.p[i].i].n &&
                           *a->slices.p[a->things.p[i].i].p == '%');
}

static void ConsumePunct(struct As *a, int c) {
  char pb[4];
  if (IsPunct(a, a->i, c)) {
    ++a->i;
  } else {
    Fail(a, "expected %s", PunctToStr(c, pb));
  }
}

static void ConsumeComma(struct As *a) {
  ConsumePunct(a, ',');
}

static int NewPrimary(struct As *a, enum ExprKind k, int128_t x) {
  AppendExpr(a);
  a->exprs.p[a->exprs.n - 1].kind = k;
  a->exprs.p[a->exprs.n - 1].x = x;
  return a->exprs.n - 1;
}

static int NewUnary(struct As *a, enum ExprKind k, int lhs) {
  AppendExpr(a);
  a->exprs.p[a->exprs.n - 1].kind = k;
  a->exprs.p[a->exprs.n - 1].lhs = lhs;
  return a->exprs.n - 1;
}

static int NewBinary(struct As *a, enum ExprKind k, int lhs, int rhs) {
  AppendExpr(a);
  a->exprs.p[a->exprs.n - 1].kind = k;
  a->exprs.p[a->exprs.n - 1].lhs = lhs;
  a->exprs.p[a->exprs.n - 1].rhs = rhs;
  return a->exprs.n - 1;
}

// primary = int
//         | symbol
//         | reference
static int ParsePrimary(struct As *a, int *rest, int i) {
  if (IsInt(a, i)) {
    *rest = i + 1;
    return NewPrimary(a, EX_INT, a->ints.p[a->things.p[i].i]);
  } else if (IsForward(a, i) || IsBackward(a, i) ||
             (IsSlice(a, i) && (a->slices.p[a->things.p[i].i].n &&
                                a->slices.p[a->things.p[i].i].p[0] != '%' &&
                                a->slices.p[a->things.p[i].i].p[0] != '@'))) {
    *rest = i + 1;
    return NewPrimary(a, EX_SYM, i);
  } else {
    Fail(a, "expected int or label");
  }
}

// postfix = primary "@gotpcrel"
//         | primary "@dtpoff"
//         | primary "@tpoff"
//         | primary
static int ParsePostfix(struct As *a, int *rest, int i) {
  int x;
  struct Slice suffix;
  x = ParsePrimary(a, &i, i);
  if (IsSlice(a, i)) {
    suffix = a->slices.p[a->things.p[i].i];
    if (suffix.n && suffix.p[0] == '@') {
      if (IS(suffix.p, suffix.n, "@gotpcrel")) {
        a->exprs.p[x].em = EM_GOTPCREL;
        ++i;
      } else if (IS(suffix.p, suffix.n, "@dtpoff")) {
        a->exprs.p[x].em = EM_DTPOFF;
        ++i;
      } else if (IS(suffix.p, suffix.n, "@tpoff")) {
        a->exprs.p[x].em = EM_TPOFF;
        ++i;
      }
    }
  }
  *rest = i;
  return x;
}

// unary = ("+" | "-" | "!" | "~") unary
//       | postfix
static int ParseUnary(struct As *a, int *rest, int i) {
  int x;
  if (IsPunct(a, i, '+')) {
    x = ParseUnary(a, rest, i + 1);
  } else if (IsPunct(a, i, '-')) {
    x = ParseUnary(a, rest, i + 1);
    if (a->exprs.p[x].kind == EX_INT) {
      a->exprs.p[x].x = -a->exprs.p[x].x;
    } else {
      x = NewPrimary(a, EX_NEG, x);
    }
  } else if (IsPunct(a, i, '!')) {
    x = ParseUnary(a, rest, i + 1);
    if (a->exprs.p[x].kind == EX_INT) {
      a->exprs.p[x].x = !a->exprs.p[x].x;
    } else {
      x = NewPrimary(a, EX_NOT, x);
    }
  } else if (IsPunct(a, i, '~')) {
    x = ParseUnary(a, rest, i + 1);
    if (a->exprs.p[x].kind == EX_INT) {
      a->exprs.p[x].x = ~a->exprs.p[x].x;
    } else {
      x = NewPrimary(a, EX_BITNOT, x);
    }
  } else {
    x = ParsePostfix(a, rest, i);
  }
  return x;
}

// mul = unary ("*" unary | "/" unary | "%" unary)*
static int ParseMul(struct As *a, int *rest, int i) {
  int x, y;
  x = ParseUnary(a, &i, i);
  for (;;) {
    if (IsPunct(a, i, '*')) {
      y = ParseUnary(a, &i, i + 1);
      if (a->exprs.p[x].kind == EX_INT && a->exprs.p[y].kind == EX_INT) {
        a->exprs.p[x].x *= a->exprs.p[y].x;
      } else {
        x = NewBinary(a, EX_MUL, x, y);
      }
    } else if (IsPunct(a, i, '/')) {
      y = ParseUnary(a, &i, i + 1);
      if (a->exprs.p[x].kind == EX_INT && a->exprs.p[y].kind == EX_INT) {
        a->exprs.p[x].x /= a->exprs.p[y].x;
      } else {
        x = NewBinary(a, EX_DIV, x, y);
      }
    } else if (IsPunct(a, i, '%')) {
      y = ParseUnary(a, &i, i + 1);
      if (a->exprs.p[x].kind == EX_INT && a->exprs.p[y].kind == EX_INT) {
        a->exprs.p[x].x %= a->exprs.p[y].x;
      } else {
        x = NewBinary(a, EX_REM, x, y);
      }
    } else {
      *rest = i;
      return x;
    }
  }
}

// add = mul ("+" mul | "-" mul)*
static int ParseAdd(struct As *a, int *rest, int i) {
  int x, y;
  x = ParseMul(a, &i, i);
  for (;;) {
    if (IsPunct(a, i, '+')) {
      y = ParseMul(a, &i, i + 1);
      if (a->exprs.p[x].kind == EX_INT && a->exprs.p[y].kind == EX_INT) {
        a->exprs.p[x].x += a->exprs.p[y].x;
      } else {
        x = NewBinary(a, EX_ADD, x, y);
      }
    } else if (IsPunct(a, i, '-')) {
      y = ParseMul(a, &i, i + 1);
      if (a->exprs.p[x].kind == EX_INT && a->exprs.p[y].kind == EX_INT) {
        a->exprs.p[x].x -= a->exprs.p[y].x;
      } else if (a->exprs.p[y].kind == EX_INT) {
        a->exprs.p[y].x = -a->exprs.p[y].x;
        x = NewBinary(a, EX_ADD, x, y);
      } else {
        x = NewBinary(a, EX_SUB, x, y);
      }
    } else {
      *rest = i;
      return x;
    }
  }
}

// shift = add ("<<" add | ">>" add)*
static int ParseShift(struct As *a, int *rest, int i) {
  int x, y;
  x = ParseAdd(a, &i, i);
  for (;;) {
    if (IsPunct(a, i, '<' << 8 | '<')) {
      y = ParseAdd(a, &i, i + 1);
      if (a->exprs.p[x].kind == EX_INT && a->exprs.p[y].kind == EX_INT) {
        a->exprs.p[x].x <<= a->exprs.p[y].x & 63;
      } else {
        x = NewBinary(a, EX_SHL, x, y);
      }
    } else if (IsPunct(a, i, '>' << 8 | '>')) {
      y = ParseAdd(a, &i, i + 1);
      if (a->exprs.p[x].kind == EX_INT && a->exprs.p[y].kind == EX_INT) {
        a->exprs.p[x].x >>= a->exprs.p[y].x & 63;
      } else {
        x = NewBinary(a, EX_SHR, x, y);
      }
    } else {
      *rest = i;
      return x;
    }
  }
}

// relational = shift ("<" shift | "<=" shift | ">" shift | ">=" shift)*
static int ParseRelational(struct As *a, int *rest, int i) {
  int x, y;
  x = ParseShift(a, &i, i);
  for (;;) {
    if (IsPunct(a, i, '<')) {
      y = ParseShift(a, &i, i + 1);
      if (a->exprs.p[x].kind == EX_INT && a->exprs.p[y].kind == EX_INT) {
        a->exprs.p[x].x = a->exprs.p[x].x < a->exprs.p[y].x;
      } else {
        x = NewBinary(a, EX_LT, x, y);
      }
    } else if (IsPunct(a, i, '>')) {
      y = ParseShift(a, &i, i + 1);
      if (a->exprs.p[x].kind == EX_INT && a->exprs.p[y].kind == EX_INT) {
        a->exprs.p[x].x = a->exprs.p[y].x < a->exprs.p[x].x;
      } else {
        x = NewBinary(a, EX_LT, y, x);
      }
    } else if (IsPunct(a, i, '<' << 8 | '=')) {
      y = ParseShift(a, &i, i + 1);
      if (a->exprs.p[x].kind == EX_INT && a->exprs.p[y].kind == EX_INT) {
        a->exprs.p[x].x = a->exprs.p[x].x <= a->exprs.p[y].x;
      } else {
        x = NewBinary(a, EX_LE, x, y);
      }
    } else if (IsPunct(a, i, '>' << 8 | '=')) {
      y = ParseShift(a, &i, i + 1);
      if (a->exprs.p[x].kind == EX_INT && a->exprs.p[y].kind == EX_INT) {
        a->exprs.p[x].x = a->exprs.p[y].x <= a->exprs.p[x].x;
      } else {
        x = NewBinary(a, EX_LE, y, x);
      }
    } else {
      *rest = i;
      return x;
    }
  }
}

// equality = relational ("==" relational | "!=" relational)*
static int ParseEquality(struct As *a, int *rest, int i) {
  int x, y;
  x = ParseRelational(a, &i, i);
  for (;;) {
    if (IsPunct(a, i, '=' << 8 | '=')) {
      y = ParseRelational(a, &i, i + 1);
      if (a->exprs.p[x].kind == EX_INT && a->exprs.p[y].kind == EX_INT) {
        a->exprs.p[x].x = a->exprs.p[x].x == a->exprs.p[y].x;
      } else {
        x = NewBinary(a, EX_EQ, x, y);
      }
    } else if (IsPunct(a, i, '!' << 8 | '=')) {
      y = ParseRelational(a, &i, i + 1);
      if (a->exprs.p[x].kind == EX_INT && a->exprs.p[y].kind == EX_INT) {
        a->exprs.p[x].x = a->exprs.p[x].x != a->exprs.p[y].x;
      } else {
        x = NewBinary(a, EX_NE, x, y);
      }
    } else {
      *rest = i;
      return x;
    }
  }
}

// and = equality ("&" equality)*
static int ParseAnd(struct As *a, int *rest, int i) {
  int x, y;
  x = ParseEquality(a, &i, i);
  for (;;) {
    if (IsPunct(a, i, '&')) {
      y = ParseEquality(a, &i, i + 1);
      if (a->exprs.p[x].kind == EX_INT && a->exprs.p[y].kind == EX_INT) {
        a->exprs.p[x].x &= a->exprs.p[y].x;
      } else {
        x = NewBinary(a, EX_AND, x, y);
      }
    } else {
      *rest = i;
      return x;
    }
  }
}

// xor = and ("^" and)*
static int ParseXor(struct As *a, int *rest, int i) {
  int x, y;
  x = ParseAnd(a, &i, i);
  for (;;) {
    if (IsPunct(a, i, '^')) {
      y = ParseAnd(a, &i, i + 1);
      if (a->exprs.p[x].kind == EX_INT && a->exprs.p[y].kind == EX_INT) {
        a->exprs.p[x].x ^= a->exprs.p[y].x;
      } else {
        x = NewBinary(a, EX_XOR, x, y);
      }
    } else {
      *rest = i;
      return x;
    }
  }
}

// or = xor ("|" xor)*
static int ParseOr(struct As *a, int *rest, int i) {
  int x, y;
  x = ParseXor(a, &i, i);
  for (;;) {
    if (IsPunct(a, i, '|')) {
      y = ParseXor(a, &i, i + 1);
      if (a->exprs.p[x].kind == EX_INT && a->exprs.p[y].kind == EX_INT) {
        a->exprs.p[x].x |= a->exprs.p[y].x;
      } else {
        x = NewBinary(a, EX_OR, x, y);
      }
    } else {
      *rest = i;
      return x;
    }
  }
}

static int Parse(struct As *a) {
  return ParseOr(a, &a->i, a->i);
}

static int128_t GetInt(struct As *a) {
  int x;
  x = Parse(a);
  if (a->exprs.p[x].kind == EX_INT) {
    return a->exprs.p[x].x;
  } else {
    Fail(a, "expected constexpr int");
  }
}

static long double GetFloat(struct As *a) {
  long double res;
  if (IsFloat(a, a->i)) {
    res = a->floats.p[a->things.p[a->i].i];
    ++a->i;
    return res;
  } else {
    Fail(a, "expected float");
  }
}

static struct Slice GetSlice(struct As *a) {
  struct Slice res;
  if (IsSlice(a, a->i)) {
    res = a->slices.p[a->things.p[a->i].i];
    ++a->i;
    return res;
  } else {
    Fail(a, "expected string");
  }
}

static void EmitData(struct As *a, const void *p, uint128_t n) {
  struct Slice *s;
  s = &a->sections.p[a->section].binary;
  s->p = realloc(s->p, s->n + n);
  if (n) memcpy(s->p + s->n, p, n);
  s->n += n;
}

static void EmitByte(struct As *a, uint128_t i) {
  uint8_t x = i;
  unsigned char b[1];
  b[0] = (x & 0xff) >> 000;
  EmitData(a, b, 1);
}

static void EmitWord(struct As *a, uint128_t i) {
  uint16_t x = i;
  unsigned char b[2];
  b[0] = (x & 0x00ff) >> 000;
  b[1] = (x & 0xff00) >> 010;
  EmitData(a, b, 2);
}

static void EmitLong(struct As *a, uint128_t i) {
  uint32_t x = i;
  unsigned char b[4];
  b[0] = (x & 0x000000ff) >> 000;
  b[1] = (x & 0x0000ff00) >> 010;
  b[2] = (x & 0x00ff0000) >> 020;
  b[3] = (x & 0xff000000) >> 030;
  EmitData(a, b, 4);
}

void EmitQuad(struct As *a, uint128_t i) {
  uint64_t x = i;
  unsigned char b[8];
  b[0] = (x & 0x00000000000000ff) >> 000;
  b[1] = (x & 0x000000000000ff00) >> 010;
  b[2] = (x & 0x0000000000ff0000) >> 020;
  b[3] = (x & 0x00000000ff000000) >> 030;
  b[4] = (x & 0x000000ff00000000) >> 040;
  b[5] = (x & 0x0000ff0000000000) >> 050;
  b[6] = (x & 0x00ff000000000000) >> 060;
  b[7] = (x & 0xff00000000000000) >> 070;
  EmitData(a, b, 8);
}

void EmitOcta(struct As *a, uint128_t i) {
  EmitQuad(a, i);
  EmitQuad(a, i >> 64);
}

static void EmitVarword(struct As *a, unsigned long x) {
  if (x > 255) EmitVarword(a, x >> 8);
  EmitByte(a, x);
}

static void OnSleb128(struct As *a, struct Slice s) {
  int c;
  int128_t x;
  for (;;) {
    x = GetInt(a);
    for (;;) {
      c = x & 0x7f;
      x >>= 7;
      if ((x == 0 && !(c & 0x40)) || (x == -1 && (c & 0x40))) {
        break;
      } else {
        c |= 0x80;
      }
      EmitByte(a, c);
      if (IsSemicolon(a)) break;
      ConsumeComma(a);
    }
  }
}

static void OnUleb128(struct As *a, struct Slice s) {
  int c;
  uint128_t x;
  for (;;) {
    x = GetInt(a);
    do {
      c = x & 0x7f;
      x >>= 7;
      if (x) c |= 0x80;
      EmitByte(a, c);
    } while (x);
    if (IsSemicolon(a)) break;
    ConsumeComma(a);
  }
}

static void OnZleb128(struct As *a, struct Slice s) {
  int c;
  uint128_t x;
  for (;;) {
    x = GetInt(a);
    x = (x << 1) ^ ((int128_t)x >> 127);
    do {
      c = x & 0x7f;
      x >>= 7;
      if (x) c |= 0x80;
      EmitByte(a, c);
    } while (x);
    if (IsSemicolon(a)) break;
    ConsumeComma(a);
  }
}

static void OnZero(struct As *a, struct Slice s) {
  long n;
  char *p;
  for (;;) {
    n = GetInt(a);
    p = calloc(n, 1);
    EmitData(a, p, n);
    free(p);
    if (IsSemicolon(a)) break;
    ConsumeComma(a);
  }
}

static void OnSpace(struct As *a, struct Slice s) {
  long n;
  char *p;
  int fill;
  p = malloc((n = GetInt(a)));
  if (IsComma(a)) {
    ConsumeComma(a);
    fill = GetInt(a);
  } else {
    fill = 0;
  }
  memset(p, fill, n);
  EmitData(a, p, n);
  free(p);
}

static long GetRelaAddend(int kind) {
  switch (kind) {
    case R_X86_64_PC8:
      return -1;
    case R_X86_64_PC16:
      return -2;
    case R_X86_64_PC32:
    case R_X86_64_PLT32:
    case R_X86_64_GOTPCRELX:
      return -4;
    default:
      return 0;
  }
}

static void EmitExpr(struct As *a, int expr, int kind,
                     void emitter(struct As *, uint128_t)) {
  if (expr == -1) {
    emitter(a, 0);
  } else if (a->exprs.p[expr].kind == EX_INT) {
    emitter(a, a->exprs.p[expr].x);
  } else {
    AppendRela(a);
    a->relas.p[a->relas.n - 1].kind = kind;
    a->relas.p[a->relas.n - 1].expr = expr;
    a->relas.p[a->relas.n - 1].section = a->section;
    a->relas.p[a->relas.n - 1].offset = a->sections.p[a->section].binary.n;
    a->relas.p[a->relas.n - 1].addend = GetRelaAddend(kind);
    emitter(a, 0);
  }
}

static void OpInt(struct As *a, int kind,
                  void emitter(struct As *, uint128_t)) {
  for (;;) {
    EmitExpr(a, Parse(a), kind, emitter);
    if (IsSemicolon(a)) break;
    ConsumeComma(a);
  }
}

static void OnByte(struct As *a, struct Slice s) {
  OpInt(a, R_X86_64_8, EmitByte);
}

static void OnWord(struct As *a, struct Slice s) {
  OpInt(a, R_X86_64_16, EmitWord);
}

static void OnLong(struct As *a, struct Slice s) {
  OpInt(a, R_X86_64_32, EmitLong);
}

static void OnQuad(struct As *a, struct Slice s) {
  OpInt(a, R_X86_64_64, EmitQuad);
}

static void OnOcta(struct As *a, struct Slice s) {
  OpInt(a, R_X86_64_64, EmitOcta);
}

static void OnFloat(struct As *a, struct Slice s) {
  float f;
  char b[4];
  for (;;) {
    if (IsFloat(a, a->i)) {
      f = GetFloat(a);
    } else {
      f = GetInt(a);
    }
    memcpy(b, &f, 4);
    EmitData(a, b, 4);
    if (IsSemicolon(a)) break;
    ConsumeComma(a);
  }
}

static void OnDouble(struct As *a, struct Slice s) {
  double f;
  char b[8];
  for (;;) {
    if (IsFloat(a, a->i)) {
      f = GetFloat(a);
    } else {
      f = GetInt(a);
    }
    memcpy(b, &f, 8);
    EmitData(a, b, 8);
    if (IsSemicolon(a)) break;
    ConsumeComma(a);
  }
}

static void OnLongDouble(struct As *a, int n) {
  char b[16];
  long double f;
  for (;;) {
    if (IsFloat(a, a->i)) {
      f = GetFloat(a);
    } else {
      f = GetInt(a);
    }
    bzero(b, 16);
    memcpy(b, &f, sizeof(f));
    EmitData(a, b, n);
    if (IsSemicolon(a)) break;
    ConsumeComma(a);
  }
}

static void OnFloat80(struct As *a, struct Slice s) {
  OnLongDouble(a, 10);
}

static void OnLdbl(struct As *a, struct Slice s) {
  OnLongDouble(a, 16);
}

static void OnAscii(struct As *a, struct Slice s) {
  struct Slice arg;
  for (;;) {
    arg = GetSlice(a);
    EmitData(a, arg.p, arg.n);
    if (IsSemicolon(a)) break;
    ConsumeComma(a);
  }
}

static void OnAsciz(struct As *a, struct Slice s) {
  struct Slice arg;
  for (;;) {
    arg = GetSlice(a);
    EmitData(a, arg.p, arg.n);
    EmitByte(a, 0);
    if (IsSemicolon(a)) break;
    ConsumeComma(a);
  }
}

static void OnAbort(struct As *a, struct Slice s) {
  Fail(a, "aborted");
}

static void OnErr(struct As *a, struct Slice s) {
  if (a->inhibiterr) return;
  Fail(a, "error");
}

static void OnError(struct As *a, struct Slice s) {
  struct Slice msg = GetSlice(a);
  if (a->inhibiterr) return;
  Fail(a, "%.*s", msg.n, msg.p);
}

static void OnWarning(struct As *a, struct Slice s) {
  struct Slice msg = GetSlice(a);
  if (a->inhibitwarn) return;
  PrintLocation(a);
  fprintf(stderr, "%.*s\n", msg.n, msg.p);
}

static void OnText(struct As *a, struct Slice s) {
  SetSection(a, 1);
}

static void OnData(struct As *a, struct Slice s) {
  SetSection(a, 2);
}

static void OnBss(struct As *a, struct Slice s) {
  SetSection(a, 3);
}

static void OnPrevious(struct As *a, struct Slice s) {
  SetSection(a, a->previous);
}

static void OnAlign(struct As *a, struct Slice s) {
  long i, n, align, fill, maxskip;
  align = GetInt(a);
  if (!IS2POW(align)) Fail(a, "alignment not power of 2");
  fill = (a->sections.p[a->section].flags & SHF_EXECINSTR) ? 0x90 : 0;
  maxskip = 268435456;
  if (IsComma(a)) {
    ++a->i;
    fill = GetInt(a);
    if (IsComma(a)) {
      ++a->i;
      maxskip = GetInt(a);
    }
  }
  i = a->sections.p[a->section].binary.n;
  n = ROUNDUP(i, align) - i;
  if (n > maxskip) return;
  a->sections.p[a->section].align = MAX(a->sections.p[a->section].align, align);
  for (i = 0; i < n; ++i) EmitByte(a, fill);
}

static int SectionFlag(struct As *a, int c) {
  switch (c) {
    case 'a':
      return SHF_ALLOC;
    case 'w':
      return SHF_WRITE;
    case 'x':
      return SHF_EXECINSTR;
    case 'g':
      return SHF_GROUP;
    case 'M':
      return SHF_MERGE;
    case 'S':
      return SHF_STRINGS;
    case 'T':
      return SHF_TLS;
    default:
      Fail(a, "unknown section flag: %`'c", c);
  }
}

static int SectionFlags(struct As *a, struct Slice s) {
  int i, flags;
  for (flags = i = 0; i < s.n; ++i) {
    flags |= SectionFlag(a, s.p[i]);
  }
  return flags;
}

static int SectionType(struct As *a, struct Slice s) {
  if (IS(s.p, s.n, "@progbits") || IS(s.p, s.n, "SHT_PROGBITS")) {
    return SHT_PROGBITS;
  } else if (IS(s.p, s.n, "@note") || IS(s.p, s.n, "SHT_NOTE")) {
    return SHT_NOTE;
  } else if (IS(s.p, s.n, "@nobits") || IS(s.p, s.n, "SHT_NOBITS")) {
    return SHT_NOBITS;
  } else if (IS(s.p, s.n, "@preinit_array") ||
             IS(s.p, s.n, "SHT_PREINIT_ARRAY")) {
    return SHT_PREINIT_ARRAY;
  } else if (IS(s.p, s.n, "@init_array") || IS(s.p, s.n, "SHT_INIT_ARRAY")) {
    return SHT_INIT_ARRAY;
  } else if (IS(s.p, s.n, "@fini_array") || IS(s.p, s.n, "SHT_FINI_ARRAY")) {
    return SHT_FINI_ARRAY;
  } else {
    Fail(a, "unknown section type: %.*s", s.n, s.p);
  }
}

static int SymbolType(struct As *a, struct Slice s) {
  if (IS(s.p, s.n, "@object") || IS(s.p, s.n, "STT_OBJECT")) {
    return STT_OBJECT;
  } else if (IS(s.p, s.n, "@function") || IS(s.p, s.n, "STT_FUNC")) {
    return STT_FUNC;
  } else if (IS(s.p, s.n, "@common") || IS(s.p, s.n, "STT_COMMON")) {
    return STT_COMMON;
  } else if (IS(s.p, s.n, "@notype") || IS(s.p, s.n, "STT_NOTYPE")) {
    return STT_NOTYPE;
  } else if (IS(s.p, s.n, "@tls_object") || IS(s.p, s.n, "STT_TLS")) {
    return STT_TLS;
  } else {
    Fail(a, "unknown symbol type: %.*s", s.n, s.p);
  }
}

static int GrabSection(struct As *a, int name, int flags, int type, int group,
                       int comdat) {
  int i;
  for (i = 0; i < a->sections.n; ++i) {
    if (!strcmp(a->strings.p[name], a->strings.p[a->sections.p[i].name])) {
      return i;
    }
  }
  return AppendSection(a, name, flags, type);
}

static void OnSection(struct As *a, struct Slice s) {
  int name, flags, type, group = -1, comdat = -1;
  name = SliceDup(a, GetSlice(a));
  if (startswith(a->strings.p[name], ".text")) {
    flags = SHF_ALLOC | SHF_EXECINSTR;
    type = SHT_PROGBITS;
  } else if (startswith(a->strings.p[name], ".data")) {
    flags = SHF_ALLOC | SHF_WRITE;
    type = SHT_PROGBITS;
  } else if (startswith(a->strings.p[name], ".bss")) {
    flags = SHF_ALLOC | SHF_WRITE;
    type = SHT_NOBITS;
  } else {
    flags = SHF_ALLOC | SHF_EXECINSTR | SHF_WRITE;
    type = SHT_PROGBITS;
  }
  if (IsComma(a)) {
    ++a->i;
    flags = SectionFlags(a, GetSlice(a));
    if (IsComma(a)) {
      ++a->i;
      type = SectionType(a, GetSlice(a));
      if (IsComma(a)) {
        ++a->i;
        group = SectionType(a, GetSlice(a));
        if (IsComma(a)) {
          ++a->i;
          comdat = SectionType(a, GetSlice(a));
        }
      }
    }
  }
  SetSection(a, GrabSection(a, name, flags, type, group, comdat));
}

static void OnPushsection(struct As *a, struct Slice s) {
  APPEND(a->sectionstack);
  a->sectionstack.p[a->sectionstack.n - 1] = a->section;
  OnSection(a, s);
}

static void OnPopsection(struct As *a, struct Slice s) {
  if (!a->sectionstack.n) Fail(a, "stack smashed");
  a->section = a->sectionstack.p[--a->sectionstack.n];
}

static void OnIdent(struct As *a, struct Slice s) {
  struct Slice arg;
  int comment, oldsection;
  comment = GrabSection(a, StrDup(a, ".comment"), SHF_MERGE | SHF_STRINGS,
                        SHT_PROGBITS, -1, -1);
  oldsection = a->section;
  a->section = comment;
  arg = GetSlice(a);
  EmitData(a, arg.p, arg.n);
  EmitByte(a, 0);
  a->section = oldsection;
}

static void OnIncbin(struct As *a, struct Slice s) {
  int fd;
  struct stat st;
  char *path, *path2;
  struct Slice *data, arg;
  arg = GetSlice(a);
  path = strndup(arg.p, arg.n);
  if ((path2 = FindInclude(a, path))) {
    if ((fd = open(path2, O_RDONLY)) == -1 || fstat(fd, &st) == -1) {
      Fail(a, "open failed: %s", path2);
    }
    data = &a->sections.p[a->section].binary;
    data->p = realloc(data->p, data->n + st.st_size);
    if (read(fd, data->p, st.st_size) != st.st_size) {
      Fail(a, "read failed: %s", path2);
    }
    data->n += st.st_size;
    close(fd);
    free(path2);
  } else {
    Fail(a, "not found: %s", path);
  }
  free(path);
}

static void OnType(struct As *a, struct Slice s) {
  int i;
  i = GetSymbol(a, a->things.p[a->i++].i);
  ConsumeComma(a);
  a->symbols.p[i].type = SymbolType(a, GetSlice(a));
}

static void OnSize(struct As *a, struct Slice s) {
  int i;
  i = GetSymbol(a, a->things.p[a->i++].i);
  ConsumeComma(a);
  a->symbols.p[i].size = GetInt(a);
}

static void OnEqu(struct As *a, struct Slice s) {
  int i;
  i = GetSymbol(a, a->things.p[a->i++].i);
  ConsumeComma(a);
  a->symbols.p[i].offset = GetInt(a);
  a->symbols.p[i].section = SHN_ABS;
}

static void OnComm(struct As *a, struct Slice s) {
  int i;
  i = GetSymbol(a, a->things.p[a->i++].i);
  ConsumeComma(a);
  a->symbols.p[i].size = GetInt(a);
  a->symbols.p[i].type = STT_COMMON;
  a->symbols.p[i].section = SHN_COMMON;
}

static void OpVisibility(struct As *a, int visibility) {
  int i;
  for (;;) {
    i = GetSymbol(a, a->things.p[a->i++].i);
    a->symbols.p[i].stv = visibility;
    if (IsSemicolon(a)) break;
    ConsumeComma(a);
  }
}

static void OnInternal(struct As *a, struct Slice s) {
  OpVisibility(a, STV_INTERNAL);
}

static void OnHidden(struct As *a, struct Slice s) {
  OpVisibility(a, STV_HIDDEN);
}

static void OnProtected(struct As *a, struct Slice s) {
  OpVisibility(a, STV_PROTECTED);
}

static void OpBind(struct As *a, int bind) {
  int i;
  for (;;) {
    i = GetSymbol(a, a->things.p[a->i++].i);
    a->symbols.p[i].stb = bind;
    if (IsSemicolon(a)) break;
    ConsumeComma(a);
  }
}

static void OnLocal(struct As *a, struct Slice s) {
  OpBind(a, STB_LOCAL);
}

static void OnWeak(struct As *a, struct Slice s) {
  OpBind(a, STB_WEAK);
}

static void OnGlobal(struct As *a, struct Slice s) {
  OpBind(a, STB_GLOBAL);
}

static int GetOpSize(struct As *a, struct Slice s, int modrm, int i) {
  if (modrm & ISREG) {
    return (modrm & 070) >> 3;
  } else {
    switch (s.p[s.n - i]) {
      case 'b':
      case 'B':
        return 0;
      case 'w':
      case 'W':
        return 1;
      case 'l':
      case 'L':
        return 2;
      case 'q':
      case 'Q':
        return 3;
      default:
        Fail(a, "could not size instruction");
    }
  }
}

static bool ConsumeSegment(struct As *a) {
  int i;
  struct Slice s;
  if (IsSlice(a, a->i)) {
    s = a->slices.p[a->things.p[a->i].i];
    if (s.n == 3 && *s.p == '%') {
      for (i = 0; i < ARRAYLEN(kSegment); ++i) {
        if (s.p[1] == kSegment[i][0] && s.p[2] == kSegment[i][1]) {
          ++a->i;
          EmitByte(a, kSegmentByte[i]);
          ConsumePunct(a, ':');
          return true;
        }
      }
    }
  }
  return false;
}

static void CopyLower(char *k, const char *p, int n) {
  int i;
  for (i = 0; i < n; ++i) {
    k[i] = tolower(p[i]);
  }
}

static unsigned long MakeKey64(const char *p, int n) {
  char k[8] = {0};
  CopyLower(k, p, n);
  return READ64BE(k);
}

static uint128_t MakeKey128(const char *p, int n) {
  char k[16] = {0};
  CopyLower(k, p, n);
  return READ128BE(k);
}

static bool Prefix(struct As *a, const char *p, int n) {
  int m, l, r;
  unsigned long x, y;
  if (n && n <= 8) {
    x = MakeKey64(p, n);
    l = 0;
    r = ARRAYLEN(kPrefix) - 1;
    while (l <= r) {
      m = (l & r) + ((l ^ r) >> 1);  // floor((a+b)/2)
      y = READ64BE(kPrefix[m]);
      if (x < y) {
        r = m - 1;
      } else if (x > y) {
        l = m + 1;
      } else {
        EmitByte(a, kPrefixByte[m]);
        return true;
      }
    }
  }
  return false;
}

static bool FindReg(const char *p, int n, struct Reg *out_reg) {
  int m, l, r;
  unsigned long x, y;
  if (n && n <= 8 && *p == '%') {
    ++p;
    --n;
    x = MakeKey64(p, n);
    l = 0;
    r = ARRAYLEN(kRegs) - 1;
    while (l <= r) {
      m = (l & r) + ((l ^ r) >> 1);  // floor((a+b)/2)
      y = READ64BE(kRegs[m].s);
      if (x < y) {
        r = m - 1;
      } else if (x > y) {
        l = m + 1;
      } else {
        *out_reg = kRegs[m];
        return true;
      }
    }
  }
  return false;
}

static int FindRegReg(struct Slice s) {
  struct Reg reg;
  if (!FindReg(s.p, s.n, &reg)) return -1;
  return reg.reg;
}

static int FindRegRm(struct Slice s) {
  struct Reg reg;
  if (!FindReg(s.p, s.n, &reg)) return -1;
  return reg.rm;
}

static int FindRegBase(struct Slice s) {
  struct Reg reg;
  if (!FindReg(s.p, s.n, &reg)) return -1;
  return reg.base;
}

static int FindRegIndex(struct Slice s) {
  struct Reg reg;
  if (!FindReg(s.p, s.n, &reg)) return -1;
  return reg.index;
}

static int RemoveRexw(int x) {
  if (x == -1) return x;
  x &= ~0x0800;
  if (((x & 0xff00) >> 8) == REX) x &= ~0xff00;
  return x;
}

static int GetRegisterReg(struct As *a) {
  int reg;
  if ((reg = FindRegReg(GetSlice(a))) == -1) {
    InvalidRegister(a);
  }
  return reg;
}

static int GetRegisterRm(struct As *a) {
  int reg;
  if ((reg = FindRegRm(GetSlice(a))) == -1) {
    InvalidRegister(a);
  }
  return reg;
}

static int ParseModrm(struct As *a, int *disp) {
  /*           ┌isreg
               │┌isrip
               ││┌hasindex
               │││┌hasbase
               ││││┌hasasz
               │││││┌rex
               ││││││       ┌scale
               ││││││       │ ┌index or size
               ││││││       │ │  ┌base or reg
               │││││├──────┐├┐├─┐├─┐
  0b00000000000000000000000000000000*/
  struct Slice str;
  int reg, modrm = 0;
  if (!ConsumeSegment(a) && IsRegister(a, a->i)) {
    *disp = 0;
    modrm = GetRegisterRm(a) | ISREG;
  } else {
    if (!IsPunct(a, a->i, '(')) {
      *disp = Parse(a);
    } else {
      *disp = -1;
    }
    if (IsPunct(a, a->i, '(')) {
      ++a->i;
      if (!IsComma(a)) {
        str = GetSlice(a);
        modrm |= HASBASE;
        if (!strncasecmp(str.p, "%rip", str.n)) {
          modrm |= ISRIP;
        } else {
          reg = FindRegBase(str);
          if (reg == -1) InvalidRegister(a);
          modrm |= reg & 007;                            // reg
          modrm |= reg & 0xff00;                         // rex
          if (((reg & 070) >> 3) == 2) modrm |= HASASZ;  // asz
        }
      }
      if (IsComma(a)) {
        ++a->i;
        modrm |= HASINDEX;
        reg = FindRegIndex(GetSlice(a));
        if (reg == -1) InvalidRegister(a);
        modrm |= (reg & 007) << 3;                     // index
        modrm |= reg & 0xff00;                         // rex
        if (((reg & 070) >> 3) == 2) modrm |= HASASZ;  // asz
        if (IsComma(a)) {
          ++a->i;
          modrm |= (_bsr(GetInt(a)) & 3) << 6;
        }
      }
      ConsumePunct(a, ')');
    }
    if (modrm & HASASZ) {
      EmitByte(a, ASZ);
    }
  }
  return modrm;
}

static void EmitImm(struct As *a, int reg, int imm) {
  switch ((reg & 030) >> 3) {
    case 0:
      EmitExpr(a, imm, R_X86_64_8, EmitByte);
      break;
    case 1:
      EmitExpr(a, imm, R_X86_64_16, EmitWord);
      break;
    case 2:
    case 3:
      EmitExpr(a, imm, R_X86_64_32S, EmitLong);
      break;
    default:
      abort();
  }
}

static void EmitModrm(struct As *a, int reg, int modrm, int disp) {
  int relo, mod;
  void (*emitter)(struct As *, uint128_t);
  reg &= 7;
  reg <<= 3;
  if (modrm & ISREG) {
    EmitByte(a, 0300 | reg | (modrm & 7));
  } else {
    if (modrm & ISRIP) {
      EmitByte(a, 005 | reg);
      emitter = EmitLong;
      relo = a->pcrelative ?: R_X86_64_PC32;
    } else if (modrm & (HASBASE | HASINDEX)) {
      if (disp == -1) {
        emitter = NULL;
        relo = 0;
        mod = 0;
      } else if (a->exprs.p[disp].kind == EX_INT) {
        if (!a->exprs.p[disp].x) {
          emitter = NULL;
          relo = 0;
          mod = 0;
        } else if (-128 <= a->exprs.p[disp].x && a->exprs.p[disp].x <= 127) {
          emitter = EmitByte;
          relo = R_X86_64_32S;
          mod = 0100;
        } else {
          emitter = EmitLong;
          relo = R_X86_64_32S;
          mod = 0200;
        }
      } else {
        emitter = EmitLong;
        relo = R_X86_64_32S;
        mod = 0200;
      }
      if (!(modrm & HASINDEX) && (modrm & 7) != 4) {
        EmitByte(a, mod | reg | modrm);
      } else if (!(modrm & HASINDEX) && (modrm & 7) == 4) {
        EmitByte(a, mod | reg | 4);  // (%rsp) must be (%rsp,%riz)
        EmitByte(a, 040 | modrm);
      } else if (!mod && (modrm & 7) == 5) {
        EmitByte(a, 0100 | reg | 4);  // (%rbp,𝑥) is special
        EmitByte(a, modrm);
        EmitByte(a, 0);
      } else {
        EmitByte(a, mod | reg | 4);
        EmitByte(a, modrm);
      }
    } else {
      EmitByte(a, 004 | reg);  // (%rbp,%riz) is disp32
      EmitByte(a, 045);
      emitter = EmitLong;
      relo = R_X86_64_32S;
    }
    if (emitter) {
      EmitExpr(a, disp, relo, emitter);
    }
  }
}

static void EmitRex(struct As *a, int x) {
  if (x & 0xff00) {
    EmitByte(a, x >> 8);
  }
}

static void EmitOpModrm(struct As *a, long op, int reg, int modrm, int disp,
                        int skew) {
  switch ((reg & 070) >> 3) {
    case 0:
      EmitVarword(a, op);
      EmitModrm(a, reg, modrm, disp);
      break;
    case 1:
      EmitVarword(a, op + skew);
      EmitModrm(a, reg, modrm, disp);
      break;
    case 2:
    case 3:
    case 4:
      EmitVarword(a, op + skew);
      EmitModrm(a, reg, modrm, disp);
      break;
    default:
      abort();
  }
}

static void EmitRexOpModrm(struct As *a, long op, int reg, int modrm, int disp,
                           int skew) {
  if (((reg & 070) >> 3) == 1) EmitByte(a, OSZ);
  EmitRex(a, reg | modrm);
  EmitOpModrm(a, op, reg, modrm, disp, skew);
}

static void OnLoad(struct As *a, struct Slice s, int op) {
  int modrm, reg, disp;
  modrm = ParseModrm(a, &disp);
  ConsumeComma(a);
  reg = GetRegisterReg(a);
  EmitRexOpModrm(a, op, reg, modrm, disp, 0);
}

static void OnLea(struct As *a, struct Slice s) {
  return OnLoad(a, s, 0x8D);
}

static void OnLar(struct As *a, struct Slice s) {
  return OnLoad(a, s, 0x0f02);
}

static void OnLsl(struct As *a, struct Slice s) {
  return OnLoad(a, s, 0x0f03);
}

static void OnMov(struct As *a, struct Slice s) {
  int reg, modrm, disp, imm;
  if (IsPunct(a, a->i, '$')) {
    ++a->i;
    imm = Parse(a);
    ConsumeComma(a);
    if (IsSlice(a, a->i)) {  // imm -> reg
      reg = GetRegisterRm(a);
      switch ((reg & 070) >> 3) {
        case 0:
          EmitRex(a, reg);
          EmitByte(a, 0xB0 + (reg & 7));
          EmitExpr(a, imm, R_X86_64_8, EmitByte);
          break;
        case 1:
          EmitByte(a, OSZ);
          EmitRex(a, reg);
          EmitByte(a, 0xB8 + (reg & 7));
          EmitExpr(a, imm, R_X86_64_16, EmitWord);
          break;
        case 2:
          EmitRex(a, reg);
          EmitByte(a, 0xB8 + (reg & 7));
          EmitExpr(a, imm, R_X86_64_32, EmitLong);
          break;
        case 3:
          EmitRex(a, reg);
          EmitByte(a, 0xB8 + (reg & 7));  // suboptimal
          EmitExpr(a, imm, R_X86_64_64, EmitQuad);
          break;
        default:
          Fail(a, "todo movd/movq");
      }
    } else {  // imm -> modrm
      modrm = ParseModrm(a, &disp);
      switch (GetOpSize(a, s, modrm, 1)) {
        case 0:
          EmitRex(a, modrm);
          EmitByte(a, 0xC6);
          EmitModrm(a, 0, modrm, disp);
          EmitExpr(a, imm, R_X86_64_8, EmitByte);
          break;
        case 1:
          EmitByte(a, OSZ);
          EmitRex(a, modrm);
          EmitByte(a, 0xC7);
          EmitModrm(a, 0, modrm, disp);
          EmitExpr(a, imm, R_X86_64_16, EmitWord);
          break;
        case 2:
          EmitRex(a, modrm);
          EmitByte(a, 0xC7);
          EmitModrm(a, 0, modrm, disp);
          EmitExpr(a, imm, R_X86_64_32, EmitLong);
          break;
        case 3:
          EmitRex(a, modrm | REXW << 8);
          EmitByte(a, 0xC7);  // suboptimal
          EmitModrm(a, 0, modrm, disp);
          EmitExpr(a, imm, R_X86_64_32, EmitLong);
          break;
        default:
          abort();
      }
    }
  } else if (IsSlice(a, a->i)) {  // reg -> reg/modrm
    reg = GetRegisterReg(a);
    ConsumeComma(a);
    modrm = ParseModrm(a, &disp);
    EmitRexOpModrm(a, 0x88, reg, modrm, disp, 1);
  } else {  // modrm -> reg
    modrm = ParseModrm(a, &disp);
    ConsumeComma(a);
    reg = GetRegisterReg(a);
    EmitRexOpModrm(a, 0x8A, reg, modrm, disp, 1);
  }
}

static void EmitMovx(struct As *a, struct Slice opname, int op) {
  int reg, modrm, disp;
  modrm = ParseModrm(a, &disp);
  ConsumeComma(a);
  reg = GetRegisterReg(a);
  EmitRex(a, reg);
  if (((reg & 070) >> 3) == 1) EmitByte(a, OSZ);
  EmitVarword(a, op + !!GetOpSize(a, opname, modrm, 2));
  EmitModrm(a, reg, modrm, disp);
}

static void OnMovzbwx(struct As *a, struct Slice s) {
  EmitMovx(a, s, 0x0FB6);
}

static void OnMovsbwx(struct As *a, struct Slice s) {
  EmitMovx(a, s, 0x0FBE);
}

static void OnMovslq(struct As *a, struct Slice s) {
  int reg, modrm, disp;
  modrm = ParseModrm(a, &disp);
  ConsumeComma(a);
  reg = GetRegisterReg(a);
  EmitByte(a, REXW);
  EmitByte(a, 0x63);
  EmitModrm(a, reg, modrm, disp);
}

static dontinline void OpAluImpl(struct As *a, struct Slice opname, int op) {
  int reg, modrm, imm, disp;
  if (IsPunct(a, a->i, '$')) {  // imm -> reg/modrm
    ++a->i;
    imm = Parse(a);
    ConsumeComma(a);
    modrm = ParseModrm(a, &disp);
    reg = GetOpSize(a, opname, modrm, 1) << 3 | op;
    EmitRexOpModrm(a, 0x80, reg, modrm, disp, 1);  // suboptimal
    EmitImm(a, reg, imm);
  } else if (IsSlice(a, a->i)) {  // reg -> reg/modrm
    reg = GetRegisterReg(a);
    ConsumeComma(a);
    modrm = ParseModrm(a, &disp);
    EmitRexOpModrm(a, op << 3, reg, modrm, disp, 1);
  } else {  // modrm -> reg
    modrm = ParseModrm(a, &disp);
    ConsumeComma(a);
    reg = GetRegisterReg(a);
    EmitRexOpModrm(a, op << 3 | 2, reg, modrm, disp, 1);
  }
}

static dontinline void OpAlu(struct As *a, struct Slice opname, int op) {
  OpAluImpl(a, opname, op);
}

static dontinline void OpBsuImpl(struct As *a, struct Slice opname, int op) {
  int reg, modrm, imm, disp;
  if (IsPunct(a, a->i, '$')) {
    ++a->i;
    imm = Parse(a);
    ConsumeComma(a);
  } else if (IsSlice(a, a->i) &&
             (a->slices.p[a->things.p[a->i].i].n == 3 &&
              !strncasecmp(a->slices.p[a->things.p[a->i].i].p, "%cl", 3)) &&
             !IsPunct(a, a->i + 1, ';')) {
    ++a->i;
    ConsumeComma(a);
    modrm = ParseModrm(a, &disp);
    reg = GetOpSize(a, opname, modrm, 1) << 3 | op;
    EmitRexOpModrm(a, 0xD2, reg, modrm, disp, 1);
    return;
  } else {
    AppendExpr(a);
    a->exprs.p[a->exprs.n - 1].kind = EX_INT;
    a->exprs.p[a->exprs.n - 1].tok = a->i;
    a->exprs.p[a->exprs.n - 1].x = 1;
    imm = a->exprs.n - 1;
  }
  modrm = ParseModrm(a, &disp);
  reg = GetOpSize(a, opname, modrm, 1) << 3 | op;
  EmitRexOpModrm(a, 0xC0, reg, modrm, disp, 1);  // suboptimal
  EmitExpr(a, imm, R_X86_64_8, EmitByte);
}

static dontinline void OpBsu(struct As *a, struct Slice opname, int op) {
  OpBsuImpl(a, opname, op);
}

static dontinline void OpXadd(struct As *a) {
  int reg, modrm, disp;
  reg = GetRegisterReg(a);
  ConsumeComma(a);
  modrm = ParseModrm(a, &disp);
  EmitRexOpModrm(a, 0x0FC0, reg, modrm, disp, 1);
}

static dontinline int OpF6Impl(struct As *a, struct Slice s, int reg) {
  int modrm, disp;
  modrm = ParseModrm(a, &disp);
  reg |= GetOpSize(a, s, modrm, 1) << 3;
  EmitRexOpModrm(a, 0xF6, reg, modrm, disp, 1);
  return reg;
}

static dontinline int OpF6(struct As *a, struct Slice s, int reg) {
  return OpF6Impl(a, s, reg);
}

static void OnTest(struct As *a, struct Slice s) {
  int reg, modrm, imm, disp;
  if (IsPunct(a, a->i, '$')) {
    ++a->i;
    imm = Parse(a);
    ConsumeComma(a);
    reg = OpF6(a, s, 0);  // suboptimal
    EmitImm(a, reg, imm);
  } else {
    reg = GetRegisterReg(a);
    ConsumeComma(a);
    modrm = ParseModrm(a, &disp);
    EmitRexOpModrm(a, 0x84, reg, modrm, disp, 1);
  }
}

static void OnCmpxchg(struct As *a, struct Slice s) {
  int reg, modrm, disp;
  reg = GetRegisterReg(a);
  ConsumeComma(a);
  modrm = ParseModrm(a, &disp);
  EmitRexOpModrm(a, 0x0FB0, reg, modrm, disp, 1);
}

static void OnImul(struct As *a, struct Slice s) {
  int reg, modrm, imm, disp;
  if (IsPunct(a, a->i, '$')) {
    ++a->i;
    imm = Parse(a);
    ConsumeComma(a);
    modrm = ParseModrm(a, &disp);
    ConsumeComma(a);
    reg = GetRegisterReg(a);
    EmitRexOpModrm(a, 0x69, reg, modrm, disp, 0);
    EmitImm(a, reg, imm);
  } else {
    modrm = ParseModrm(a, &disp);
    if (IsComma(a)) {
      ++a->i;
      reg = GetRegisterReg(a);
      EmitRexOpModrm(a, 0x0FAF, reg, modrm, disp, 0);
    } else {
      reg = GetOpSize(a, s, modrm, 1) << 3 | 5;
      EmitRexOpModrm(a, 0xF6, reg, modrm, disp, 1);
    }
  }
}

static void OpBit(struct As *a, int op) {
  int reg, modrm, disp;
  modrm = ParseModrm(a, &disp);
  ConsumeComma(a);
  reg = GetRegisterReg(a);
  EmitRexOpModrm(a, op, reg, modrm, disp, 0);
}

static void OnXchg(struct As *a, struct Slice s) {
  int reg, modrm, disp;
  reg = GetRegisterReg(a);
  ConsumeComma(a);
  modrm = ParseModrm(a, &disp);
  EmitRexOpModrm(a, 0x86, reg, modrm, disp, 1);
}

static void OpBump(struct As *a, struct Slice s, int reg) {
  int modrm, disp;
  modrm = ParseModrm(a, &disp);
  EmitRexOpModrm(a, 0xFE, GetOpSize(a, s, modrm, 1) << 3 | reg, modrm, disp, 1);
}

static void OpShrld(struct As *a, struct Slice s, int op) {
  int imm, reg, modrm, disp, skew;
  if (IsSlice(a, a->i) &&
      (a->slices.p[a->things.p[a->i].i].n == 3 &&
       !strncasecmp(a->slices.p[a->things.p[a->i].i].p, "%cl", 3))) {
    ++a->i;
    ConsumeComma(a);
    skew = 1;
    reg = GetRegisterReg(a);
    ConsumeComma(a);
    modrm = ParseModrm(a, &disp);
    EmitRexOpModrm(a, 0x0F00 | op | skew, reg, modrm, disp, 0);
  } else {
    skew = 0;
    ConsumePunct(a, '$');
    imm = GetInt(a);
    ConsumeComma(a);
    reg = GetRegisterReg(a);
    modrm = ParseModrm(a, &disp);
    EmitRexOpModrm(a, 0x0F00 | op | skew, reg, modrm, disp, 0);
    EmitExpr(a, imm, R_X86_64_8, EmitByte);
  }
}

static void OnShrd(struct As *a, struct Slice s) {
  OpShrld(a, s, 0xAC);
}

static void OnShld(struct As *a, struct Slice s) {
  OpShrld(a, s, 0xA4);
}

static void OpSseMov(struct As *a, int opWsdVsd, int opVsdWsd) {
  int reg, modrm, disp;
  if (IsRegister(a, a->i)) {
    reg = GetRegisterReg(a);
    ConsumeComma(a);
    modrm = ParseModrm(a, &disp);
    EmitRexOpModrm(a, opWsdVsd, reg, modrm, disp, 0);
  } else {
    modrm = ParseModrm(a, &disp);
    ConsumeComma(a);
    reg = GetRegisterReg(a);
    EmitRexOpModrm(a, opVsdWsd, reg, modrm, disp, 0);
  }
}

static void OpMovntdq(struct As *a) {
  int reg, modrm, disp;
  reg = GetRegisterReg(a);
  ConsumeComma(a);
  modrm = ParseModrm(a, &disp);
  EmitRexOpModrm(a, 0x660FE7, reg, modrm, disp, 0);
}

static void OpMovdqx(struct As *a, int op) {
  OpSseMov(a, op + 0x10, op);
}

static void OnMovdqu(struct As *a, struct Slice s) {
  EmitByte(a, 0xF3);
  OpMovdqx(a, 0x0F6F);
}

static void OnMovups(struct As *a, struct Slice s) {
  OpSseMov(a, 0x0F11, 0x0F10);
}

static void OnMovupd(struct As *a, struct Slice s) {
  EmitByte(a, 0x66);
  OnMovups(a, s);
}

static void OnMovdqa(struct As *a, struct Slice s) {
  EmitByte(a, 0x66);
  OpMovdqx(a, 0x0F6F);
}

static void OnMovaps(struct As *a, struct Slice s) {
  OpSseMov(a, 0x0F29, 0x0F28);
}

static void OnMovapd(struct As *a, struct Slice s) {
  EmitByte(a, 0x66);
  OpSseMov(a, 0x0F29, 0x0F28);
}

static void OpMovdq(struct As *a, bool is64) {
  int reg, modrm, boop, disp, ugh;
  EmitByte(a, 0x66);  // todo mmx
  if (IsRegister(a, a->i)) {
    reg = GetRegisterReg(a);
    ConsumeComma(a);
    modrm = ParseModrm(a, &disp);
    if (!(modrm & ISREG)) {
      if (((reg & 070) >> 3) == 4) {
        if (is64) reg |= REXW << 8;
        boop = 0x10;
      } else {
        boop = 0;
      }
    } else {
      if (((reg & 070) >> 3) == 4) {
        boop = 0x10;
      } else {
        boop = 0;
        ugh = modrm & 7;
        modrm &= ~7;
        modrm |= reg & 7;
        reg &= ~7;
        reg |= ugh;
      }
    }
  } else {
    modrm = ParseModrm(a, &disp);
    ConsumeComma(a);
    reg = GetRegisterReg(a);
    if (!(modrm & ISREG)) {
      if (((reg & 070) >> 3) == 4) {
        if (is64) reg |= REXW << 8;
        boop = 0;
      } else {
        boop = 0x10;
      }
    } else {
      boop = ((modrm & 070) >> 3) == 4 ? 0 : 0x10;
    }
  }
  EmitRexOpModrm(a, 0x0F6E + boop, reg, modrm, disp, 0);
}

static void OnMovss(struct As *a, struct Slice s) {
  OpSseMov(a, 0xF30F11, 0xF30F10);
}

static void OnMovsd(struct As *a, struct Slice s) {
  OpSseMov(a, 0xF20F11, 0xF20F10);
}

static bool IsSsePrefix(int c) {
  return c == 0x66 || c == 0xF2 || c == 0xF3;  // must come before rex
}

static dontinline void OpSseImpl(struct As *a, int op) {
  int reg, modrm, disp;
  if (IsSsePrefix((op & 0xff000000) >> 24)) {
    EmitByte(a, (op & 0xff000000) >> 24);
    op &= 0xffffff;
  }
  if (IsSsePrefix((op & 0x00ff0000) >> 16)) {
    EmitByte(a, (op & 0x00ff0000) >> 16);
    op &= 0xffff;
  }
  modrm = ParseModrm(a, &disp);
  ConsumeComma(a);
  reg = GetRegisterReg(a);
  EmitRexOpModrm(a, op, reg, modrm, disp, 0);
}

static dontinline void OpSse(struct As *a, int op) {
  OpSseImpl(a, op);
}

static dontinline void OpSseIbImpl(struct As *a, int op) {
  int imm;
  ConsumePunct(a, '$');
  imm = Parse(a);
  ConsumeComma(a);
  OpSse(a, op);
  EmitExpr(a, imm, R_X86_64_8, EmitByte);
}

static dontinline void OpSseIb(struct As *a, int op) {
  OpSseIbImpl(a, op);
}

static bool HasXmmOnLine(struct As *a) {
  int i;
  for (i = 0; !IsPunct(a, a->i + i, ';'); ++i) {
    if (IsSlice(a, a->i + i) && a->slices.p[a->things.p[a->i + i].i].n >= 4 &&
        (startswith(a->slices.p[a->things.p[a->i + i].i].p, "xmm") ||
         startswith(a->slices.p[a->things.p[a->i + i].i].p, "%xmm"))) {
      return true;
    }
  }
  return false;
}

static void OnMovd(struct As *a, struct Slice s) {
  OpMovdq(a, false);
}

static void OnMovq(struct As *a, struct Slice s) {
  if (HasXmmOnLine(a)) {
    OpMovdq(a, true);
  } else {
    OnMov(a, s);
  }
}

static void OnPush(struct As *a, struct Slice s) {
  int modrm, disp;
  if (IsPunct(a, a->i, '$')) {
    ++a->i;
    EmitByte(a, 0x68);
    EmitLong(a, GetInt(a));
  } else {
    modrm = RemoveRexw(ParseModrm(a, &disp));
    EmitRexOpModrm(a, 0xFF, 6, modrm, disp, 0);  // suboptimal
  }
}

static void OnRdpid(struct As *a, struct Slice s) {
  EmitVarword(a, 0xf30fc7);
  EmitByte(a, 0370 | GetRegisterReg(a));
}

static void OnPop(struct As *a, struct Slice s) {
  int modrm, disp;
  modrm = RemoveRexw(ParseModrm(a, &disp));
  EmitRexOpModrm(a, 0x8F, 0, modrm, disp, 0);  // suboptimal
}

static void OnNop(struct As *a, struct Slice opname) {
  int modrm, disp;
  if (IsSemicolon(a)) {
    EmitByte(a, 0x90);
  } else {
    modrm = ParseModrm(a, &disp);
    EmitRexOpModrm(a, 0x0F1F, 6, modrm, disp, 0);
  }
}

static void OnRet(struct As *a, struct Slice s) {
  if (IsPunct(a, a->i, '$')) {
    ++a->i;
    EmitByte(a, 0xC2);
    EmitWord(a, GetInt(a));
  } else {
    EmitByte(a, 0xC3);
  }
}

static dontinline void OpCmovccImpl(struct As *a, int cc) {
  int reg, modrm, disp;
  modrm = ParseModrm(a, &disp);
  ConsumeComma(a);
  reg = GetRegisterReg(a);
  EmitRexOpModrm(a, 0x0F40 | cc, reg, modrm, disp, 0);
}

static dontinline void OpCmovcc(struct As *a, int cc) {
  OpCmovccImpl(a, cc);
}

static dontinline void OpSetccImpl(struct As *a, int cc) {
  int modrm, disp;
  modrm = ParseModrm(a, &disp);
  EmitRexOpModrm(a, 0x0F90 | cc, 6, modrm, disp, 0);
}

static dontinline void OpSetcc(struct As *a, int cc) {
  OpSetccImpl(a, cc);
}

static void OnFile(struct As *a, struct Slice s) {
  int fileno;
  struct Slice path;
  fileno = GetInt(a);
  path = GetSlice(a);
  (void)fileno;
  (void)path;
  // TODO: DWARF
}

static void OnLoc(struct As *a, struct Slice s) {
  int fileno, lineno;
  fileno = GetInt(a);
  lineno = GetInt(a);
  (void)fileno;
  (void)lineno;
  // TODO: DWARF
}

static void OnCall(struct As *a, struct Slice s) {
  int modrm, disp;
  if (IsPunct(a, a->i, '*')) ++a->i;
  modrm = RemoveRexw(ParseModrm(a, &disp));
  if (modrm & (ISREG | ISRIP | HASINDEX | HASBASE)) {
    if (modrm & ISRIP) a->pcrelative = R_X86_64_GOTPCRELX;
    EmitRexOpModrm(a, 0xFF, 2, modrm, disp, 0);
    a->pcrelative = 0;
  } else {
    EmitByte(a, 0xE8);
    EmitExpr(a, disp, R_X86_64_PC32, EmitLong);
  }
}

static dontinline void OpJmpImpl(struct As *a, int cc) {
  int modrm, disp;
  if (IsPunct(a, a->i, '*')) ++a->i;
  modrm = RemoveRexw(ParseModrm(a, &disp));
  if (cc == -1) {
    if (modrm & (ISREG | ISRIP | HASINDEX | HASBASE)) {
      if (modrm & ISRIP) a->pcrelative = R_X86_64_GOTPCRELX;
      EmitRexOpModrm(a, 0xFF, 4, modrm, disp, 0);
      a->pcrelative = 0;
    } else {
      EmitByte(a, 0xE9);
      EmitExpr(a, disp, R_X86_64_PC32, EmitLong);
    }
  } else {
    EmitByte(a, 0x0F);
    EmitByte(a, 0x80 + cc);
    EmitExpr(a, disp, R_X86_64_PC32, EmitLong);
  }
}

static dontinline void OpJmp(struct As *a, int cc) {
  OpJmpImpl(a, cc);
}

static dontinline void OpFpu1Impl(struct As *a, int op, int reg) {
  int modrm, disp;
  modrm = ParseModrm(a, &disp);
  EmitRexOpModrm(a, op, reg, modrm, disp, 0);
}

static dontinline void OpFpu1(struct As *a, int op, int reg) {
  OpFpu1Impl(a, op, reg);
}

static void OnFxch(struct As *a, struct Slice s) {
  int rm;
  rm = !IsSemicolon(a) ? GetRegisterRm(a) : 1;
  EmitByte(a, 0xD9);
  EmitByte(a, 0310 | (rm & 7));
}

static void OnBswap(struct As *a, struct Slice s) {
  int srm;
  srm = GetRegisterRm(a);
  EmitRex(a, srm);
  EmitByte(a, 0x0F);
  EmitByte(a, 0310 | (srm & 7));
}

static dontinline void OpFcomImpl(struct As *a, int op) {
  int rm;
  if (IsSemicolon(a)) {
    rm = 1;
  } else {
    rm = GetRegisterRm(a);
    if (IsComma(a)) {
      ++a->i;
      if (GetRegisterReg(a) & 7) {
        Fail(a, "bad register");
      }
    }
  }
  EmitVarword(a, op | (rm & 7));
}

static dontinline void OpFcom(struct As *a, int op) {
  OpFcomImpl(a, op);
}

// clang-format off
static void OnAdc(struct As *a, struct Slice s) { OpAlu(a, s, 2); }
static void OnAdd(struct As *a, struct Slice s) { OpAlu(a, s, 0); }
static void OnAddpd(struct As *a, struct Slice s) { OpSse(a, 0x660F58); }
static void OnAddps(struct As *a, struct Slice s) { OpSse(a, 0x0F58); }
static void OnAddsd(struct As *a, struct Slice s) { OpSse(a, 0xF20F58); }
static void OnAddss(struct As *a, struct Slice s) { OpSse(a, 0xF30F58); }
static void OnAddsubpd(struct As *a, struct Slice s) { OpSse(a, 0x660FD0); }
static void OnAddsubps(struct As *a, struct Slice s) { OpSse(a, 0xF20FD0); }
static void OnAnd(struct As *a, struct Slice s) { OpAlu(a, s, 4); }
static void OnAndnpd(struct As *a, struct Slice s) { OpSse(a, 0x660F55); }
static void OnAndnps(struct As *a, struct Slice s) { OpSse(a, 0x0F55); }
static void OnAndpd(struct As *a, struct Slice s) { OpSse(a, 0x660F54); }
static void OnAndps(struct As *a, struct Slice s) { OpSse(a, 0x0F54); }
static void OnBlendpd(struct As *a, struct Slice s) { OpSseIb(a, 0x660F3A0D); }
static void OnBlendvpd(struct As *a, struct Slice s) { OpSse(a, 0x660F3815); }
static void OnBsf(struct As *a, struct Slice s) { OpBit(a, 0x0FBC); }
static void OnBsr(struct As *a, struct Slice s) { OpBit(a, 0x0FBD); }
static void OnCbtw(struct As *a, struct Slice s) { EmitVarword(a, 0x6698); }
static void OnClc(struct As *a, struct Slice s) { EmitByte(a, 0xF8); }
static void OnCld(struct As *a, struct Slice s) { EmitByte(a, 0xFC); }
static void OnCli(struct As *a, struct Slice s) { EmitByte(a, 0xFA); }
static void OnCltd(struct As *a, struct Slice s) { EmitByte(a, 0x99); }
static void OnCltq(struct As *a, struct Slice s) { EmitVarword(a, 0x4898); }
static void OnCmc(struct As *a, struct Slice s) { EmitByte(a, 0xF5); }
static void OnCmovb(struct As *a, struct Slice s) { OpCmovcc(a, 2); }
static void OnCmovbe(struct As *a, struct Slice s) { OpCmovcc(a, 6); }
static void OnCmovl(struct As *a, struct Slice s) { OpCmovcc(a, 12); }
static void OnCmovle(struct As *a, struct Slice s) { OpCmovcc(a, 14); }
static void OnCmovnb(struct As *a, struct Slice s) { OpCmovcc(a, 3); }
static void OnCmovnbe(struct As *a, struct Slice s) { OpCmovcc(a, 7); }
static void OnCmovnl(struct As *a, struct Slice s) { OpCmovcc(a, 13); }
static void OnCmovnle(struct As *a, struct Slice s) { OpCmovcc(a, 15); }
static void OnCmovno(struct As *a, struct Slice s) { OpCmovcc(a, 1); }
static void OnCmovnp(struct As *a, struct Slice s) { OpCmovcc(a, 11); }
static void OnCmovns(struct As *a, struct Slice s) { OpCmovcc(a, 9); }
static void OnCmovnz(struct As *a, struct Slice s) { OpCmovcc(a, 5); }
static void OnCmovo(struct As *a, struct Slice s) { OpCmovcc(a, 0); }
static void OnCmovp(struct As *a, struct Slice s) { OpCmovcc(a, 10); }
static void OnCmovs(struct As *a, struct Slice s) { OpCmovcc(a, 8); }
static void OnCmovz(struct As *a, struct Slice s) { OpCmovcc(a, 4); }
static void OnCmp(struct As *a, struct Slice s) { OpAlu(a, s, 7); }
static void OnCmppd(struct As *a, struct Slice s) { OpSseIb(a, 0x660FC2); }
static void OnCmpps(struct As *a, struct Slice s) { OpSseIb(a, 0x0FC2); }
static void OnCmpsd(struct As *a, struct Slice s) { OpSseIb(a, 0xF20FC2); }
static void OnCmpss(struct As *a, struct Slice s) { OpSseIb(a, 0xF30FC2); }
static void OnComisd(struct As *a, struct Slice s) { OpSse(a, 0x660F2F); }
static void OnComiss(struct As *a, struct Slice s) { OpSse(a, 0x0F2F); }
static void OnCqto(struct As *a, struct Slice s) { EmitVarword(a, 0x4899); }
static void OnCvtdq2pd(struct As *a, struct Slice s) { OpSse(a, 0xF30FE6); }
static void OnCvtdq2ps(struct As *a, struct Slice s) { OpSse(a, 0xF5B); }
static void OnCvtpd2dq(struct As *a, struct Slice s) { OpSse(a, 0xF20FE6); }
static void OnCvtpd2ps(struct As *a, struct Slice s) { OpSse(a, 0x660F5A); }
static void OnCvtps2dq(struct As *a, struct Slice s) { OpSse(a, 0x660F5B); }
static void OnCvtps2pd(struct As *a, struct Slice s) { OpSse(a, 0x0F5A); }
static void OnCvtsd2ss(struct As *a, struct Slice s) { OpSse(a, 0xF20F5A); }
static void OnCvtsi2sd(struct As *a, struct Slice s) { OpSse(a, 0xF20F2A); }
static void OnCvtsi2ss(struct As *a, struct Slice s) { OpSse(a, 0xF30F2A); }
static void OnCvtss2sd(struct As *a, struct Slice s) { OpSse(a, 0xF30F5A); }
static void OnCvttpd2dq(struct As *a, struct Slice s) { OpSse(a, 0x660FE6); }
static void OnCvttps2dq(struct As *a, struct Slice s) { OpSse(a, 0xF30F5B); }
static void OnCvttsd2si(struct As *a, struct Slice s) { OpSse(a, 0xF20F2C); }
static void OnCvttss2si(struct As *a, struct Slice s) { OpSse(a, 0xF30F2C); }
static void OnCwtd(struct As *a, struct Slice s) { EmitVarword(a, 0x6699); }
static void OnCwtl(struct As *a, struct Slice s) { EmitByte(a, 0x98); }
static void OnDec(struct As *a, struct Slice s) { OpBump(a, s, 1); }
static void OnDiv(struct As *a, struct Slice s) { OpF6(a, s, 6); }
static void OnDivpd(struct As *a, struct Slice s) { OpSse(a, 0x660F5E); }
static void OnDivps(struct As *a, struct Slice s) { OpSse(a, 0x0F5E); }
static void OnDivsd(struct As *a, struct Slice s) { OpSse(a, 0xF20F5E); }
static void OnDivss(struct As *a, struct Slice s) { OpSse(a, 0xF30F5E); }
static void OnDppd(struct As *a, struct Slice s) { OpSse(a, 0x660F3A41); }
static void OnFabs(struct As *a, struct Slice s) { EmitVarword(a, 0xD9E1); }
static void OnFaddl(struct As *a, struct Slice s) { OpFpu1(a, 0xDC, 0); }
static void OnFaddp(struct As *a, struct Slice s) { EmitVarword(a, 0xDEC1); }
static void OnFadds(struct As *a, struct Slice s) { OpFpu1(a, 0xD8, 0); }
static void OnFchs(struct As *a, struct Slice s) { EmitVarword(a, 0xD9E0); }
static void OnFcmovb(struct As *a, struct Slice s) { OpFcom(a, 0xDAC0); }
static void OnFcmovbe(struct As *a, struct Slice s) { OpFcom(a, 0xDAD0); }
static void OnFcmove(struct As *a, struct Slice s) { OpFcom(a, 0xDAC8); }
static void OnFcmovnb(struct As *a, struct Slice s) { OpFcom(a, 0xDBC0); }
static void OnFcmovnbe(struct As *a, struct Slice s) { OpFcom(a, 0xDBD0); }
static void OnFcmovne(struct As *a, struct Slice s) { OpFcom(a, 0xDBC8); }
static void OnFcmovnu(struct As *a, struct Slice s) { OpFcom(a, 0xDBD8); }
static void OnFcmovu(struct As *a, struct Slice s) { OpFcom(a, 0xDAD8); }
static void OnFcomi(struct As *a, struct Slice s) { OpFcom(a, 0xDBF0); }
static void OnFcomip(struct As *a, struct Slice s) { OpFcom(a, 0xDFF0); }
static void OnFdivrp(struct As *a, struct Slice s) { EmitVarword(a, 0xDEF9); }
static void OnFildl(struct As *a, struct Slice s) { OpFpu1(a, 0xDB, 0); }
static void OnFildll(struct As *a, struct Slice s) { OpFpu1(a, 0xDF, 5); }
static void OnFildq(struct As *a, struct Slice s) { OpFpu1(a, 0xDF, 5); }
static void OnFilds(struct As *a, struct Slice s) { OpFpu1(a, 0xDF, 0); }
static void OnFistpq(struct As *a, struct Slice s) { OpFpu1(a, 0xDF, 7); }
static void OnFisttpq(struct As *a, struct Slice s) { OpFpu1(a, 0xDD, 1); }
static void OnFisttps(struct As *a, struct Slice s) { OpFpu1(a, 0xDF, 1); }
static void OnFld(struct As *a, struct Slice s) { OpFpu1(a, 0xD9, 0); }
static void OnFld1(struct As *a, struct Slice s) { EmitVarword(a, 0xd9e8); }
static void OnFldcw(struct As *a, struct Slice s) { OpFpu1(a, 0xD9, 5); }
static void OnFldl(struct As *a, struct Slice s) { OpFpu1(a, 0xDD, 0); }
static void OnFldl2e(struct As *a, struct Slice s) { EmitVarword(a, 0xd9ea); }
static void OnFldl2t(struct As *a, struct Slice s) { EmitVarword(a, 0xd9e9); }
static void OnFldlg2(struct As *a, struct Slice s) { EmitVarword(a, 0xd9ec); }
static void OnFldln2(struct As *a, struct Slice s) { EmitVarword(a, 0xd9ed); }
static void OnFldpi(struct As *a, struct Slice s) { EmitVarword(a, 0xd9eb); }
static void OnFlds(struct As *a, struct Slice s) { OpFpu1(a, 0xD9, 0); }
static void OnFldt(struct As *a, struct Slice s) { OpFpu1(a, 0xDB, 5); }
static void OnFldz(struct As *a, struct Slice s) { EmitVarword(a, 0xd9ee); }
static void OnFmulp(struct As *a, struct Slice s) { EmitVarword(a, 0xdec9); }
static void OnFnstcw(struct As *a, struct Slice s) { OpFpu1(a, 0xD9, 7); }
static void OnFnstsw(struct As *a, struct Slice s) { OpFpu1(a, 0xDF, 4); }
static void OnFstp(struct As *a, struct Slice s) { OpFpu1(a, 0xDD, 3); }
static void OnFstpl(struct As *a, struct Slice s) { OpFpu1(a, 0xDD, 3); }
static void OnFstps(struct As *a, struct Slice s) { OpFpu1(a, 0xD9, 3); }
static void OnFstpt(struct As *a, struct Slice s) { OpFpu1(a, 0xDB, 7); }
static void OnFsubrp(struct As *a, struct Slice s) { EmitVarword(a, 0xDEE9); }
static void OnFtst(struct As *a, struct Slice s) { EmitVarword(a, 0xD9E4); }
static void OnFucomi(struct As *a, struct Slice s) { OpFcom(a, 0xDBE8); }
static void OnFucomip(struct As *a, struct Slice s) { OpFcom(a, 0xDFE8); }
static void OnFwait(struct As *a, struct Slice s) { EmitByte(a, 0x9B); }
static void OnFxam(struct As *a, struct Slice s) { EmitVarword(a, 0xD9E5); }
static void OnFxtract(struct As *a, struct Slice s) { EmitVarword(a, 0xD9F4); }
static void OnHaddpd(struct As *a, struct Slice s) { OpSse(a, 0x660F7C); }
static void OnHaddps(struct As *a, struct Slice s) { OpSse(a, 0xF20F7C); }
static void OnHlt(struct As *a, struct Slice s) { EmitByte(a, 0xF4); }
static void OnHsubpd(struct As *a, struct Slice s) { OpSse(a, 0x660F7D); }
static void OnHsubps(struct As *a, struct Slice s) { OpSse(a, 0xF20F7D); }
static void OnIdiv(struct As *a, struct Slice s) { OpF6(a, s, 7); }
static void OnInc(struct As *a, struct Slice s) { OpBump(a, s, 0); }
static void OnInt1(struct As *a, struct Slice s) { EmitByte(a, 0xF1); }
static void OnInt3(struct As *a, struct Slice s) { EmitByte(a, 0xCC); }
static void OnJb(struct As *a, struct Slice s) { OpJmp(a, 2); }
static void OnJbe(struct As *a, struct Slice s) { OpJmp(a, 6); }
static void OnJl(struct As *a, struct Slice s) { OpJmp(a, 12); }
static void OnJle(struct As *a, struct Slice s) { OpJmp(a, 14); }
static void OnJmp(struct As *a, struct Slice s) { OpJmp(a, -1); }
static void OnJnb(struct As *a, struct Slice s) { OpJmp(a, 3); }
static void OnJnbe(struct As *a, struct Slice s) { OpJmp(a, 7); }
static void OnJnl(struct As *a, struct Slice s) { OpJmp(a, 13); }
static void OnJnle(struct As *a, struct Slice s) { OpJmp(a, 15); }
static void OnJno(struct As *a, struct Slice s) { OpJmp(a, 1); }
static void OnJnp(struct As *a, struct Slice s) { OpJmp(a, 11); }
static void OnJns(struct As *a, struct Slice s) { OpJmp(a, 9); }
static void OnJnz(struct As *a, struct Slice s) { OpJmp(a, 5); }
static void OnJo(struct As *a, struct Slice s) { OpJmp(a, 0); }
static void OnJp(struct As *a, struct Slice s) { OpJmp(a, 10); }
static void OnJs(struct As *a, struct Slice s) { OpJmp(a, 8); }
static void OnJz(struct As *a, struct Slice s) { OpJmp(a, 4); }
static void OnLeave(struct As *a, struct Slice s) { EmitByte(a, 0xC9); }
static void OnLodsb(struct As *a, struct Slice s) { EmitByte(a, 0xAC); }
static void OnLodsl(struct As *a, struct Slice s) { EmitByte(a, 0xAD); }
static void OnLodsq(struct As *a, struct Slice s) { EmitVarword(a, 0x48AD); }
static void OnLodsw(struct As *a, struct Slice s) { EmitVarword(a, 0x66AD); }
static void OnMaxpd(struct As *a, struct Slice s) { OpSse(a, 0x660F5F); }
static void OnMaxps(struct As *a, struct Slice s) { OpSse(a, 0x0F5F); }
static void OnMaxsd(struct As *a, struct Slice s) { OpSse(a, 0xF20F5F); }
static void OnMaxss(struct As *a, struct Slice s) { OpSse(a, 0xF30F5F); }
static void OnMfence(struct As *a, struct Slice s) { EmitVarword(a, 0x0faef0); }
static void OnMinpd(struct As *a, struct Slice s) { OpSse(a, 0x660F5D); }
static void OnMinps(struct As *a, struct Slice s) { OpSse(a, 0x0F5D); }
static void OnMinsd(struct As *a, struct Slice s) { OpSse(a, 0xF20F5D); }
static void OnMinss(struct As *a, struct Slice s) { OpSse(a, 0xF30F5D); }
static void OnMovmskpd(struct As *a, struct Slice s) { OpSse(a, 0x660F50); }
static void OnMovmskps(struct As *a, struct Slice s) { OpSse(a, 0x0F50); }
static void OnMovntdq(struct As *a, struct Slice s) { OpMovntdq(a); }
static void OnMovsb(struct As *a, struct Slice s) { EmitByte(a, 0xA4); }
static void OnMovsl(struct As *a, struct Slice s) { EmitByte(a, 0xA5); }
static void OnMovsq(struct As *a, struct Slice s) { EmitVarword(a, 0x48A5); }
static void OnMovsw(struct As *a, struct Slice s) { EmitVarword(a, 0x66A5); }
static void OnMpsadbw(struct As *a, struct Slice s) { OpSseIb(a, 0x660F3A42); }
static void OnMul(struct As *a, struct Slice s) { OpF6(a, s, 4); }
static void OnMulpd(struct As *a, struct Slice s) { OpSse(a, 0x660F59); }
static void OnMulps(struct As *a, struct Slice s) { OpSse(a, 0x0F59); }
static void OnMulsd(struct As *a, struct Slice s) { OpSse(a, 0xF20F59); }
static void OnMulss(struct As *a, struct Slice s) { OpSse(a, 0xF30F59); }
static void OnNeg(struct As *a, struct Slice s) { OpF6(a, s, 3); }
static void OnNot(struct As *a, struct Slice s) { OpF6(a, s, 2); }
static void OnOr(struct As *a, struct Slice s) { OpAlu(a, s, 1); }
static void OnOrpd(struct As *a, struct Slice s) { OpSse(a, 0x660F56); }
static void OnOrps(struct As *a, struct Slice s) { OpSse(a, 0x0F56); }
static void OnPabsb(struct As *a, struct Slice s) { OpSse(a, 0x660F381C); }
static void OnPabsd(struct As *a, struct Slice s) { OpSse(a, 0x660F381E); }
static void OnPabsw(struct As *a, struct Slice s) { OpSse(a, 0x660F381D); }
static void OnPackssdw(struct As *a, struct Slice s) { OpSse(a, 0x660F6B); }
static void OnPacksswb(struct As *a, struct Slice s) { OpSse(a, 0x660F63); }
static void OnPackusdw(struct As *a, struct Slice s) { OpSse(a, 0x660F382B); }
static void OnPackuswb(struct As *a, struct Slice s) { OpSse(a, 0x660F67); }
static void OnPaddb(struct As *a, struct Slice s) { OpSse(a, 0x660FFC); }
static void OnPaddd(struct As *a, struct Slice s) { OpSse(a, 0x660FFE); }
static void OnPaddq(struct As *a, struct Slice s) { OpSse(a, 0x660FD4); }
static void OnPaddsb(struct As *a, struct Slice s) { OpSse(a, 0x660FEC); }
static void OnPaddsw(struct As *a, struct Slice s) { OpSse(a, 0x660FED); }
static void OnPaddusb(struct As *a, struct Slice s) { OpSse(a, 0x660FDC); }
static void OnPaddusw(struct As *a, struct Slice s) { OpSse(a, 0x660FDD); }
static void OnPaddw(struct As *a, struct Slice s) { OpSse(a, 0x660FFD); }
static void OnPalignr(struct As *a, struct Slice s) { OpSse(a, 0x660F3A0F); }
static void OnPand(struct As *a, struct Slice s) { OpSse(a, 0x660FDB); }
static void OnPandn(struct As *a, struct Slice s) { OpSse(a, 0x660FDF); }
static void OnPause(struct As *a, struct Slice s) { EmitVarword(a, 0xF390); }
static void OnPavgb(struct As *a, struct Slice s) { OpSse(a, 0x660FE0); }
static void OnPavgw(struct As *a, struct Slice s) { OpSse(a, 0x660FE3); }
static void OnPblendvb(struct As *a, struct Slice s) { OpSse(a, 0x660F3810); }
static void OnPblendw(struct As *a, struct Slice s) { OpSseIb(a, 0x660F3A0E); }
static void OnPcmpeqb(struct As *a, struct Slice s) { OpSse(a, 0x660F74); }
static void OnPcmpeqd(struct As *a, struct Slice s) { OpSse(a, 0x660F76); }
static void OnPcmpeqq(struct As *a, struct Slice s) { OpSse(a, 0x660F3829); }
static void OnPcmpeqw(struct As *a, struct Slice s) { OpSse(a, 0x660F75); }
static void OnPcmpgtb(struct As *a, struct Slice s) { OpSse(a, 0x660F64); }
static void OnPcmpgtd(struct As *a, struct Slice s) { OpSse(a, 0x660F66); }
static void OnPcmpgtq(struct As *a, struct Slice s) { OpSse(a, 0x660F3837); }
static void OnPcmpgtw(struct As *a, struct Slice s) { OpSse(a, 0x660F65); }
static void OnPcmpistri(struct As *a, struct Slice s) { OpSseIb(a, 0x660F3A63); }
static void OnPcmpistrm(struct As *a, struct Slice s) { OpSseIb(a, 0x660F3A62); }
static void OnPhaddd(struct As *a, struct Slice s) { OpSse(a, 0x660F3802); }
static void OnPhaddsw(struct As *a, struct Slice s) { OpSse(a, 0x660F3803); }
static void OnPhaddw(struct As *a, struct Slice s) { OpSse(a, 0x660F3801); }
static void OnPhminposuw(struct As *a, struct Slice s) { OpSse(a, 0x660F3841); }
static void OnPhsubd(struct As *a, struct Slice s) { OpSse(a, 0x660F3806); }
static void OnPhsubsw(struct As *a, struct Slice s) { OpSse(a, 0x660F3807); }
static void OnPhsubw(struct As *a, struct Slice s) { OpSse(a, 0x660F3805); }
static void OnPmaddubsw(struct As *a, struct Slice s) { OpSse(a, 0x660F3804); }
static void OnPmaddwd(struct As *a, struct Slice s) { OpSse(a, 0x660FF5); }
static void OnPmaxsb(struct As *a, struct Slice s) { OpSse(a, 0x660F383C); }
static void OnPmaxsd(struct As *a, struct Slice s) { OpSse(a, 0x660F383D); }
static void OnPmaxsw(struct As *a, struct Slice s) { OpSse(a, 0x660FEE); }
static void OnPmaxub(struct As *a, struct Slice s) { OpSse(a, 0x660FDE); }
static void OnPmaxud(struct As *a, struct Slice s) { OpSse(a, 0x660F383F); }
static void OnPmaxuw(struct As *a, struct Slice s) { OpSse(a, 0x660F383E); }
static void OnPminsb(struct As *a, struct Slice s) { OpSse(a, 0x660F3838); }
static void OnPminsd(struct As *a, struct Slice s) { OpSse(a, 0x660F3839); }
static void OnPminsw(struct As *a, struct Slice s) { OpSse(a, 0x660FEA); }
static void OnPminub(struct As *a, struct Slice s) { OpSse(a, 0x660FDA); }
static void OnPminud(struct As *a, struct Slice s) { OpSse(a, 0x660F383B); }
static void OnPminuw(struct As *a, struct Slice s) { OpSse(a, 0x660F383A); }
static void OnPmovmskb(struct As *a, struct Slice s) { OpSse(a, 0x660FD7); }
static void OnPmuldq(struct As *a, struct Slice s) { OpSse(a, 0x660F3828); }
static void OnPmulhrsw(struct As *a, struct Slice s) { OpSse(a, 0x660F380B); }
static void OnPmulhuw(struct As *a, struct Slice s) { OpSse(a, 0x660FE4); }
static void OnPmulhw(struct As *a, struct Slice s) { OpSse(a, 0x660FE5); }
static void OnPmulld(struct As *a, struct Slice s) { OpSse(a, 0x660F3840); }
static void OnPmullw(struct As *a, struct Slice s) { OpSse(a, 0x660FD5); }
static void OnPmuludq(struct As *a, struct Slice s) { OpSse(a, 0x660FF4); }
static void OnPopcnt(struct As *a, struct Slice s) { OpBit(a, 0xF30FB8); }
static void OnPor(struct As *a, struct Slice s) { OpSse(a, 0x660FEB); }
static void OnPsadbw(struct As *a, struct Slice s) { OpSse(a, 0x660FF6); }
static void OnPshufb(struct As *a, struct Slice s) { OpSse(a, 0x660F3800); }
static void OnPshufd(struct As *a, struct Slice s) { OpSseIb(a, 0x660F70); }
static void OnPshufhw(struct As *a, struct Slice s) { OpSseIb(a, 0xF30F70); }
static void OnPshuflw(struct As *a, struct Slice s) { OpSseIb(a, 0xF20F70); }
static void OnPsignb(struct As *a, struct Slice s) { OpSse(a, 0x660F3808); }
static void OnPsignd(struct As *a, struct Slice s) { OpSse(a, 0x660F380A); }
static void OnPsignw(struct As *a, struct Slice s) { OpSse(a, 0x660F3809); }
static void OnPslld(struct As *a, struct Slice s) { OpSse(a, 0x660FF2); }
static void OnPsllq(struct As *a, struct Slice s) { OpSse(a, 0x660FF3); }
static void OnPsllw(struct As *a, struct Slice s) { OpSse(a, 0x660FF1); }
static void OnPsrad(struct As *a, struct Slice s) { OpSse(a, 0x660FE2); }
static void OnPsraw(struct As *a, struct Slice s) { OpSse(a, 0x660FE1); }
static void OnPsrld(struct As *a, struct Slice s) { OpSse(a, 0x660FD2); }
static void OnPsrlq(struct As *a, struct Slice s) { OpSse(a, 0x660FD3); }
static void OnPsrlw(struct As *a, struct Slice s) { OpSse(a, 0x660FD1); }
static void OnPsubb(struct As *a, struct Slice s) { OpSse(a, 0x660FF8); }
static void OnPsubd(struct As *a, struct Slice s) { OpSse(a, 0x660FFA); }
static void OnPsubq(struct As *a, struct Slice s) { OpSse(a, 0x660FFB); }
static void OnPsubsb(struct As *a, struct Slice s) { OpSse(a, 0x660FE8); }
static void OnPsubsw(struct As *a, struct Slice s) { OpSse(a, 0x660FE9); }
static void OnPsubusb(struct As *a, struct Slice s) { OpSse(a, 0x660FD8); }
static void OnPsubusw(struct As *a, struct Slice s) { OpSse(a, 0x660FD9); }
static void OnPsubw(struct As *a, struct Slice s) { OpSse(a, 0x660FF9); }
static void OnPtest(struct As *a, struct Slice s) { OpSse(a, 0x660F3817); }
static void OnPunpckhbw(struct As *a, struct Slice s) { OpSse(a, 0x660F68); }
static void OnPunpckhdq(struct As *a, struct Slice s) { OpSse(a, 0x660F6A); }
static void OnPunpckhqdq(struct As *a, struct Slice s) { OpSse(a, 0x660F6D); }
static void OnPunpckhwd(struct As *a, struct Slice s) { OpSse(a, 0x660F69); }
static void OnPunpcklbw(struct As *a, struct Slice s) { OpSse(a, 0x660F60); }
static void OnPunpckldq(struct As *a, struct Slice s) { OpSse(a, 0x660F62); }
static void OnPunpcklqdq(struct As *a, struct Slice s) { OpSse(a, 0x660F6C); }
static void OnPunpcklwd(struct As *a, struct Slice s) { OpSse(a, 0x660F61); }
static void OnPxor(struct As *a, struct Slice s) { OpSse(a, 0x660FEF); }
static void OnRcl(struct As *a, struct Slice s) { OpBsu(a, s, 2); }
static void OnRcpps(struct As *a, struct Slice s) { OpSse(a, 0x0F53); }
static void OnRcpss(struct As *a, struct Slice s) { OpSse(a, 0xF30F53); }
static void OnRcr(struct As *a, struct Slice s) { OpBsu(a, s, 3); }
static void OnRdtsc(struct As *a, struct Slice s) { EmitVarword(a, 0x0f31); }
static void OnRdtscp(struct As *a, struct Slice s) { EmitVarword(a, 0x0f01f9); }
static void OnRol(struct As *a, struct Slice s) { OpBsu(a, s, 0); }
static void OnRor(struct As *a, struct Slice s) { OpBsu(a, s, 1); }
static void OnRoundsd(struct As *a, struct Slice s) { OpSseIb(a, 0x660F3A0B); }
static void OnRoundss(struct As *a, struct Slice s) { OpSseIb(a, 0x660F3A0A); }
static void OnRsqrtps(struct As *a, struct Slice s) { OpSse(a, 0x0F52); }
static void OnRsqrtss(struct As *a, struct Slice s) { OpSse(a, 0xF30F52); }
static void OnSal(struct As *a, struct Slice s) { OpBsu(a, s, 6); }
static void OnSar(struct As *a, struct Slice s) { OpBsu(a, s, 7); }
static void OnSbb(struct As *a, struct Slice s) { OpAlu(a, s, 3); }
static void OnSetb(struct As *a, struct Slice s) { OpSetcc(a, 2); }
static void OnSetbe(struct As *a, struct Slice s) { OpSetcc(a, 6); }
static void OnSetl(struct As *a, struct Slice s) { OpSetcc(a, 12); }
static void OnSetle(struct As *a, struct Slice s) { OpSetcc(a, 14); }
static void OnSetnb(struct As *a, struct Slice s) { OpSetcc(a, 3); }
static void OnSetnbe(struct As *a, struct Slice s) { OpSetcc(a, 7); }
static void OnSetnl(struct As *a, struct Slice s) { OpSetcc(a, 13); }
static void OnSetnle(struct As *a, struct Slice s) { OpSetcc(a, 15); }
static void OnSetno(struct As *a, struct Slice s) { OpSetcc(a, 1); }
static void OnSetnp(struct As *a, struct Slice s) { OpSetcc(a, 11); }
static void OnSetns(struct As *a, struct Slice s) { OpSetcc(a, 9); }
static void OnSetnz(struct As *a, struct Slice s) { OpSetcc(a, 5); }
static void OnSeto(struct As *a, struct Slice s) { OpSetcc(a, 0); }
static void OnSetp(struct As *a, struct Slice s) { OpSetcc(a, 10); }
static void OnSets(struct As *a, struct Slice s) { OpSetcc(a, 8); }
static void OnSetz(struct As *a, struct Slice s) { OpSetcc(a, 4); }
static void OnSfence(struct As *a, struct Slice s) { EmitVarword(a, 0x0faef8); }
static void OnShl(struct As *a, struct Slice s) { OpBsu(a, s, 4); }
static void OnShr(struct As *a, struct Slice s) { OpBsu(a, s, 5); }
static void OnShufpd(struct As *a, struct Slice s) { OpSseIb(a, 0x660FC6); }
static void OnShufps(struct As *a, struct Slice s) { OpSseIb(a, 0x0FC6); }
static void OnSqrtpd(struct As *a, struct Slice s) { OpSse(a, 0x660F51); }
static void OnSqrtps(struct As *a, struct Slice s) { OpSse(a, 0x0F51); }
static void OnSqrtsd(struct As *a, struct Slice s) { OpSse(a, 0xF20F51); }
static void OnSqrtss(struct As *a, struct Slice s) { OpSse(a, 0xF30F51); }
static void OnStc(struct As *a, struct Slice s) { EmitByte(a, 0xF9); }
static void OnStd(struct As *a, struct Slice s) { EmitByte(a, 0xFD); }
static void OnSti(struct As *a, struct Slice s) { EmitByte(a, 0xFB); }
static void OnStosb(struct As *a, struct Slice s) { EmitByte(a, 0xAA); }
static void OnStosl(struct As *a, struct Slice s) { EmitByte(a, 0xAB); }
static void OnStosq(struct As *a, struct Slice s) { EmitVarword(a, 0x48AB); }
static void OnStosw(struct As *a, struct Slice s) { EmitVarword(a, 0x66AB); }
static void OnSub(struct As *a, struct Slice s) { OpAlu(a, s, 5); }
static void OnSubpd(struct As *a, struct Slice s) { OpSse(a, 0x660F5C); }
static void OnSubps(struct As *a, struct Slice s) { OpSse(a, 0x0F5C); }
static void OnSubsd(struct As *a, struct Slice s) { OpSse(a, 0xF20F5C); }
static void OnSubss(struct As *a, struct Slice s) { OpSse(a, 0xF30F5C); }
static void OnSyscall(struct As *a, struct Slice s) { EmitVarword(a, 0x0F05); }
static void OnUcomisd(struct As *a, struct Slice s) { OpSse(a, 0x660F2E); }
static void OnUcomiss(struct As *a, struct Slice s) { OpSse(a, 0x0F2E); }
static void OnUd2(struct As *a, struct Slice s) { EmitVarword(a, 0x0F0B); }
static void OnUnpckhpd(struct As *a, struct Slice s) { OpSse(a, 0x660F15); }
static void OnUnpcklpd(struct As *a, struct Slice s) { OpSse(a, 0x660F14); }
static void OnXadd(struct As *a, struct Slice s) { OpXadd(a); }
static void OnXor(struct As *a, struct Slice s) { OpAlu(a, s, 6); }
static void OnXorpd(struct As *a, struct Slice s) { OpSse(a, 0x660F57); }
static void OnXorps(struct As *a, struct Slice s) { OpSse(a, 0x0F57); }
// clang-format on

static const struct Directive8 {
  char s[8];
  void (*f)(struct As *, struct Slice);
} kDirective8[] = {
    {".abort", OnAbort},       //
    {".align", OnAlign},       //
    {".ascii", OnAscii},       //
    {".asciz", OnAsciz},       //
    {".balign", OnAlign},      //
    {".bss", OnBss},           //
    {".byte", OnByte},         //
    {".comm", OnComm},         //
    {".data", OnData},         //
    {".double", OnDouble},     //
    {".equ", OnEqu},           //
    {".err", OnErr},           //
    {".error", OnError},       //
    {".file", OnFile},         //
    {".float", OnFloat},       //
    {".float80", OnFloat80},   //
    {".global", OnGlobal},     //
    {".globl", OnGlobal},      //
    {".hidden", OnHidden},     //
    {".ident", OnIdent},       //
    {".incbin", OnIncbin},     //
    {".ldbl", OnLdbl},         //
    {".loc", OnLoc},           //
    {".local", OnLocal},       //
    {".long", OnLong},         //
    {".octa", OnOcta},         //
    {".quad", OnQuad},         //
    {".section", OnSection},   //
    {".short", OnWord},        //
    {".size", OnSize},         //
    {".sleb128", OnSleb128},   //
    {".space", OnSpace},       //
    {".text", OnText},         //
    {".type", OnType},         //
    {".uleb128", OnUleb128},   //
    {".warning", OnWarning},   //
    {".weak", OnWeak},         //
    {".word", OnWord},         //
    {".zero", OnZero},         //
    {".zleb128", OnZleb128},   //
    {"adc", OnAdc},            //
    {"adcb", OnAdc},           //
    {"adcl", OnAdc},           //
    {"adcq", OnAdc},           //
    {"adcw", OnAdc},           //
    {"add", OnAdd},            //
    {"addb", OnAdd},           //
    {"addl", OnAdd},           //
    {"addpd", OnAddpd},        //
    {"addps", OnAddps},        //
    {"addq", OnAdd},           //
    {"addsd", OnAddsd},        //
    {"addss", OnAddss},        //
    {"addsubpd", OnAddsubpd},  //
    {"addsubps", OnAddsubps},  //
    {"addw", OnAdd},           //
    {"and", OnAnd},            //
    {"andb", OnAnd},           //
    {"andl", OnAnd},           //
    {"andnpd", OnAndnpd},      //
    {"andnps", OnAndnps},      //
    {"andpd", OnAndpd},        //
    {"andps", OnAndps},        //
    {"andq", OnAnd},           //
    {"andw", OnAnd},           //
    {"blendpd", OnBlendpd},    //
    {"blendvpd", OnBlendvpd},  //
    {"bsf", OnBsf},            //
    {"bsr", OnBsr},            //
    {"bswap", OnBswap},        //
    {"call", OnCall},          //
    {"callq", OnCall},         //
    {"cbtw", OnCbtw},          //
    {"cbw", OnCbtw},           //
    {"cdq", OnCltd},           //
    {"cdqe", OnCltq},          //
    {"clc", OnClc},            //
    {"cld", OnCld},            //
    {"cli", OnCli},            //
    {"cltd", OnCltd},          //
    {"cltq", OnCltq},          //
    {"cmc", OnCmc},            //
    {"cmova", OnCmovnbe},      //
    {"cmovae", OnCmovnb},      //
    {"cmovb", OnCmovb},        //
    {"cmovbe", OnCmovbe},      //
    {"cmovc", OnCmovb},        //
    {"cmove", OnCmovz},        //
    {"cmovg", OnCmovnle},      //
    {"cmovge", OnCmovnl},      //
    {"cmovl", OnCmovl},        //
    {"cmovle", OnCmovle},      //
    {"cmovna", OnCmovbe},      //
    {"cmovnae", OnCmovb},      //
    {"cmovnb", OnCmovnb},      //
    {"cmovnbe", OnCmovnbe},    //
    {"cmovnc", OnCmovnb},      //
    {"cmovne", OnCmovnz},      //
    {"cmovng", OnCmovle},      //
    {"cmovnge", OnCmovl},      //
    {"cmovnl", OnCmovnl},      //
    {"cmovnle", OnCmovnle},    //
    {"cmovno", OnCmovno},      //
    {"cmovnp", OnCmovnp},      //
    {"cmovns", OnCmovns},      //
    {"cmovnz", OnCmovnz},      //
    {"cmovo", OnCmovo},        //
    {"cmovp", OnCmovp},        //
    {"cmovpe", OnCmovp},       //
    {"cmovpo", OnCmovnp},      //
    {"cmovs", OnCmovs},        //
    {"cmovz", OnCmovz},        //
    {"cmp", OnCmp},            //
    {"cmpb", OnCmp},           //
    {"cmpl", OnCmp},           //
    {"cmppd", OnCmppd},        //
    {"cmpps", OnCmpps},        //
    {"cmpq", OnCmp},           //
    {"cmpsd", OnCmpsd},        //
    {"cmpss", OnCmpss},        //
    {"cmpw", OnCmp},           //
    {"cmpxchg", OnCmpxchg},    //
    {"comisd", OnComisd},      //
    {"comiss", OnComiss},      //
    {"cqo", OnCqto},           //
    {"cqto", OnCqto},          //
    {"cvtdq2pd", OnCvtdq2pd},  //
    {"cvtdq2ps", OnCvtdq2ps},  //
    {"cvtpd2dq", OnCvtpd2dq},  //
    {"cvtpd2ps", OnCvtpd2ps},  //
    {"cvtps2dq", OnCvtps2dq},  //
    {"cvtps2pd", OnCvtps2pd},  //
    {"cvtsd2ss", OnCvtsd2ss},  //
    {"cvtsd2ss", OnCvtsd2ss},  //
    {"cvtsi2sd", OnCvtsi2sd},  //
    {"cvtsi2sd", OnCvtsi2sd},  //
    {"cvtsi2ss", OnCvtsi2ss},  //
    {"cvtsi2ss", OnCvtsi2ss},  //
    {"cvtss2sd", OnCvtss2sd},  //
    {"cwd", OnCwtd},           //
    {"cwde", OnCwtl},          //
    {"cwtd", OnCwtd},          //
    {"cwtl", OnCwtl},          //
    {"dec", OnDec},            //
    {"decb", OnDec},           //
    {"decl", OnDec},           //
    {"decq", OnDec},           //
    {"decw", OnDec},           //
    {"div", OnDiv},            //
    {"divpd", OnDivpd},        //
    {"divps", OnDivps},        //
    {"divsd", OnDivsd},        //
    {"divss", OnDivss},        //
    {"dppd", OnDppd},          //
    {"fabs", OnFabs},          //
    {"faddl", OnFaddl},        //
    {"faddp", OnFaddp},        //
    {"fadds", OnFadds},        //
    {"fchs", OnFchs},          //
    {"fcmovb", OnFcmovb},      //
    {"fcmovbe", OnFcmovbe},    //
    {"fcmove", OnFcmove},      //
    {"fcmovnb", OnFcmovnb},    //
    {"fcmovnbe", OnFcmovnbe},  //
    {"fcmovne", OnFcmovne},    //
    {"fcmovnu", OnFcmovnu},    //
    {"fcmovu", OnFcmovu},      //
    {"fcomi", OnFcomi},        //
    {"fcomip", OnFcomip},      //
    {"fdivrp", OnFdivrp},      //
    {"fildl", OnFildl},        //
    {"fildll", OnFildll},      //
    {"fildq", OnFildq},        //
    {"filds", OnFilds},        //
    {"fistpll", OnFistpq},     //
    {"fistpq", OnFistpq},      //
    {"fisttpll", OnFisttpq},   //
    {"fisttpq", OnFisttpq},    //
    {"fisttps", OnFisttps},    //
    {"fld", OnFld},            //
    {"fld1", OnFld1},          //
    {"fldcw", OnFldcw},        //
    {"fldl", OnFldl},          //
    {"fldl2e", OnFldl2e},      //
    {"fldl2t", OnFldl2t},      //
    {"fldlg2", OnFldlg2},      //
    {"fldln2", OnFldln2},      //
    {"fldpi", OnFldpi},        //
    {"flds", OnFlds},          //
    {"fldt", OnFldt},          //
    {"fldz", OnFldz},          //
    {"fmulp", OnFmulp},        //
    {"fnstcw", OnFnstcw},      //
    {"fnstsw", OnFnstsw},      //
    {"fstp", OnFstp},          //
    {"fstpl", OnFstpl},        //
    {"fstps", OnFstps},        //
    {"fstpt", OnFstpt},        //
    {"fsubrp", OnFsubrp},      //
    {"ftst", OnFtst},          //
    {"fucomi", OnFucomi},      //
    {"fucomip", OnFucomip},    //
    {"fwait", OnFwait},        //
    {"fxam", OnFxam},          //
    {"fxch", OnFxch},          //
    {"fxtract", OnFxtract},    //
    {"haddpd", OnHaddpd},      //
    {"haddps", OnHaddps},      //
    {"hlt", OnHlt},            //
    {"hsubpd", OnHsubpd},      //
    {"hsubps", OnHsubps},      //
    {"icebp", OnInt1},         //
    {"idiv", OnIdiv},          //
    {"imul", OnImul},          //
    {"inc", OnInc},            //
    {"incb", OnInc},           //
    {"incl", OnInc},           //
    {"incq", OnInc},           //
    {"incw", OnInc},           //
    {"int1", OnInt1},          //
    {"int3", OnInt3},          //
    {"ja", OnJnbe},            //
    {"jae", OnJnb},            //
    {"jb", OnJb},              //
    {"jbe", OnJbe},            //
    {"jc", OnJb},              //
    {"je", OnJz},              //
    {"jg", OnJnle},            //
    {"jge", OnJnl},            //
    {"jl", OnJl},              //
    {"jle", OnJle},            //
    {"jmp", OnJmp},            //
    {"jmpq", OnJmp},           //
    {"jna", OnJbe},            //
    {"jnae", OnJb},            //
    {"jnb", OnJnb},            //
    {"jnbe", OnJnbe},          //
    {"jnc", OnJnb},            //
    {"jne", OnJnz},            //
    {"jng", OnJle},            //
    {"jnge", OnJl},            //
    {"jnl", OnJnl},            //
    {"jnle", OnJnle},          //
    {"jno", OnJno},            //
    {"jnp", OnJnp},            //
    {"jns", OnJns},            //
    {"jnz", OnJnz},            //
    {"jo", OnJo},              //
    {"jp", OnJp},              //
    {"jpe", OnJp},             //
    {"jpo", OnJnp},            //
    {"js", OnJs},              //
    {"jz", OnJz},              //
    {"lar", OnLar},            //
    {"lea", OnLea},            //
    {"leave", OnLeave},        //
    {"lodsb", OnLodsb},        //
    {"lodsl", OnLodsl},        //
    {"lodsq", OnLodsq},        //
    {"lodsw", OnLodsw},        //
    {"lsl", OnLsl},            //
    {"maxpd", OnMaxpd},        //
    {"maxps", OnMaxps},        //
    {"maxsd", OnMaxsd},        //
    {"maxss", OnMaxss},        //
    {"mfence", OnMfence},      //
    {"minpd", OnMinpd},        //
    {"minps", OnMinps},        //
    {"minsd", OnMinsd},        //
    {"minss", OnMinss},        //
    {"mov", OnMov},            //
    {"movabs", OnMov},         //
    {"movapd", OnMovapd},      //
    {"movaps", OnMovaps},      //
    {"movb", OnMov},           //
    {"movd", OnMovd},          //
    {"movdqa", OnMovdqa},      //
    {"movdqa", OnMovdqa},      //
    {"movdqu", OnMovdqu},      //
    {"movdqu", OnMovdqu},      //
    {"movl", OnMov},           //
    {"movmskpd", OnMovmskpd},  //
    {"movmskps", OnMovmskps},  //
    {"movq", OnMovq},          //
    {"movsb", OnMovsb},        //
    {"movsbl", OnMovsbwx},     //
    {"movsbq", OnMovsbwx},     //
    {"movsbw", OnMovsbwx},     //
    {"movsd", OnMovsd},        //
    {"movsd", OnMovsd},        //
    {"movsl", OnMovsl},        //
    {"movslq", OnMovslq},      //
    {"movsq", OnMovsq},        //
    {"movss", OnMovss},        //
    {"movss", OnMovss},        //
    {"movsw", OnMovsw},        //
    {"movswl", OnMovsbwx},     //
    {"movswq", OnMovsbwx},     //
    {"movupd", OnMovupd},      //
    {"movups", OnMovups},      //
    {"movw", OnMov},           //
    {"movzbl", OnMovzbwx},     //
    {"movzbq", OnMovzbwx},     //
    {"movzbw", OnMovzbwx},     //
    {"movzwl", OnMovzbwx},     //
    {"movzwq", OnMovzbwx},     //
    {"mpsadbw", OnMpsadbw},    //
    {"mul", OnMul},            //
    {"mulpd", OnMulpd},        //
    {"mulps", OnMulps},        //
    {"mulsd", OnMulsd},        //
    {"mulss", OnMulss},        //
    {"neg", OnNeg},            //
    {"negb", OnNeg},           //
    {"negl", OnNeg},           //
    {"negq", OnNeg},           //
    {"negw", OnNeg},           //
    {"nop", OnNop},            //
    {"nopb", OnNop},           //
    {"nopl", OnNop},           //
    {"nopq", OnNop},           //
    {"nopw", OnNop},           //
    {"not", OnNot},            //
    {"notb", OnNot},           //
    {"notl", OnNot},           //
    {"notq", OnNot},           //
    {"notw", OnNot},           //
    {"or", OnOr},              //
    {"orb", OnOr},             //
    {"orl", OnOr},             //
    {"orpd", OnOrpd},          //
    {"orps", OnOrps},          //
    {"orq", OnOr},             //
    {"orw", OnOr},             //
    {"pabsb", OnPabsb},        //
    {"pabsd", OnPabsd},        //
    {"pabsw", OnPabsw},        //
    {"packssdw", OnPackssdw},  //
    {"packsswb", OnPacksswb},  //
    {"packusdw", OnPackusdw},  //
    {"packuswb", OnPackuswb},  //
    {"paddb", OnPaddb},        //
    {"paddd", OnPaddd},        //
    {"paddq", OnPaddq},        //
    {"paddsb", OnPaddsb},      //
    {"paddsw", OnPaddsw},      //
    {"paddusb", OnPaddusb},    //
    {"paddusw", OnPaddusw},    //
    {"paddw", OnPaddw},        //
    {"palignr", OnPalignr},    //
    {"pand", OnPand},          //
    {"pandn", OnPandn},        //
    {"pause", OnPause},        //
    {"pavgb", OnPavgb},        //
    {"pavgw", OnPavgw},        //
    {"pblendvb", OnPblendvb},  //
    {"pblendw", OnPblendw},    //
    {"pcmpeqb", OnPcmpeqb},    //
    {"pcmpeqd", OnPcmpeqd},    //
    {"pcmpeqq", OnPcmpeqq},    //
    {"pcmpeqw", OnPcmpeqw},    //
    {"pcmpgtb", OnPcmpgtb},    //
    {"pcmpgtd", OnPcmpgtd},    //
    {"pcmpgtq", OnPcmpgtq},    //
    {"pcmpgtw", OnPcmpgtw},    //
    {"phaddd", OnPhaddd},      //
    {"phaddsw", OnPhaddsw},    //
    {"phaddw", OnPhaddw},      //
    {"phsubd", OnPhsubd},      //
    {"phsubsw", OnPhsubsw},    //
    {"phsubw", OnPhsubw},      //
    {"pmaddwd", OnPmaddwd},    //
    {"pmaxsb", OnPmaxsb},      //
    {"pmaxsd", OnPmaxsd},      //
    {"pmaxsw", OnPmaxsw},      //
    {"pmaxub", OnPmaxub},      //
    {"pmaxud", OnPmaxud},      //
    {"pmaxuw", OnPmaxuw},      //
    {"pminsb", OnPminsb},      //
    {"pminsd", OnPminsd},      //
    {"pminsw", OnPminsw},      //
    {"pminub", OnPminub},      //
    {"pminud", OnPminud},      //
    {"pminuw", OnPminuw},      //
    {"pmovmskb", OnPmovmskb},  //
    {"pmuldq", OnPmuldq},      //
    {"pmulhrsw", OnPmulhrsw},  //
    {"pmulhuw", OnPmulhuw},    //
    {"pmulhw", OnPmulhw},      //
    {"pmulld", OnPmulld},      //
    {"pmullw", OnPmullw},      //
    {"pmuludq", OnPmuludq},    //
    {"pop", OnPop},            //
    {"popcnt", OnPopcnt},      //
    {"por", OnPor},            //
    {"psadbw", OnPsadbw},      //
    {"pshufb", OnPshufb},      //
    {"pshufd", OnPshufd},      //
    {"pshufhw", OnPshufhw},    //
    {"pshuflw", OnPshuflw},    //
    {"psignb", OnPsignb},      //
    {"psignd", OnPsignd},      //
    {"psignw", OnPsignw},      //
    {"pslld", OnPslld},        //
    {"psllq", OnPsllq},        //
    {"psllw", OnPsllw},        //
    {"psrad", OnPsrad},        //
    {"psraw", OnPsraw},        //
    {"psrld", OnPsrld},        //
    {"psrlq", OnPsrlq},        //
    {"psrlw", OnPsrlw},        //
    {"psubb", OnPsubb},        //
    {"psubd", OnPsubd},        //
    {"psubq", OnPsubq},        //
    {"psubsb", OnPsubsb},      //
    {"psubsw", OnPsubsw},      //
    {"psubusb", OnPsubusb},    //
    {"psubusw", OnPsubusw},    //
    {"psubw", OnPsubw},        //
    {"ptest", OnPtest},        //
    {"push", OnPush},          //
    {"pxor", OnPxor},          //
    {"rcl", OnRcl},            //
    {"rclb", OnRcl},           //
    {"rcll", OnRcl},           //
    {"rclq", OnRcl},           //
    {"rclw", OnRcl},           //
    {"rcpps", OnRcpps},        //
    {"rcpss", OnRcpss},        //
    {"rcr", OnRcr},            //
    {"rcrb", OnRcr},           //
    {"rcrl", OnRcr},           //
    {"rcrq", OnRcr},           //
    {"rcrw", OnRcr},           //
    {"rdpid", OnRdpid},        //
    {"rdtsc", OnRdtsc},        //
    {"rdtscp", OnRdtscp},      //
    {"ret", OnRet},            //
    {"rol", OnRol},            //
    {"rolb", OnRol},           //
    {"roll", OnRol},           //
    {"rolq", OnRol},           //
    {"rolw", OnRol},           //
    {"ror", OnRor},            //
    {"rorb", OnRor},           //
    {"rorl", OnRor},           //
    {"rorq", OnRor},           //
    {"rorw", OnRor},           //
    {"roundsd", OnRoundsd},    //
    {"roundss", OnRoundss},    //
    {"rsqrtps", OnRsqrtps},    //
    {"rsqrtss", OnRsqrtss},    //
    {"sal", OnSal},            //
    {"salb", OnSal},           //
    {"sall", OnSal},           //
    {"salq", OnSal},           //
    {"salw", OnSal},           //
    {"sar", OnSar},            //
    {"sarb", OnSar},           //
    {"sarl", OnSar},           //
    {"sarq", OnSar},           //
    {"sarw", OnSar},           //
    {"sbb", OnSbb},            //
    {"sbbb", OnSbb},           //
    {"sbbl", OnSbb},           //
    {"sbbq", OnSbb},           //
    {"sbbw", OnSbb},           //
    {"seta", OnSetnbe},        //
    {"setae", OnSetnb},        //
    {"setb", OnSetb},          //
    {"setbe", OnSetbe},        //
    {"setc", OnSetb},          //
    {"sete", OnSetz},          //
    {"setg", OnSetnle},        //
    {"setge", OnSetnl},        //
    {"setl", OnSetl},          //
    {"setle", OnSetle},        //
    {"setna", OnSetbe},        //
    {"setnae", OnSetb},        //
    {"setnb", OnSetnb},        //
    {"setnbe", OnSetnbe},      //
    {"setnc", OnSetnb},        //
    {"setne", OnSetnz},        //
    {"setng", OnSetle},        //
    {"setnge", OnSetl},        //
    {"setnl", OnSetnl},        //
    {"setnle", OnSetnle},      //
    {"setno", OnSetno},        //
    {"setnp", OnSetnp},        //
    {"setns", OnSetns},        //
    {"setnz", OnSetnz},        //
    {"seto", OnSeto},          //
    {"setp", OnSetp},          //
    {"setpe", OnSetp},         //
    {"setpo", OnSetnp},        //
    {"sets", OnSets},          //
    {"setz", OnSetz},          //
    {"sfence", OnSfence},      //
    {"shl", OnShl},            //
    {"shlb", OnShl},           //
    {"shld", OnShld},          //
    {"shll", OnShl},           //
    {"shlq", OnShl},           //
    {"shlw", OnShl},           //
    {"shr", OnShr},            //
    {"shrb", OnShr},           //
    {"shrd", OnShrd},          //
    {"shrl", OnShr},           //
    {"shrq", OnShr},           //
    {"shrw", OnShr},           //
    {"shufpd", OnShufpd},      //
    {"shufps", OnShufps},      //
    {"sqrtpd", OnSqrtpd},      //
    {"sqrtps", OnSqrtps},      //
    {"sqrtsd", OnSqrtsd},      //
    {"sqrtss", OnSqrtss},      //
    {"stc", OnStc},            //
    {"std", OnStd},            //
    {"sti", OnSti},            //
    {"stosb", OnStosb},        //
    {"stosl", OnStosl},        //
    {"stosq", OnStosq},        //
    {"stosw", OnStosw},        //
    {"sub", OnSub},            //
    {"subb", OnSub},           //
    {"subl", OnSub},           //
    {"subpd", OnSubpd},        //
    {"subps", OnSubps},        //
    {"subq", OnSub},           //
    {"subsd", OnSubsd},        //
    {"subss", OnSubss},        //
    {"subw", OnSub},           //
    {"syscall", OnSyscall},    //
    {"test", OnTest},          //
    {"testb", OnTest},         //
    {"testl", OnTest},         //
    {"testq", OnTest},         //
    {"testw", OnTest},         //
    {"ucomisd", OnUcomisd},    //
    {"ucomiss", OnUcomiss},    //
    {"ud2", OnUd2},            //
    {"unpckhpd", OnUnpckhpd},  //
    {"unpcklpd", OnUnpcklpd},  //
    {"wait", OnFwait},         //
    {"xadd", OnXadd},          //
    {"xchg", OnXchg},          //
    {"xor", OnXor},            //
    {"xorb", OnXor},           //
    {"xorl", OnXor},           //
    {"xorpd", OnXorpd},        //
    {"xorps", OnXorps},        //
    {"xorq", OnXor},           //
    {"xorw", OnXor},           //
};

static const struct Directive16 {
  char s[16];
  void (*f)(struct As *, struct Slice);
} kDirective16[] = {
    {".internal", OnInternal},        //
    {".popsection", OnPopsection},    //
    {".previous", OnPrevious},        //
    {".protected", OnProtected},      //
    {".pushsection", OnPushsection},  //
    {"cvtsd2ssl", OnCvtsd2ss},        //
    {"cvtsd2ssq", OnCvtsd2ss},        //
    {"cvtsi2sdl", OnCvtsi2sd},        //
    {"cvtsi2sdq", OnCvtsi2sd},        //
    {"cvtsi2sdq", OnCvtsi2sd},        //
    {"cvtsi2ssl", OnCvtsi2ss},        //
    {"cvtsi2ssq", OnCvtsi2ss},        //
    {"cvttpd2dq", OnCvttpd2dq},       //
    {"cvttps2dq", OnCvttps2dq},       //
    {"cvttsd2si", OnCvttsd2si},       //
    {"cvttsd2sil", OnCvttsd2si},      //
    {"cvttsd2siq", OnCvttsd2si},      //
    {"cvttss2si", OnCvttss2si},       //
    {"cvttss2sil", OnCvttss2si},      //
    {"cvttss2siq", OnCvttss2si},      //
    {"movntdq", OnMovntdq},           //
    {"pcmpistri", OnPcmpistri},       //
    {"pcmpistrm", OnPcmpistrm},       //
    {"phminposuw", OnPhminposuw},     //
    {"pmaddubsw", OnPmaddubsw},       //
    {"punpckhbw", OnPunpckhbw},       //
    {"punpckhdq", OnPunpckhdq},       //
    {"punpckhqdq", OnPunpckhqdq},     //
    {"punpckhwd", OnPunpckhwd},       //
    {"punpcklbw", OnPunpcklbw},       //
    {"punpckldq", OnPunpckldq},       //
    {"punpcklqdq", OnPunpcklqdq},     //
    {"punpcklwd", OnPunpcklwd},       //
};

static bool OnDirective8(struct As *a, struct Slice s) {
  int m, l, r;
  uint64_t x, y;
  if (s.n && s.n <= 8) {
    x = MakeKey64(s.p, s.n);
    l = 0;
    r = ARRAYLEN(kDirective8) - 1;
    while (l <= r) {
      m = (l & r) + ((l ^ r) >> 1);  // floor((a+b)/2)
      y = READ64BE(kDirective8[m].s);
      if (x < y) {
        r = m - 1;
      } else if (x > y) {
        l = m + 1;
      } else {
        kDirective8[m].f(a, s);
        return true;
      }
    }
  }
  return false;
}

static bool OnDirective16(struct As *a, struct Slice s) {
  int m, l, r;
  unsigned __int128 x, y;
  if (s.n && s.n <= 16) {
    x = MakeKey128(s.p, s.n);
    l = 0;
    r = ARRAYLEN(kDirective16) - 1;
    while (l <= r) {
      m = (l & r) + ((l ^ r) >> 1);  // floor((a+b)/2)
      y = READ128BE(kDirective16[m].s);
      if (x < y) {
        r = m - 1;
      } else if (x > y) {
        l = m + 1;
      } else {
        kDirective16[m].f(a, s);
        return true;
      }
    }
  }
  return false;
}

static void OnDirective(struct As *a) {
  struct Slice s;
  for (;;) {
    s = GetSlice(a);
    if (Prefix(a, s.p, s.n)) {
      if (IsSemicolon(a)) {
        return;
      }
    } else {
      break;
    }
  }
  if (!OnDirective8(a, s)) {
    if (!OnDirective16(a, s)) {
      Fail(a, "unexpected op: %.*s", s.n, s.p);
    }
  }
  ConsumePunct(a, ';');
}

static void Assemble(struct As *a) {
  while (a->i < a->things.n) {
    if (IsSemicolon(a)) {
      ++a->i;
      continue;
    }
    switch (a->things.p[a->i].t) {
      case TT_SLICE:
        if (IsPunct(a, a->i + 1, ':')) {
          OnSymbol(a, a->things.p[a->i].i);
        } else {
          OnDirective(a);
        }
        break;
      case TT_INT:
        if (IsPunct(a, a->i + 1, ':')) {
          OnLocalLabel(a, a->ints.p[a->things.p[a->i].i]);
          break;
        }
        Fail(a, "unexpected token");
      default:
        Fail(a, "unexpected token");
    }
  }
}

static int FindLabelForward(struct As *a, int id) {
  int i;
  for (i = 0; i < a->labels.n; ++i) {
    if (a->labels.p[i].id == id && a->labels.p[i].tok > a->i) {
      return a->labels.p[i].symbol;
    }
  }
  Fail(a, "label not found");
}

static int FindLabelBackward(struct As *a, int id) {
  int i;
  for (i = a->labels.n; i--;) {
    if (a->labels.p[i].id == id && a->labels.p[i].tok < a->i) {
      return a->labels.p[i].symbol;
    }
  }
  Fail(a, "label not found");
}

static int ResolveSymbol(struct As *a, int i) {
  switch (a->things.p[i].t) {
    case TT_SLICE:
      return GetSymbol(a, a->things.p[i].i);
    case TT_BACKWARD:
      return FindLabelBackward(a, a->ints.p[a->things.p[i].i]);
    case TT_FORWARD:
      return FindLabelForward(a, a->ints.p[a->things.p[i].i]);
    default:
      Fail(a, "this corruption %d", a->things.p[i].t);
  }
}

static void EvaluateExpr(struct As *a, int i) {
  if (i == -1) return;
  if (a->exprs.p[i].isevaluated) return;
  if (a->exprs.p[i].isvisited) Fail(a, "circular expr");
  a->exprs.p[i].isvisited = true;
  EvaluateExpr(a, a->exprs.p[i].lhs);
  EvaluateExpr(a, a->exprs.p[i].rhs);
  a->i = a->exprs.p[i].tok;
  switch (a->exprs.p[i].kind) {
    case EX_INT:
      break;
    case EX_SYM:
      a->exprs.p[i].x = ResolveSymbol(a, a->exprs.p[i].x);
      break;
    default:
      break;
  }
  a->exprs.p[i].isevaluated = true;
}

static void Write32(char b[4], int x) {
  b[0] = x >> 000;
  b[1] = x >> 010;
  b[2] = x >> 020;
  b[3] = x >> 030;
}

static void MarkUsedSymbols(struct As *a, int i) {
  if (i == -1) return;
  MarkUsedSymbols(a, a->exprs.p[i].lhs);
  MarkUsedSymbols(a, a->exprs.p[i].rhs);
  if (a->exprs.p[i].kind == EX_SYM) {
    a->symbols.p[a->exprs.p[i].x].isused = true;
  }
}

static void Evaluate(struct As *a) {
  int i;
  struct Expr *e;
  for (i = 0; i < a->relas.n; ++i) {
    EvaluateExpr(a, a->relas.p[i].expr);
    if (a->relas.p[i].kind == R_X86_64_PC32) {
      e = a->exprs.p + a->relas.p[i].expr;
      if (e->kind == EX_SYM && a->symbols.p[e->x].stb == STB_LOCAL &&
          a->symbols.p[e->x].section == a->relas.p[i].section) {
        a->relas.p[i].isdead = true;
        Write32((a->sections.p[a->relas.p[i].section].binary.p +
                 a->relas.p[i].offset),
                (a->symbols.p[e->x].offset - a->relas.p[i].offset +
                 a->relas.p[i].addend));
      }
    }
  }
  for (i = 0; i < a->relas.n; ++i) {
    if (a->relas.p[i].isdead) continue;
    MarkUsedSymbols(a, a->relas.p[i].expr);
  }
}

static void MarkUndefinedSymbolsGlobal(struct As *a) {
  int i;
  for (i = 0; i < a->symbols.n; ++i) {
    if (a->symbols.p[i].isused && !a->symbols.p[i].section &&
        a->symbols.p[i].stb == STB_LOCAL) {
      a->symbols.p[i].stb = STB_GLOBAL;
    }
  }
}

static bool IsLocal(struct As *a, int name) {
  if (name < 0) return true;
  return a->slices.p[name].n >= 2 && !memcmp(a->slices.p[name].p, ".L", 2);
}

static bool IsLiveSymbol(struct As *a, int i) {
  return !(!a->symbols.p[i].isused && a->symbols.p[i].stb == STB_LOCAL &&
           a->symbols.p[i].section && IsLocal(a, a->symbols.p[i].name));
}

static void Objectify(struct As *a, int path) {
  char *p;
  int i, j, s, e;
  struct ElfWriter *elf;
  elf = elfwriter_open(a->strings.p[path], 0644, EM_NEXGEN32E);
  for (i = 0; i < a->symbols.n; ++i) {
    if (!IsLiveSymbol(a, i)) continue;
    p = strndup(a->slices.p[a->symbols.p[i].name].p,
                a->slices.p[a->symbols.p[i].name].n);
    a->symbols.p[i].ref = elfwriter_appendsym(
        elf, p, ELF64_ST_INFO(a->symbols.p[i].stb, a->symbols.p[i].type),
        a->symbols.p[i].stv, a->symbols.p[i].offset, a->symbols.p[i].size);
    if (a->symbols.p[i].section >= SHN_LORESERVE) {
      elfwriter_setsection(elf, a->symbols.p[i].ref, a->symbols.p[i].section);
    }
    free(p);
  }
  for (i = 0; i < a->sections.n; ++i) {
    elfwriter_align(elf, a->sections.p[i].align, 0);
    s = elfwriter_startsection(elf, a->strings.p[a->sections.p[i].name],
                               a->sections.p[i].type, a->sections.p[i].flags);
    for (j = 0; j < a->symbols.n; ++j) {
      if (!IsLiveSymbol(a, j)) continue;
      if (a->symbols.p[j].section != i) continue;
      elfwriter_setsection(elf, a->symbols.p[j].ref, s);
    }
    for (j = 0; j < a->relas.n; ++j) {
      if (a->relas.p[j].isdead) continue;
      if (a->relas.p[j].section != i) continue;
      e = a->relas.p[j].expr;
      a->i = a->exprs.p[e].tok;
      switch (a->exprs.p[e].kind) {
        case EX_INT:
          break;
        case EX_SYM:
          elfwriter_appendrela(elf, a->relas.p[j].offset,
                               a->symbols.p[a->exprs.p[e].x].ref,
                               a->relas.p[j].kind, a->relas.p[j].addend);
          break;
        case EX_ADD:
          if (a->exprs.p[a->exprs.p[e].lhs].kind == EX_SYM &&
              a->exprs.p[a->exprs.p[e].rhs].kind == EX_INT) {
            elfwriter_appendrela(
                elf, a->relas.p[j].offset,
                a->symbols.p[a->exprs.p[a->exprs.p[e].lhs].x].ref,
                a->relas.p[j].kind,
                a->relas.p[j].addend + a->exprs.p[a->exprs.p[e].rhs].x);
          } else {
            Fail(a, "bad addend");
          }
          break;
        default:
          Fail(a, "unsupported relocation type");
      }
    }
    if (a->sections.p[i].binary.n) {
      memcpy(elfwriter_reserve(elf, a->sections.p[i].binary.n),
             a->sections.p[i].binary.p, a->sections.p[i].binary.n);
    }
    elfwriter_commit(elf, a->sections.p[i].binary.n);
    elfwriter_finishsection(elf);
  }
  elfwriter_close(elf);
}

static void CheckIntegrity(struct As *a) {
  int i;
  for (i = 0; i < a->things.n; ++i) {
    CHECK_LT((int)a->things.p[i].s, a->sauces.n);
    switch (a->things.p[i].t) {
      case TT_INT:
      case TT_FORWARD:
      case TT_BACKWARD:
        CHECK_LT(a->things.p[i].i, a->ints.n);
        break;
      case TT_FLOAT:
        CHECK_LT(a->things.p[i].i, a->floats.n);
        break;
      case TT_SLICE:
        CHECK_LT(a->things.p[i].i, a->slices.n);
        break;
      default:
        break;
    }
  }
  for (i = 0; i < a->sections.n; ++i) {
    CHECK_LT(a->sections.p[i].name, a->strings.n);
  }
  for (i = 0; i < a->symbols.n; ++i) {
    CHECK_LT(a->symbols.p[i].name, a->slices.n);
    CHECK_LT(a->symbols.p[i].section, a->sections.n);
  }
  for (i = 0; i < a->labels.n; ++i) {
    CHECK_LT(a->labels.p[i].tok, a->things.n);
    CHECK_LT(a->labels.p[i].symbol, a->symbols.n);
  }
  for (i = 0; i < a->relas.n; ++i) {
    CHECK_LT(a->relas.p[i].expr, a->exprs.n);
    CHECK_LT(a->relas.p[i].section, a->sections.n);
  }
  for (i = 0; i < a->exprs.n; ++i) {
    CHECK_LT(a->exprs.p[i].tok, a->things.n);
    if (a->exprs.p[i].lhs != -1) CHECK_LT(a->exprs.p[i].lhs, a->exprs.n);
    if (a->exprs.p[i].rhs != -1) CHECK_LT(a->exprs.p[i].rhs, a->exprs.n);
    switch (a->exprs.p[i].kind) {
      case EX_SYM:
        CHECK_LT(a->exprs.p[i].x, a->things.n);
        CHECK(a->things.p[a->exprs.p[i].x].t == TT_SLICE ||
              a->things.p[a->exprs.p[i].x].t == TT_FORWARD ||
              a->things.p[a->exprs.p[i].x].t == TT_BACKWARD);
        break;
      default:
        break;
    }
  }
}

static void PrintThings(struct As *a) {
  int i;
  char pbuf[4], fbuf[32];
  for (i = 0; i < a->things.n; ++i) {
    printf("%s:%d:: ", a->strings.p[a->sauces.p[a->things.p[i].s].path],
           a->sauces.p[a->things.p[i].s].line);
    switch (a->things.p[i].t) {
      case TT_INT:
        printf("TT_INT %jd\n", a->ints.p[a->things.p[i].i]);
        break;
      case TT_FLOAT:
        g_xfmt_p(fbuf, &a->floats.p[a->things.p[i].i], 19, sizeof(fbuf), 0);
        printf("TT_FLOAT %s\n", fbuf);
        break;
      case TT_SLICE:
        printf("TT_SLICE %.*s\n", a->slices.p[a->things.p[i].i].n,
               a->slices.p[a->things.p[i].i].p);
        break;
      case TT_PUNCT:
        printf("TT_PUNCT %s\n", PunctToStr(a->things.p[i].i, pbuf));
        break;
      case TT_BACKWARD:
        printf("TT_BACKWARD %jd\n", a->ints.p[a->things.p[i].i]);
        break;
      case TT_FORWARD:
        printf("TT_FORWARD %jd\n", a->ints.p[a->things.p[i].i]);
        break;
      default:
        abort();
    }
  }
}

void Assembler(int argc, char *argv[]) {
  struct As *a;
  a = NewAssembler();
  ReadFlags(a, argc, argv);
  SaveString(&a->incpaths, strdup("."));
  SaveString(&a->incpaths, xdirname(a->strings.p[a->inpath]));
  Tokenize(a, a->inpath);
  /* PrintThings(a); */
  Assemble(a);
  /* CheckIntegrity(a); */
  Evaluate(a);
  MarkUndefinedSymbolsGlobal(a);
  Objectify(a, a->outpath);
  /* malloc_stats(); */
  FreeAssembler(a);
}
