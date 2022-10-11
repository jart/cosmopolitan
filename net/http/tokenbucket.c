/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/atomic.h"
#include "net/http/tokenbucket.h"

/**
 * Returns byte comparison mask w/ 0xff if equal otherwise 0x00.
 */
static inline uint64_t CompareEq(uint64_t x, uint64_t y) {
  uint64_t mask, zoro = x ^ y;
  mask = ((((zoro >> 1) | 0x8080808080808080) - zoro) & 0x8080808080808080);
  return (mask << 1) - (mask >> 7);
}

/**
 * Atomically increments all signed bytes in array without overflow.
 *
 * This function should be called periodically so buckets have tokens.
 * While many threads can consumes tokens, only a single thread can use
 * the replenish operation.
 *
 * This function implements a SWAR algorithm offering the best possible
 * performance under the constraint that operations happen atomically.
 * This function should take 2ms to add a token to 2**22 buckets which
 * need a 4mb array that has one bucket for every 1024 IPv4 addresses.
 * However that doesn't matter since no locks are held during that 2ms
 * therefore replenishing doesn't block threads that acquire tokens.
 *
 * @param w is word array that aliases byte token array
 * @param n is number of 64-bit words in `w` array
 */
void ReplenishTokens(atomic_uint_fast64_t *w, size_t n) {
  for (size_t i = 0; i < n; ++i) {
    uint64_t x = atomic_load_explicit(w + i, memory_order_relaxed);
    atomic_fetch_add_explicit(
        w + i, 0x0101010101010101 & ~CompareEq(x, 0x7f7f7f7f7f7f7f7f),
        memory_order_acq_rel);
  }
}

/**
 * Atomically decrements signed byte index if it's positive.
 *
 * This function should be called to take a token from the right bucket
 * whenever a client wants to use some type of resource. This routine
 * discriminates based on `c` which is the netmask bit count. There must
 * exist `1 << c` signed bytes (or buckets) in the `b` array.
 *
 * Tokens are considered available if the bucket corresponding `x` has a
 * positive number. This function returns true of a token was atomically
 * acquired using a lockeless spinless algorithm. Buckets are allowed to
 * drift into a slightly negative state, but overflow is impractical.
 *
 * @param w is array of token buckets
 * @param n is ipv4 address
 * @param c is cidr
 */
bool AcquireToken(atomic_schar *b, uint32_t x, int c) {
  uint32_t i = x >> (32 - c);
  return atomic_load_explicit(b + i, memory_order_relaxed) > 0 &&
         atomic_fetch_add_explicit(b + i, -1, memory_order_acq_rel) > 0;
}

/**
 * Returns current number of tokens in bucket.
 *
 * @param w is array of token buckets
 * @param n is ipv4 address
 * @param c is cidr
 */
int CountTokens(atomic_schar *b, uint32_t x, int c) {
  uint32_t i = x >> (32 - c);
  return atomic_load_explicit(b + i, memory_order_relaxed);
}
