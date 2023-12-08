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
#include "libc/assert.h"
#include "libc/nt/enum/sio.h"
#include "libc/nt/struct/guid.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/nt/winsock.h"
#include "libc/sock/sock.h"
#include "libc/sock/wsaid.internal.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/sock.h"

__msabi extern typeof(__sys_closesocket_nt) *const __imp_closesocket;

// returns address of winsock function where msdn says we must do this
// this should be called once, since WSAIoctl has ~2 microsec overhead
void *__get_wsaid(const struct NtGuid *lpFunctionGuid) {
  int r;
  int64_t h;
  void *lpFunc;
  uint32_t dwBytes;
  h = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0,
                kNtWsaFlagOverlapped);
  r = WSAIoctl(h, kNtSioGetExtensionFunctionPointer, lpFunctionGuid,
               sizeof(struct NtGuid), &lpFunc, sizeof(lpFunc), &dwBytes, 0, 0);
  unassert(r != -1);
  __imp_closesocket(h);
  return lpFunc;
}
