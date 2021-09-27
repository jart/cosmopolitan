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
#include "libc/thread/thread.h"

int worker(void* arg) {
  (void)arg;
  return 0;
}

int main() {
  thread_descriptor_t* thread = allocate_thread();
  
  if (thread) {
    long rc = start_thread(thread, &worker, NULL);
    printf("thread created: %ld\n", rc);
  } else {
    printf("ERROR: thread stack could not be allocated\n");
  }
  return 0;
}
