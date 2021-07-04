#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/stdio/stdio.h"
#include "libc/x/x.h"
#include "third_party/stb/stb_image.h"

/**
 * @fileoverview Utility for printing HTML <img> tags.
 */

int main(int argc, char *argv[]) {
  void *p;
  size_t n;
  int yn, xn, cn;
  if (argc != 2) return 1;
  if (!(p = xslurp(argv[1], &n))) return 2;
  if (!(p = stbi_load_from_memory(p, n, &xn, &yn, &cn, 0))) return 3;
  printf("<img src=\"%s\" width=\"%d\" height=\"%d\"\n"
         "     alt=\"[%s]\">\n",
         argv[1], (xn + 1) >> 1, (yn + 1) >> 1, argv[1]);
  return 0;
}
