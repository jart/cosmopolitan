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

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main() {

  if (close(1))
    return 1;
  if (open("/dev/null", O_WRONLY) != 1)
    return 2;

  if (printf("a") != 1)
    return 4;
  if (printf("%s", "") != 0)
    return 5;
  if (printf("%s", "a") != 1)
    return 6;
  if (printf("%10s", "a") != 10)
    return 6;
  if (printf("%-10s", "a") != 10)
    return 6;
  if (printf("%-10s%-40s %9s %8s  %8s %8s\n", "Benchmark", "prog", "ops",
             "time", "ops/sec", "time/op") != 89)
    return 7;
  if (fprintf(stdout, "%-10s%-40s %9s %8s  %8s %8s\n", "Benchmark", "prog",
              "ops", "time", "ops/sec", "time/op") != 89)
    return 8;
}
