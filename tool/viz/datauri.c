#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/errno.h"
#include "libc/log/log.h"
#include "libc/mem/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/x/x.h"
#include "net/http/escape.h"
#include "net/http/http.h"
#include "third_party/getopt/getopt.internal.h"
#include "third_party/stb/stb_image.h"

#define USAGE \
  " [FLAGS] FILE...\n\
Utility for printing data:base64 URIs.\n\
\n\
FLAGS\n\
\n\
  -h      Help\n"

void PrintUsage(int rc, FILE *f) {
  fputs("Usage: ", f);
  fputs(program_invocation_name, f);
  fputs(USAGE, f);
  exit(rc);
}

void PrintUri(const char *path) {
  size_t n;
  void *img;
  if (!(img = gc(xslurp(path, &n)))) exit(2);
  fputs("data:", stdout);
  fputs(FindContentType(path, -1), stdout);
  fputs(";base64,", stdout);
  fputs(gc(EncodeBase64(img, n, 0)), stdout);
}

int main(int argc, char *argv[]) {
  int i;
  while ((i = getopt(argc, argv, "?h")) != -1) {
    switch (i) {
      case '?':
      case 'h':
        PrintUsage(0, stdout);
      default:
        PrintUsage(1, stderr);
    }
  }
  if (optind == argc) {
    PrintUsage(1, stderr);
  }
  for (i = optind; i < argc; ++i) {
    PrintUri(argv[i]);
  }
}
