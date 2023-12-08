/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/intrin/safemacros.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "third_party/regex/regex.h"

/* Error message strings for error codes listed in `regex.h'.  This list
   needs to be in sync with the codes listed there, naturally. */

static const char kRegexErrors[] =
    "No error\0"
    "No match\0"
    "Invalid regexp\0"
    "Unknown collating element\0"
    "Unknown character class name\0"
    "Trailing backslash\0"
    "Invalid back reference\0"
    "Missing ']'\0"
    "Missing ')'\0"
    "Missing '}'\0"
    "Invalid contents of {}\0"
    "Invalid character range\0"
    "Out of memory\0"
    "Repetition not preceded by valid expression\0";

static const char *IndexDoubleNulString(const char *s, unsigned i) {
  size_t n;
  while (i--) {
    if ((n = strlen(s))) {
      s += n + 1;
    } else {
      return NULL;
    }
  }
  return s;
}

/**
 * Converts regular expression error code to string.
 *
 * @param e is error code
 * @return number of bytes needed to hold entire string
 */
size_t regerror(int e, const regex_t *preg, char *buf, size_t size) {
  return 1 + (snprintf)(buf, size, "%s",
                        firstnonnull(IndexDoubleNulString(kRegexErrors, e),
                                     "Unknown error"));
}
