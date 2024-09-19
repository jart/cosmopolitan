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

#include <signal.h>
#include <stdatomic.h>
#include <sys/mman.h>
#include <unistd.h>

atomic_int *got;

void onsig(int sig) {
  *got = sig;
}

int main(int argc, char *argv[]) {

  // create process shared memory
  got = mmap(0, 4, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  if (got == MAP_FAILED)
    return 5;

  // listen for signal
  if (signal(SIGUSR1, onsig))
    return 6;

  // block signals
  sigset_t full;
  if (sigfillset(&full))
    return 7;
  if (sigprocmask(SIG_BLOCK, &full, 0))
    return 8;

  // create child process
  int pid;
  if (!(pid = fork())) {
    sigset_t empty;
    sigemptyset(&empty);
    sigsuspend(&empty);
    *got |= 128;
    _exit(0);
  }

  // send signal
  if (kill(pid, SIGUSR1))
    return 9;

  // wait for child to die
  int ws;
  if (wait(&ws) != pid)
    return 10;
  if (ws)
    return 11;
  if (*got != (128 | SIGUSR1))
    return 12;
}
