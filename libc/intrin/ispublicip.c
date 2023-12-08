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
#include "net/http/ip.h"

/**
 * Returns true if IPv4 address is Globally Reachable.
 *
 * We intentionally omit TEST-NET here which can be used to simulate
 * public Internet traffic using non-Internet IPs.
 *
 * @return true 92.4499% of the time
 * @see IANA IPv4 Special-Purpose Address Registry
 */
bool IsPublicIp(uint32_t x) {
  return !((x & 0xffffffff) == 0x00000000 /* 0.0.0.0/32         */ ||
           (x & 0xff000000) == 0x00000000 /* 0.0.0.0/8          */ ||
           (x & 0xff000000) == 0x0a000000 /* 10.0.0.0/8         */ ||
           (x & 0xff000000) == 0x7f000000 /* 127.0.0.0/8        */ ||
           (x & 0xfff00000) == 0xac100000 /* 172.16.0.0/12      */ ||
           (x & 0xffffff00) == 0xc0000000 /* 192.0.0.0/24       */ ||
           (x & 0xffff0000) == 0xc0a80000 /* 192.168.0.0/16     */ ||
           (x & 0xffff0000) == 0xa9fe0000 /* 169.254.0.0/16     */ ||
           (x & 0xffc00000) == 0x64400000 /* 100.64.0.0/10      */ ||
           (x & 0xfffe0000) == 0xc6120000 /* 198.18.0.0/15      */ ||
           (x & 0xf0000000) == 0xf0000000 /* 240.0.0.0/4        */ ||
           (x & 0xffffffff) == 0xffffffff /* 255.255.255.255/32 */);
}
