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
#include "libc/assert.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"

asm(".ident\t\"\\n\\n\
Apache License, Version 2.0\\n\
Copyright 2010 Google Inc.\"");
asm(".include \"libc/disclaimer.inc\"");

const char base32def[] = "0123456789abcdefghjkmnpqrstvwxyz";

int tobits(int b) {
  int bits = 0; while (b && (b >>= 1)) bits++;
  return bits;
}

// the next function is based on
// https://github.com/google/google-authenticator-libpam/blob/master/src/base32.c
// Copyright 2010 Google Inc.; Author: Markus Gutschke
// licensed under Apache License, Version 2.0
char* EncodeBase32(const char *s, size_t sl,
                   const char *a, size_t al,
                   size_t *ol) {
  size_t count = 0;
  char *r;
  if (al == 0) {
    a = base32def;
    al = sizeof(base32def)/sizeof(a[0]);
  }
  unassert(2 <= al && al <= 128);
  int bl = tobits(al);
  int mask = (1 << bl) - 1;
  *ol = (sl * 8 + bl - 1) / bl; // calculate exact output length
  if (!(r = malloc(*ol + 1))) {
    *ol = 0;
    return r;
  }
  if (sl > 0) {
    int buffer = s[0];
    size_t next = 1;
    int bitsLeft = 8;
    while (count < *ol && (bitsLeft > 0 || next < sl)) {
      if (bitsLeft < bl) {
        if (next < sl) {
          buffer <<= 8;
          buffer |= s[next++] & 0xFF;
          bitsLeft += 8;
        } else {
          int pad = bl - bitsLeft;
          buffer <<= pad;
          bitsLeft += pad;
        }
      }
      bitsLeft -= bl;
      r[count++] = a[mask & (buffer >> bitsLeft)];
    }
  }
  if (count < *ol) r[count] = '\000';
  return r;
}
