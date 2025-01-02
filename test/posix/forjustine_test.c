// for justine with love 2025-01-02
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static bool altstack_installed;

static void* chump(void* v) {
  stack_t* s = v;
  if (sigaltstack(s, NULL)) {
    pthread_mutex_lock(&lock);
    altstack_installed = true;
    pthread_mutex_unlock(&lock);
    pthread_cond_signal(&cond);
    return NULL;
  }
  pthread_mutex_lock(&lock);
  altstack_installed = true;
  pthread_cond_signal(&cond);
  pthread_mutex_unlock(&lock);
  while (1)
    poll(NULL, 0, -1);
  return NULL;
}

int main(void) {
  void* v;
  stack_t s = {.ss_size = sysconf(_SC_SIGSTKSZ)};
  s.ss_sp = malloc(s.ss_size);
  if (s.ss_sp == NULL)
    return EXIT_FAILURE;
  pthread_t tid;
  if (pthread_create(&tid, NULL, chump, &s))
    return EXIT_FAILURE;
  pthread_mutex_lock(&lock);
  while (!altstack_installed)
    pthread_cond_wait(&cond, &lock);
  pthread_mutex_unlock(&lock);
  free(s.ss_sp);
  if (pthread_cancel(tid) || pthread_join(tid, &v))
    return EXIT_FAILURE;
  return v == PTHREAD_CANCELED ? EXIT_SUCCESS : EXIT_FAILURE;
}
