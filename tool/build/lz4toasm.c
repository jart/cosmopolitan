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
#include "libc/fmt/conv.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/kompressor.h"
#include "libc/nexgen32e/lz4.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/str/tab.internal.h"
#include "libc/x/x.h"
#include "third_party/getopt/getopt.internal.h"

/**
 * @fileoverview LZ4 content embedder.
 *
 * This tool converts an LZ4-compressed file into assembly source code,
 * that will appear to C/C++ code as a global constant holding the
 * uncompressed contents, which is unpacked automatically.
 *
 * @note this is a build tool that assumes input data is trustworthy
 */

#define COLS 8

struct stat st_;
size_t extractedsize;

wontreturn void usage(char *argv[], FILE *f, int rc) {
  fprintf(f, "%s: %s [-o %s] [-s %s] %s\n", "Usage", argv[0], "PATH", "SYMBOL",
          "FILE");
  exit(rc);
}

int main(int argc, char *argv[]) {
  const char *symbol = "kData";
  const char *lz4path = "/dev/stdin";
  const char *outpath = "/dev/stdout";
  const char *initprio = "400,_init_";
  unsigned char *lz4data;
  int opt;
  FILE *fin, *fout;

  ShowCrashReports();

  while ((opt = getopt(argc, argv, "ho:s:z:")) != -1) {
    switch (opt) {
      case 's':
        symbol = optarg;
        break;
      case 'o':
        outpath = optarg;
        break;
      case 'z':
        extractedsize = strtoul(optarg, NULL, 0);
        break;
      case 'h':
      case '?':
        usage(argv, stdout, 0);
      default:
        usage(argv, stderr, 1);
    }
  }
  if (argc - optind) {
    lz4path = argv[optind];
  }

  CHECK_NOTNULL((fin = fopen(lz4path, "r")));
  CHECK_NE(-1, fstat(fileno(fin), &st_));
  CHECK_NOTNULL((lz4data = malloc(st_.st_size)));
  CHECK_EQ(1, fread(lz4data, st_.st_size, 1, fin));
  CHECK_NE(-1, fclose(fin));

  CHECK_NOTNULL((fout = fopen(outpath, "w")));

  CHECK_EQ(LZ4_MAGICNUMBER, LZ4_MAGIC(lz4data));
  CHECK_EQ(1, LZ4_FRAME_VERSION(lz4data));

  const unsigned char *frame = /* lz4check( */ lz4data /* ) */;
  const unsigned char *block1 = frame + LZ4_FRAME_HEADERSIZE(frame);
  const unsigned char *block2 = block1 + LZ4_BLOCK_SIZE(frame, block1);
  CHECK(LZ4_BLOCK_ISCOMPRESSED(block1));
  CHECK(LZ4_BLOCK_ISEOF(block2));
  const unsigned char *data = LZ4_BLOCK_DATA(block1);
  size_t size = LZ4_BLOCK_DATASIZE(block1);

  fprintf(fout,
          "/\t%s -o %s -s %s %s\n"
          "#include \"libc/macros.internal.h\"\n"
          "\n",
          argv[0], outpath, symbol, lz4path);

  if (!extractedsize) {
    if (LZ4_FRAME_BLOCKCONTENTSIZEFLAG(frame)) {
      extractedsize = LZ4_FRAME_BLOCKCONTENTSIZE(frame);
    } else {
      extractedsize = lz4len(data, size);
    }
  }

  size_t bss = ROUNDUP(extractedsize, 8);
  size_t misalign = bss - extractedsize;

  fprintf(fout, "\t.rodata\n");
  fprintf(fout, "\t.align\t4\n");
  fprintf(fout, "%sLen:\n", symbol);
  fprintf(fout, "\t.long\t%lu\n", extractedsize);
  fprintf(fout, "\t.endobj\t%sLen,globl,hidden\n", symbol);
  fprintf(fout, "\t.previous\n");

  fprintf(fout, "\n");
  fprintf(fout, "\t.initbss %s%s\n", initprio, symbol);
  fprintf(fout, "%s:\n", symbol);
  fprintf(fout, "\t.zero\t%lu\n", bss);
  fprintf(fout, "\t.endobj\t%s,globl,hidden\n", symbol);
  fprintf(fout, "\t.previous\n");

  fprintf(fout, "\n");
  fprintf(fout, "\t.init.start %s%s\n", initprio, symbol);
  fprintf(fout, "\tpush\t%%rsi\n");
  fprintf(fout, "\tmov\t$%zu,%%edx\n", size);
  fprintf(fout, "\tcall\tlz4cpy\n");
  if (misalign) {
    fprintf(fout, "\tlea\t%zu(%%rax),%%rdi\n", misalign);
  } else {
    fprintf(fout, "\tmov\t%%rax,%%rdi\n");
  }
  fprintf(fout, "\tpop\t%%rsi\n");
  fprintf(fout, "\tadd\t$%zu,%%rsi\n", ROUNDUP(size, 8));
  fprintf(fout, "\t.init.end %s%s\n", initprio, symbol);

  fprintf(fout, "\n");
  fprintf(fout, "\t.initro %s%s\n", initprio, symbol);
  fprintf(fout, "%sLz4:\n", symbol);
  int col = 0;
  char16_t glyphs[COLS + 1];
  for (unsigned i = 0; i < size; ++i) {
    unsigned char ch = data[i];
    if (col == 0) {
      fprintf(fout, "\t.byte\t");
      bzero(glyphs, sizeof(glyphs));
    }
    /* TODO(jart): Fix Emacs */
    glyphs[col] = kCp437[ch == '"' || ch == '#' ? '.' : ch];
    if (col) fputc(',', fout);
    fprintf(fout, "0x%02x", ch);
    if (++col == COLS) {
      col = 0;
      fprintf(fout, "\t#%hs\n", glyphs);
    }
  }
  while (col++ != COLS) {
    fprintf(fout, ",0x00");
  }
  fprintf(fout, "\n");
  fprintf(fout, "\t.endobj\t%sLz4,globl,hidden\n", symbol);
  fprintf(fout, "\t.previous\n");

  return fclose(fout);
}
