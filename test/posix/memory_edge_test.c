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

#include <stdlib.h>
#include <string.h>

int main() {
  void *ptr1, *ptr2, *ptr3;

  ptr1 = malloc(0);
  if (ptr1 == NULL)
    return 1;
  free(ptr1);

  ptr1 = malloc(1);
  if (ptr1 == NULL)
    return 2;

  ptr2 = realloc(ptr1, 0);
  if (ptr2 == NULL)
    return 3;

  ptr1 = malloc(100);
  if (ptr1 == NULL)
    return 4;

  memset(ptr1, 0xAA, 100);

  ptr2 = realloc(ptr1, 200);
  if (ptr2 == NULL)
    return 5;

  if (((char *)ptr2)[50] != (char)0xAA)
    return 6;

  ptr3 = realloc(ptr2, 50);
  if (ptr3 == NULL)
    return 7;

  if (((char *)ptr3)[25] != (char)0xAA)
    return 8;

  free(ptr3);

  return 0;
}
