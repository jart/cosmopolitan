/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ This is free and unencumbered software released into the public domain.      │
│                                                                              │
│ Anyone is free to copy, modify, publish, use, compile, sell, or              │
│ distribute this software, either in source code form or as a compiled        │
│ binary, for any purpose, commercial or non-commercial, and by any            │
│ means.                                                                       │
│                                                                              │
│ In jurisdictions that recognize copyright laws, the author or authors        │
│ of this software dedicate any and all copyright interest in the              │
│ software to the public domain. We make this dedication for the benefit       │
│ of the public at large and to the detriment of our heirs and                 │
│ successors. We intend this dedication to be an overt act of                  │
│ relinquishment in perpetuity of all present and future rights to this        │
│ software under copyright law.                                                │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,              │
│ EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF           │
│ MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.       │
│ IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR            │
│ OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        │
│ ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR        │
│ OTHER DEALINGS IN THE SOFTWARE.                                              │
│                                                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/str/str.h"

/**
 * Extracts non-empty tokens from string.
 *
 * @param s is mutated and should be NULL on subsequent calls
 * @param sep is a NUL-terminated set of wchars to consider separators
 * @param state tracks progress between calls
 * @return pointer to next token or NULL for end
 */
wchar_t *wcstok(wchar_t *s, const wchar_t *sep, wchar_t **state) {
  size_t leadingseps, tokenlen;
  if (!s) {
    s = *state;
    if (!s) {
      return NULL;
    }
  }
  leadingseps = wcsspn(s, sep);
  s += leadingseps;
  if (*s) {
    tokenlen = wcscspn(s, sep);
    if (s[tokenlen]) {
      s[tokenlen] = L'\0';
      *state = &s[tokenlen + 1];
      return s;
    } else if (tokenlen) {
      s[tokenlen] = L'\0';
      *state = NULL;
      return s;
    }
  }
  return (*state = NULL);
}
