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
#include "libc/alg/alg.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
/* clang-format off */

static void KnuthMorrisPrattInit(m, T, W)
  ssize_t m, T[m + 1];
  const char W[m];
{
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

static size_t KnuthMorrisPratt(m, T, W, n, S)
  const long n, m, T[m + 1];
  const char W[m], S[n];
{
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

/* clang-format on */

/**
 * Searches for fixed-length substring in memory region.
 *
 * @param haystack is the region of memory to be searched
 * @param haystacklen is its character count
 * @param needle contains the memory for which we're searching
 * @param needlelen is its character count
 * @return pointer to first result or NULL if not found
 */
void *(memmem)(const void *haystack_, size_t haystacklen, const void *needle_,
               size_t needlelen) {
  const char *haystack, *needle, *h;
  haystack = haystack_;
  needle = needle_;
  if (needlelen > haystacklen) return NULL;
  if (!needlelen) return (/*unconst*/ void *)haystack;
  h = memchr(haystack, *needle, haystacklen);
  if (!h || needlelen == 1) return (/*unconst*/ void *)h;
  haystacklen -= h - haystack;
  long stacktmp[16];
  void *freeme = NULL;
  long *T = (needlelen + 1 < ARRAYLEN(stacktmp))
                ? &stacktmp[0]
                : (freeme = malloc((needlelen + 1) * sizeof(long)));
  KnuthMorrisPrattInit(needlelen, T, needle);
  size_t i = KnuthMorrisPratt(needlelen, T, needle, haystacklen, h);
  free(freeme);
  if (i < haystacklen) {
    return (/*unconst*/ char *)h + i * sizeof(char);
  } else {
    return NULL;
  }
}
