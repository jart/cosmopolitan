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
#include "libc/errno.h"
#include "libc/nt/winsock.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"
#include "libc/str/str.h"

textwindows int64_t winsockblock(int64_t fh, unsigned eventbit, int64_t rc) {
  int64_t eh;
  struct NtWsaNetworkEvents ev;
  if (rc != -1) return rc;
  if (WSAGetLastError() != EWOULDBLOCK) return __winsockerr();
  eh = WSACreateEvent();
  memset(&ev, 0, sizeof(ev));
  if (WSAEventSelect(fh, eh, 1u << eventbit) != -1 &&
      WSAEnumNetworkEvents(fh, eh, &ev) != -1) {
    if (!ev.iErrorCode[eventbit]) {
      rc = 0;
    } else {
      errno = ev.iErrorCode[eventbit];
    }
  } else {
    __winsockerr();
  }
  WSACloseEvent(eh);
  return rc;
}
