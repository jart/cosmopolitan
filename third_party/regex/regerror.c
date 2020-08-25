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
#include "libc/bits/safemacros.h"
#include "libc/fmt/fmt.h"
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

/**
 * Converts regular expression error code to string.
 *
 * @param e is error code
 * @return number of bytes needed to hold entire string
 */
size_t regerror(int e, const regex_t *preg, char *buf, size_t size) {
  return 1 + snprintf(buf, size, "%s",
                      firstnonnull(indexdoublenulstring(kRegexErrors, e),
                                   "Unknown error"));
}
