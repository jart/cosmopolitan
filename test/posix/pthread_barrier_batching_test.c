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

#include <pthread.h>
#include <stdlib.h>

// posix requires barriers reset themselves automatically once the count
// has been satisfied. so test that barrier can be popped multiple times
// in quick succession without deadlocking.

#define NTHREADS 2
#define ROUNDS   10

long rounder[NTHREADS];
pthread_barrier_t barrier;

void *worker(void *arg) {
  long id = (long)arg;
  for (long round = 1; round <= ROUNDS; ++round) {
    rounder[id] = round;
    pthread_barrier_wait(&barrier);
    for (long j = 0; j < NTHREADS; ++j)
      if (rounder[j] < round)
        exit(20);
  }
  return 0;
}

int main() {
  if (pthread_barrier_init(&barrier, 0, NTHREADS))
    exit(10);
  pthread_t threads[NTHREADS];
  for (long i = 0; i < NTHREADS; ++i)
    if (pthread_create(&threads[i], 0, worker, (void *)i))
      exit(11);
  for (long i = 0; i < NTHREADS; ++i)
    if (pthread_join(threads[i], 0))
      exit(12);
  if (pthread_barrier_destroy(&barrier))
    exit(13);
}
