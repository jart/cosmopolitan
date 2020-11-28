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
#include "libc/fmt/fmt.h"
#include "libc/stdio/stdio.h"

/**
 * Decodes data from stream.
 *
 * To read a line of data from a well-formed trustworthy file:
 *
 *   int x, y;
 *   char text[256];
 *   fscanf(f, "%d %d %s\n", &x, &y, text);
 *
 * Please note that this function is brittle by default, which makes it
 * a good fit for yolo coding. With some toil it can be used in a way
 * that makes it reasonably hardened although getline() may be better.
 *
 * @see libc/fmt/vcscanf.c
 */
int(fscanf)(FILE *stream, const char *fmt, ...) {
  int rc;
  va_list va;
  va_start(va, fmt);
  rc = (vcscanf)((int (*)(void *))fgetc, (void *)ungetc, stream, fmt, va);
  va_end(va);
  return rc;
}
