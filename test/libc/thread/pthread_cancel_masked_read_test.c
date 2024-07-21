#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

int pfds[2];
int got_cleanup;

void cleanup(void* arg) {
  got_cleanup = 1;
}

void* worker(void* arg) {
  char buf[8];
  if (pthread_setcancelstate(PTHREAD_CANCEL_MASKED, 0))
    _Exit(10);
  pthread_cleanup_push(cleanup, 0);
  if (read(pfds[0], buf, sizeof(buf)) != -1)
    _Exit(11);
  if (errno != ECANCELED)
    _Exit(12);
  pthread_cleanup_pop(0);
  return (void*)123;
}

int main(int argc, char* argv[]) {
  void* rc;
  pthread_t th;
  if (pipe(pfds))
    return 1;
  if (pthread_create(&th, 0, worker, 0))
    return 2;
  if (pthread_cancel(th))
    return 3;
  if (pthread_join(th, &rc))
    return 4;
  if (rc != (void*)123)
    return 5;
  if (got_cleanup)
    return 7;
}
