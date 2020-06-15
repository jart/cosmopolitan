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
#include "libc/sock/sock.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ai.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/sock.h"
#include "tool/decode/lib/socknames.h"

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
