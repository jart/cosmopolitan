#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ex.h"
#include "libc/x/x.h"
#include "third_party/getopt/getopt.h"

#define USAGE \
  " [-p] [-m MODE] DIR...\n\
Utility for creating directories.\n\
\n\
FLAGS\n\
\n\
  -h       Help\n\
  -m MODE  Octal mode\n\
  -p       Make parent directories\n"

const char *prog;

wontreturn void PrintUsage(int rc, FILE *f) {
  fputs("Usage: ", f);
  fputs(prog, f);
  fputs(USAGE, f);
  exit(rc);
}

int main(int argc, char *argv[]) {
  int i, mode = 0755;
  int (*mkdirp)(const char *, unsigned) = mkdir;
  prog = argc > 0 ? argv[0] : "mkdir.com";

  while ((i = getopt(argc, argv, "?hpm:")) != -1) {
    switch (i) {
      case 'p':
        mkdirp = makedirs;
        break;
      case 'm':
        mode = strtol(optarg, 0, 8);
        break;
      case '?':
      case 'h':
        PrintUsage(0, stdout);
      default:
        PrintUsage(EX_USAGE, stderr);
    }
  }

  if (optind == argc) {
    fputs(prog, stderr);
    fputs(": missing argument\n", stderr);
    fputs("Try '", stderr);
    fputs(prog, stderr);
    fputs(" -h' for more information.\n", stderr);
    exit(1);
  }

  for (i = optind; i < argc; ++i) {
    if (mkdirp(argv[i], mode) == -1) {
      fputs(prog, stderr);
      fputs(": cannot create directory '", stderr);
      fputs(argv[i], stderr);
      fputs("' ", stderr);
      fputs(_strerdoc(errno), stderr);
      fputc('\n', stderr);
      exit(1);
    }
  }

  return 0;
}
