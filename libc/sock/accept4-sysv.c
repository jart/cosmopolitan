/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/assert.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.internal.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sock.h"

int sys_accept4(int server, void *addr, uint32_t *addrsize, int flags) {
  int olderr, client, file_mode;
  union sockaddr_storage_bsd bsd;
  uint32_t size = sizeof(bsd);
  void *out_addr = !IsBsd() ? addr : &bsd;
  uint32_t *out_addrsize = !IsBsd() ? addrsize : &size;
  static bool demodernize;
  if (demodernize) goto TriedAndTrue;
  olderr = errno;
  client = __sys_accept4(server, out_addr, out_addrsize, flags);
  if (client == -1 && errno == ENOSYS) {
    errno = olderr;
    demodernize = true;
  TriedAndTrue:
    if ((client = __sys_accept(server, out_addr, out_addrsize, 0)) != -1) {
      // __sys_accept() has inconsistent flag inheritence across platforms
      _unassert((file_mode = __sys_fcntl(client, F_GETFD)) != -1);
      _unassert(!__sys_fcntl(client, F_SETFD,
                             ((file_mode & ~FD_CLOEXEC) |
                              (flags & SOCK_CLOEXEC ? FD_CLOEXEC : 0))));
      _unassert((file_mode = __sys_fcntl(client, F_GETFL)) != -1);
      _unassert(!__sys_fcntl(client, F_SETFL,
                             ((file_mode & ~O_NONBLOCK) |
                              (flags & SOCK_NONBLOCK ? O_NONBLOCK : 0))));
    }
  }
  if (client != -1 && IsBsd()) {
    sockaddr2linux(&bsd, size, addr, addrsize);
  }
  return client;
}
