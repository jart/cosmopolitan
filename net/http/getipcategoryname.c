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
 * Describes IP address.
 * @see CategorizeIp()
 */
const char *GetIpCategoryName(int c) {
  switch (c) {
    case kIpMulticast:
      return "MULTICAST";
    case kIpLoopback:
      return "LOOPBACK";
    case kIpPrivate:
      return "PRIVATE";
    case kIpTestnet:
      return "TESTNET";
    case kIpAfrinic:
      return "AFRINIC";
    case kIpLacnic:
      return "LACNIC";
    case kIpApnic:
      return "APNIC";
    case kIpArin:
      return "ARIN";
    case kIpRipe:
      return "RIPE";
    case kIpDod:
      return "DOD";
    case kIpAtt:
      return "AT&T";
    case kIpApple:
      return "APPLE";
    case kIpFord:
      return "FORD";
    case kIpCogent:
      return "COGENT";
    case kIpPrudential:
      return "PRUDENTIAL";
    case kIpUsps:
      return "USPS";
    case kIpComcast:
      return "COMCAST";
    case kIpFuture:
      return "FUTURE";
    case kIpAnonymous:
      return "ANONYMOUS";
    default:
      return NULL;
  }
}
