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
#include "libc/thread/tls.h"

void *hog(void) {
  return &__get_tls()->tib_errno;
}

int main(int argc, char *argv[]) {
  printf("%zu\n", sizeof(struct CosmoTib));
}
