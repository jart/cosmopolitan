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
#include "libc/bits/bits.h"
#include "libc/dns/dnsheader.h"
#include "libc/sysv/errfuns.h"

/**
 * Serializes DNS message header to wire.
 *
 * @return number of bytes written (always 12) or -1 w/ errno
 * @see pascalifydnsname()
 */
int serializednsheader(uint8_t *buf, size_t size,
                       const struct DnsHeader header) {
  if (size < 12) return enospc();
  buf[0x0] = header.id >> 010u;
  buf[0x1] = header.id >> 000u;
  buf[0x2] = header.bf1;
  buf[0x3] = header.bf2;
  buf[0x4] = header.qdcount >> 010u;
  buf[0x5] = header.qdcount >> 000u;
  buf[0x6] = header.ancount >> 010u;
  buf[0x7] = header.ancount >> 000u;
  buf[0x8] = header.nscount >> 010u;
  buf[0x9] = header.nscount >> 000u;
  buf[0xa] = header.arcount >> 010u;
  buf[0xb] = header.arcount >> 000u;
  return 12;
}

/**
 * Serializes DNS message header to wire.
 *
 * @return number of bytes read (always 12) or -1 w/ errno
 */
int deserializednsheader(struct DnsHeader *header, const uint8_t *buf,
                         size_t size) {
  if (size < 12) return ebadmsg();
  header->id = read16be(buf + 0);
  header->bf1 = buf[2];
  header->bf2 = buf[3];
  header->qdcount = read16be(buf + 4);
  header->ancount = read16be(buf + 6);
  header->nscount = read16be(buf + 8);
  header->arcount = read16be(buf + 10);
  return 12;
}
