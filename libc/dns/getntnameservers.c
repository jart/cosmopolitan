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
#include "libc/alg/arraylist.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/dns/dns.h"
#include "libc/dns/resolvconf.h"
#include "libc/nt/enum/keyaccess.h"
#include "libc/nt/enum/reggetvalueflags.h"
#include "libc/nt/registry.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"

/**
 * Extracts DNS nameserver IPs from Windows Registry.
 *
 * @param resolv points to a ResolvConf object, which should be zero
 *     initialized by the caller; or if it already contains items,
 *     this function will append
 * @return number of nameservers appended, or -1 w/ errno
 */
textwindows int getntnameservers(struct ResolvConf *resolv) {
  int rc;
  int64_t hkInterfaces = kNtInvalidHandleValue;
  uint32_t keycount = 0;
  if (!RegOpenKeyEx(
          kNtHkeyLocalMachine,
          u"SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces",
          0, kNtKeyRead, &hkInterfaces) &&
      !RegQueryInfoKey(hkInterfaces, NULL, NULL, NULL, &keycount, NULL, NULL,
                       NULL, NULL, NULL, NULL, NULL)) {
    struct sockaddr_in nameserver;
    nameserver.sin_family = AF_INET;
    nameserver.sin_port = htons(DNS_PORT);
    rc = 0;
    for (uint32_t i = 0; i < keycount; ++i) {
      char16_t value[128], ifaceuuid[64];
      uint32_t valuebytes, ifaceuuidlen = sizeof(ifaceuuid);
      if (!RegEnumKeyEx(hkInterfaces, i, ifaceuuid, &ifaceuuidlen, NULL, NULL,
                        NULL, NULL) &&
          ((!RegGetValue(hkInterfaces, ifaceuuid, u"DhcpIpAddress",
                         kNtRrfRtRegSz | kNtRrfRtRegMultiSz, NULL, value,
                         ((valuebytes = sizeof(value)), &valuebytes)) &&
            valuebytes > 2 * sizeof(char16_t)) ||
           (!RegGetValue(hkInterfaces, ifaceuuid, u"IpAddress",
                         kNtRrfRtRegSz | kNtRrfRtRegMultiSz, NULL, value,
                         ((valuebytes = sizeof(value)), &valuebytes)) &&
            valuebytes > 2 * sizeof(char16_t))) &&
          ((!RegGetValue(hkInterfaces, ifaceuuid, u"DhcpNameServer",
                         kNtRrfRtRegSz | kNtRrfRtRegMultiSz, NULL, value,
                         ((valuebytes = sizeof(value)), &valuebytes)) &&
            valuebytes > 2 * sizeof(char16_t)) ||
           (!RegGetValue(hkInterfaces, ifaceuuid, u"NameServer",
                         kNtRrfRtRegSz | kNtRrfRtRegMultiSz, NULL, value,
                         ((valuebytes = sizeof(value)), &valuebytes)) &&
            valuebytes > 2 * sizeof(char16_t)))) {
        char value8[128];
        tprecode16to8(value8, sizeof(value8), value);
        if (inet_pton(AF_INET, value8, &nameserver.sin_addr.s_addr) == 1) {
          if (append(&resolv->nameservers, &nameserver) != -1) ++rc;
        }
      }
    }
  } else {
    rc = __winerr();
  }
  RegCloseKey(hkInterfaces);
  return rc;
}
