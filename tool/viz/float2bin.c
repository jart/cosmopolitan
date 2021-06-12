/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "third_party/gdtoa/gdtoa.h"

const char kDoubleBits[] = "\
┌sign\n\
│ ┌exponent\n\
│ │          ┌fraction\n\
│ │          │\n\
│┌┴────────┐┌┴─────────────────────────────────────────────────┐\n";

const char kLongDoubleBits[] = "\
┌sign\n\
│ ┌exponent\n\
│ │             ┌intpart\n\
│ │             │ ┌fraction\n\
│ │             │ │\n\
│┌┴────────────┐│┌┴────────────────────────────────────────────────────────────┐\n";

int main(int argc, char *argv[]) {
  int i;
  union {
    double f;
    uint64_t i;
  } u;
  if (argc <= 1) return 1;
  fputs(kDoubleBits, stdout);
  for (i = 1; i < argc; ++i) {
    u.f = strtod(argv[i], 0);
    printf("%064lb %.15g\n", u.i, u.f);
  }
  return 0;
}
