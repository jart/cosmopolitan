/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "tool/decode/lib/socknames.h"
#include "libc/sock/sock.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/sock.h"
#include "third_party/musl/netdb.h"

const struct IdName kAddressFamilyNames[] = {
    {AF_UNSPEC, "AF_UNSPEC"},
    {AF_UNIX, "AF_UNIX"},
    {AF_INET, "AF_INET"},
    {0, 0},
};

const struct IdName kSockTypeNames[] = {
    {SOCK_STREAM, "SOCK_STREAM"},
    {SOCK_DGRAM, "SOCK_DGRAM"},
    {SOCK_RAW, "SOCK_RAW"},
    {SOCK_RDM, "SOCK_RDM"},
    {SOCK_SEQPACKET, "SOCK_SEQPACKET"},
    {0, 0},
};

const struct IdName kAddrInfoFlagNames[] = {
    {AI_PASSIVE, "AI_PASSIVE"},
    {AI_CANONNAME, "AI_CANONNAME"},
    {AI_NUMERICHOST, "AI_NUMERICHOST"},
    {0, 0},
};

const struct IdName kProtocolNames[] = {
    {IPPROTO_IP, "IPPROTO_IP"},   {IPPROTO_ICMP, "IPPROTO_ICMP"},
    {IPPROTO_TCP, "IPPROTO_TCP"}, {IPPROTO_UDP, "IPPROTO_UDP"},
    {IPPROTO_RAW, "IPPROTO_RAW"}, {0, 0},
};
