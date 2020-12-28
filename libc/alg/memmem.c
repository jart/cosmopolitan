/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/alg/alg.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"

static void KnuthMorrisPrattInit(ssize_t m, ssize_t *T, const char *W) {
  ssize_t i = 2;
  ssize_t j = 0;
  T[0] = -1;
  T[1] = 0;
  while (i < m) {
    if (W[i - 1] == W[j]) {
      T[i++] = j++ + 1;
    } else if (j > 0) {
      j = T[j];
    } else {
      T[i++] = 0;
    }
  }
  T[m] = 0;
}

static size_t KnuthMorrisPratt(long m, const long *T, const char *W, long n,
                               const char *S) {
  long i = 0, j = 0;
  while (i + j < n) {
    if (W[i] == S[i + j]) {
      i++;
      if (i == m) break;
    } else {
      j = j + i - T[i];
      if (i > 0) i = T[i];
    }
  }
  return j;
}

/**
 * Searches for fixed-length substring in memory region.
 *
 * @param haystack is the region of memory to be searched
 * @param haystacklen is its character count
 * @param needle contains the memory for which we're searching
 * @param needlelen is its character count
 * @return pointer to first result or NULL if not found
 */
void *(memmem)(const void *haystackp, size_t haystacklen, const void *needlep,
               size_t needlelen) {
  long *T;
  size_t i;
  const char *haystack, *needle, *h;
  needle = needlep;
  haystack = haystackp;
  if (needlelen > haystacklen) return NULL;
  if (!needlelen) return haystack;
  h = memchr(haystack, *needle, haystacklen);
  if (!h || needlelen == 1) return h;
  haystacklen -= h - haystack;
  if (needlelen < haystacklen && memcmp(h, needle, needlelen) == 0) {
    return h;
  } else {
    T = malloc((needlelen + 1) * sizeof(long));
    KnuthMorrisPrattInit(needlelen, T, needle);
    i = KnuthMorrisPratt(needlelen, T, needle, haystacklen, h);
    free(T);
    return i < haystacklen ? h + i * sizeof(char) : NULL;
  }
}
