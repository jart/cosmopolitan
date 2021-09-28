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
#include "libc/thread/create.h"
#include "libc/time/time.h"

int worker(void* arg) {
  (void)arg;
  return 0;
}

int main() {
  cthread_t thread;
  int rc = cthread_create(&thread, NULL, &worker, NULL);
  if (rc == 0) {
    printf("thread created: %p\n", thread);
    sleep(1000);
  } else {
    printf("ERROR: thread could not be started: %d\n", rc);
  }
  return 0;
}
