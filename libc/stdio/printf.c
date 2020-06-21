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
#include "libc/stdio/stdio.h"

/**
 * Formats and writes string to stdout.
 *
 * Cosmopolitan supports most of the standard formatting behaviors
 * described by `man 3 printf`, in addition to the following:
 *
 * - %jd, %jx, etc. are {,u}intmax_t which in Cosmopolitan is 128-bit.
 *
 * - %'d or %,d may be used to insert thousands separators. The prior is
 *   consistent with C; the latter is consistent with Python.
 *
 * - %m inserts strerror(errno) into the formatted output. This is
 *   consistent with glibc, musl, and uclibc.
 *
 * - %n inserts "\n" on non-Windows and "\r\n" on Windows. This is the
 *   same behavior as Java. It's incompatible with glibc's behavior,
 *   since that's problematic according to Android's security team.
 *
 * - %hs converts UTF-16/UCS-2 → UTF-8, which can be helpful on Windows.
 *   Formatting (e.g. %-10hs) will use monospace display width rather
 *   than string length or codepoint count.
 *
 * - %ls (or %Ls) converts UTF-32 → UTF-8. Formatting (e.g. %-10ls) will
 *   use monospace display width rather than string length.
 *
 * - The %#s and %#c alternate forms display values using the standard
 *   IBM standard 256-letter alphabet. Using %#.*s to specify length
 *   will allow true binary (i.e. with NULs) to be formatted.
 *
 * - The %'s and %'c alternate forms are Cosmopolitan extensions for
 *   escaping string literals for C/C++ and Python. The outer quotation
 *   marks can be added automatically using %`s. If constexpr format
 *   strings are used, we can avoid linking cescapec() too.
 *
 * - The backtick modifier (%`s and %`c) and repr() directive (%r) both
 *   ask the formatting machine to represent values as real code rather
 *   than using arbitrary traditions for displaying values. This means
 *   it implies the quoting modifier, wraps the value with {,u,L}['"]
 *   quotes, displays NULL as "NULL" rather than "(null)", etc.
 *
 * @see palandprintf() for intuitive reference documentation
 * @see {,v}{,s{,n},{,{,x}as},f,d}printf
 */
int(printf)(const char* fmt, ...) {
  int rc;
  va_list va;
  va_start(va, fmt);
  rc = (vfprintf)(stdout, fmt, va);
  va_end(va);
  return rc;
}
