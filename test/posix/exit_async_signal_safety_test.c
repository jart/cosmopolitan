#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>

atomic_bool ready;

void* work(void* arg) {
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0);
  ready = true;
  pthread_exit(0);
}

int main() {

  for (int i = 0; i < 1000; ++i) {
    pthread_t th;
    if (pthread_create(&th, 0, work, 0))
      _Exit(1);
    for (;;)
      if (ready)
        break;
    pthread_cancel(th);
    if (pthread_join(th, 0))
      _Exit(3);
  }

  while (!pthread_orphan_np())
    pthread_decimate_np();
}
