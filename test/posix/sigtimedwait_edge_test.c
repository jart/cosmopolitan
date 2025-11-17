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
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main() {
  sigset_t set;
  siginfo_t info;
  struct timespec timeout;
  int result;

  sigemptyset(&set);
  sigaddset(&set, SIGUSR1);

  if (sigprocmask(SIG_BLOCK, &set, NULL) != 0)
    return 1;

  timeout.tv_sec = 0;
  timeout.tv_nsec = 1000000;

  result = sigtimedwait(&set, &info, &timeout);
  if (result != -1)
    return 2;

  if (errno == ENOSYS)
    return 0;  // skip test

  if (errno != EAGAIN)
    return 3;

  if (raise(SIGUSR1) != 0)
    return 4;

  timeout.tv_sec = 1;
  timeout.tv_nsec = 0;

  result = sigtimedwait(&set, &info, &timeout);
  if (result != SIGUSR1)
    return 5;

  if (info.si_signo != SIGUSR1)
    return 6;

  if (info.si_pid != getpid())
    return 7;

  return 0;
}
