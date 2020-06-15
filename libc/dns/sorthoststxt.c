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
#include "libc/dns/dns.h"
#include "libc/dns/hoststxt.h"

/**
 * Compares hostnames in HOSTS.TXT table.
 * @see dnsnamecmp(), parsehoststxt()
 */
static int cmphoststxt(const struct HostsTxtEntry *e1,
                       const struct HostsTxtEntry *e2, const char *strings) {
  if (e1 == e2) return 0;
  return dnsnamecmp(&strings[e1->name], &strings[e2->name]);
}

/**
 * Sorts entries in HOSTS.TXT table.
 *
 * This function enables resolvehoststxt() to be called so hard-coded
 * hostname lookups take logarithmic time; you can blackhole all the
 * spam you want, in your /etc/hosts file.
 *
 * The sorted order, defined by dnsnamecmp(), also makes it possible to
 * efficiently search for subdomains, once the initial sort is done.
 */
void sorthoststxt(struct HostsTxt *ht) {
  if (ht->entries.p) {
    qsort_r(ht->entries.p, ht->entries.i, sizeof(*ht->entries.p),
            (void *)cmphoststxt, ht->strings.p);
  }
}
