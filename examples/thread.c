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
#include "libc/intrin/kprintf.h"
#include "libc/log/log.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/thread/create.h"
#include "libc/thread/detach.h"
#include "libc/thread/join.h"
#include "libc/thread/self.h"
#include "libc/thread/sem.h"
#include "libc/time/time.h"

cthread_sem_t semaphore;
_Thread_local int test_tls = 0x12345678;

static void *worker(void *arg) {
  int tid;
  cthread_t self;
  cthread_sem_signal(&semaphore);
  self = cthread_self();
  tid = self->tid;
  printf("[%p] %d -> %#x\n", self, tid, test_tls);
  if (test_tls != 0x12345678) {
    printf(".tdata test #2 failed\n");
  }
  return (void *)4;
}

int main() {
  int rc, tid;
  void *exitcode;
  cthread_t self, thread;
  self = cthread_self();
  tid = self->tid;
  printf("[%p] %d -> %#x\n", self, tid, test_tls);
  if (test_tls != 0x12345678) {
    printf(".tdata test #1 failed\n");
  }
  cthread_sem_init(&semaphore, 0);
  rc = cthread_create(&thread, NULL, &worker, NULL);
  if (rc == 0) {
    cthread_sem_wait(&semaphore, 0, NULL);
    printf("thread created: %p\n", thread);
#if 1
    cthread_join(thread, &exitcode);
#else
    exitcode = cthread_detach(thread);
#endif
    cthread_sem_signal(&semaphore);
    cthread_sem_wait(&semaphore, 0, NULL);
    printf("thread joined: %p -> %p\n", thread, exitcode);
  } else {
    fprintf(stderr, "ERROR: thread could not be started: %d\n", rc);
  }
  return 0;
}
