/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/calls.h"
#include "libc/calls/struct/dirent.h"
#include "libc/errno.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/dt.h"
#include "libc/sysv/consts/o.h"
#include "libc/x/x.h"
#include "net/https/https.h"
#include "third_party/mbedtls/x509_crt.h"

STATIC_YOINK("ssl_root_support");

static void FreeSslRoots(mbedtls_x509_crt *c) {
  mbedtls_x509_crt_free(c);
  free(c);
}

/**
 * Returns singleton of SSL roots stored in /zip/usr/share/ssl/root/...
 */
mbedtls_x509_crt *GetSslRoots(void) {
  int fd;
  DIR *d;
  uint8_t *p;
  size_t n, m;
  struct dirent *e;
  static bool once;
  char path[PATH_MAX];
  static mbedtls_x509_crt *c;
  if (!once) {
    if ((c = calloc(1, sizeof(*c)))) {
      m = stpcpy(path, "/zip/usr/share/ssl/root/") - path;
      if ((d = opendir(path))) {
        while ((e = readdir(d))) {
          if (e->d_type != DT_REG) continue;
          if (m + (n = strlen(e->d_name)) >= ARRAYLEN(path)) continue;
          memcpy(path + m, e->d_name, n + 1);
          CHECK((p = xslurp(path, &n)));
          CHECK_GE(mbedtls_x509_crt_parse(c, p, n + 1), 0, "%s", path);
          free(p);
        }
        closedir(d);
      }
      __cxa_atexit(FreeSslRoots, c, 0);
    }
    once = true;
  }
  return c;
}
