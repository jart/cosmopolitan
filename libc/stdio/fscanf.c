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
#include "libc/fmt/internal.h"
#include "libc/stdio/stdio.h"

/**
 * Decodes data from stream.
 *
 * To read a line of data from a well-formed trustworthy file:
 *
 *     int x, y;
 *     char text[256];
 *     fscanf(f, "%d %d %s\n", &x, &y, text);
 *
 * Please note that this function is brittle by default, which makes it
 * a good fit for yolo coding. With some toil it can be used in a way
 * that makes it reasonably hardened although getline() may be better.
 *
 * @see libc/fmt/vcscanf.c
 */
int fscanf(FILE *stream, const char *fmt, ...) {
  int rc;
  va_list va;
  va_start(va, fmt);
  flockfile(stream);
  rc = __vcscanf((void *)fgetc_unlocked,   //
                 (void *)ungetc_unlocked,  //
                 stream, fmt, va);
  funlockfile(stream);
  va_end(va);
  return rc;
}
