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
#include "libc/alg/arraylist.h"
#include "libc/dns/dns.h"
#include "libc/dns/hoststxt.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"

/**
 * Parses HOSTS.TXT contents.
 *
 * Hostnames were invented by Peggy Karp; her format looks like this:
 *
 *     # this is a comment
 *     # IP            CANON [ALT...]
 *     203.0.113.1     lol.example. lol
 *     203.0.113.2     cat.example. cat
 *
 * @param htp points to a HostsTxt object, which should be zero
 *     initialized by the caller; or if it already contains items,
 *     this function will append
 * @param f is the file content; see fopen() and fmemopen()
 * @return 0 on success, or -1 w/ errno
 * @see hoststxtsort() which is the logical next step
 */
int parsehoststxt(struct HostsTxt *ht, FILE *f) {
  int rc = 0;
  char *line;
  size_t linesize;
  line = NULL;
  linesize = 0;
  while ((getline(&line, &linesize, f)) != -1) {
    struct HostsTxtEntry entry;
    char *addr, *name, *tok, *comment;
    if ((comment = strchr(line, '#'))) *comment = '\0';
    if ((addr = strtok_r(line, " \t\r\n\v", &tok)) &&
        inet_pton(AF_INET, addr, entry.ip) == 1) {
      entry.canon = ht->strings.i;
      while ((name = strtok_r(NULL, " \t\r\n\v", &tok))) {
        entry.name = ht->strings.i;
        if (concat(&ht->strings, name, strnlen(name, DNS_NAME_MAX) + 1) == -1 ||
            append(&ht->entries, &entry) == -1) {
          rc = -1;
        }
      }
    }
  }
  free(line);
  return rc | ferror(f);
}
