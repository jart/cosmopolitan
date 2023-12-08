/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/sock/struct/sockaddr.h"
#include "libc/macros.internal.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/sock/struct/sockaddr.internal.h"
#include "libc/sock/struct/sockaddr6.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"

// computes byte length of socket address
uint8_t __get_sockaddr_len(const struct sockaddr_storage *addr) {
  if (addr->ss_family == AF_INET) {
    return sizeof(struct sockaddr_in);
  } else if (addr->ss_family == AF_INET6) {
    return sizeof(struct sockaddr_in6);
  } else if (addr->ss_family == AF_UNIX) {
    struct sockaddr_un *sun = (struct sockaddr_un *)addr;
    return sizeof(sun->sun_family) +
           strnlen(sun->sun_path, sizeof(sun->sun_path)) + 1;
  } else {
    return sizeof(struct sockaddr_storage);
  }
}

// converts bsd sockaddr to cosmo abi
void __convert_bsd_to_sockaddr(struct sockaddr_storage *addr) {
  union {
    struct sockaddr cosmo;
    struct sockaddr_bsd bsd;
  } *pun = (void *)addr;
  pun->cosmo.sa_family = pun->bsd.sa_family;
}

// converts cosmo sockaddr abi to bsd
void __convert_sockaddr_to_bsd(struct sockaddr_storage *addr) {
  uint8_t len;
  union {
    struct sockaddr cosmo;
    struct sockaddr_bsd bsd;
  } *pun = (void *)addr;
  len = __get_sockaddr_len(addr);
  pun->bsd.sa_family = pun->cosmo.sa_family;
  pun->bsd.sa_len = len;
}

// copies sockaddr from internal memory to user's buffer
void __write_sockaddr(const struct sockaddr_storage *addr, void *out_addr,
                      uint32_t *inout_addrsize) {
  if (!out_addr) return;
  if (!inout_addrsize) return;
  uint32_t insize = *inout_addrsize;
  if (insize) bzero(out_addr, insize);
  uint32_t outsize = __get_sockaddr_len(addr);
  uint32_t copysize = MIN(insize, outsize);
  if (copysize) memcpy(out_addr, addr, copysize);
  *inout_addrsize = outsize;
}
