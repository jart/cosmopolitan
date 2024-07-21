#include <limits.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>

sigset_t parent_mask;
sigset_t child_mask;

void* worker(void* arg) {
  if (pthread_sigmask(SIG_SETMASK, 0, &child_mask))
    _Exit(1);
  return 0;
}

int main(int argc, char* argv[]) {
  pthread_t th;
  sigemptyset(&parent_mask);
  sigaddset(&parent_mask, SIGSYS);
  sigaddset(&parent_mask, SIGUSR2);
  sigaddset(&parent_mask, SIGWINCH);
  if (pthread_sigmask(SIG_SETMASK, &parent_mask, 0))
    return 1;
  if (pthread_create(&th, 0, worker, 0))
    return 2;
  if (pthread_join(th, 0))
    return 3;
  for (int i = 1; i <= _NSIG; ++i)
    if (sigismember(&parent_mask, i) != sigismember(&child_mask, i))
      return 4;
}
