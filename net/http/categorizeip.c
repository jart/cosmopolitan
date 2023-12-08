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
 * Classifies IP address.
 *
 * @return integer e.g. kIpLoopback, kIpPrivate, etc.
 * @see GetIpCategoryName()
 */
int CategorizeIp(uint32_t x) {
  int a;
  if (IsLoopbackIp(x)) return kIpLoopback;
  if (IsPrivateIp(x)) return kIpPrivate;
  if (IsMulticastIp(x)) return kIpMulticast;
  if (IsAnonymousIp(x)) return kIpAnonymous; /* order matters */
  if (IsTestnetIp(x)) return kIpTestnet;     /* order matters */
  if (IsAfrinicIp(x)) return kIpAfrinic;
  if (IsLacnicIp(x)) return kIpLacnic;
  if (IsApnicIp(x)) return kIpApnic;
  if (IsArinIp(x)) return kIpArin;
  if (IsRipeIp(x)) return kIpRipe;
  if (IsDodIp(x)) return kIpDod;
  a = (x & 0xff000000) >> 24;
  if (a == 12) return kIpAtt;
  if (a == 17) return kIpApple;
  if (a == 19) return kIpFord;
  if (a == 38) return kIpCogent;
  if (a == 48) return kIpPrudential;
  if (a == 56) return kIpUsps;
  if (a == 73) return kIpComcast;
  if (a >= 240) return kIpFuture;
  return kIpUnknown;
}
