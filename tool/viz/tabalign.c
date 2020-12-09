/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/alg/arraylist.internal.h"
#include "libc/bits/safemacros.internal.h"
#include "libc/fmt/conv.h"
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/unicode/unicode.h"
#include "third_party/getopt/getopt.h"

#define kOneTrueTabWidth 8

struct Pool {
  size_t i, n;
  char *p;
};

struct Lines {
  size_t i, n;
  struct Line {
    uint16_t off;
    uint16_t col;
    int32_t line;
  } * p;
};

static bool chunk_;
static FILE *fi_, *fo_;
static struct Pool pool_;
static struct Lines lines_;
static size_t mincol_, col_, maxcol_, linecap_;
static char *inpath_, *outpath_, *delim_, *line_;

wontreturn void usage(int rc, FILE *f) {
  fprintf(f, "%s%s%s\n", "Usage: ", program_invocation_name,
          " [-c] [-m MINCOL] [-M MAXCOL] [-F DELIM] [-o FILE] [FILE...]\n"
          "\n"
          "  This program aligns monospace text. It's aware of tabs,\n"
          "  color codes, wide characters, combining characters etc.\n");
  exit(rc);
}

void getopts(int *argc, char *argv[]) {
  int opt;
  delim_ = "#";
  outpath_ = "-";
  while ((opt = getopt(*argc, argv, "?hco:m:M:F:")) != -1) {
    switch (opt) {
      case 'm':
        mincol_ = strtol(optarg, NULL, 0);
        break;
      case 'M':
        maxcol_ = strtol(optarg, NULL, 0);
        break;
      case 'c':
        chunk_ = true;
        break;
      case 'o':
        outpath_ = optarg;
        break;
      case 'F':
        delim_ = optarg;
        break;
      case '?':
      case 'h':
        usage(EXIT_SUCCESS, stdout);
      default:
        usage(EX_USAGE, stderr);
    }
  }
  if (optind == *argc) {
    argv[(*argc)++] = "-";
  }
}

void flush(void) {
  size_t i, j;
  const char *s;
  struct Line l;
  col_ = roundup(col_ + 1, kOneTrueTabWidth);
  if (maxcol_) col_ = min(col_, maxcol_);
  for (i = 0; i < lines_.i; ++i) {
    l = lines_.p[i];
    s = &pool_.p[l.line];
    if (l.off < USHRT_MAX) {
      fwrite(s, l.off, 1, fo_);
      for (j = l.col; j < col_;) {
        fputc('\t', fo_);
        if (j % kOneTrueTabWidth == 0) {
          j += 8;
        } else {
          j += kOneTrueTabWidth - (j & (kOneTrueTabWidth - 1));
        }
      }
      fwrite(s + l.off, strlen(s) - l.off, 1, fo_);
    } else {
      fwrite(s, strlen(s), 1, fo_);
    }
    fputc('\n', fo_);
  }
  col_ = mincol_;
  pool_.i = 0;
  lines_.i = 0;
}

void processfile(void) {
  char *p;
  int col, s;
  size_t off, len;
  while ((getline(&line_, &linecap_, fi_)) != -1) {
    chomp(line_);
    len = strlen(line_);
    s = concat(&pool_, line_, len + 1);
    if (len < USHRT_MAX) {
      if ((p = strstr(line_, delim_))) {
        off = p - line_;
        col = strnwidth(line_, off);
        if (col < USHRT_MAX) {
          col_ = max(col_, col);
          append(&lines_, &((struct Line){.line = s, .off = off, .col = col}));
          continue;
        }
      } else {
        if (chunk_) {
          flush();
          fputs(line_, fo_);
          fputc('\n', fo_);
          continue;
        }
      }
    }
    append(&lines_, &((struct Line){.line = s, .off = 0xffff, .col = 0xffff}));
  }
}

int main(int argc, char *argv[]) {
  size_t i;
  getopts(&argc, argv);
  CHECK_NOTNULL((fo_ = fopen(outpath_, "w")));
  for (i = optind; i < argc; ++i) {
    CHECK_NOTNULL((fi_ = fopen((inpath_ = argv[i]), "r")));
    processfile();
    CHECK_NE(-1, fclose_s(&fi_));
  }
  flush();
  CHECK_NE(-1, fclose_s(&fo_));
  free(lines_.p);
  free(pool_.p);
  free(line_);
  return 0;
}
