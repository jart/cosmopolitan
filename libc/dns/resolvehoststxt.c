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
#include "libc/alg/alg.h"
#include "libc/dns/consts.h"
#include "libc/dns/dns.h"
#include "libc/dns/hoststxt.h"
#include "libc/sock/sock.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/errfuns.h"

static int hoststxtgetcmp(const char *node, const struct HostsTxtEntry *entry,
                          const char *strings) {
  return dnsnamecmp(node, &strings[entry->name]);
}

/**
 * Finds address associated with name in HOSTS.TXT table.
 *
 * This function performs binary search, so sorthoststxt() must be
 * called on the table beforehand.
 *
 * @param ht can be gethoststxt()
 * @param af can be AF_INET, AF_UNSPEC
 * @param name can be a local or fully-qualified hostname
 * @param addr should point to a struct sockaddr_in; if this function
 *     succeeds, its sin_family and sin_addr fields will be modified
 * @param addrsize is the byte size of addr
 * @param canon be used to return a pointer to the canonical name
 * @return number of matches found, or -1 w/ errno
 * @error EAFNOSUPPORT
 */
int resolvehoststxt(const struct HostsTxt *ht, int af, const char *name,
                    struct sockaddr *addr, uint32_t addrsize,
                    const char **canon) {
  struct sockaddr_in *addr4;
  struct HostsTxtEntry *entry;
  if (af != AF_INET && af != AF_UNSPEC) return eafnosupport();
  if ((entry = bsearch_r(name, ht->entries.p, ht->entries.i,
                         sizeof(struct HostsTxtEntry), (void *)hoststxtgetcmp,
                         ht->strings.p))) {
    if (addr) {
      if (addrsize < kMinSockaddr4Size) return einval();
      addr4 = (struct sockaddr_in *)addr;
      addr4->sin_family = AF_INET;
      memcpy(&addr4->sin_addr.s_addr, &entry->ip[0], 4);
    }
    if (canon) *canon = &ht->strings.p[entry->canon];
    return 1;
  } else {
    return 0;
  }
}
