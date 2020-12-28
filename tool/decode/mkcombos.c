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
#include "libc/calls/calls.h"
#include "libc/fmt/conv.h"
#include "libc/log/check.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/fileno.h"
#include "third_party/getopt/getopt.h"
#include "third_party/zlib/zlib.h"
#include "tool/decode/lib/bitabuilder.h"

static size_t linecap;
static FILE *fin, *fout;
static uint32_t bit, maxbit;
static struct BitaBuilder *bitset;
static char *line, *tok, *s1, *category, *g_inpath, *g_outpath;

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
 * Builds sparse unicode dataset for combining codepoints, e.g.
 *
 * UNDERLINING
 *
 *   - COMBINING MACRON BELOW               U+0331 (\xCC\xB1)
 *     e.g. A̱ḆC̱Ḏ (too short)                41CCB1 42CCB1 43CCB1 44CCB1
 *
 *   - COMBINING DOUBLE MACRON BELOW        U+035F (\xCD\x9F)
 *     e.g. A͟B͟C͟D͟ (too long)                 41CD9F 42CD9F 43CD9F 44CD9F
 *
 *   - DOUBLE PLUS COMBINING MACRON BELOW   3×U+035F + 1×U+0331
 *     e.g. A͟B͟C͟Ḏ (just right)               41CCB1 42CCB1 43CCB1 44CD9F
 *
 * STRIKETHROUGH
 *
 *   - COMBINING LONG STROKE OVERLAY        U+0336 (\xCC\xB6)
 *     e.g. A̶B̶C̶D̶                            41CCB6 42CCB6 43CCB6 44CCB6
 *
 *   - COMBINING SHORT STROKE OVERLAY       U+0335 (\xCC\xB5)
 *     e.g. A̵B̵C̵D̵                            41CCB5 42CCB5 43CCB5 44CCB5
 *
 *   - COMBINING SHORT SOLIDUS OVERLAY      U+0337 (\xCC\xB7)
 *     e.g. A̷B̷C̷D̷                            41CCB7 42CCB7 43CCB7 44CCB7
 *
 *   - COMBINING LONG SOLIDUS OVERLAY       U+0338 (\xCC\xB8)
 *     e.g. A̸B̸C̸D̸                            41CCB8 42CCB8 43CCB8 44CCB8
 *
 * @see unicode.org/reports/tr11/#Definitions
 * @see https://www.compart.com/en/unicode/category/Sk (Modifier Symbol)
 */
int main(int argc, char *argv[]) {
  GetOpts(argc, argv);
  CHECK_NOTNULL(fin = fopen(g_inpath, "r"));
  bitset = bitabuilder_new();
  while ((getline(&line, &linecap, fin)) != -1) {
    tok = line;
    s1 = strsep(&tok, ";");
    strsep(&tok, ";");
    category = strsep(&tok, ";");
    if (!s1 || !category) continue;
    bit = strtoimax(s1, NULL, 16);
    if (bit != 0x00AD &&
        ((0x1160 <= bit && bit <= 0x11FF) ||
         (strcmp(category, "Me") == 0 || strcmp(category, "Mn") == 0 ||
          strcmp(category, "Cf") == 0))) {
      maxbit = max(bit, maxbit);
      CHECK(bitabuilder_setbit(bitset, bit));
    }
  }
  CHECK_NOTNULL(fout = fopen(g_outpath, "wb"));
  CHECK(bitabuilder_fwrite(bitset, fout));
  bitabuilder_free(&bitset);
  return fclose(fin) | fclose(fout);
}
