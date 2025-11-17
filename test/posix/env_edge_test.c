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
#include <string.h>

int main() {
  char *value;

  if (setenv("TEST_VAR", "test_value", 1) != 0)
    return 1;

  value = getenv("TEST_VAR");
  if (!value || strcmp(value, "test_value") != 0)
    return 2;

  if (setenv("TEST_VAR", "new_value", 0) != 0)
    return 3;

  value = getenv("TEST_VAR");
  if (!value || strcmp(value, "test_value") != 0)
    return 4;

  if (setenv("TEST_VAR", "new_value", 1) != 0)
    return 5;

  value = getenv("TEST_VAR");
  if (!value || strcmp(value, "new_value") != 0)
    return 6;

  if (unsetenv("TEST_VAR") != 0)
    return 7;

  value = getenv("TEST_VAR");
  if (value != NULL)
    return 8;

  if (setenv("", "value", 1) == 0)
    return 9;

  if (errno != EINVAL)
    return 10;

  if (setenv("VAR=", "value", 1) == 0)
    return 11;

  if (errno != EINVAL)
    return 12;

  return 0;
}
