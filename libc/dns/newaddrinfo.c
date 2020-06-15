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
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/sock/sock.h"
#include "libc/sysv/consts/af.h"

struct addrinfo *newaddrinfo(uint16_t port) {
  void *mem;
  struct addrinfo *ai = NULL;
  /* shoehorning is ok since this'll never be realloc()'d */
  uint32_t size = ROUNDUP(sizeof(struct addrinfo), sizeof(void *));
  uint32_t addrlen = sizeof(struct sockaddr_in);
  if ((ai = mem = calloc(1, size + addrlen + DNS_NAME_MAX + 1))) {
    ai->ai_family = AF_INET;
    ai->ai_addrlen = addrlen;
    ai->ai_addr4 = (struct sockaddr_in *)((char *)mem + size);
    ai->ai_addr4->sin_family = AF_INET;
    ai->ai_addr4->sin_port = htons(port);
    ai->ai_canonname = (char *)mem + size + addrlen;
  }
  return ai;
}
