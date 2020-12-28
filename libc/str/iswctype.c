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
#include "libc/macros.h"
#include "libc/str/str.h"

#define ALNUM  1
#define ALPHA  2
#define BLANK  3
#define CNTRL  4
#define DIGIT  5
#define GRAPH  6
#define LOWER  7
#define PRINT  8
#define PUNCT  9
#define SPACE  10
#define UPPER  11
#define XDIGIT 12

static const struct {
  char name[7];
  char type;
} kWcTypes[] = {
    {"alnum", ALNUM}, {"alpha", ALPHA}, {"blank", BLANK}, {"cntrl", CNTRL},
    {"digit", DIGIT}, {"graph", GRAPH}, {"lower", LOWER}, {"print", PRINT},
    {"punct", PUNCT}, {"space", SPACE}, {"upper", UPPER}, {"xdigit", XDIGIT},
};

static int CompareStrings(const char *l, const char *r) {
  size_t i = 0;
  while (l[i] == r[i] && r[i]) ++i;
  return (l[i] & 0xff) - (r[i] & 0xff);
}

wctype_t wctype(const char *name) {
  unsigned i;
  for (i = 0; i < ARRAYLEN(kWcTypes); ++i) {
    if (CompareStrings(name, kWcTypes[i].name) == 0) {
      return kWcTypes[i].type;
    }
  }
  return 0;
}

int iswctype(wint_t wc, wctype_t type) {
  switch (type) {
    case ALNUM:
      return iswalnum(wc);
    case ALPHA:
      return iswalpha(wc);
    case BLANK:
      return iswblank(wc);
    case CNTRL:
      return iswcntrl(wc);
    case DIGIT:
      return iswdigit(wc);
    case GRAPH:
      return iswgraph(wc);
    case LOWER:
      return iswlower(wc);
    case PRINT:
      return iswprint(wc);
    case PUNCT:
      return iswpunct(wc);
    case SPACE:
      return iswspace(wc);
    case UPPER:
      return iswupper(wc);
    case XDIGIT:
      return iswxdigit(wc);
    default:
      return 0;
  }
}
