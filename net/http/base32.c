/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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

// these functions are based on
// https://github.com/google/google-authenticator-libpam/blob/master/src/base32.c
// Copyright 2010 Google Inc.; Author: Markus Gutschke
// licensed under Apache License, Version 2.0

/**
 * Encodes binary to base32 ascii representation.
 *
 * @param s is input value
 * @param sl if -1 implies strlen
 * @param a is alphabet string (with power of 2 length)
 * @param al is alphabet length (if 0 then Crockford's encoding is used)
 * @param ol if non-NULL receives output length
 * @return allocated NUL-terminated buffer, or NULL w/ errno
 */
char* EncodeBase32(const char *s, size_t sl,
                   const char *a, size_t al,
                   size_t *ol) {
  size_t count = 0;
  char *r = NULL;
  if (sl == -1) sl = s ? strlen(s) : 0;
  if (al == 0) {
    a = base32def;
    al = sizeof(base32def)/sizeof(a[0]);
  }
  unassert(2 <= al && al <= 128);
  int bl = tobits(al);
  int mask = (1 << bl) - 1;
  size_t n = (sl * 8 + bl - 1) / bl; // calculate output length
  if ((r = malloc(n + 1))) {
    int buffer = s[0];
    size_t next = 1;
    int bitsLeft = 8;
    while (count < n && (bitsLeft > 0 || next < sl)) {
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
    r[count] = '\0';
  }
  if (ol) *ol = r ? count : 0;
  return r;
}

static signed char kBase32cust[256];
static signed char kBase32[256] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -2, -2, -1, -1, -2, -1, -1,  // 0x00
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0x10
    -2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -2, -1, -1,  // 0x20
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1,  // 0x30
    -1, 10, 11, 12, 13, 14, 15, 16, 17,  1, 18, 19,  1, 20, 21, -1,  // 0x40
    22, 23, 24, 25, 26,  0, 27, 28, 29, 30, 31, -1, -1, -1, -1, -1,  // 0x50
    -1, 10, 11, 12, 13, 14, 15, 16, 17,  1, 18, 19,  1, 20, 21, -1,  // 0x60
    22, 23, 24, 25, 26,  0, 27, 28, 29, 30, 31, -1, -1, -1, -1, -1,  // 0x70
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0x80
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0x90
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0xa0
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0xb0
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0xc0
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0xd0
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0xe0
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0xf0
};

/**
 * Decodes base32 ascii representation to binary.
 *
 * This uses Crockford's encoding and skips whitespaces.
 * The decoding stops at the first character not in the alphabet.
 *
 * @param s is input value
 * @param sl if -1 implies strlen
 * @param a is alphabet string (with power of 2 length)
 * @param al is alphabet length (if 0 then Crockford's encoding is used)
 * @param ol if non-NULL receives output length
 * @return allocated NUL-terminated buffer, or NULL w/ errno
 */
char* DecodeBase32(const char *s, size_t sl,
                   const char *a, size_t al,
                   size_t *ol) {
  size_t count = 0;
  char *r = NULL;
  if (sl == -1) sl = s ? strlen(s) : 0;
  if (al == 0) {
    a = base32def;
    al = sizeof(base32def)/sizeof(a[0]);
  }
  unassert(2 <= al && al <= 128);
  int bl = tobits(al);
  size_t n = (sl * bl + 1) / 8 + 1; // calculate output length
  // process input
  if ((r = malloc(n + 1))) {
    unsigned int buffer = 0;
    signed char *map = kBase32;
    int bitsLeft = 0;

    if (a != base32def) {
      // if the provided alphabet doesn't match the default
      map = kBase32cust;
      memset(map, -1, 256);
      // populate the map based on alphabet
      for (int i = 0; i < al; i++) map[a[i] & 0xff] = i;
    }
    while (count < n && *s) {
      signed char m = map[*s++ & 0xff];
      if (m == -2) continue;
      if (m == -1) break;
      buffer <<= bl;
      buffer |= m;
      bitsLeft += bl;
      if (bitsLeft >= 8) {
        r[count++] = buffer >> (bitsLeft - 8);
        bitsLeft -= 8;
      }
    }
    r[count] = '\0';
  }
  if (ol) *ol = r ? count : 0;
  return r;
}
