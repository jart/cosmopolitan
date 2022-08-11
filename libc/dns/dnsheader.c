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
#include "libc/intrin/bits.h"
#include "libc/dns/dnsheader.h"
#include "libc/sysv/errfuns.h"

/**
 * Serializes DNS message h to wire.
 *
 * @return number of bytes written (always 12) or -1 w/ errno
 * @see pascalifydnsname()
 */
void SerializeDnsHeader(uint8_t p[restrict 12], const struct DnsHeader *h) {
  p[0x0] = h->id >> 8;
  p[0x1] = h->id;
  p[0x2] = h->bf1;
  p[0x3] = h->bf2;
  p[0x4] = h->qdcount >> 8;
  p[0x5] = h->qdcount;
  p[0x6] = h->ancount >> 8;
  p[0x7] = h->ancount;
  p[0x8] = h->nscount >> 8;
  p[0x9] = h->nscount;
  p[0xa] = h->arcount >> 8;
  p[0xb] = h->arcount;
}

/**
 * Serializes DNS message h to wire.
 *
 * @return number of bytes read (always 12) or -1 w/ errno
 */
void DeserializeDnsHeader(struct DnsHeader *h, const uint8_t p[restrict 12]) {
  h->id = READ16BE(p);
  h->bf1 = p[2];
  h->bf2 = p[3];
  h->qdcount = READ16BE(p + 4);
  h->ancount = READ16BE(p + 6);
  h->nscount = READ16BE(p + 8);
  h->arcount = READ16BE(p + 10);
}
