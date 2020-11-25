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
#include "libc/dns/dns.h"
#include "libc/dns/dnsquestion.h"
#include "libc/sysv/errfuns.h"

/**
 * Serializes DNS question record to wire.
 *
 * @return number of bytes written
 * @see pascalifydnsname()
 */
int serializednsquestion(uint8_t *buf, size_t size, struct DnsQuestion dq) {
  int wrote;
  if ((wrote = pascalifydnsname(buf, size, dq.qname)) == -1) return -1;
  if (wrote + 1 + 4 > size) return enospc();
  buf[wrote + 1] = dq.qtype >> 010, buf[wrote + 2] = dq.qtype >> 000;
  buf[wrote + 3] = dq.qclass >> 010, buf[wrote + 4] = dq.qclass >> 000;
  return wrote + 5;
}
