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
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  pid_t pid, result;
  int status;

  pid = fork();
  if (pid == 0) {
    _exit(42);
  } else if (pid > 0) {
    result = waitpid(pid, &status, 0);
    if (result != pid)
      return 1;

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 42)
      return 2;

    if (waitpid(pid, &status, 0) != -1 || errno != ECHILD)
      return 3;

    if (waitpid(-1, &status, WNOHANG) != -1 || errno != ECHILD)
      return 4;

    if (waitpid(0, &status, 0) != -1 || errno != ECHILD)
      return 5;
  } else {
    return 6;
  }

  return 0;
}
