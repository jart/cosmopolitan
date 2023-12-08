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
#include "libc/stdio/stdio.h"

/**
 * Formats and writes text to stdout.
 *
 * Cosmopolitan supports most of the standard formatting behaviors
 * described by `man 3 printf`, in addition to the following
 *
 * - `%jjd`, `%jjx`, etc. are {,u}int128_t (cosmopolitan only)
 *
 * - `%'d` or `%,d` may be used to insert thousands separators. The prior is
 *   consistent with C; the latter is consistent with Python.
 *
 * - `%m` inserts strerror(errno) into the formatted output. This is
 *   consistent with glibc, musl, and uclibc.
 *
 * - `%hs` converts UTF-16/UCS-2 → UTF-8, which can be helpful on Windows.
 *   Formatting (e.g. %-10hs) will use monospace display width rather
 *   than string length or codepoint count.
 *
 * - `%ls` (or `%Ls`) converts UTF-32 → UTF-8. Formatting (e.g. %-10ls)
 *   will use monospace display width rather than string length.
 *
 * - The `%#s` and `%#c` alternate forms display values using the
 *   standard IBM standard 256-letter alphabet. Using `%#.*s` to specify
 *   length will allow true binary (i.e. with NULs) to be formatted.
 *
 * - The `%'s` and `%'c` alternate forms are Cosmopolitan extensions for
 *   escaping string literals for C/C++ and Python. The outer quotation
 *   marks can be added automatically using ``%`s``. If constexpr format
 *   strings are used, we can avoid linking cescapec() too.
 *
 * - The backtick modifier (``%`s`` and ``%`c``) and repr() directive
 *   (`%r`) both ask the formatting machine to represent values as real
 *   code rather than using arbitrary traditions for displaying values.
 *   This means it implies the quoting modifier, wraps the value with
 *   {,u,L}['"] quotes, displays NULL as "NULL" rather than "(null)".
 *
 * @see __fmt() for intuitive reference documentation
 * @see {,v}{,s{,n},{,{,x}as},f,d}printf
 */
int printf(const char* fmt, ...) {
  int rc;
  va_list va;
  va_start(va, fmt);
  rc = vfprintf(stdout, fmt, va);
  va_end(va);
  return rc;
}
