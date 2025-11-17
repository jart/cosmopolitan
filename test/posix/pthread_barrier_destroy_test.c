// Copyright 2025 Justine Alexandra Roberts Tunney
//
// Permission to use, copy, modify, and/or distribute this software for
// any purpose with or without fee is hereby granted, provided that the
// above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

// test pthread_barrier_destroy() either waits for completion or returns
// EBUSY when there are still waiters waiting.

#define NTHREADS   4
#define ITERATIONS 100

void *worker(void *arg) {
  pthread_barrier_t *barrier = arg;
  pthread_barrier_wait(barrier);
  return 0;
}

int main() {
  pthread_barrier_t barrier;
  for (int i = 0; i < ITERATIONS; ++i) {
    if (pthread_barrier_init(&barrier, 0, NTHREADS + 1))
      exit(10);
    pthread_t threads[NTHREADS];
    for (int i = 0; i < NTHREADS; ++i)
      if (pthread_create(&threads[i], 0, worker, &barrier))
        exit(11);
    pthread_barrier_wait(&barrier);
    int res = pthread_barrier_destroy(&barrier);
    if (res && res != EBUSY)
      exit(14);
    if (!res)
      memset(&barrier, -1, sizeof(barrier));
    for (int i = 0; i < NTHREADS; ++i)
      if (pthread_join(threads[i], 0))
        exit(12);
    if (res == EBUSY)
      if (pthread_barrier_destroy(&barrier))
        exit(13);
  }
}
