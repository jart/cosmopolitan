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
#include "net/http/ip.h"

/**
 * Returns true if `x` is Cloudflare IPv4 address.
 *
 * @see https://www.cloudflare.com/ips/ (April 8, 2021)
 */
bool IsCloudflareIp(uint32_t x) {
  return (x & 0xfffffc00) == 0x6715f400 ||  // 103.21.244.0/22
         (x & 0xfffffc00) == 0x6716c800 ||  // 103.22.200.0/22
         (x & 0xfffffc00) == 0x671f0400 ||  // 103.31.4.0/22
         (x & 0xfff80000) == 0x68100000 ||  // 104.16.0.0/13
         (x & 0xfffc0000) == 0x68180000 ||  // 104.24.0.0/14
         (x & 0xffffc000) == 0x6ca2c000 ||  // 108.162.192.0/18
         (x & 0xfffffc00) == 0x83004800 ||  // 131.0.72.0/22
         (x & 0xffffc000) == 0x8d654000 ||  // 141.101.64.0/18
         (x & 0xfffe0000) == 0xa29e0000 ||  // 162.158.0.0/15
         (x & 0xfff80000) == 0xac400000 ||  // 172.64.0.0/13
         (x & 0xfffff000) == 0xadf53000 ||  // 173.245.48.0/20
         (x & 0xfffff000) == 0xbc726000 ||  // 188.114.96.0/20
         (x & 0xfffff000) == 0xbe5df000 ||  // 190.93.240.0/20
         (x & 0xfffffc00) == 0xc5eaf000 ||  // 197.234.240.0/22
         (x & 0xffff8000) == 0xc6298000;    // 198.41.128.0/17
}
