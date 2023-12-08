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
#include "libc/str/str.h"

/**
 * Tokenizes string.
 *
 * This works by mutating the caller's pointer and the string itself.
 * The returned value is the next available token, or NULL if we've
 * reached the end. Upon each call, *str is updated to point past the
 * terminator we inserted. Multiple delimiter bytes may be passed, which
 * are treated as a set of characters, not a substring.
 *
 * @param str on first call points to var holding string to be tokenized
 *     and is used by this function to track state on subsequent calls
 * @param delim is a set of characters that constitute separators
 * @return next token or NULL when we've reached the end or *str==NULL
 * @note unlike strtok() this does empty tokens and is re-entrant
 */
char *strsep(char **str, const char *delim) {
  size_t i;
  char *token, *next;
  token = *str;
  if (token) {
    i = strcspn(token, delim);
    next = NULL;
    if (token[i]) {
      token[i] = '\0';
      next = &token[i + 1];
    }
    *str = next;
  }
  return token;
}
