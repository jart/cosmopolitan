/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/bits/bits.h"
#include "libc/bits/popcnt.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/elf/def.h"
#include "libc/fmt/conv.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/bsr.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/x/x.h"
#include "third_party/gdtoa/gdtoa.h"
#include "tool/build/lib/elfwriter.h"

/* WORK IN PROGRESS */

#define OSZ      0x66
#define ASZ      0x67
#define REX      0x40  // byte
#define REXB     0x41  // src
#define REXX     0x42  // index
#define REXR     0x44  // dest
#define REXW     0x48  // quad
#define HASASZ   0x00010000
#define HASBASE  0x00020000
#define HASINDEX 0x00040000
#define ISRIP    0x00080000
#define ISREG    0x00100000

#define APPEND(L)   L.p = realloc(L.p, ++L.n * sizeof(*L.p))
#define IS(P, N, S) (N == strlen(S) && !strncasecmp(P, S, strlen(S)))

struct Strings {
  size_t n;
  char **p;
};

struct Assembler {
  int i;
  int section;
  int previous;
  struct Ints {
    size_t n;
    long *p;
  } ints;
  struct Floats {
    size_t n;
    long double *p;
  } floats;
  struct Slices {
    size_t n;
    struct Slice {
      size_t n;
      char *p;
    } * p;
  } slices;
  struct Sauces {
    size_t n;
    struct Sauce {
      const char *path;
      int line;
    } * p;
  } sauces;
  struct Things {
    size_t n;
    struct Thing {
      enum ThingType {
        TT_INT,
        TT_FLOAT,
        TT_SLICE,
        TT_PUNCT,
        TT_FORWARD,
        TT_BACKWARD,
      } t;
      int i;
      int s;
    } * p;
  } things;
  struct Sections {
    size_t n;
    struct Section {
      const char *name;
      int flags;
      int type;
      int align;
      struct Slice binary;
    } * p;
  } sections;
  struct Symbols {
    size_t n;
    struct Symbol {
      int name;
      int section;
      int stb;
      int stv;
      int type;
      long location;
      long size;
    } * p;
  } symbols;
  struct Labels {
    size_t n;
    struct Label {
      int s;
      int id;
      int section;
      long location;
    } * p;
  } labels;
};

static const char kPrefixByte[30] = {
    0xf3, 0xf3, 0xf3, 0xf2, 0xf2, 0xf0, 0x26, 0x2e, 0x36, 0x3e,
    0x64, 0x65, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
    0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x66, 0x67,
};

static const char kPrefix[30][8] = {
    "rep",    "repz",    "repe",    "repne",    "repnz",  "lock",
    "es",     "cs",      "ss",      "ds",       "fs",     "gs",
    "rex",    "rex.b",   "rex.x",   "rex.xb",   "rex.r",  "rex.rb",
    "rex.rx", "rex.rxb", "rex.w",   "rex.wb",   "rex.wx", "rex.wxb",
    "rex.wr", "rex.wrb", "rex.wrx", "rex.wrxb", "data16", "addr32",
};

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
  {"esi",   6 | 2<<3,                     6 | 2<<3,                     6 | 2<<3,            6 | 2<<3           },
  {"esp",   4 | 2<<3,                     4 | 2<<3,                     4 | 2<<3,            4 | 2<<3           },
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
  {"rsi",   6 | 3<<3 | REXW<<8,           6 | 3<<3 | REXW<<8,           6 | 3<<3,            6 | 3<<3           },
  {"rsp",   4 | 3<<3 | REXW<<8,           4 | 3<<3 | REXW<<8,           4 | 3<<3,            4 | 3<<3           },
  {"si",    6 | 1<<3,                     6 | 1<<3,                     6 | 1<<3,            6 | 1<<3           },
  {"sil",   6 | REX<<8,                   6 | REX<<8,                   6 | REX<<8,          6 | REX<<8         },
  {"sp",    4 | 1<<3,                     4 | 1<<3,                     4 | 1<<3,            4 | 1<<3           },
  {"spl",   4 | REX<<8,                   4 | REX<<8,                   4 | REX<<8,          4 | REX<<8         },
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

static bool g_ignore_err;
static const char *g_input_path;
static const char *g_output_path;
static struct Strings g_include_paths;

static void PrintSlice(struct Slice s) {
  fprintf(stderr, "%.*s\n", s.n, s.p);
}

static void AppendString(struct Strings *l, const char *p) {
  l->p = realloc(l->p, ++l->n * sizeof(*l->p));
  l->p[l->n - 1] = p;
}

static void ReadFlags(int argc, char *argv[]) {
  int i;
  g_input_path = "-";
  g_output_path = "a.out";
  for (i = 1; i < argc; ++i) {
    if (!strcmp(argv[i], "-o")) {
      g_output_path = argv[++i];
    } else if (startswith(argv[i], "-o")) {
      g_output_path = argv[i] + 2;
    } else if (!strcmp(argv[i], "-I")) {
      AppendString(&g_include_paths, argv[++i]);
    } else if (startswith(argv[i], "-I")) {
      AppendString(&g_include_paths, argv[i] + 2);
    } else if (!strcmp(argv[i], "-Z")) {
      g_ignore_err = true;
    } else if (argv[i][0] != '-') {
      g_input_path = argv[i];
    }
  }
}

static int AppendSection(struct Assembler *a, const char *name, int flags,
                         int type) {
  int i;
  APPEND(a->sections);
  i = a->sections.n - 1;
  a->sections.p[i].name = name;
  a->sections.p[i].flags = flags;
  a->sections.p[i].type = type;
  a->sections.p[i].align = 1;
  a->sections.p[i].binary.p = NULL;
  a->sections.p[i].binary.n = 0;
  return i;
}

static struct Assembler *NewAssembler(void) {
  struct Assembler *a = calloc(1, sizeof(struct Assembler));
  APPEND(a->slices);
  a->slices.p[0].p = NULL;
  a->slices.p[0].n = 0;
  AppendSection(a, "", 0, SHT_NULL);
  AppendSection(a, ".text", SHF_ALLOC | SHF_EXECINSTR, SHT_PROGBITS);
  AppendSection(a, ".data", SHF_ALLOC | SHF_WRITE, SHT_PROGBITS);
  AppendSection(a, ".bss", SHF_ALLOC | SHF_WRITE, SHT_NOBITS);
  a->section = 1;
  return a;
}

static int AppendLine(struct Assembler *a, char *path, int line) {
  if (!a->sauces.n || (line != a->sauces.p[a->sauces.n - 1].line ||
                       path != a->sauces.p[a->sauces.n - 1].path)) {
    APPEND(a->sauces);
    a->sauces.p[a->sauces.n - 1].path = path;
    a->sauces.p[a->sauces.n - 1].line = line;
  }
  return a->sauces.n - 1;
}

static int ReadCharLiteral(struct Slice *buf, int c, char *p, int *i) {
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
      if (isxdigit(p[*i])) {
        c = hextoint(p[(*i)++]);
        if (isxdigit(p[*i])) {
          c = c * 16 + hextoint(p[(*i)++]);
        }
      }
      return c;
    case '0' ... '7':
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

static void CanonicalizeNewline(char *p) {
  int i = 0, j = 0;
  while (p[i]) {
    if (p[i] == '\r' && p[i + 1] == '\n') {
      i += 2;
      p[j++] = '\n';
    } else if (p[i] == '\r') {
      i++;
      p[j++] = '\n';
    } else {
      p[j++] = p[i++];
    }
  }
  p[j] = '\0';
}

static void RemoveBackslashNewline(char *p) {
  int i, j, n;
  for (i = j = n = 0; p[i];) {
    if (p[i] == '\\' && p[i + 1] == '\n') {
      i += 2;
      n++;
    } else if (p[i] == '\n') {
      p[j++] = p[i++];
      for (; n > 0; n--) p[j++] = '\n';
    } else {
      p[j++] = p[i++];
    }
  }
  for (; n > 0; n--) p[j++] = '\n';
  p[j] = '\0';
}

static char *ReadFile(const char *path) {
  char *p;
  FILE *fp;
  int buflen, nread, end, n;
  if (strcmp(path, "-") == 0) {
    fp = stdin;
  } else {
    fp = fopen(path, "r");
    if (!fp) return NULL;
  }
  buflen = 4096;
  nread = 0;
  p = calloc(1, buflen);
  for (;;) {
    end = buflen - 2;
    n = fread(p + nread, 1, end - nread, fp);
    if (n == 0) break;
    nread += n;
    if (nread == end) {
      buflen *= 2;
      p = realloc(p, buflen);
    }
  }
  if (fp != stdin) fclose(fp);
  if (nread > 0 && p[nread - 1] == '\\') {
    p[nread - 1] = '\n';
  } else if (nread == 0 || p[nread - 1] != '\n') {
    p[nread++] = '\n';
  }
  p[nread] = '\0';
  return p;
}

static void Tokenize(struct Assembler *a, char *path) {
  bool bol;
  int c, i, line;
  char *p, *path2;
  struct Slice buf;
  if (!(p = ReadFile(path))) return;
  if (!memcmp(p, "\357\273\277", 3)) p += 3;
  CanonicalizeNewline(p);
  RemoveBackslashNewline(p);
  line = 1;
  bol = true;
  while ((c = *p)) {
    if (c == '#' || (c == '/' && bol) || (c == '/' && p[1] == '/')) {
      p = strchr(p, '\n');
      continue;
    }
    if (c == '\n') {
      APPEND(a->things);
      a->things.p[a->things.n - 1].t = TT_PUNCT;
      a->things.p[a->things.n - 1].s = AppendLine(a, path, line);
      a->things.p[a->things.n - 1].i = ';';
      ++p;
      bol = true;
      ++line;
      continue;
    }
    bol = false;
    if (c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\f' ||
        c == '\v' || c == ',') {
      ++p;
      continue;
    }
    if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_' ||
        c == '%' || c == '@' || (c == '.' && !('0' <= p[1] && p[1] <= '9'))) {
      for (i = 1;; ++i) {
        if (!(('a' <= p[i] && p[i] <= 'z') || ('A' <= p[i] && p[i] <= 'Z') ||
              ('0' <= p[i] && p[i] <= '9') || p[i] == '.' || p[i] == '_' ||
              p[i] == '$')) {
          break;
        }
      }
      APPEND(a->things);
      a->things.p[a->things.n - 1].t = TT_SLICE;
      a->things.p[a->things.n - 1].s = AppendLine(a, path, line);
      a->things.p[a->things.n - 1].i = a->slices.n;
      APPEND(a->slices);
      a->slices.p[a->slices.n - 1].p = p;
      a->slices.p[a->slices.n - 1].n = i;
      p += i;
      continue;
    }
    if (('0' <= c && c <= '9') || (c == '.' && '0' <= p[1] && p[1] <= '9')) {
      bool isfloat = c == '.';
      if (c == '0' && p[1] != '.') {
        if (p[1] == 'x' || p[1] == 'X') {
          for (i = 2;; ++i) {
            if (!(('0' <= p[i] && p[i] <= '9') ||
                  ('a' <= p[i] && p[i] <= 'f') ||
                  ('A' <= p[i] && p[i] <= 'F'))) {
              break;
            }
          }
        } else if (p[1] == 'b' || p[1] == 'B') {
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
      APPEND(a->things);
      if (isfloat) {
        APPEND(a->floats);
        a->floats.p[a->floats.n - 1] = strtold(p, NULL);
        a->things.p[a->things.n - 1].i = a->floats.n - 1;
        a->things.p[a->things.n - 1].t = TT_FLOAT;
      } else {
        APPEND(a->ints);
        a->ints.p[a->ints.n - 1] = strtol(p, NULL, 0);
        a->things.p[a->things.n - 1].i = a->ints.n - 1;
        if (p[i] == 'f' || p[i] == 'F') {
          a->things.p[a->things.n - 1].t = TT_FORWARD;
        } else if (p[i] == 'b' || p[i] == 'B') {
          a->things.p[a->things.n - 1].t = TT_BACKWARD;
        } else {
          a->things.p[a->things.n - 1].t = TT_INT;
        }
      }
      a->things.p[a->things.n - 1].s = AppendLine(a, path, line);
      p += i;
      continue;
    }
    if (c == '\'') {
      i = 1;
      c = p[i++];
      c = ReadCharLiteral(&buf, c, p, &i);
      if (p[i] == '\'') ++i;
      p += i;
      APPEND(a->things);
      a->things.p[a->things.n - 1].t = TT_INT;
      a->things.p[a->things.n - 1].s = AppendLine(a, path, line);
      a->things.p[a->things.n - 1].i = a->ints.n;
      APPEND(a->ints);
      a->ints.p[a->ints.n - 1] = c;
      continue;
    }
    if (c == '"') {
      buf.n = 0;
      buf.p = NULL;
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
        if (fileexists(buf.p)) {
          Tokenize(a, buf.p);
        } else {
          for (i = 0; i < g_include_paths.n; ++i) {
            path2 = xstrcat(g_include_paths.p[i], '/', buf.p);
            if (fileexists(path2)) {
              Tokenize(a, path2);
              free(path2);
              break;
            } else {
              free(path2);
            }
          }
        }
        free(buf.p);
      } else {
        APPEND(a->things);
        a->things.p[a->things.n - 1].t = TT_SLICE;
        a->things.p[a->things.n - 1].s = AppendLine(a, path, line);
        a->things.p[a->things.n - 1].i = a->slices.n;
        APPEND(a->slices);
        a->slices.p[a->slices.n - 1] = buf;
      }
      continue;
    }

    APPEND(a->things);
    a->things.p[a->things.n - 1].t = TT_PUNCT;
    a->things.p[a->things.n - 1].s = AppendLine(a, path, line);
    a->things.p[a->things.n - 1].i = c;
    ++p;
  }
}

static bool IsLocal(struct Assembler *a, int name) {
  if (name < 0) return true;
  return a->slices.p[name].n >= 2 && !memcmp(a->slices.p[name].p, ".L", 2);
}

static int GetSymbol(struct Assembler *a, int name) {
  int i;
  for (i = 0; i < a->symbols.n; ++i) {
    if (a->slices.p[a->symbols.p[i].name].n == a->slices.p[name].n &&
        !memcmp(a->slices.p[a->symbols.p[i].name].p, a->slices.p[name].p,
                a->slices.p[name].n)) {
      return i;
    }
  }
  APPEND(a->symbols);
  i = a->symbols.n - 1;
  memset(&a->symbols.p[i], 0, sizeof(a->symbols.p[i]));
  a->symbols.p[i].name = name;
  return i;
}

static wontreturn void Fail(struct Assembler *a, const char *fmt, ...) {
  va_list va;
  fprintf(stderr, "%s:%d:: ", a->sauces.p[a->things.p[a->i].s].path,
          a->sauces.p[a->things.p[a->i].s].line);
  va_start(va, fmt);
  vfprintf(stderr, fmt, va);
  va_end(va);
  fputc('\n', stderr);
  __die();
}

static void Label(struct Assembler *a, int name) {
  int i = GetSymbol(a, name);
  if (a->symbols.p[i].section) {
    Fail(a, "already defined: %.*s", a->slices.p[name].n, a->slices.p[name].p);
  }
  a->symbols.p[i].section = a->section;
  a->symbols.p[i].location = a->sections.p[a->section].binary.n;
  a->i += 2;
}

static void LocalLabel(struct Assembler *a, int id) {
  APPEND(a->labels);
  a->labels.p[a->labels.n - 1].s = a->things.p[a->i].s;
  a->labels.p[a->labels.n - 1].id = id;
  a->labels.p[a->labels.n - 1].section = a->section;
  a->labels.p[a->labels.n - 1].location = a->sections.p[a->section].binary.n;
  a->i += 2;
}

static void SetSection(struct Assembler *a, int section) {
  a->previous = a->section;
  a->section = section;
}

static bool IsInt(struct Assembler *a, int i) {
  return a->things.p[i].t == TT_INT;
}

static bool IsFloat(struct Assembler *a, int i) {
  return a->things.p[i].t == TT_FLOAT;
}

static bool IsSlice(struct Assembler *a, int i) {
  return a->things.p[i].t == TT_SLICE;
}

static bool IsPunct(struct Assembler *a, int i, int c) {
  return a->things.p[i].t == TT_PUNCT && a->things.p[i].i == c;
}

static void ConsumePunct(struct Assembler *a, int c) {
  if (IsPunct(a, a->i, c)) {
    ++a->i;
  } else {
    Fail(a, "expected %`'c", c);
  }
}

static long GetInt(struct Assembler *a) {
  long res;
  if (IsInt(a, a->i)) {
    res = a->ints.p[a->things.p[a->i].i];
    ++a->i;
    return res;
  } else {
    Fail(a, "expected int");
  }
}

static long double GetFloat(struct Assembler *a) {
  long double res;
  if (IsFloat(a, a->i)) {
    res = a->floats.p[a->things.p[a->i].i];
    ++a->i;
    return res;
  } else {
    Fail(a, "expected float");
  }
}

static struct Slice GetString(struct Assembler *a) {
  struct Slice res;
  if (IsSlice(a, a->i)) {
    res = a->slices.p[a->things.p[a->i].i];
    ++a->i;
    return res;
  } else {
    Fail(a, "expected string");
  }
}

static void Emit(struct Assembler *a, int c) {
  APPEND(a->sections.p[a->section].binary);
  a->sections.p[a->section].binary.p[a->sections.p[a->section].binary.n - 1] =
      c;
}

static void EmitWord(struct Assembler *a, long x) {
  Emit(a, x >> 000);
  Emit(a, x >> 010);
}

static void EmitLong(struct Assembler *a, long x) {
  Emit(a, x >> 000);
  Emit(a, x >> 010);
  Emit(a, x >> 020);
  Emit(a, x >> 030);
}

static void EmitQuad(struct Assembler *a, long x) {
  EmitLong(a, x >> 000);
  EmitLong(a, x >> 040);
}

static void OnZero(struct Assembler *a) {
  long i, n;
  while (IsInt(a, a->i)) {
    n = GetInt(a);
    for (i = 0; i < n; ++i) {
      Emit(a, 0);
    }
  }
}

static void OnFill(struct Assembler *a) {
  int x;
  long i, n;
  n = GetInt(a);
  if (IsInt(a, a->i)) {
    x = GetInt(a);
  } else {
    x = 0;
  }
  for (i = 0; i < n; ++i) {
    Emit(a, x);
  }
}

static void OnByte(struct Assembler *a) {
  while (IsInt(a, a->i)) {
    Emit(a, GetInt(a));
  }
}

static void OnWord(struct Assembler *a) {
  long x;
  while (IsInt(a, a->i)) {
    x = GetInt(a);
    Emit(a, x >> 000);
    Emit(a, x >> 010);
  }
}

static void OnLong(struct Assembler *a) {
  while (IsInt(a, a->i)) {
    EmitLong(a, GetInt(a));
  }
}

static void OnQuad(struct Assembler *a) {
  while (IsInt(a, a->i)) {
    EmitQuad(a, GetInt(a));
  }
}

static void OnFloat(struct Assembler *a) {
  float f;
  char b[4];
  while (IsFloat(a, a->i)) {
    f = GetFloat(a);
    memcpy(b, &f, 4);
    Emit(a, b[0]);
    Emit(a, b[1]);
    Emit(a, b[2]);
    Emit(a, b[3]);
  }
}

static void OnDouble(struct Assembler *a) {
  double f;
  char b[8];
  while (IsFloat(a, a->i)) {
    f = GetFloat(a);
    memcpy(b, &f, 8);
    Emit(a, b[0]);
    Emit(a, b[1]);
    Emit(a, b[2]);
    Emit(a, b[3]);
    Emit(a, b[4]);
    Emit(a, b[5]);
    Emit(a, b[6]);
    Emit(a, b[7]);
  }
}

static void OnAscii(struct Assembler *a) {
  size_t i;
  struct Slice arg;
  while (IsSlice(a, a->i)) {
    arg = GetString(a);
    for (i = 0; i < arg.n; ++i) {
      Emit(a, arg.p[i]);
    }
  }
}

static void OnAsciz(struct Assembler *a) {
  size_t i;
  struct Slice arg;
  while (IsSlice(a, a->i)) {
    arg = GetString(a);
    for (i = 0; i < arg.n; ++i) {
      Emit(a, arg.p[i]);
    }
    Emit(a, 0);
  }
}

static void OnAbort(struct Assembler *a) {
  Fail(a, "aborted");
}

static void OnErr(struct Assembler *a) {
  if (g_ignore_err) return;
  Fail(a, "error");
}

static void OnError(struct Assembler *a) {
  struct Slice msg = GetString(a);
  if (g_ignore_err) return;
  Fail(a, "%.*s", msg.n, msg.p);
}

static void OnText(struct Assembler *a) {
  SetSection(a, 0);
}

static void OnData(struct Assembler *a) {
  SetSection(a, 1);
}

static void OnBss(struct Assembler *a) {
  SetSection(a, 2);
}

static void OnPrevious(struct Assembler *a) {
  SetSection(a, a->previous);
}

static void OnAlign(struct Assembler *a) {
  long i, n, align, fill, maxskip;
  align = GetInt(a);
  if (popcnt(align) != 1) Fail(a, "alignment not power of 2");
  fill = (a->sections.p[a->section].flags & SHF_EXECINSTR) ? 0x90 : 0;
  maxskip = 268435456;
  if (IsInt(a, a->i)) {
    fill = GetInt(a);
    if (IsInt(a, a->i)) {
      maxskip = GetInt(a);
    }
  }
  i = a->sections.p[a->section].binary.n;
  n = ROUNDUP(i, align) - i;
  if (n > maxskip) return;
  a->sections.p[a->section].align = MAX(a->sections.p[a->section].align, align);
  for (i = 0; i < n; ++i) {
    Emit(a, fill);
  }
}

static int SectionFlag(struct Assembler *a, int c) {
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

static int SectionFlags(struct Assembler *a, struct Slice s) {
  int i, flags;
  for (flags = i = 0; i < s.n; ++i) {
    flags |= SectionFlag(a, s.p[i]);
  }
  return flags;
}

static int SectionType(struct Assembler *a, struct Slice s) {
  if (IS(s.p, s.n, "@progbits")) {
    return SHT_PROGBITS;
  } else if (IS(s.p, s.n, "@note")) {
    return SHT_NOTE;
  } else if (IS(s.p, s.n, "@nobits")) {
    return SHT_NOBITS;
  } else if (IS(s.p, s.n, "@preinit_array")) {
    return SHT_PREINIT_ARRAY;
  } else if (IS(s.p, s.n, "@init_array")) {
    return SHT_INIT_ARRAY;
  } else if (IS(s.p, s.n, "@fini_array")) {
    return SHT_FINI_ARRAY;
  } else {
    Fail(a, "unknown section type: %.*s", s.n, s.p);
  }
}

static void OnSection(struct Assembler *a) {
  char *name;
  int flags, type;
  struct Slice arg;
  arg = GetString(a);
  name = strndup(arg.p, arg.n);
  if (startswith(name, ".text")) {
    flags = SHF_ALLOC | SHF_EXECINSTR;
    type = SHT_PROGBITS;
  } else if (startswith(name, ".data")) {
    flags = SHF_ALLOC | SHF_WRITE;
    type = SHT_PROGBITS;
  } else if (startswith(name, ".bss")) {
    flags = SHF_ALLOC | SHF_WRITE;
    type = SHT_NOBITS;
  } else {
    flags = SHF_ALLOC | SHF_EXECINSTR | SHF_WRITE;
    type = SHT_PROGBITS;
  }
  if (IsSlice(a, a->i)) {
    flags = SectionFlags(a, GetString(a));
    if (IsSlice(a, a->i)) {
      type = SectionType(a, GetString(a));
    }
  }
  SetSection(a, AppendSection(a, name, flags, type));
}

static void OnIncbin(struct Assembler *a) {
  int fd;
  char *path;
  struct stat st;
  struct Slice arg;
  arg = GetString(a);
  path = strndup(arg.p, arg.n);
  if ((fd = open(path, O_RDONLY)) == -1 || fstat(fd, &st) == -1) {
    Fail(a, "open failed: %s", path);
  }
  a->sections.p[a->section].binary.p =
      realloc(a->sections.p[a->section].binary.p,
              a->sections.p[a->section].binary.n + st.st_size);
  if (read(fd, a->sections.p[a->section].binary.p, st.st_size) != st.st_size) {
    Fail(a, "read failed: %s", path);
  }
  a->sections.p[a->section].binary.n += st.st_size;
  close(fd);
  free(path);
}

static int SymbolType(struct Assembler *a, struct Slice s) {
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

static void OnType(struct Assembler *a) {
  int i;
  i = GetSymbol(a, a->things.p[a->i++].i);
  a->symbols.p[i].type = SymbolType(a, GetString(a));
}

static void OnSize(struct Assembler *a) {
  int i;
  i = GetSymbol(a, a->things.p[a->i++].i);
  a->symbols.p[i].size = GetInt(a);
}

static void OnInternal(struct Assembler *a) {
  int i;
  while (IsSlice(a, a->i)) {
    i = GetSymbol(a, a->things.p[a->i++].i);
    a->symbols.p[i].stv = STV_INTERNAL;
  }
}

static void OnHidden(struct Assembler *a) {
  int i;
  while (IsSlice(a, a->i)) {
    i = GetSymbol(a, a->things.p[a->i++].i);
    a->symbols.p[i].stv = STV_HIDDEN;
  }
}

static void OnProtected(struct Assembler *a) {
  int i;
  while (IsSlice(a, a->i)) {
    i = GetSymbol(a, a->things.p[a->i++].i);
    a->symbols.p[i].stv = STV_PROTECTED;
  }
}

static void OnLocal(struct Assembler *a) {
  int i;
  while (IsSlice(a, a->i)) {
    i = GetSymbol(a, a->things.p[a->i++].i);
    a->symbols.p[i].stb = STB_LOCAL;
  }
}

static void OnWeak(struct Assembler *a) {
  int i;
  while (IsSlice(a, a->i)) {
    i = GetSymbol(a, a->things.p[a->i++].i);
    a->symbols.p[i].stb = STB_WEAK;
  }
}

static void OnGlobal(struct Assembler *a) {
  int i;
  while (IsSlice(a, a->i)) {
    i = GetSymbol(a, a->things.p[a->i++].i);
    a->symbols.p[i].stb = STB_GLOBAL;
  }
}

static bool IsSizableOp(const char *op, struct Slice s) {
  size_t n = strlen(op);
  if (n == s.n) return !memcmp(op, s.p, n);
  if (n + 1 == s.n && !memcmp(op, s.p, n)) {
    switch (s.p[n]) {
      case 'b':
      case 'B':
      case 'w':
      case 'W':
      case 'l':
      case 'L':
      case 'q':
      case 'Q':
        return true;
      default:
        break;
    }
  }
  return false;
}

static int GetOpSize(struct Assembler *a, struct Slice s) {
  switch (s.p[s.n - 1]) {
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

static int CompareString8(const char a[8], const char b[8]) {
  uint64_t x, y;
  x = READ64BE(a);
  y = READ64BE(b);
  return x > y ? 1 : x < y ? -1 : 0;
}

static bool FindReg(const char *p, size_t n, struct Reg *out_reg) {
  char key[8];
  int i, c, m, l, r;
  if (n && n <= 8) {
    if (*p == '%') ++p, --n;
    memset(key, 0, sizeof(key));
    for (i = 0; i < n; ++i) {
      key[i] = tolower(p[i]);
    }
    l = 0;
    r = ARRAYLEN(kRegs) - 1;
    while (l <= r) {
      m = (l + r) >> 1;
      c = CompareString8(kRegs[m].s, key);
      if (c < 0) {
        l = m + 1;
      } else if (c > 0) {
        r = m - 1;
      } else {
        *out_reg = kRegs[m];
        return true;
      }
    }
  }
  return false;
}

static int RemoveRexw(int x) {
  if (x == -1) return x;
  x &= ~0x0800;
  if (((x & 0xff00) >> 8) == REX) x &= ~0xff00;
  return x;
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

static wontreturn void InvalidRegister(struct Assembler *a) {
  Fail(a, "invalid register");
}

static int GetRegisterReg(struct Assembler *a) {
  int reg;
  struct Slice wut;
  if ((reg = FindRegReg(GetString(a))) == -1) InvalidRegister(a);
  return reg;
}

static int GetRegisterRm(struct Assembler *a) {
  int reg;
  struct Slice wut;
  if ((reg = FindRegRm(GetString(a))) == -1) InvalidRegister(a);
  return reg;
}

static int ParseMemory(struct Assembler *a, long *disp) {
  /*            ┌isrip
                │┌hasindex
                ││┌hasbase
                │││┌hasasz
                ││││┌rex
                │││││       ┌scale
                │││││       │ ┌index
                │││││       │ │  ┌base
                ││││├──────┐├┐├─┐├─┐
  0b00000000000000000000000000000000*/
  struct Slice str;
  int reg, scale, mem = 0;
  *disp = IsInt(a, a->i) ? GetInt(a) : 0;
  if (IsPunct(a, a->i, '(')) {
    ++a->i;
    if ((str = GetString(a)).n) {
      mem |= HASBASE;
      if (!strncasecmp(str.p, "%rip", str.n)) {
        mem |= ISRIP;
      } else {
        reg = FindRegBase(str);
        if (reg == -1) InvalidRegister(a);
        mem |= reg & 007;                            // reg
        mem |= reg & 0xff00;                         // rex
        if (((reg & 070) >> 3) == 2) mem |= HASASZ;  // asz
      }
    }
    if (!IsPunct(a, a->i, ')')) {
      mem |= HASINDEX;
      reg = FindRegIndex(GetString(a));
      if (reg == -1) InvalidRegister(a);
      mem |= (reg & 007) << 3;                     // index
      mem |= reg & 0xff00;                         // rex
      if (((reg & 070) >> 3) == 2) mem |= HASASZ;  // asz
      if (!IsPunct(a, a->i, ')')) {
        mem |= (bsr(GetInt(a)) & 3) << 6;
      }
    } else {
      mem |= 4 << 3;  // rsp index (hint: there is none)
    }
    ConsumePunct(a, ')');
  }
  return mem;
}

static void EncodeModrm(struct Assembler *a, int reg, int mem, long disp) {
  reg &= 7;
  reg <<= 3;
  if (mem & (HASBASE | HASINDEX)) {
    if (mem & ISRIP) {
      Emit(a, 005 | reg);
    } else {
      Emit(a, 0204 | reg);  // suboptimal
      Emit(a, mem);
    }
  } else {
    Emit(a, 004 | reg);
    Emit(a, 045);
  }
  EmitLong(a, disp);
}

static void OnMov(struct Assembler *a, struct Slice op) {
  long imm, disp;
  int reg, rm, mem, modrm;
  if (IsPunct(a, a->i, '$')) {
    ++a->i;
    imm = GetInt(a);
    if (IsSlice(a, a->i)) {  // imm -> reg
      reg = GetRegisterRm(a);
      if (reg & 0xff00) {
        Emit(a, reg >> 8);
      }
      switch ((reg & 070) >> 3) {
        case 0:
          Emit(a, 0xb0 + (reg & 7));
          Emit(a, imm);
          break;
        case 1:
          Emit(a, OSZ);
          Emit(a, 0xb8 + (reg & 7));
          EmitWord(a, imm);
          break;
        case 2:
          Emit(a, 0xb8 + (reg & 7));
          EmitLong(a, imm);
          break;
        case 3:
          Emit(a, 0xb8 + (reg & 7));  // suboptimal
          EmitQuad(a, imm);
          break;
        default:
          Fail(a, "todo movd/movq");
      }
    } else {  // imm -> mem
      mem = ParseMemory(a, &disp);
      if (mem & 0xff00) {
        Emit(a, mem >> 8);
      }
      switch (GetOpSize(a, op)) {
        case 0:
          Emit(a, 0xc6);
          EncodeModrm(a, 0, mem, disp);
          Emit(a, imm);
          break;
        case 1:
          Emit(a, OSZ);
          Emit(a, 0xc7);
          EncodeModrm(a, 0, mem, disp);
          EmitWord(a, imm);
          break;
        case 2:
          Emit(a, 0xc7);
          EncodeModrm(a, 0, mem, disp);
          EmitLong(a, imm);
          break;
        case 3:
          Emit(a, 0xc7);  // suboptimal
          EncodeModrm(a, 0, mem, disp);
          EmitQuad(a, imm);
          break;
        default:
          unreachable;
      }
    }
  } else if (IsSlice(a, a->i)) {
    reg = GetRegisterReg(a);
    if (IsSlice(a, a->i)) {  // reg -> reg
      rm = GetRegisterRm(a);
      if (((reg & 070) >> 3) != ((rm & 070) >> 3)) {
        Fail(a, "size mismatch");
      }
      if ((reg | rm) & 0xff00) {
        Emit(a, (reg | rm) >> 8);
      }
      modrm = 0300 | (reg & 7) << 3 | rm & 7;
      switch ((reg & 070) >> 3) {
        case 0:
          Emit(a, 0x88);
          Emit(a, modrm);
          break;
        case 1:
          Emit(a, OSZ);
          Emit(a, 0x89);
          Emit(a, modrm);
          break;
        case 2:
        case 3:
          Emit(a, 0x89);
          Emit(a, modrm);
          break;
        case 4:
          Emit(a, 0x66);
          Emit(a, 0x0F);
          Emit(a, 0x6F);
          Emit(a, modrm);
          break;
        default:
          unreachable;
      }
    } else {  // reg -> mem
      mem = ParseMemory(a, &disp);
      if ((reg | mem) & 0xff00) {
        Emit(a, (reg | mem) >> 8);
      }
      modrm = 0300 | (reg & 7) << 3 | rm & 7;
      switch ((reg & 070) >> 3) {
        case 0:
          Emit(a, 0x88);
          EncodeModrm(a, reg, mem, 0);
          break;
        case 1:
          Emit(a, OSZ);
          Emit(a, 0x89);
          EncodeModrm(a, reg, mem, 0);
          break;
        case 2:
        case 3:
          Emit(a, 0x89);
          EncodeModrm(a, reg, mem, 0);
          break;
        default:
          Fail(a, "todo movdqu");
      }
    }
  } else {  // mem -> reg
    mem = ParseMemory(a, &disp);
    reg = GetRegisterReg(a);
    if ((reg | mem) & 0xff00) {
      Emit(a, (reg | mem) >> 8);
    }
    modrm = 0300 | (reg & 7) << 3 | rm & 7;
    switch ((reg & 070) >> 3) {
      case 0:
        Emit(a, 0x8A);
        EncodeModrm(a, reg, mem, 0);
        break;
      case 1:
        Emit(a, OSZ);
        Emit(a, 0x8B);
        EncodeModrm(a, reg, mem, 0);
        break;
      case 2:
      case 3:
        Emit(a, 0x8B);
        EncodeModrm(a, reg, mem, 0);
        break;
      default:
        Fail(a, "todo movdqu");
    }
  }
}

static void OnPush(struct Assembler *a) {
  long x;
  int reg;
  if (IsPunct(a, a->i, '$')) {
    ++a->i;
    x = GetInt(a);
    if (-128 <= x && x <= 127) {
      Emit(a, 0x6A);
      Emit(a, x);
    } else {
      Emit(a, 0x68);
      EmitLong(a, x);
    }
  } else {
    reg = RemoveRexw(GetRegisterRm(a));
    if (reg & 0xff00) Emit(a, reg >> 8);
    if (((reg & 070) >> 3) == 1) Emit(a, OSZ);
    Emit(a, 0x50 + (reg & 7));
  }
}

static void OnPop(struct Assembler *a) {
  int reg;
  reg = RemoveRexw(GetRegisterRm(a));
  if (reg & 0xff00) Emit(a, reg >> 8);
  if (((reg & 070) >> 3) == 1) Emit(a, OSZ);
  Emit(a, 0x58 + (reg & 7));
}

static void OnRet(struct Assembler *a) {
  if (IsPunct(a, a->i, '$')) {
    ++a->i;
    Emit(a, 0xC2);
    EmitWord(a, GetInt(a));
  } else {
    Emit(a, 0xC3);
  }
}

static void OnLeave(struct Assembler *a) {
  Emit(a, 0xC9);
}
static void OnHlt(struct Assembler *a) {
  Emit(a, 0xF4);
}
static void OnCmc(struct Assembler *a) {
  Emit(a, 0xF5);
}
static void OnClc(struct Assembler *a) {
  Emit(a, 0xF8);
}
static void OnStc(struct Assembler *a) {
  Emit(a, 0xF9);
}
static void OnCli(struct Assembler *a) {
  Emit(a, 0xFA);
}
static void OnSti(struct Assembler *a) {
  Emit(a, 0xFB);
}
static void OnCld(struct Assembler *a) {
  Emit(a, 0xFC);
}
static void OnStd(struct Assembler *a) {
  Emit(a, 0xFD);
}
static void OnLodsb(struct Assembler *a) {
  Emit(a, 0xAC);
}
static void OnLodsw(struct Assembler *a) {
  Emit(a, OSZ);
  Emit(a, 0xAD);
}
static void OnLodsl(struct Assembler *a) {
  Emit(a, 0xAD);
}
static void OnLodsq(struct Assembler *a) {
  Emit(a, REXW);
  Emit(a, 0xAD);
}
static void OnStosb(struct Assembler *a) {
  Emit(a, 0xAA);
}
static void OnStosw(struct Assembler *a) {
  Emit(a, OSZ);
  Emit(a, 0xAB);
}
static void OnStosl(struct Assembler *a) {
  Emit(a, 0xAB);
}
static void OnStosq(struct Assembler *a) {
  Emit(a, REXW);
  Emit(a, 0xAB);
}
static void OnMovsb(struct Assembler *a) {
  Emit(a, 0xA4);
}
static void OnMovsw(struct Assembler *a) {
  Emit(a, OSZ);
  Emit(a, 0xA5);
}
static void OnMovsl(struct Assembler *a) {
  Emit(a, 0xA5);
}
static void OnMovsq(struct Assembler *a) {
  Emit(a, REXW);
  Emit(a, 0xA5);
}

static bool Prefix(struct Assembler *a, const char *s, size_t n) {
  int i;
  char key[8];
  if (n <= 8) {
    memset(key, 0, 8);
    for (i = 0; i < n; ++i) key[i] = tolower(s[i]);
    for (i = 0; i < sizeof(kPrefix) / sizeof(*kPrefix); i++) {
      if (!memcmp(key, kPrefix[i], 8)) {
        Emit(a, kPrefixByte[i]);
        return true;
      }
    }
  }
  return false;
}

static void Directive(struct Assembler *a) {
  struct Slice s;
  for (;;) {
    s = GetString(a);
    if (!Prefix(a, s.p, s.n)) break;
  }
  if (s.n >= 1 && s.p[0] == '.') {
    if (IS(s.p, s.n, ".zero")) {
      OnZero(a);
    } else if (IS(s.p, s.n, ".align") || IS(s.p, s.n, ".balign")) {
      OnAlign(a);
    } else if (IS(s.p, s.n, ".byte")) {
      OnByte(a);
    } else if (IS(s.p, s.n, ".word") || IS(s.p, s.n, ".short")) {
      OnWord(a);
    } else if (IS(s.p, s.n, ".long")) {
      OnLong(a);
    } else if (IS(s.p, s.n, ".quad")) {
      OnQuad(a);
    } else if (IS(s.p, s.n, ".float")) {
      OnFloat(a);
    } else if (IS(s.p, s.n, ".double")) {
      OnDouble(a);
    } else if (IS(s.p, s.n, ".ascii")) {
      OnAscii(a);
    } else if (IS(s.p, s.n, ".asciz")) {
      OnAsciz(a);
    } else if (IS(s.p, s.n, ".text")) {
      OnText(a);
    } else if (IS(s.p, s.n, ".data")) {
      OnData(a);
    } else if (IS(s.p, s.n, ".bss")) {
      OnBss(a);
    } else if (IS(s.p, s.n, ".previous")) {
      OnPrevious(a);
    } else if (IS(s.p, s.n, ".section")) {
      OnSection(a);
    } else if (IS(s.p, s.n, ".abort")) {
      OnAbort(a);
    } else if (IS(s.p, s.n, ".err")) {
      OnErr(a);
    } else if (IS(s.p, s.n, ".error")) {
      OnError(a);
    } else if (IS(s.p, s.n, ".fill") || IS(s.p, s.n, ".space")) {
      OnFill(a);
    } else if (IS(s.p, s.n, ".type")) {
      OnType(a);
    } else if (IS(s.p, s.n, ".size")) {
      OnSize(a);
    } else if (IS(s.p, s.n, ".local")) {
      OnLocal(a);
    } else if (IS(s.p, s.n, ".internal")) {
      OnInternal(a);
    } else if (IS(s.p, s.n, ".weak")) {
      OnWeak(a);
    } else if (IS(s.p, s.n, ".hidden")) {
      OnHidden(a);
    } else if (IS(s.p, s.n, ".globl") || IS(s.p, s.n, ".global")) {
      OnGlobal(a);
    } else if (IS(s.p, s.n, ".protected")) {
      OnProtected(a);
    } else if (IS(s.p, s.n, ".incbin")) {
      OnIncbin(a);
    } else {
      Fail(a, "unexpected directive: %.*s", s.n, s.p);
    }
  } else if (IS(s.p, s.n, "ret")) {
    OnRet(a);
  } else if (IS(s.p, s.n, "leave")) {
    OnLeave(a);
  } else if (IS(s.p, s.n, "push")) {
    OnPush(a);
  } else if (IS(s.p, s.n, "pop")) {
    OnPop(a);
  } else if (IS(s.p, s.n, "hlt")) {
    OnHlt(a);
  } else if (IS(s.p, s.n, "cmc")) {
    OnCmc(a);
  } else if (IS(s.p, s.n, "clc")) {
    OnClc(a);
  } else if (IS(s.p, s.n, "stc")) {
    OnStc(a);
  } else if (IS(s.p, s.n, "cli")) {
    OnCli(a);
  } else if (IS(s.p, s.n, "sti")) {
    OnSti(a);
  } else if (IS(s.p, s.n, "cld")) {
    OnCld(a);
  } else if (IS(s.p, s.n, "std")) {
    OnStd(a);
  } else if (IS(s.p, s.n, "stosb")) {
    OnStosb(a);
  } else if (IS(s.p, s.n, "stosw")) {
    OnStosw(a);
  } else if (IS(s.p, s.n, "stosl")) {
    OnStosl(a);
  } else if (IS(s.p, s.n, "stosq")) {
    OnStosq(a);
  } else if (IS(s.p, s.n, "lodsb")) {
    OnLodsb(a);
  } else if (IS(s.p, s.n, "lodsw")) {
    OnLodsw(a);
  } else if (IS(s.p, s.n, "lodsl")) {
    OnLodsl(a);
  } else if (IS(s.p, s.n, "lodsq")) {
    OnLodsq(a);
  } else if (IS(s.p, s.n, "movsb")) {
    OnMovsb(a);
  } else if (IS(s.p, s.n, "movsw")) {
    OnMovsw(a);
  } else if (IS(s.p, s.n, "movsl")) {
    OnMovsl(a);
  } else if (IS(s.p, s.n, "movsq")) {
    OnMovsq(a);
  } else if (IsSizableOp("mov", s)) {
    OnMov(a, s);
  } else {
    Fail(a, "unexpected op: %.*s", s.n, s.p);
  }
  ConsumePunct(a, ';');
}

static void Assemble(struct Assembler *a) {
  while (a->i < a->things.n) {
    if (IsPunct(a, a->i, ';')) {
      ++a->i;
      continue;
    }
    switch (a->things.p[a->i].t) {
      case TT_SLICE:
        if (IsPunct(a, a->i + 1, ':')) {
          Label(a, a->things.p[a->i].i);
        } else {
          Directive(a);
        }
        break;
      case TT_INT:
        if (IsPunct(a, a->i + 1, ':')) {
          LocalLabel(a, a->ints.p[a->things.p[a->i].i]);
        }
        // fallthrough
      default:
        Fail(a, "unexpected token");
    }
  }
}

static void Objectify(struct Assembler *a, const char *path) {
  size_t i, j;
  struct ElfWriter *elf;
  elf = elfwriter_open(path, 0644);
  for (i = 0; i < a->sections.n; ++i) {
    elfwriter_align(elf, a->sections.p[i].align, 0);
    elfwriter_startsection(elf, a->sections.p[i].name, a->sections.p[i].type,
                           a->sections.p[i].flags);
    for (j = 0; j < a->symbols.n; ++j) {
      if (a->symbols.p[j].section != i) continue;
      elfwriter_appendsym(
          elf,
          strndup(a->slices.p[a->symbols.p[j].name].p,
                  a->slices.p[a->symbols.p[j].name].n),
          ELF64_ST_INFO(a->symbols.p[j].stb, a->symbols.p[j].type),
          a->symbols.p[j].stv, a->symbols.p[j].location, a->symbols.p[j].size);
    }
    memcpy(elfwriter_reserve(elf, a->sections.p[i].binary.n),
           a->sections.p[i].binary.p, a->sections.p[i].binary.n);
    elfwriter_commit(elf, a->sections.p[i].binary.n);
    elfwriter_finishsection(elf);
  }
  elfwriter_close(elf);
}

static void PrintThings(struct Assembler *a) {
  int i;
  char fbuf[32];
  for (i = 0; i < a->things.n; ++i) {
    printf("%s:%d:: ", a->sauces.p[a->things.p[i].s].path,
           a->sauces.p[a->things.p[i].s].line);
    switch (a->things.p[i].t) {
      case TT_INT:
        printf("TT_INT %ld\n", a->ints.p[a->things.p[i].i]);
        break;
      case TT_FLOAT:
        g_xfmt_p(fbuf, &a->floats.p[a->things.p[i].i], 19, sizeof(fbuf), 0);
        printf("TT_FLOAT %s\n", fbuf);
        break;
      case TT_SLICE:
        printf("TT_SLICE %`'.*s\n", a->slices.p[a->things.p[i].i].n,
               a->slices.p[a->things.p[i].i].p);
        break;
      case TT_PUNCT:
        printf("TT_PUNCT %`'c\n", a->things.p[i].i);
        break;
      default:
        unreachable;
    }
  }
}

void Assembler(int argc, char *argv[]) {
  struct Assembler *a;
  showcrashreports();
  if (argc == 1) {
    system("o//third_party/chibicc/as.com -o /tmp/o third_party/chibicc/hog.s");
    system("objdump -wxd /tmp/o");
    exit(0);
  }
  ReadFlags(argc, argv);
  a = NewAssembler();
  Tokenize(a, g_input_path);
  /* PrintThings(a); */
  Assemble(a);
  Objectify(a, g_output_path);
}

int main(int argc, char *argv[]) {
  Assembler(argc, argv);
  return 0;
}
