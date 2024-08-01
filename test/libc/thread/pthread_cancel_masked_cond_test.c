#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

int got_cleanup;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mu = PTHREAD_MUTEX_INITIALIZER;

void cleanup(void* arg) {
  got_cleanup = 1;
}

void* worker(void* arg) {
  if (pthread_setcancelstate(PTHREAD_CANCEL_MASKED, 0))
    _Exit(10);
  pthread_cleanup_push(cleanup, 0);
  if (pthread_mutex_lock(&mu))
    _Exit(11);
  if (pthread_cond_wait(&cv, &mu) != ECANCELED)
    _Exit(12);
  if (pthread_mutex_trylock(&mu) != EBUSY)
    _Exit(13);
  if (pthread_mutex_unlock(&mu))
    _Exit(14);
  pthread_cleanup_pop(0);
  return (void*)123;
}

int main(int argc, char* argv[]) {
  void* rc;
  pthread_t th;
  if (pthread_create(&th, 0, worker, 0))
    return 2;
  if (pthread_cancel(th))
    return 3;
  if (pthread_join(th, &rc))
    return 4;
  if (rc != (void*)123)
    return 5;
  if (got_cleanup)
    return 6;
}
