/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
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
  char *token = *str;
  if (token) {
    size_t i = strcspn(token, delim);
    char *next = NULL;
    if (token[i]) {
      token[i] = '\0';
      next = &token[i + 1];
    }
    *str = next;
  }
  return token;
}
