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
#include "libc/calls/hefty/copyfile.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/fmt.h"
#include "libc/runtime/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/ok.h"
#include "libc/x/x.h"
#include "third_party/getopt/getopt.h"

#define USAGE \
  " SRC... DST\n\
\n\
SYNOPSIS\n\
\n\
  Copies Files\n\
\n\
FLAGS\n\
\n\
  -?\n\
  -h      help\n\
  -f      force\n\
  -n      no clobber\n\
  -a      preserve all\n\
  -p      preserve owner and timestamps\n\
\n"

int flags;
bool force;

wontreturn void PrintUsage(int rc, FILE *f) {
  fprintf(f, "%s%s%s", "Usage: ", program_invocation_name, USAGE);
  exit(rc);
}

void GetOpts(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "?hfnap")) != -1) {
    switch (opt) {
      case 'f':
        force = true;
        break;
      case 'n':
        flags |= COPYFILE_NOCLOBBER;
        break;
      case 'a':
      case 'p':
        flags |= COPYFILE_PRESERVE_OWNER;
        flags |= COPYFILE_PRESERVE_TIMESTAMPS;
        break;
      case 'h':
      case '?':
        PrintUsage(EXIT_SUCCESS, stdout);
      default:
        PrintUsage(EX_USAGE, stderr);
    }
  }
}

int cp(const char *src, const char *dst) {
  if (endswith(dst, "/") || isdirectory(dst)) {
    dst = gc(xasprintf("%s/%s", dst, basename));
  }
  if (!force && access(dst, W_OK) == -1 && errno != ENOENT) goto OnFail;
  if (copyfile(src, dst, flags) == -1) goto OnFail;
  return 0;
OnFail:
  fprintf(stderr, "%s %s %s: %s\n", "error: cp", src, dst, strerror(errno));
  return 1;
}

int main(int argc, char *argv[]) {
  int i;
  GetOpts(argc, argv);
  if (argc - optind < 2) PrintUsage(EX_USAGE, stderr);
  for (i = optind; i < argc - 1; ++i) {
    if (cp(argv[i], argv[argc - 1]) == -1) {
      return -1;
    }
  }
  return 0;
}
