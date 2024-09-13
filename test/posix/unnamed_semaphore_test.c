#include <pthread.h>
#include <semaphore.h>

#define THREADS    10
#define ITERATIONS 100000

int g_count;
sem_t g_sem;

void *worker(void *arg) {
  for (int i = 0; i < ITERATIONS; ++i) {
    if (sem_wait(&g_sem))
      exit(6);
    ++g_count;
    if (sem_post(&g_sem))
      exit(7);
  }
  return 0;
}

int main(int argc, char *argv[]) {
  pthread_t th[THREADS];
  if (sem_init(&g_sem, 0, 1))
    return 1;
  for (int i = 0; i < THREADS; ++i)
    if (pthread_create(&th[i], 0, worker, 0))
      return 2;
  for (int i = 0; i < THREADS; ++i)
    if (pthread_join(th[i], 0))
      return 3;
  if (g_count != THREADS * ITERATIONS)
    return 4;
  if (sem_destroy(&g_sem))
    return 5;
}
