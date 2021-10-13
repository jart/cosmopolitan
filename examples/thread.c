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
#include "libc/thread/self.h"
#include "libc/thread/detach.h"
#include "libc/thread/join.h"
#include "libc/thread/nativesem.h"
#include "libc/time/time.h"

cthread_native_sem_t semaphore;

int worker(void* arg) {
  cthread_native_sem_signal(&semaphore);
  
  cthread_t self = cthread_self();
  int tid = self->tid;
  sleep(1);
  //sleep(10000);
  //printf("[%p] %d\n", self, tid);
  (void)arg;
  return 4;
}

int main() {
  cthread_native_sem_init(&semaphore, 0);
  
  cthread_t thread;
  int rc = cthread_create(&thread, NULL, &worker, NULL);
  if (rc == 0) {
    cthread_native_sem_wait(&semaphore, 0, 0, NULL);
    //printf("thread created: %p\n", thread);
    sleep(1);
#if 1
    cthread_join(thread, &rc);
#else
    rc = cthread_detach(thread);
    sleep(2);
#endif
    cthread_native_sem_signal(&semaphore);
    cthread_native_sem_wait(&semaphore, 0, 0, NULL);
    //printf("thread joined: %p -> %d\n", thread, rc);
  } else {
    printf("ERROR: thread could not be started: %d\n", rc);
  }
  return 0;
}
