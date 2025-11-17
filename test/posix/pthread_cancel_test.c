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
#include <unistd.h>

volatile int thread_started = 0;
volatile int cleanup_called = 0;

void cleanup_handler(void *arg) {
  cleanup_called = 1;
}

void *cancellable_thread(void *arg) {
  thread_started = 1;

  pthread_cleanup_push(cleanup_handler, NULL);

  while (1) {
    pthread_testcancel();
    usleep(1000);
  }

  pthread_cleanup_pop(0);
  return NULL;
}

int main() {
  pthread_t thread;
  void *result;

  if (pthread_create(&thread, NULL, cancellable_thread, NULL) != 0)
    return 1;

  while (!thread_started)
    usleep(100);

  usleep(5000);

  if (pthread_cancel(thread) != 0)
    return 2;

  if (pthread_join(thread, &result) != 0)
    return 3;

  if (result != PTHREAD_CANCELED)
    return 4;

  if (!cleanup_called)
    return 5;

  return 0;
}