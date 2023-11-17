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
#include "libc/cxxabi.h"
#include "libc/dce.h"
#include "libc/dns/resolvconf.h"
#include "libc/intrin/pushpop.internal.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/stdio/stdio.h"
#include "libc/thread/thread.h"

static struct ResolvConf *g_resolvconf;
static struct ResolvConfInitialStaticMemory {
  struct ResolvConf rv;
  pthread_mutex_t lock;
  struct sockaddr_in nameservers[3];
} g_resolvconf_init;

/**
 * Returns singleton with DNS server address.
 */
const struct ResolvConf *GetResolvConf(void) {
  int rc;
  FILE *f;
  struct ResolvConfInitialStaticMemory *init;
  init = &g_resolvconf_init;
  pthread_mutex_lock(&init->lock);
  if (!g_resolvconf) {
    g_resolvconf = &init->rv;
    pushmov(&init->rv.nameservers.n, ARRAYLEN(init->nameservers));
    init->rv.nameservers.p = init->nameservers;
    __cxa_atexit((void *)FreeResolvConf, &g_resolvconf, NULL);
    if (!IsWindows()) {
      if ((f = fopen("/etc/resolv.conf", "r"))) {
        rc = ParseResolvConf(g_resolvconf, f);
      } else {
        rc = -1;
      }
      fclose(f);
    } else {
      rc = GetNtNameServers(g_resolvconf);
    }
    if (rc == -1 && !IsTiny()) {
      /* TODO(jart): Elevate robustness. */
    }
  }
  pthread_mutex_unlock(&init->lock);
  return g_resolvconf;
}

/**
 * Frees resolv.conf data structure populated by ParseResolvConf().
 */
void FreeResolvConf(struct ResolvConf **rvp) {
  if (*rvp) {
    if ((*rvp)->nameservers.p != g_resolvconf_init.nameservers) {
      free((*rvp)->nameservers.p);
    }
    if (*rvp != &g_resolvconf_init.rv) {
      free(*rvp);
    }
    *rvp = 0;
  }
}
