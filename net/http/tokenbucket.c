/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "net/http/tokenbucket.h"
#include "libc/intrin/atomic.h"

/**
 * Atomically increments all signed bytes in array, without overflowing.
 *
 * Under the token bucket model, operations are denied by default unless
 * tokens exist to allow them. This function must be called periodically
 * from a single background thread to replenish the buckets with tokens.
 * For example, this function may be called once per second which allows
 * one operation per second on average with bursts up to 127 per second.
 * This policy needn't be applied uniformly. For example, you might find
 * out that a large corporation funnels all their traffic through one ip
 * address, so you could replenish their tokens multiple times a second.
 *
 * @param w is word array that aliases byte token array
 * @param n is number of 64-bit words in `w` array
 */
void ReplenishTokens(atomic_uint_fast64_t *w, size_t n) {
  for (size_t i = 0; i < n; ++i) {
    uint64_t a = atomic_load_explicit(w + i, memory_order_relaxed);
    if (a == 0x7f7f7f7f7f7f7f7f) continue;
    uint64_t b = 0x8080808080808080;
    uint64_t c = 0x7f7f7f7f7f7f7f7f ^ a;
    uint64_t d = ((((c >> 1 | b) - c) & b) ^ b) >> 7;
    atomic_fetch_add_explicit(w + i, d, memory_order_relaxed);
  }
}

/**
 * Atomically decrements signed byte index if it's positive.
 *
 * Multiple threads are able to call this method, to determine if enough
 * tokens exist to perform an operation. Return values greater than zero
 * mean a token was atomically acquired. Values less than, or equal zero
 * means the bucket is empty. There must exist `1 << c` signed bytes (or
 * buckets) in the `b` array.
 *
 * Since this design uses signed bytes, your returned number may be used
 * to control how much burstiness is allowed. For example:
 *
 *     int t = AcquireToken(tok.b, ip, 22);
 *     if (t < 64) {
 *       if (t > 8) write(client, "HTTP/1.1 429 \r\n\r\n", 17);
 *       close(client);
 *       return;
 *     }
 *
 * Could be used to send rejections to clients that exceed their tokens,
 * whereas clients who've grossly exceeded their tokens, could simply be
 * dropped.
 *
 * @param b is array of token buckets
 * @param x is ipv4 address
 * @param c is cidr
 */
int AcquireToken(atomic_schar *b, uint32_t x, int c) {
  uint32_t i = x >> (32 - c);
  int t = atomic_load_explicit(b + i, memory_order_relaxed);
  if (t <= 0) return t;
  return atomic_fetch_add_explicit(b + i, -1, memory_order_relaxed);
}

/**
 * Returns current number of tokens in bucket.
 *
 * @param b is array of token buckets
 * @param x is ipv4 address
 * @param c is cidr
 */
int CountTokens(atomic_schar *b, uint32_t x, int c) {
  uint32_t i = x >> (32 - c);
  return atomic_load_explicit(b + i, memory_order_relaxed);
}
