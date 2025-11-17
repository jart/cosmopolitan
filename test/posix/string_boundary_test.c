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
  char buf[16];
  char *result;

  memset(buf, 'A', sizeof(buf));
  buf[15] = '\0';

  if (strlen(buf) != 15)
    return 1;

  memset(buf, 0, sizeof(buf));
  if (strlen(buf) != 0)
    return 2;

  strcpy(buf, "hello");
  if (strcmp(buf, "hello") != 0)
    return 3;

  if (strcmp(buf, "hell") <= 0)
    return 4;

  if (strcmp("", "") != 0)
    return 5;

  result = strstr("hello world", "world");
  if (!result || strcmp(result, "world") != 0)
    return 6;

  result = strstr("hello", "world");
  if (result != NULL)
    return 7;

  result = strchr("hello", 'e');
  if (!result || *result != 'e')
    return 8;

  result = strchr("hello", 'x');
  if (result != NULL)
    return 9;

  return 0;
}
