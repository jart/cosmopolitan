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
#include "libc/dns/dns.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

/**
 * Writes dotted hostname to DNS message wire.
 *
 * The wire format is basically a sequence of Pascal strings, for each
 * label in the name. We only do enough validation to maintain protocol
 * invariants.
 *
 * @param name is a dotted NUL-terminated hostname string
 * @return bytes written (excluding NUL) or -1 w/ errno
 */
int pascalifydnsname(uint8_t *buf, size_t size, const char *name) {
  size_t i, j, k, namelen;
  if ((namelen = strlen(name)) > DNS_NAME_MAX) return enametoolong();
  i = 0;
  if (size || namelen) {
    if (namelen + 1 > size) return enospc();
    buf[0] = '\0';
    j = 0;
    for (;;) {
      for (k = 0; name[j + k] && name[j + k] != '.'; ++k) {
        buf[i + k + 1] = name[j + k];
      }
      if (k) {
        if (k > DNS_LABEL_MAX) return enametoolong();
        buf[i] = k;
        i += k + 1;
      }
      j += k + 1;
      if (!name[j - 1]) {
        break;
      }
    }
    buf[i] = '\0';
  }
  return i;
}
