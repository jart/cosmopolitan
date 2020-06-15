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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"

#define __NR_accept4_linux 0x0120 /* rhel5:enosysevil */

int accept4$sysv(int server, void *addr, uint32_t *addrsize, int flags) {
  static bool once, demodernize;
  int olderr, client;
  if (!flags || demodernize) goto TriedAndTrue;
  olderr = errno;
  client = __accept4$sysv(server, addr, addrsize, flags);
  if (client == -1 && errno == ENOSYS) {
    errno = olderr;
  TriedAndTrue:
    client = fixupnewsockfd$sysv(__accept$sysv(server, addr, addrsize), flags);
  } else if (SupportsLinux() && !once) {
    once = true;
    if (client == __NR_accept4_linux) {
      demodernize = true;
      goto TriedAndTrue;
    }
  }
  if (client != -1 && IsBsd()) {
    sockaddr2linux(addr);
  }
  return client;
}
