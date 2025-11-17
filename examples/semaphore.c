#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>

#define PHILOSOPHERS 9  // number of hungry philosophers
#define THOUGHTS     3  // number of breaks for thinking
#define SEATS        3  // number of seats at the tables

atomic_int occupied;
sem_t sem;

void *philosopher(void *arg) {
  int tid = (long)arg;

  printf("philosopher#%d arrives\n", tid);

  for (int i = 0; i < THOUGHTS; ++i) {

    sem_wait(&sem);
    printf("philosopher#%d sat down to eat (%d occupied)\n", tid, ++occupied);
    usleep(rand() % 1000000);

    printf("philosopher#%d gets up to think (%d occupied)\n", tid, --occupied);
    sem_post(&sem);
    usleep(rand() % 1000000);
  }

  printf("philosopher#%d leaves\n", tid);
  return 0;
}

int main(int argc, char *argv[]) {

  printf("inviting %d philosophers to %d person dinner\n", PHILOSOPHERS, SEATS);
  sem_init(&sem, 0, SEATS);

  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  for (long i = 0; i < PHILOSOPHERS; ++i) {
    pthread_t th;
    pthread_create(&th, &attr, philosopher, (void *)i);
  }

  pthread_exit(0);
}
