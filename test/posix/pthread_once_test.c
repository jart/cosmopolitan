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
#include <stdatomic.h>
#include <stdlib.h>

#define NUM_THREADS    2
#define NUM_ITERATIONS 256

int global_data;
atomic_int barrier;

void init_function(void) {
  global_data = 42;
}

void await(void) {
  --barrier;
  for (;;)
    if (!barrier)
      break;
}

void *thread_func(void *arg) {
  await();
  pthread_once_t *p_once = (pthread_once_t *)arg;
  pthread_once(p_once, init_function);
  if (global_data != 42)
    exit(5);
  return NULL;
}

int main(void) {
  for (int iter = 0; iter < NUM_ITERATIONS; iter++) {
    pthread_once_t once_control = PTHREAD_ONCE_INIT;
    pthread_t threads[NUM_THREADS];
    global_data = 0;
    barrier = NUM_THREADS;
    for (int i = 0; i < NUM_THREADS; i++)
      if (pthread_create(&threads[i], 0, thread_func, (void *)&once_control))
        exit(10);
    for (int i = 0; i < NUM_THREADS; i++)
      if (pthread_join(threads[i], 0))
        exit(11);
  }
}
