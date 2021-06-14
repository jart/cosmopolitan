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
#include "libc/sysv/consts/af.h"
#include "libc/sock/sock.h"
#include "libc/sock/internal.h"
#include "libc/sysv/errfuns.h"
#include "libc/nt/winsock.h"
#include "libc/nt/errors.h"

// TODO: Remove me
#include "libc/bits/weaken.h"
#include "libc/stdio/stdio.h"
#define PRINTF  weaken(printf)

#define MAX_INTERFACES  32

#if 1

#define GAA_FLAG_SKIP_UNICAST                   0x0001
#define GAA_FLAG_SKIP_ANYCAST                   0x0002
#define GAA_FLAG_SKIP_MULTICAST                 0x0004
#define GAA_FLAG_SKIP_DNS_SERVER                0x0008
#define GAA_FLAG_INCLUDE_PREFIX                 0x0010
#define GAA_FLAG_SKIP_FRIENDLY_NAME             0x0020
#define GAA_FLAG_INCLUDE_WINS_INFO              0x0040
#define GAA_FLAG_INCLUDE_GATEWAYS               0x0080
#define GAA_FLAG_INCLUDE_ALL_INTERFACES         0x0100
#define GAA_FLAG_INCLUDE_ALL_COMPARTMENTS       0x0200
#define GAA_FLAG_INCLUDE_TUNNEL_BINDINGORDER    0x0400
#define GAA_FLAG_SKIP_DNS_INFO                  0x0800

typedef struct _IP_ADAPTER_ADDRESSES_XP {
    union {
        uint64_t Alignment;
        struct {
            uint32_t Length;
            uint32_t IfIndex;
        };
    };
    struct _IP_ADAPTER_ADDRESSES_XP *Next;
    char * AdapterName;
    void * FirstUnicastAddress;
    void * FirstAnycastAddress;
    void * FirstMulticastAddress;
    void * FirstDnsServerAddress;
    wchar_t *DnsSuffix;
    wchar_t *Description;
    wchar_t *FriendlyName;
    /* Filler */
} IP_ADAPTER_ADDRESSES_XP, *PIP_ADAPTER_ADDRESSES_XP;

extern unsigned long GetAdaptersAddresses(
        uint32_t Family, 
        uint32_t Flags,
        void * Reserved,
        char *AdapterAddresses,
        uint32_t *SizePointer);

textwindows int ioctl_siocgifconf_nt(int fd, struct ifconf *ifc) {
    uint32_t rv, size;
    char * adapter_addresses;
    struct _IP_ADAPTER_ADDRESSES_XP *aa;

    PRINTF("FABDEBUG> ioctl(SIOCGIFCONF) for Windows... printf=%p, GetAdaptersAddresses=%p\n", weaken(printf), weaken(GetAdaptersAddresses));
    if (!weaken(GetAdaptersAddresses)) {
        PRINTF("FABDEBUG> NULL GetAdaptersAddresses\n");
        return enomem();
    }
    rv = weaken(GetAdaptersAddresses)(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, NULL, &size);
    if (rv != kNtErrorBufferOverflow) {
        PRINTF("FABDEBUG> GetAdaptersAddresses failed %d\n", WSAGetLastError());
        return ebadf();
    }
    PRINTF("FABDEBUG> size=%lu\n", size);

    adapter_addresses = (char *)weaken(malloc)(size);
    if (!adapter_addresses) {
        return enomem();
    }

    rv = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, adapter_addresses, &size);
    if (rv != kNtErrorSuccess) {
        PRINTF("FABDEBUG> GetAdaptersAddresses failed %d\n", WSAGetLastError());
        weaken(free)(adapter_addresses);
        return efault();
    }

    for (aa = (struct _IP_ADAPTER_ADDRESSES_XP *)adapter_addresses; aa != NULL; aa = aa->Next) {
        PRINTF("FABDEBUG> Adapter name = %s", aa->AdapterName);
    }
    weaken(free)(adapter_addresses);

    return 0;
}
#else
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
      PRINTF("FABDEBUG> WSAIoctl failed %d\n", WSAGetLastError());
    return weaken(__winsockerr)();
  }

  PRINTF("FABDEBUG> WSAIoctl success\n");
  count = dwBytes / sizeof(struct NtInterfaceInfo);
  PRINTF("CI> SIO_GET_INTERFACE_LIST success:\n");
  for (i = 0; i < count; ++i) {
    PRINTF("CI>\t #i: addr=%08x, flags=%08x, bcast=%08x\n", i, iflist[i].iiAddress.sin_addr.s_addr, iflist[i].iiFlags, iflist[i].iiBroadcastAddress.sin_addr.s_addr);
  }
  return ret;
}

#endif
