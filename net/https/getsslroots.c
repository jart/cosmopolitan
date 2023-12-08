/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/cosmo.h"
#include "libc/limits.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/dt.h"
#include "libc/sysv/consts/o.h"
#include "third_party/mbedtls/x509_crt.h"

__static_yoink("ssl_root_support");

#define SSL_ROOT_DIR "/zip/usr/share/ssl/root"

static struct {
  _Atomic(uint32_t) once;
  mbedtls_x509_crt chain;
} g_ssl_roots;

static void FreeSslRoots(void) {
  mbedtls_x509_crt_free(&g_ssl_roots.chain);
}

static void InitSslRoots(void) {
  DIR *dir;
  if (!(dir = opendir(SSL_ROOT_DIR))) {
    perror(SSL_ROOT_DIR);
    return;
  }
  struct dirent *ent;
  while ((ent = readdir(dir))) {
    if (ent->d_type != DT_REG &&  //
        ent->d_type != DT_UNKNOWN) {
      continue;
    }
    char path[PATH_MAX];
    strlcpy(path, SSL_ROOT_DIR "/", sizeof(path));
    strlcat(path, ent->d_name, sizeof(path));
    uint8_t *data;
    int fd = open(path, O_RDONLY);         // punt error to lseek
    size_t size = lseek(fd, 0, SEEK_END);  // punt error to calloc
    if ((data = calloc(1, size + 1)) && pread(fd, data, size, 0) == size) {
      if (mbedtls_x509_crt_parse(&g_ssl_roots.chain, data, size + 1)) {
        tinyprint(2, path, ": error loading ssl root\n", NULL);
      }
    } else {
      perror(path);
    }
    free(data);
    close(fd);
  }
  closedir(dir);
  atexit(FreeSslRoots);
}

/**
 * Returns singleton of SSL roots stored in /zip/usr/share/ssl/root/...
 */
mbedtls_x509_crt *GetSslRoots(void) {
  cosmo_once(&g_ssl_roots.once, InitSslRoots);
  return &g_ssl_roots.chain;
}
