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
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
  int pipefd[2];
  char buf[1024];
  int i, total_written = 0;

  if (pipe(pipefd) != 0)
    return 1;

  if (fcntl(pipefd[1], F_SETFL, O_NONBLOCK) != 0)
    return 2;

  for (i = 0; i < 1024; i++)
    buf[i] = (char)i;

  for (i = 0; i < 1000; i++) {
    ssize_t n = write(pipefd[1], buf, sizeof(buf));
    if (n < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK)
        break;
      return 3;
    }
    total_written += n;
    if (total_written > 2000000)
      return 4;
  }

  if (total_written == 0)
    return 5;

  close(pipefd[0]);
  close(pipefd[1]);

  return 0;
}
