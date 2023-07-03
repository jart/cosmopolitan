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
#include "libc/fmt/conv.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "third_party/getopt/getopt.internal.h"

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

wontreturn void PrintUsage(int rc, int fd) {
  tinyprint(fd, "USAGE\n\n  ", prog, USAGE, NULL);
  exit(rc);
}

int main(int argc, char *argv[]) {
  int i, mode = 0755;
  int (*mkdirp)(const char *, unsigned) = mkdir;

  prog = argv[0];
  if (!prog) prog = "mkdir";

  while ((i = getopt(argc, argv, "hpm:")) != -1) {
    switch (i) {
      case 'p':
        mkdirp = makedirs;
        break;
      case 'm':
        mode = strtol(optarg, 0, 8);
        break;
      case 'h':
        PrintUsage(0, 1);
      default:
        PrintUsage(1, 2);
    }
  }

  if (optind == argc) {
    tinyprint(2, prog, ": missing operand\n", NULL);
    exit(1);
  }

  for (i = optind; i < argc; ++i) {
    if (mkdirp(argv[i], mode) == -1) {
      perror(argv[i]);
      exit(1);
    }
  }

  return 0;
}
