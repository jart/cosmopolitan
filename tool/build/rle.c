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
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/log/check.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/errfuns.h"
#include "third_party/getopt/getopt.h"

#define USAGE1 \
  "NAME\n\
\n\
  rle - Run Length Encoder\n\
\n\
SYNOPSIS\n\
\n\
  "

#define USAGE2 \
  " [FLAGS] [FILE...]\n\
\n\
DESCRIPTION\n\
\n\
  This is a primitive compression algorithm. Its advantage is that\n\
  the concomitant rldecode() library is seventeen bytes, and works\n\
  on IA-16, IA-32, and NexGen32e without needing to be recompiled.\n\
\n\
  This CLI is consistent with gzip, bzip2, lzma, etc.\n\
\n\
FLAGS\n\
\n\
  -1 .. -9   ignored\n\
  -a         ignored\n\
  -c         send to stdout\n\
  -d         decompress\n\
  -f         ignored\n\
  -t         test integrity\n\
  -S SUFFIX  overrides .rle extension\n\
  -h         shows this information\n"

FILE *fin_, *fout_;
bool decompress_, test_;
const char *suffix_, *hint_;

void StartErrorMessage(void) {
  fputs("error: ", stderr);
  fputs(hint_, stderr);
  fputs(": ", stderr);
}

void PrintIoErrorMessage(void) {
  int err;
  err = errno;
  StartErrorMessage();
  fputs(strerror(err), stderr);
  fputc('\n', stderr);
}

void PrintUsage(int rc, FILE *f) {
  fputs(USAGE1, f);
  fputs(program_invocation_name, f);
  fputs(USAGE2, f);
  exit(rc);
}

void GetOpts(int argc, char *argv[]) {
  int opt;
  fin_ = stdin;
  suffix_ = ".rle";
  while ((opt = getopt(argc, argv, "123456789S:acdfho:t")) != -1) {
    switch (opt) {
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case 'a':
      case 'f':
        break;
      case 'c':
        fout_ = stdout;
        break;
      case 'd':
        decompress_ = true;
        break;
      case 't':
        test_ = true;
        break;
      case 'o':
        fclose_s(&fout_);
        if (!(fout_ = fopen((hint_ = optarg), "w"))) {
          PrintIoErrorMessage();
          exit(1);
        }
        break;
      case 'S':
        suffix_ = optarg;
        break;
      case 'h':
      case '?':
        PrintUsage(EXIT_SUCCESS, stdout);
      default:
        PrintUsage(EX_USAGE, stderr);
    }
  }
}

int RunLengthEncode1(void) {
  int byte1, byte2, runlength;
  byte2 = -1;
  runlength = 0;
  if ((byte1 = fgetc(fin_)) == -1) return -1;
  do {
    while (++runlength < 255) {
      if ((byte2 = fgetc(fin_)) != byte1) break;
    }
    if (fputc(runlength, fout_) == -1 || fputc(byte1, fout_) == -1) {
      return -1;
    }
    runlength = 0;
  } while ((byte1 = byte2) != -1);
  return feof(fin_) ? 0 : -1;
}

int RunLengthEncode2(void) { return fputc(0, fout_) | fputc(0, fout_); }

int EmitRun(unsigned char count, unsigned char byte) {
  do {
    if (fputc(byte, fout_) == -1) return -1;
  } while (--count);
  return 0;
}

int RunLengthDecode(void) {
  int byte1, byte2;
  if ((byte1 = fgetc(fin_)) == -1) return einval();
  if ((byte2 = fgetc(fin_)) == -1) return einval();
  while (byte1) {
    if (!test_ && EmitRun(byte1, byte2) == -1) return -1;
    if ((byte1 = fgetc(fin_)) == -1) break;
    if ((byte2 = fgetc(fin_)) == -1) return einval();
  }
  if (byte1 != 0 || byte2 != 0) return einval();
  fgetc(fin_);
  return feof(fin_) ? 0 : -1;
}

int RunLengthCode(void) {
  if (test_ || decompress_) {
    return RunLengthDecode();
  } else {
    return RunLengthEncode1();
  }
}

int Run(char **paths, size_t count) {
  int rc;
  char *p;
  size_t i, suffixlen;
  const char pathbuf[PATH_MAX];
  if (!count) {
    hint_ = "/dev/stdin";
    if (!fout_) fout_ = stdout;
    rc = RunLengthCode();
    rc |= fclose_s(&fin_);
  } else {
    rc = fclose_s(&fin_);
    for (i = 0; i < count && rc != -1; ++i) {
      rc = -1;
      if ((fin_ = fopen((hint_ = paths[i]), "r"))) {
        if (test_ || fout_) {
          rc = RunLengthCode();
        } else {
          suffixlen = strlen(suffix_);
          if (!IsTrustworthy() && strlen(paths[i]) + suffixlen + 1 > PATH_MAX) {
            return eoverflow();
          }
          p = stpcpy(pathbuf, paths[i]);
          if (!decompress_) {
            strcpy(p, suffix_);
          } else if (p - pathbuf > suffixlen &&
                     memcmp(p - suffixlen, suffix_, suffixlen) == 0) {
            p[-suffixlen] = '\0';
          } else {
            return enotsup();
          }
          if ((fout_ = fopen((hint_ = pathbuf), "w"))) {
            rc = RunLengthCode();
            if (rc != -1 && !decompress_) {
              rc = RunLengthEncode2();
            }
            if ((rc |= fclose_s(&fout_)) != -1) {
              unlink(paths[i]);
            }
          }
        }
        rc |= fclose_s(&fin_);
      }
    }
  }
  if (rc != -1 && fout_) {
    rc = RunLengthEncode2();
    rc |= fclose_s(&fout_);
  }
  return rc;
}

int main(int argc, char *argv[]) {
  GetOpts(argc, argv);
  if (Run(argv + optind, argc - optind) != -1) {
    return EXIT_SUCCESS;
  } else {
    PrintIoErrorMessage();
    return EXIT_FAILURE;
  }
}
