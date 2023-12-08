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
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/limits.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/ok.h"
#include "third_party/getopt/getopt.internal.h"
#include "third_party/zlib/zlib.h"

#define USAGE \
  " PATH...\n\
\n\
SYNOPSIS\n\
\n\
  Compress Files\n\
\n\
FLAGS\n\
\n\
  -?\n\
  -h    help\n\
  -f    force\n\
  -c    use stdout\n\
  -d    decompress\n\
  -A    append mode\n\
  -x    exclusive mode\n\
  -k    keep input file\n\
  -0    disable compression\n\
  -1    fastest compression\n\
  -4    coolest compression\n\
  -9    maximum compression\n\
  -a    ascii mode (ignored)\n\
  -F    fixed strategy (advanced)\n\
  -L    filtered strategy (advanced)\n\
  -R    run length strategy (advanced)\n\
  -H    huffman only strategy (advanced)\n\
\n"

bool opt_keep;
bool opt_force;
char opt_level;
bool opt_append;
char opt_strategy;
bool opt_exclusive;
bool opt_usestdout;
bool opt_decompress;

const char *prog;
char databuf[32768];
char pathbuf[PATH_MAX];

wontreturn void PrintUsage(int rc, FILE *f) {
  fputs("usage: ", f);
  fputs(prog, f);
  fputs(USAGE, f);
  exit(rc);
}

void GetOpts(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "?hfcdakxALFRHF0123456789")) != -1) {
    switch (opt) {
      case 'k':
        opt_keep = true;
        break;
      case 'f':
        opt_force = true;
        break;
      case 'A':
        opt_append = true;
        break;
      case 'c':
        opt_usestdout = true;
        break;
      case 'x':
        opt_exclusive = true;
        break;
      case 'd':
        opt_decompress = true;
        break;
      case 'F':
        opt_strategy = 'F';  // Z_FIXED
        break;
      case 'L':
        opt_strategy = 'f';  // Z_FILTERED
        break;
      case 'R':
        opt_strategy = 'R';  // Z_RLE
        break;
      case 'H':
        opt_strategy = 'h';  // Z_HUFFMAN_ONLY
        break;
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        opt_level = opt;
        break;
      case 'h':
      case '?':
        PrintUsage(EXIT_SUCCESS, stdout);
      default:
        PrintUsage(EX_USAGE, stderr);
    }
  }
}

void Compress(const char *inpath) {
  FILE *input;
  gzFile output;
  int rc, errnum;
  const char *outpath;
  char *p, openflags[5];
  if ((!inpath || opt_usestdout) && (!isatty(1) || opt_force)) {
    opt_usestdout = true;
  } else {
    fputs(prog, stderr);
    fputs(": compressed data not written to a terminal."
          " Use -f to force compression.\n",
          stderr);
    exit(1);
  }
  if (inpath) {
    input = fopen(inpath, "rb");
  } else {
    inpath = "/dev/stdin";
    input = stdin;
  }
  p = openflags;
  *p++ = opt_append ? 'a' : 'w';
  *p++ = 'b';
  if (opt_exclusive) *p++ = 'x';
  if (opt_level) *p++ = opt_level;
  if (opt_strategy) *p++ = opt_strategy;
  *p = 0;
  if (opt_usestdout) {
    outpath = "/dev/stdout";
    output = gzdopen(1, openflags);
  } else {
    if (strlen(inpath) + 3 + 1 > PATH_MAX) _Exit(2);
    stpcpy(stpcpy(pathbuf, inpath), ".gz");
    outpath = pathbuf;
    output = gzopen(outpath, openflags);
  }
  if (!output) {
    fputs(outpath, stderr);
    fputs(": gzopen() failed\n", stderr);
    fputs(_strerdoc(errno), stderr);
    fputs("\n", stderr);
    exit(1);
  }
  do {
    rc = fread(databuf, 1, sizeof(databuf), input);
    if (rc == -1) {
      errnum = 0;
      fputs(inpath, stderr);
      fputs(": read failed: ", stderr);
      fputs(_strerdoc(ferror(input)), stderr);
      fputs("\n", stderr);
      _Exit(1);
    }
    if (!gzwrite(output, databuf, rc)) {
      fputs(outpath, stderr);
      fputs(": gzwrite failed: ", stderr);
      fputs(gzerror(output, &errnum), stderr);
      fputs("\n", stderr);
      _Exit(1);
    }
  } while (rc == sizeof(databuf));
  if (input != stdin) {
    if (fclose(input)) {
      fputs(inpath, stderr);
      fputs(": close failed\n", stderr);
      _Exit(1);
    }
  }
  if (gzclose(output)) {
    fputs(outpath, stderr);
    fputs(": gzclose failed\n", stderr);
    _Exit(1);
  }
  if (!opt_keep && !opt_usestdout && (opt_force || !access(inpath, W_OK))) {
    unlink(inpath);
  }
}

void Decompress(const char *inpath) {
  FILE *output;
  gzFile input;
  int rc, n, errnum;
  const char *outpath;
  outpath = 0;
  if (inpath) {
    input = gzopen(inpath, "rb");
  } else {
    opt_usestdout = true;
    inpath = "/dev/stdin";
    input = gzdopen(0, "rb");
  }
  if (!input) {
    fputs(inpath, stderr);
    fputs(": gzopen() failed\n", stderr);
    fputs(_strerdoc(errno), stderr);
    fputs("\n", stderr);
    exit(1);
  }
  if (opt_usestdout) {
    output = stdout;
    outpath = "/dev/stdout";
  } else if (endswith(inpath, ".gz")) {
    n = strlen(inpath);
    if (n - 3 + 1 > PATH_MAX) _Exit(2);
    memcpy(pathbuf, inpath, n - 3);
    pathbuf[n - 3] = 0;
    outpath = pathbuf;
    if (!(output = fopen(outpath, opt_append ? "wa" : "wb"))) {
      fputs(outpath, stderr);
      fputs(": open failed: ", stderr);
      fputs(_strerdoc(errno), stderr);
      fputs("\n", stderr);
      _Exit(1);
    }
  } else {
    fputs(inpath, stderr);
    fputs(": needs to end with .gz unless -c is passed\n", stderr);
    _Exit(1);
  }
  do {
    rc = gzread(input, databuf, sizeof(databuf));
    if (rc == -1) {
      errnum = 0;
      fputs(inpath, stderr);
      fputs(": gzread failed: ", stderr);
      fputs(gzerror(input, &errnum), stderr);
      fputs("\n", stderr);
      _Exit(1);
    }
    if (fwrite(databuf, rc, 1, output) != 1) {
      fputs(outpath, stderr);
      fputs(": write failed: ", stderr);
      fputs(_strerdoc(ferror(output)), stderr);
      fputs("\n", stderr);
      _Exit(1);
    }
  } while (rc == sizeof(databuf));
  if (gzclose(input)) {
    fputs(inpath, stderr);
    fputs(": gzclose failed\n", stderr);
    _Exit(1);
  }
  if (output != stdout) {
    if (fclose(output)) {
      fputs(outpath, stderr);
      fputs(": close failed\n", stderr);
      _Exit(1);
    }
  }
  if (!opt_keep && !opt_usestdout && (opt_force || !access(inpath, W_OK))) {
    unlink(inpath);
  }
}

int main(int argc, char *argv[]) {
  int i;
  prog = argv[0];
  if (!prog) prog = "gzip";
  GetOpts(argc, argv);
  if (opt_decompress) {
    if (optind == argc) {
      Decompress(0);
    } else {
      for (i = optind; i < argc; ++i) {
        Decompress(argv[i]);
      }
    }
  } else {
    if (optind == argc) {
      Compress(0);
    } else {
      for (i = optind; i < argc; ++i) {
        Compress(argv[i]);
      }
    }
  }
  return 0;
}
