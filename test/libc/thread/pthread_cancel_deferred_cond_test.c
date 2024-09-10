#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "libc/stdio/stdio.h"

int got_cleanup;
pthread_cond_t cv;
pthread_mutex_t mu;

void cleanup(void* arg) {
  got_cleanup = 1;
}

void* worker(void* arg) {
  pthread_cleanup_push(cleanup, 0);
  if (pthread_mutex_lock(&mu))
    _Exit(11);
  pthread_cond_wait(&cv, &mu);
  _Exit(12);
  pthread_cleanup_pop(0);
}

int main(int argc, char* argv[]) {
  void* rc;
  pthread_t th;
  pthread_mutexattr_t at;
  pthread_mutexattr_init(&at);
  pthread_mutexattr_settype(&at, PTHREAD_MUTEX_NORMAL);
  pthread_mutex_init(&mu, &at);
  pthread_mutexattr_destroy(&at);
  pthread_cond_init(&cv, 0);
  if (pthread_create(&th, 0, worker, 0))
    return 2;
  if (pthread_cancel(th))
    return 3;
  if (pthread_join(th, &rc))
    return 4;
  if (rc != PTHREAD_CANCELED)
    return 5;
  if (!got_cleanup)
    return 6;
  if (pthread_mutex_trylock(&mu) != EBUSY)
    return 7;
  if (pthread_mutex_unlock(&mu))
    return 8;
  pthread_mutex_destroy(&mu);
  pthread_cond_destroy(&cv);
}
