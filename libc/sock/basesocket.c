/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/nt/enum/sio.h"
#include "libc/nt/errors.h"
#include "libc/nt/winsock.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"

static textwindows int64_t GetNtBspSocket(int64_t socket, uint32_t ioctl) {
  uint32_t bytes;
  int64_t bsp_socket;
  if (WSAIoctl(socket, ioctl, NULL, 0, &bsp_socket, sizeof(bsp_socket), &bytes,
               NULL, NULL) != -1) {
    return bsp_socket;
  } else {
    return -1;
  }
}

textwindows int64_t GetNtBaseSocket(int64_t socket) {
  int64_t base_socket;
  for (;;) {
    base_socket = GetNtBspSocket(socket, kNtSioBaseHandle);
    if (base_socket != -1) return base_socket;
    if (WSAGetLastError() == WSAENOTSOCK) return __winsockerr();
    /*
     * Even though Microsoft documentation clearly states that Layered
     * Spyware Providers must never ever intercept the SIO_BASE_HANDLE
     * ioctl, Komodia LSPs (that Lenovo got sued for preinstalling) do
     * so anyway in order to redirect decrypted https requests through
     * some foreign proxy and inject ads which breaks high-performance
     * network event io. However it doesn't handle SIO_BSP_HANDLE_POLL
     * which will at least let us obtain the socket associated with the
     * next winsock protocol chain entry. If this succeeds, loop around
     * and call SIO_BASE_HANDLE again with the returned BSP socket, to
     * make sure we unwrap all layers and retrieve the real base socket.
     */
    base_socket = GetNtBspSocket(socket, kNtSioBspHandlePoll);
    if (base_socket != -1 && base_socket != socket) {
      socket = base_socket;
    } else {
      return __winsockerr();
    }
  }
}
