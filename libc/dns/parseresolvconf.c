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
#include "libc/dns/resolvconf.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/inaddr.h"

/**
 * Parses /etc/resolv.conf file.
 *
 * The content of the file usually looks like this:
 *
 *     nameserver 8.8.8.8
 *     nameserver 8.8.4.4
 *
 * @param resolv points to a ResolvConf object, which should be zero
 *     initialized by the caller; or if it already contains items,
 *     this function will append
 * @param f is an open stream with file content
 * @return number of nameservers appended, or -1 w/ errno
 */
int parseresolvconf(struct ResolvConf *resolv, struct FILE *f) {
  /* TODO(jart): options ndots:5 */
  int rc = 0;
  char stackline[32];
  char *line = stackline;
  size_t linecap = sizeof(stackline);
  struct sockaddr_in nameserver;
  nameserver.sin_family = AF_INET;
  nameserver.sin_port = htons(DNS_PORT);
  while (getline(&line, &linecap, f) != -1) {
    char *directive, *value, *tok, *comment;
    if ((comment = strchr(line, '#'))) *comment = '\0';
    if ((directive = strtok_r(line, " \t\r\n\v", &tok)) &&
        (value = strtok_r(NULL, " \t\r\n\v", &tok))) {
      if ((strcmp(directive, "nameserver") == 0 &&
           inet_pton(AF_INET, value, &nameserver.sin_addr.s_addr) == 1)) {
        if (append(&resolv->nameservers, &nameserver) != -1) ++rc;
      }
    }
  }
  free_s(&line);
  return rc | ferror(f);
}
