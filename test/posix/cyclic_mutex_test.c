// Copyright 2024 Justine Alexandra Roberts Tunney
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

#include <cosmo.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>

pthread_mutex_t x;
pthread_mutex_t y;

void ignore_signal(int sig) {
}

int main(int argc, char *argv[]) {

#ifdef MODE_DBG
  GetSymbolTable();
  signal(SIGTRAP, ignore_signal);
  kprintf("running %s\n", argv[0]);
#endif

  pthread_mutexattr_t attr;
  if (pthread_mutexattr_init(&attr))
    return 1;
  if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK))
    return 2;
  if (pthread_mutex_init(&x, &attr))
    return 3;
  if (pthread_mutex_init(&y, &attr))
    return 4;
  if (pthread_mutexattr_destroy(&attr))
    return 5;

  if (pthread_mutex_lock(&x))
    return 6;
  if (pthread_mutex_lock(&y))
    return 7;
  if (pthread_mutex_unlock(&y))
    return 8;
  if (pthread_mutex_unlock(&x))
    return 9;

  if (pthread_mutex_lock(&y))
    return 10;
  if (pthread_mutex_lock(&y) != EDEADLK)
    return 11;
  if (pthread_mutex_lock(&x) != EDEADLK)
    return 12;
  if (pthread_mutex_unlock(&x) != EPERM)
    return 13;
  if (pthread_mutex_unlock(&y))
    return 14;

  if (pthread_mutex_destroy(&y))
    return 15;
  if (pthread_mutex_destroy(&x))
    return 16;
}
