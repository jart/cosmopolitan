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

#include <errno.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * @fileoverview Tests that EAGAIN won't corrupt pipe.
 *
 * This is a real bug when using CancelIoEx() on winsock writes, so we
 * need to make sure it doesn't happen on pipes too.
 */

#define ITERATIONS 100000
#define ASYMMETRY  3

int fds[2];
int got_read_eagains;
int got_write_eagains;

void *worker(void *arg) {
  for (int expect = 0; expect < ITERATIONS;) {
    int number;
    ssize_t rc = read(fds[0], &number, sizeof(number));
    if (rc == -1) {
      if (errno == EAGAIN) {
        ++got_read_eagains;
        if (poll(&(struct pollfd){fds[0], POLLIN}, 1, -1) == -1)
          exit(11);
        continue;
      }
      perror("read");
      exit(8);
    }
    size_t got = rc;
    if (got != sizeof(int))
      exit(9);
    if (expect != number)
      exit(10);
    ++expect;
  }
  return 0;
}

int main(int argc, char *argv[]) {

  if (pipe2(fds, O_NONBLOCK))
    return 1;

  pthread_t th;
  if (pthread_create(&th, 0, worker, 0))
    return 2;

  int number = 0;
  for (;;) {
    int chunk = 0;
    int numbers[ASYMMETRY];
    for (;;) {
      numbers[chunk] = number + chunk;
      if (++chunk == ASYMMETRY)
        break;
      if (number + chunk == ITERATIONS)
        break;
    }
    for (;;) {
      ssize_t rc = write(fds[1], numbers, chunk * sizeof(int));
      if (rc == -1) {
        if (errno == EAGAIN) {
          ++got_write_eagains;
          if (poll(&(struct pollfd){fds[1], POLLOUT}, 1, -1) == -1)
            return 10;
          continue;
        }
        return 3;
      }
      if (rc % sizeof(int))
        return 4;
      chunk = rc / sizeof(int);
      number += chunk;
      break;
    }
    if (number == ITERATIONS)
      break;
  }

  if (pthread_join(th, 0))
    return 5;

  if (!got_read_eagains && !got_write_eagains)
    return 7;
}
