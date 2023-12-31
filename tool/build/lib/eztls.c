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
#include "tool/build/lib/eztls.h"
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/iovec.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/strace.internal.h"
#include "libc/macros.internal.h"
#include "libc/sysv/consts/sig.h"
#include "libc/thread/thread.h"
#include "libc/x/x.h"
#include "libc/x/xsigaction.h"
#include "net/https/https.h"
#include "third_party/mbedtls/debug.h"
#include "third_party/mbedtls/net_sockets.h"
#include "third_party/mbedtls/ssl.h"

_Thread_local int mytid;
_Thread_local struct EzTlsBio ezbio;
_Thread_local mbedtls_ssl_config ezconf;
_Thread_local mbedtls_ssl_context ezssl;
_Thread_local mbedtls_ctr_drbg_context ezrng;

void EzSanity(void) {
  unassert(mytid);
  unassert(mytid == gettid());
}

void EzTlsDie(const char *s, int r) {
  EzSanity();
  if (IsTiny()) {
    kprintf("error: %s (-0x%04x %s)\n", s, -r, GetTlsError(r));
  } else {
    kprintf("error: %s (grep -0x%04x)\n", s, -r);
  }
  EzDestroy();
  pthread_exit(0);
}

static ssize_t EzWritevAll(int fd, struct iovec *iov, int iovlen) {
  EzSanity();
  int i;
  ssize_t rc;
  size_t wrote, total;
  i = 0;
  total = 0;
  do {
    if (i) {
      while (i < iovlen && !iov[i].iov_len) ++i;
      if (i == iovlen) break;
    }
    if ((rc = writev(fd, iov + i, iovlen - i)) != -1) {
      wrote = rc;
      total += wrote;
      do {
        if (wrote >= iov[i].iov_len) {
          wrote -= iov[i++].iov_len;
        } else {
          iov[i].iov_base = (char *)iov[i].iov_base + wrote;
          iov[i].iov_len -= wrote;
          wrote = 0;
        }
      } while (wrote);
    } else {
      // WARNF("writev() failed %m");
      if (errno != EINTR) {
        return total ? total : -1;
      }
    }
  } while (i < iovlen);
  return total;
}

int EzTlsFlush(struct EzTlsBio *bio, const unsigned char *buf, size_t len) {
  EzSanity();
  struct iovec v[2];
  if (len || bio->c > 0) {
    v[0].iov_base = bio->u;
    v[0].iov_len = MAX(0, bio->c);
    v[1].iov_base = (void *)buf;
    v[1].iov_len = len;
    if (EzWritevAll(bio->fd, v, 2) != -1) {
      if (bio->c > 0) bio->c = 0;
    } else if (errno == EAGAIN) {
      return MBEDTLS_ERR_SSL_TIMEOUT;
    } else if (errno == EPIPE || errno == ECONNRESET || errno == ENETRESET) {
      return MBEDTLS_ERR_NET_CONN_RESET;
    } else {
      // WARNF("EzTlsSend error %s", strerror(errno));
      return MBEDTLS_ERR_NET_SEND_FAILED;
    }
  }
  return 0;
}

static int EzTlsSend(void *ctx, const unsigned char *buf, size_t len) {
  EzSanity();
  int rc;
  struct EzTlsBio *bio = ctx;
  if (bio->c >= 0 && bio->c + len <= sizeof(bio->u)) {
    memcpy(bio->u + bio->c, buf, len);
    bio->c += len;
    return len;
  }
  if ((rc = EzTlsFlush(bio, buf, len)) < 0) return rc;
  return len;
}

static int EzTlsRecvImpl(void *ctx, unsigned char *p, size_t n, uint32_t o) {
  EzSanity();
  int r;
  struct iovec v[2];
  struct EzTlsBio *bio = ctx;
  if ((r = EzTlsFlush(bio, 0, 0)) < 0) return r;
  if (bio->a < bio->b) {
    r = MIN(n, bio->b - bio->a);
    memcpy(p, bio->t + bio->a, r);
    if ((bio->a += r) == bio->b) bio->a = bio->b = 0;
    return r;
  }
  v[0].iov_base = p;
  v[0].iov_len = n;
  v[1].iov_base = bio->t;
  v[1].iov_len = sizeof(bio->t);
  while ((r = readv(bio->fd, v, 2)) == -1) {
    // WARNF("tls read() error %s", strerror(errno));
    if (errno == EINTR) {
      return MBEDTLS_ERR_SSL_WANT_READ;
    } else if (errno == EAGAIN) {
      return MBEDTLS_ERR_SSL_TIMEOUT;
    } else if (errno == EPIPE || errno == ECONNRESET || errno == ENETRESET) {
      return MBEDTLS_ERR_NET_CONN_RESET;
    } else {
      return MBEDTLS_ERR_NET_RECV_FAILED;
    }
  }
  if (r > n) bio->b = r - n;
  return MIN(n, r);
}

static int EzTlsRecv(void *ctx, unsigned char *buf, size_t len, uint32_t tmo) {
  EzSanity();
  return EzTlsRecvImpl(ctx, buf, len, tmo);
}

void EzFd(int fd) {
  EzSanity();
  mbedtls_ssl_session_reset(&ezssl);
  ezbio.fd = fd;
}

void EzHandshake(void) {
  EzSanity();
  int rc;
  while ((rc = mbedtls_ssl_handshake(&ezssl))) {
    if (rc != MBEDTLS_ERR_SSL_WANT_READ) {
      EzTlsDie("handshake failed", rc);
    }
  }
  while ((rc = EzTlsFlush(&ezbio, 0, 0))) {
    if (rc != MBEDTLS_ERR_SSL_WANT_READ) {
      EzTlsDie("handshake flush failed", rc);
    }
  }
}

int EzHandshake2(void) {
  EzSanity();
  int rc;
  while ((rc = mbedtls_ssl_handshake(&ezssl))) {
    if (rc == MBEDTLS_ERR_NET_CONN_RESET) {
      return rc;
    } else if (rc != MBEDTLS_ERR_SSL_WANT_READ) {
      EzTlsDie("handshake failed", rc);
    }
  }
  while ((rc = EzTlsFlush(&ezbio, 0, 0))) {
    if (rc == MBEDTLS_ERR_NET_CONN_RESET) {
      return rc;
    } else if (rc != MBEDTLS_ERR_SSL_WANT_READ) {
      EzTlsDie("handshake flush failed", rc);
    }
  }
  return 0;
}

void EzInitialize(void) {
  unassert(!mytid);
  mytid = gettid();
  mbedtls_ssl_init(&ezssl);
  mbedtls_ssl_config_init(&ezconf);
  mbedtls_platform_zeroize(&ezbio, sizeof(ezbio));
  ezconf.disable_compression = 1;
  InitializeRng(&ezrng);
}

static void OnSslDebug(void *ctx, int level, const char *file, int line,
                       const char *message) {
  char sline[12];
  char slevel[12];
  FormatInt32(sline, line);
  FormatInt32(slevel, level);
  tinyprint(2, file, ":", sline, ": (", slevel, ") ", message, "\n", NULL);
}

void EzSetup(char psk[32]) {
  int rc;
  EzSanity();
  mbedtls_ssl_conf_dbg(&ezconf, OnSslDebug, 0);
  mbedtls_ssl_conf_rng(&ezconf, mbedtls_ctr_drbg_random, &ezrng);
  if ((rc = mbedtls_ssl_conf_psk(&ezconf, psk, 32, "runit", 5))) {
    EzTlsDie("EzSetup mbedtls_ssl_conf_psk", rc);
  }
  if ((rc = mbedtls_ssl_setup(&ezssl, &ezconf))) {
    EzTlsDie("EzSetup mbedtls_ssl_setup", rc);
  }
  mbedtls_ssl_set_bio(&ezssl, &ezbio, EzTlsSend, 0, EzTlsRecv);
}

void EzDestroy(void) {
  if (!mytid) return;
  EzSanity();
  mbedtls_ssl_free(&ezssl);
  mbedtls_ctr_drbg_free(&ezrng);
  mbedtls_ssl_config_free(&ezconf);
  mytid = 0;
}
