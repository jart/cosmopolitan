#include <pthread.h>
#include <stdio.h>

// how to spawn a thread

void *my_thread(void *arg) {
  printf("my_thread(%p) is running\n", arg);
  return (void *)0x456L;
}

int main(int argc, char *argv[]) {
  void *res;
  pthread_t th;
  pthread_create(&th, 0, my_thread, (void *)0x123L);
  pthread_join(th, &res);
  printf("my_thread() returned %p\n", res);
}
