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
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

// test that stdio implementation unwinds locks on thread cancelation

FILE *out;

void *writer(void *arg) {
  for (;;)
    if (fputc('c', out) != 'c')
      exit(40);
}

int main(void) {

  int pipefds[2];
  if (pipe(pipefds))
    exit(1);

  if (!(out = fdopen(pipefds[1], "a")))
    exit(2);
  char buf[4];
  if (setvbuf(out, buf, _IOFBF, 4))
    exit(3);

  pthread_t th;
  if (pthread_create(&th, 0, writer, 0))
    exit(4);

  // wait for pipe buffer to fill
  usleep(3e5);

  // cancel the thread while it's inside an stdio write operation
  if (pthread_cancel(th))
    exit(5);
  void *status;
  if (pthread_join(th, &status))
    exit(6);
  if (status != PTHREAD_CANCELED)
    exit(7);

  // drain pipe so we can write again
  for (;;) {
    int has;
    if (ioctl(pipefds[0], FIONREAD, &has))
      exit(8);
    if (!has)
      break;
    char c;
    if (read(pipefds[0], &c, 1) != 1)
      exit(9);
  }

  // this won't deadlock any stdio mutexes
  if (fprintf(out, "hello\n") < 0)
    exit(11);
  if (fflush(out))
    exit(12);
  if (fclose(out))
    exit(13);
}
