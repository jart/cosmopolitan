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
#include "libc/calls/struct/iovec.h"
#include "libc/errno.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

int main(int argc, char *argv[]) {
  char buf[65536];
  memset(buf, '\n', sizeof(buf));
  for (;;) {
    ssize_t rc = writev(1, &(struct iovec){buf, sizeof(buf)}, 1);
    if (rc != sizeof(buf)) {
      printf("got %ld (%s)\n", rc, strerror(errno));
      return 1;
    }
  }
}
