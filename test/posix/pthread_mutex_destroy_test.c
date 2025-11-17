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

int main() {
  pthread_mutex_t mutex1, mutex2, static_mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutexattr_t attr;

  if (pthread_mutex_init(&mutex1, NULL) != 0)
    return 1;

  if (pthread_mutex_lock(&mutex1) != 0)
    return 2;

  if (pthread_mutex_unlock(&mutex1) != 0)
    return 3;

  if (pthread_mutex_destroy(&mutex1) != 0)
    return 4;

  if (pthread_mutex_lock(&static_mutex) != 0)
    return 5;

  if (pthread_mutex_unlock(&static_mutex) != 0)
    return 6;

  if (pthread_mutex_destroy(&static_mutex) != 0)
    return 7;

  if (pthread_mutexattr_init(&attr) != 0)
    return 8;

  if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) != 0)
    return 9;

  if (pthread_mutex_init(&mutex2, &attr) != 0)
    return 10;

  if (pthread_mutex_lock(&mutex2) != 0)
    return 11;

  if (pthread_mutex_lock(&mutex2) != 0)
    return 12;

  if (pthread_mutex_unlock(&mutex2) != 0)
    return 13;

  if (pthread_mutex_unlock(&mutex2) != 0)
    return 14;

  if (pthread_mutex_destroy(&mutex2) != 0)
    return 15;

  if (pthread_mutexattr_destroy(&attr) != 0)
    return 16;

  return 0;
}