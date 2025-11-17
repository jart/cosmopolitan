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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Tests that line-buffered streams flush only on newline characters.
int main(void) {
  int pipefds[2];
  FILE *fp;
  char buf[16];
  ssize_t rc;

  if (pipe(pipefds) == -1)
    exit(1);

  // Make the read end non-blocking
  if (fcntl(pipefds[0], F_SETFL, O_NONBLOCK) == -1)
    exit(2);

  fp = fdopen(pipefds[1], "w");
  if (!fp)
    exit(3);

  // Enable line buffering.
  if (setvbuf(fp, NULL, _IOLBF, BUFSIZ) != 0)
    exit(4);

  // Write a partial line. It should be buffered.
  if (fputs("test", fp) == EOF)
    exit(5);

  // Try to read. This should fail with EAGAIN because nothing was flushed.
  rc = read(pipefds[0], buf, sizeof(buf));
  if (rc != -1 || errno != EAGAIN)
    exit(6);

  // Write a newline. This should trigger a flush.
  if (fputc('\n', fp) == EOF)
    exit(7);

  // The read should now succeed.
  rc = read(pipefds[0], buf, sizeof(buf));
  if (rc != 5) // "test\n"
    exit(8);

  if (strncmp(buf, "test\n", 5) != 0)
    exit(9);

  fclose(fp);
  close(pipefds[0]);

  return 0;
}
