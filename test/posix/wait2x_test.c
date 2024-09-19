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
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int ws, rc;

  // create shared memory for synchronization
  atomic_int *ready =
      mmap(0, 4, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

  if ((rc = waitpid(-1, &ws, WNOHANG)) != -1)
    return 1;
  if (errno != ECHILD)
    return 2;

  // create process
  int pid = fork();
  if (pid == -1)
    return 3;
  if (!pid) {
    for (;;)
      if (*ready)
        break;
    _Exit(0);
  }

  // wait on process
  if ((rc = waitpid(pid, &ws, WNOHANG)) == -1)
    return 4;
  if (rc != 0)
    return 5;
  if (ws)
    return 6;

  // signal subprocess
  *ready = 1;

  if ((rc = waitpid(pid, &ws, 0)) == -1)
    return 7;
  if (rc != pid)
    return 8;
  if (ws)
    return 9;

  // wait again
  if ((rc = waitpid(pid, &ws, WNOHANG)) != -1)
    return 10;
  if (errno != ECHILD)
    return 11;
}
