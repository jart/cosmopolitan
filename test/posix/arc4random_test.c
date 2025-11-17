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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
  uint32_t val1, val2;
  uint32_t uniform1, uniform2;
  unsigned char buf1[16], buf2[16];
  int i;

  val1 = arc4random();
  val2 = arc4random();

  if (val1 == val2)
    return 1;

  uniform1 = arc4random_uniform(100);
  uniform2 = arc4random_uniform(100);

  if (uniform1 >= 100 || uniform2 >= 100)
    return 2;

  if (uniform1 == uniform2 && uniform1 == 0)
    return 3;

  arc4random_buf(buf1, sizeof(buf1));
  arc4random_buf(buf2, sizeof(buf2));

  if (memcmp(buf1, buf2, sizeof(buf1)) == 0)
    return 4;

  for (i = 0; i < 10; i++) {
    uint32_t bounded = arc4random_uniform(1000000);
    if (bounded >= 1000000)
      return 5;
  }

  arc4random_buf(buf1, 0);

  return 0;
}
