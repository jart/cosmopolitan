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
#include <time.h>
#include <unistd.h>

int main() {
  time_t t1, t2;
  struct timespec ts;

  t1 = time(NULL);
  if (t1 == (time_t)-1)
    return 1;

  sleep(1);

  t2 = time(NULL);
  if (t2 <= t1)
    return 2;

  if (clock_gettime(CLOCK_REALTIME, &ts) != 0)
    return 3;

  if (ts.tv_sec <= 0)
    return 4;

  if (ts.tv_nsec < 0 || ts.tv_nsec >= 1000000000)
    return 5;

  if (clock_gettime(-1, &ts) == 0)
    return 6;

  if (errno != EINVAL)
    return 7;

  return 0;
}
