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
#include <sys/resource.h>
#include <unistd.h>

int main() {
  struct rlimit rlim, new_rlim;

  if (getrlimit(RLIMIT_NOFILE, &rlim) != 0)
    return 1;

  if (rlim.rlim_cur == 0)
    return 2;

  new_rlim = rlim;
  if (new_rlim.rlim_cur > 8)
    new_rlim.rlim_cur = 8;

  if (setrlimit(RLIMIT_NOFILE, &new_rlim) != 0) {
    if (errno == EPERM)
      return 0;
    return 3;
  }

  if (getrlimit(RLIMIT_NOFILE, &new_rlim) != 0)
    return 4;

  if (new_rlim.rlim_cur != 8)
    return 5;

  if (setrlimit(RLIMIT_NOFILE, &rlim) != 0)
    return 6;

  if (getrlimit(-1, &rlim) == 0)
    return 7;

  if (errno != EINVAL)
    return 8;

  return 0;
}