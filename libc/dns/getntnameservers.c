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
  char value8[128];
  int64_t hkInterfaces;
  struct sockaddr_in nameserver;
  char16_t value[128], ifaceuuid[64];
  uint32_t i, keycount, valuebytes, ifaceuuidlen;
  keycount = 0;
  hkInterfaces = kNtInvalidHandleValue;
  if (!RegOpenKeyEx(
          kNtHkeyLocalMachine,
          u"SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces",
          0, kNtKeyRead, &hkInterfaces) &&
      !RegQueryInfoKey(hkInterfaces, NULL, NULL, NULL, &keycount, NULL, NULL,
                       NULL, NULL, NULL, NULL, NULL)) {
    nameserver.sin_family = AF_INET;
    nameserver.sin_port = htons(DNS_PORT);
    rc = 0;
    for (i = 0; i < keycount; ++i) {
      ifaceuuidlen = sizeof(ifaceuuid);
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
