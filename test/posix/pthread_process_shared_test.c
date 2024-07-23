#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  int ws;
  FILE *f;
  pid_t pid;
  pthread_condattr_t ca;
  pthread_mutexattr_t ma;
  struct Shared {
    atomic_int state;
    pthread_cond_t cond;
    pthread_mutex_t lock;
  } *s;
  if (!(f = tmpfile()))
    return 0;
  if (ftruncate(fileno(f), 512))
    return 1;
  if ((s = (struct Shared *)mmap(0, 512, PROT_READ | PROT_WRITE,
                                 MAP_SHARED | MAP_ANONYMOUS, -1, 0)) ==
      MAP_FAILED)
    return 2;
  if (pthread_condattr_init(&ca))
    return 3;
  if (pthread_mutexattr_init(&ma))
    return 4;
  if (pthread_condattr_setpshared(&ca, PTHREAD_PROCESS_SHARED))
    return 5;
  if (pthread_mutexattr_setpshared(&ma, PTHREAD_PROCESS_SHARED))
    return 6;
  if (pthread_cond_init(&s->cond, &ca))
    return 7;
  if (pthread_mutex_init(&s->lock, &ma))
    return 8;
  if (pthread_mutexattr_destroy(&ma))
    return 9;
  if (pthread_condattr_destroy(&ca))
    return 10;
  if ((pid = fork()) == -1)
    return 11;
  if (!pid) {
    alarm(2);
    if (pthread_mutex_lock(&s->lock))
      return 12;
    s->state = 1;
    if (pthread_cond_wait(&s->cond, &s->lock))
      return 13;
    if (pthread_mutex_unlock(&s->lock))
      return 14;
    for (;;)
      if (s->state == 2)
        break;
    if (pthread_mutex_lock(&s->lock))
      return 15;
    if (pthread_cond_signal(&s->cond))
      return 16;
    if (pthread_mutex_unlock(&s->lock))
      return 17;
    _exit(0);
  }
  alarm(2);
  for (;;)
    if (s->state == 1)
      break;
  if (pthread_mutex_lock(&s->lock))
    return 18;
  if (pthread_cond_signal(&s->cond))
    return 19;
  if (pthread_mutex_unlock(&s->lock))
    return 20;
  if (pthread_mutex_lock(&s->lock))
    return 21;
  s->state = 2;
  if (pthread_cond_wait(&s->cond, &s->lock))
    return 22;
  if (pthread_mutex_unlock(&s->lock))
    return 23;
  if (wait(&ws) != pid)
    return 24;
  if (!WIFEXITED(ws))
    return 25;
  if (WEXITSTATUS(ws))
    return 26;
  if (pthread_mutex_destroy(&s->lock))
    return 27;
  if (pthread_cond_destroy(&s->cond))
    return 28;
  return 0;
}
