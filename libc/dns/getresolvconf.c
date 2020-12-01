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
#include "libc/bits/pushpop.h"
#include "libc/dce.h"
#include "libc/dns/resolvconf.h"
#include "libc/fmt/fmt.h"
#include "libc/macros.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"

static struct ResolvConf *g_resolvconf;
static struct ResolvConfInitialStaticMemory {
  struct ResolvConf rv;
  struct sockaddr_in nameservers[3];
} g_resolvconf_init;

/**
 * Returns singleton with DNS server address.
 */
const struct ResolvConf *getresolvconf(void) {
  int rc;
  FILE *f;
  struct ResolvConfInitialStaticMemory *init;
  init = &g_resolvconf_init;
  if (!g_resolvconf) {
    g_resolvconf = &init->rv;
    pushmov(&init->rv.nameservers.n, ARRAYLEN(init->nameservers));
    init->rv.nameservers.p = init->nameservers;
    __cxa_atexit(freeresolvconf, &g_resolvconf, NULL);
    if (!IsWindows()) {
      if ((f = fopen("/etc/resolv.conf", "r"))) {
        rc = parseresolvconf(g_resolvconf, f);
      } else {
        rc = -1;
      }
      fclose(f);
    } else {
      rc = getntnameservers(g_resolvconf);
    }
    if (rc == -1 && !IsTiny()) {
      fprintf(stderr, "%s: %m\n", "nameserver discovery failed");
    }
  }
  return g_resolvconf;
}
