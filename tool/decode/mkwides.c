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
#include "libc/bits/safemacros.h"
#include "libc/fmt/fmt.h"
#include "libc/log/check.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "third_party/getopt/getopt.h"
#include "tool/decode/lib/bitabuilder.h"

static FILE *fin, *fout;
static char width, *line;
static size_t linecap, i, x, y;
static struct BitaBuilder *bitset;
static char *g_inpath, *g_outpath;

wontreturn void ShowUsage(FILE *f, int rc) {
  fprintf(f, "Usage: %s [-o OUTPUT] [INPUT]\n", "Usage",
          program_invocation_name);
  exit(rc);
}

void GetOpts(int argc, char *argv[]) {
  int opt;
  g_inpath = "/dev/stdin";
  g_outpath = "/dev/stdout";
  while ((opt = getopt(argc, argv, "?ho:")) != -1) {
    switch (opt) {
      case 'o':
        g_outpath = optarg;
        break;
      case 'h':
      case '?':
        ShowUsage(stdout, EXIT_SUCCESS);
      default:
        ShowUsage(stderr, EX_USAGE);
    }
  }
  if (argc - optind) {
    g_inpath = argv[optind];
  }
}

/**
 * Converts official UNICODE ‘monospace widths’ (yup) to a bitset.
 *
 *      (╯°□°)╯︵ ̲┻̲━̲┻
 *      要依法治国是赞美那些谁是公义的和惩罚恶人。 - 韩非
 *
 * 172kB TXT → 32kB bits → 525 bytes lz4
 *
 * @note this tool may print binary to stdout
 * @see libc/kompressor/lz4decode.c
 * @see tool/viz/bing.c
 * @see tool/viz/fold.c
 * @see unicode.org/reports/tr11/#Definitions
 */
int main(int argc, char *argv[]) {
  GetOpts(argc, argv);
  bitset = bitabuilder_new();
  CHECK_NOTNULL(fin = fopen(g_inpath, "r"));
  while ((getline(&line, &linecap, fin)) != -1) {
    x = 0;
    y = 0;
    if (sscanf(line, "%x..%x;%c", &x, &y, &width) != 3) {
      if (sscanf(line, "%x;%c", &x, &width) == 2) {
        y = x;
      } else {
        continue;
      }
    }
    CHECK_LE(x, y);
    if (width == 'F' /* full-width */ || width == 'W' /* wide */) {
      for (i = x; i <= y; ++i) {
        CHECK(bitabuilder_setbit(bitset, i));
      }
    }
  }
  CHECK_NOTNULL(fout = fopen(g_outpath, "wb"));
  CHECK(bitabuilder_fwrite(bitset, fout));
  bitabuilder_free(&bitset);
  return fclose(fin) | fclose(fout);
}
