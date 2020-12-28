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
