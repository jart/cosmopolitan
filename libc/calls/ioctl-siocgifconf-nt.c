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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/sysv/consts/o.h"
#include "libc/sock/sock.h"
#include "libc/sock/internal.h"
#include "libc/sysv/errfuns.h"
#include "libc/nt/winsock.h"

// TODO: Remove me
#include "libc/stdio/stdio.h"

#define MAX_INTERFACES  32

/* Reference:
 *  - Description of ioctls: https://docs.microsoft.com/en-us/windows/win32/winsock/winsock-ioctls
 *  - Structure INTERFACE_INFO: https://docs.microsoft.com/en-us/windows/win32/api/ws2ipdef/ns-ws2ipdef-interface_info
 *  - WSAIoctl man page: https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsaioctl
 *  - Using SIOCGIFCONF in Win32: https://docs.microsoft.com/en-us/windows/win32/winsock/using-unix-ioctls-in-winsock
 */

textwindows int ioctl_siocgifconf_nt(int fd, struct ifconf *ifc) {
  struct NtInterfaceInfo iflist[MAX_INTERFACES];
  uint32_t dwBytes;
  int ret;
  int i, count;

  if (g_fds.p[fd].kind != kFdSocket) {
    return ebadf();
  }

  ret = WSAIoctl(g_fds.p[fd].handle, kNtSioGetInterfaceList, NULL, 0, &iflist, sizeof(iflist), &dwBytes, NULL, NULL);
  if (ret == -1) {
    return __winsockerr();
  }

  count = dwBytes / sizeof(struct NtInterfaceInfo);
  printf("CI> SIO_GET_INTERFACE_LIST success:\n");
  for (i = 0; i < count; ++i) {
    printf("CI>\t #i: %08x\n", i, iflist[i].iiAddress.sin_addr.s_addr);
  }
  return ret;
}

