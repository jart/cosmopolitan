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

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

volatile int signal_count = 0;
volatile int nested_signal = 0;

void signal_handler(int sig) {
  signal_count++;
  if (signal_count == 1) {
    raise(SIGUSR1);
    if (signal_count == 2) {
      nested_signal = 1;
    }
  }
}

int main() {
  struct sigaction sa;

  sa.sa_handler = signal_handler;
  sa.sa_flags = SA_NODEFER;
  sigemptyset(&sa.sa_mask);

  if (sigaction(SIGUSR1, &sa, NULL) != 0)
    return 1;

  if (raise(SIGUSR1) != 0)
    return 2;

  if (signal_count != 2)
    return 3;

  if (!nested_signal)
    return 4;

  return 0;
}
