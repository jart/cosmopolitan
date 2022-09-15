/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/fmt/conv.h"
#include "libc/log/check.h"
#include "libc/math.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/str/tab.internal.h"
#include "libc/x/xasprintf.h"

/**
 * @fileoverview Tool for generating rldecode'd character sets, e.g.
 *
 *     # generate http token table
 *     o//tool/build/xlat.com -TiC ' ()<>@,;:\"/[]?={}' -i
 */

int dig;
int xlat[256];
bool identity;
const char *symbol;

static int Bing(int c) {
  if (!c) return L'∅';
  if (c == ' ') return L'␠';
  if (c == '$') return L'§';
  if (c == '\\') return L'⭝';
  return kCp437[c & 255];
}

static void Fill(int f(int)) {
  int i;
  for (i = 0; i < 256; ++i) {
    if (f(i)) {
      xlat[i] = identity ? i : 1;
    }
  }
}

static void Invert(void) {
  int i;
  for (i = 0; i < 256; ++i) {
    xlat[i] = !xlat[i];
  }
}

static void Negate(void) {
  int i;
  for (i = 0; i < 256; ++i) {
    xlat[i] = ~xlat[i] & 255;
  }
}

static void Negative(void) {
  int i;
  for (i = 0; i < 256; ++i) {
    xlat[i] = -xlat[i] & 255;
  }
}

static bool ArgNeedsShellQuotes(const char *s) {
  if (*s) {
    for (;;) {
      switch (*s++ & 255) {
        case 0:
          return false;
        case '-':
        case '.':
        case '/':
        case '_':
        case '=':
        case ':':
        case '0' ... '9':
        case 'A' ... 'Z':
        case 'a' ... 'z':
          break;
        default:
          return true;
      }
    }
  } else {
    return true;
  }
}

static char *AddShellQuotes(const char *s) {
  char *p, *q;
  size_t i, j, n;
  n = strlen(s);
  p = malloc(1 + n * 5 + 1 + 1);
  j = 0;
  p[j++] = '\'';
  for (i = 0; i < n; ++i) {
    if (s[i] != '\'') {
      p[j++] = s[i];
    } else {
      p[j + 0] = '\'';
      p[j + 1] = '"';
      p[j + 2] = '\'';
      p[j + 3] = '"';
      p[j + 4] = '\'';
      j += 5;
    }
  }
  p[j++] = '\'';
  p[j] = 0;
  if ((q = realloc(p, j + 1))) p = q;
  return p;
}

static const char *GetArg(char *argv[], int i, int *k) {
  if (argv[*k][i + 1]) {
    return argv[*k] + i + 1;
  } else {
    return argv[++*k];
  }
}

int main(int argc, char *argv[]) {
  const char *arg;
  int i, j, k, opt;
  dig = 1;
  symbol = "kXlatTab";

  for (k = 1; k < argc; ++k) {
    if (argv[k][0] != '-') {
      for (i = 0; argv[k][i]; ++i) {
        /* xlat[argv[k][i] & 255] = identity ? i : dig; */
        xlat[argv[k][i] & 255] = identity ? (argv[k][i] & 255) : dig;
      }
    } else {
      i = 0;
    moar:
      ++i;
      if ((opt = argv[k][i])) {
        switch (opt) {
          case 's':
            symbol = GetArg(argv, i, &k);
            break;
          case 'x':
            dig = atoi(GetArg(argv, i, &k)) & 255;
            break;
          case 'i':
            Invert();
            goto moar;
          case 'I':
            identity = !identity;
            goto moar;
          case 'n':
            Negative();
            goto moar;
          case 'N':
            Negate();
            goto moar;
          case 'T':
            Fill(isascii);
            goto moar;
          case 'C':
            Fill(iscntrl);
            goto moar;
          case 'A':
            Fill(isalpha);
            goto moar;
          case 'B':
            Fill(isblank);
            goto moar;
          case 'G':
            Fill(isgraph);
            goto moar;
          case 'P':
            Fill(ispunct);
            goto moar;
          case 'D':
            Fill(isdigit);
            goto moar;
          case 'U':
            Fill(isupper);
            goto moar;
          case 'L':
            Fill(islower);
            goto moar;
          default:
            fprintf(stderr, "error: unrecognized option: %c\n", opt);
            return 1;
        }
      }
    }
  }

  ////////////////////////////////////////////////////////////
  printf("#include \"libc/macros.internal.h\"\n");
  printf("\n");

  printf("//\tgenerated by:\n");
  printf("//\t");
  for (i = 0; i < argc; ++i) {
    if (i) printf(" ");
    printf("%s", !ArgNeedsShellQuotes(argv[i]) ? argv[i]
                                               : _gc(AddShellQuotes(argv[i])));
  }
  printf("\n");

  ////////////////////////////////////////////////////////////
  printf("//\n");
  printf("//\t    present            absent\n");
  printf("//\t    ────────────────   ────────────────\n");
  for (i = 0; i < 16; ++i) {
    char16_t absent[16];
    char16_t present[16];
    for (j = 0; j < 16; ++j) {
      if (xlat[i * 16 + j]) {
        absent[j] = L' ';
        present[j] = Bing(i * 16 + j);
      } else {
        absent[j] = Bing(i * 16 + j);
        present[j] = L' ';
      }
    }
    printf("//\t    %.16hs   %.16hs   0x%02x\n", present, absent, i * 16);
  }

  ////////////////////////////////////////////////////////////
  printf("//\n");
  printf("//\tconst char %s[256] = {\n//\t", symbol);
  for (i = 0; i < 16; ++i) {
    printf("  ");
    for (j = 0; j < 16; ++j) {
      printf("%2d,", (char)xlat[i * 16 + j]);
    }
    printf(" // 0x%02x\n//\t", i * 16);
  }
  printf("};\n");
  printf("\n");

  ////////////////////////////////////////////////////////////
  printf("\t.initbss 300,_init_%s\n", symbol);
  printf("%s:\n", symbol);
  printf("\t.zero\t256\n");
  printf("\t.endobj\t%s,globl\n", symbol);
  printf("\t.previous\n");
  printf("\n");

  ////////////////////////////////////////////////////////////
  printf("\t.initro 300,_init_%s\n", symbol);
  printf("%s.rom:\n", symbol);

  int thebloat = 0;
  int thetally = 0;
  int thecount = 0;
  int runstart = 0;
  int runchar = -1;
  int runcount = 0;
  for (i = 0;; ++i) {
    if (i < 256 && xlat[i] == runchar) {
      ++runcount;
    } else {
      if (runcount) {
        printf("\t.byte\t%-24s# %02x-%02x %hc-%hc\n",
               _gc(xasprintf("%3d,%d", runcount, runchar)), runstart,
               runstart + runcount - 1, Bing(runstart),
               Bing(runstart + runcount - 1));
        thetally += 2;
        thecount += runcount;
      }
      if (i < 256) {
        runcount = 1;
        runchar = xlat[i];
        runstart = i;
      }
    }
    if (i == 256) {
      break;
    }
  }
  CHECK_EQ(256, thecount);
  printf("\t.byte\t%-24s# terminator\n", "0,0");
  thetally += 2;
  thebloat = thetally;
  for (i = 0; (thetally + i) % 8; i += 2) {
    printf("\t.byte\t%-24s# padding\n", "0,0");
    thebloat += 2;
  }

  printf("\t.endobj\t%s.rom,globl\n", symbol);
  printf("\n");

  ////////////////////////////////////////////////////////////
  printf("\t.init.start 300,_init_%s\n", symbol);
  printf("\tcall\trldecode\n");
  thebloat += 5;
  int padding = 8 - thetally % 8;
  if (padding < 8) {
    if (padding >= 4) {
      thebloat += 1;
      printf("\tlodsl\n");
      padding -= 4;
    }
    if (padding >= 2) {
      thebloat += 2;
      printf("\tlodsw\n");
    }
  }
  printf("\t.init.end 300,_init_%s\n", symbol);

  ////////////////////////////////////////////////////////////
  printf("\n");
  printf("//\t%d bytes total (%d%% original size)\n", thebloat,
         (int)round((double)thebloat / 256 * 100));
}
