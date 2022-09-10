#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/thread/thread.h"
#include "libc/stdio/stdio.h"

/**
 * @fileoverview Basic POSIX Threads Example.
 *
 *     $ make -j8 o//examples/thread.com
 *     $ o//examples/thread.com
 *     hi there
 *
 */

void *worker(void *arg) {
  fputs(arg, stdout);
  return "there\n";
}

int main() {
  void *result;
  pthread_t id;
  pthread_create(&id, 0, worker, "hi ");
  pthread_join(id, &result);
  fputs(result, stdout);
}
