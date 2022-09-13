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
#include "libc/calls/struct/stat.h"
#include "libc/errno.h"
#include "libc/intrin/bits.h"
#include "libc/intrin/lockcmpxchg.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/time/time.h"
#include "net/https/sslcache.h"
#include "third_party/mbedtls/ssl.h"
#include "third_party/mbedtls/x509_crt.h"

#define PROT  (PROT_READ | PROT_WRITE)
#define FLAGS MAP_SHARED

static uint32_t HashSslSession(mbedtls_ssl_session *session) {
  int i;
  uint32_t h;
  h = session->ciphersuite;
  h *= 0x9e3779b1;
  h = session->compression;
  h *= 0x9e3779b1;
  for (i = 0; i < session->id_len; i++) {
    h += session->id[i];
    h *= 0x9e3779b1;
  }
  return h;
}

static struct SslCache *OpenSslCache(const char *path, size_t size) {
  int fd;
  struct stat st;
  struct SslCache *c = NULL;
  if (path) {
    if ((fd = open(path, O_RDWR | O_CREAT, 0600)) != -1) {
      CHECK_NE(-1, fstat(fd, &st));
      if (st.st_size && st.st_size != size) {
        WARNF("unlinking sslcache because size changed from %,zu to %,zu",
              st.st_size, size);
        unlink(path);
        fd = open(path, O_RDWR | O_CREAT, 0600);
        st.st_size = 0;
      }
      if (fd != -1) {
        if (!st.st_size) CHECK_NE(-1, ftruncate(fd, size));
        c = mmap(0, size, PROT, FLAGS, fd, 0);
        close(fd);
      }
    } else {
      WARNF("sslcache open(%`'s) failed %s", path, strerror(errno));
    }
  }
  return c;
}

struct SslCache *CreateSslCache(const char *path, size_t bytes, int lifetime) {
  size_t ents, size;
  struct SslCache *c;
  if (!bytes) bytes = 10 * 1024 * 1024;
  if (lifetime <= 0) lifetime = 24 * 60 * 60;
  ents = _rounddown2pow(MAX(2, bytes / sizeof(struct SslCacheEntry)));
  size = sizeof(struct SslCache) + sizeof(struct SslCacheEntry) * ents;
  size = ROUNDUP(size, FRAMESIZE);
  c = OpenSslCache(path, size);
  if (!c) c = mmap(0, size, PROT, FLAGS | MAP_ANONYMOUS, -1, 0);
  CHECK_NE(MAP_FAILED, c);
  VERBOSEF("opened %`'s %,zu bytes with %,u slots",
           c ? path : "anonymous shared memory", size, ents);
  c->lifetime = lifetime;
  c->size = size;
  c->mask = ents - 1;
  return c;
}

void FreeSslCache(struct SslCache *cache) {
  if (!cache) return;
  CHECK_NE(-1, munmap(cache, cache->size));
}

int UncacheSslSession(void *data, mbedtls_ssl_session *session) {
  int64_t ts;
  uint64_t tick;
  unsigned char *ticket;
  struct SslCache *cache;
  mbedtls_x509_crt *cert;
  struct SslCacheEntry *e;
  uint32_t i, hash, ticketlen;
  INFOF("uncache");
  cache = data;
  hash = HashSslSession(session);
  i = hash & cache->mask;
  e = cache->p + i;
  if (!(tick = e->tick) || hash != e->hash) {
    NOISEF("%u empty", i);
    return 1;
  }
  asm volatile("" ::: "memory");
  if (session->ciphersuite != e->session.ciphersuite ||
      session->compression != e->session.compression ||
      session->id_len != e->session.id_len ||
      memcmp(session->id, e->session.id, e->session.id_len)) {
    VERBOSEF("%u sslcache collision", i);
    return 1;
  }
  ts = time(0);
  if (!(e->time <= ts && ts <= e->time + cache->lifetime)) {
    DEBUGF("%u sslcache expired", i);
    _lockcmpxchg(&e->tick, tick, 0);
    return 1;
  }
  cert = 0;
  ticket = 0;
  if ((e->certlen && (!(cert = calloc(1, sizeof(*cert))) ||
                      mbedtls_x509_crt_parse_der(cert, e->cert, e->certlen)))) {
    goto Contention;
  }
  if ((ticketlen = e->ticketlen)) {
    if (!(ticket = malloc(ticketlen))) goto Contention;
    memcpy(ticket, e->ticket, ticketlen);
  }
  mbedtls_ssl_session_free(session);
  memcpy(session, &e->session, sizeof(*session));
  asm volatile("" ::: "memory");
  if (tick != e->tick) goto Contention;
  session->peer_cert = cert;
  session->ticket = ticket;
  session->ticket_len = ticketlen;
  DEBUGF("%u restored ssl from cache", i);
  return 0;
Contention:
  WARNF("%u sslcache contention 0x%08x", i, hash);
  mbedtls_x509_crt_free(cert);
  free(ticket);
  free(cert);
  return 1;
}

int CacheSslSession(void *data, const mbedtls_ssl_session *session) {
  int pid;
  uint64_t tick;
  uint32_t i, hash;
  struct SslCache *cache;
  struct SslCacheEntry *e;
  cache = data;
  if (session->peer_cert &&
      session->peer_cert->raw.len > sizeof(cache->p[0].cert)) {
    WARNF("%s too big %zu", "cert", session->peer_cert->raw.len);
    return 1;
  }
  if (session->ticket && session->ticket_len > sizeof(cache->p[0].ticket)) {
    WARNF("%s too big %zu", "ticket", session->ticket_len);
    return 1;
  }
  pid = getpid();
  hash = HashSslSession(session);
  i = hash & cache->mask;
  e = cache->p + i;
  e->tick = 0;
  e->pid = pid;
  asm volatile("" ::: "memory");
  memcpy(&e->session, session, sizeof(*session));
  if (session->peer_cert) {
    e->certlen = session->peer_cert->raw.len;
    memcpy(e->cert, session->peer_cert->raw.p, session->peer_cert->raw.len);
  } else {
    e->certlen = 0;
  }
  if (session->ticket) {
    e->ticketlen = session->ticket_len;
    memcpy(e->ticket, session->ticket, session->ticket_len);
  } else {
    e->ticketlen = 0;
  }
  e->hash = hash;
  e->time = time(0);
  tick = rdtsc();
  asm volatile("" ::: "memory");
  if (tick && _lockcmpxchg(&e->pid, pid, 0)) {
    DEBUGF("%u saved %s%s %`#.*s", i,
           mbedtls_ssl_get_ciphersuite_name(session->ciphersuite),
           session->compression ? " DEFLATE" : "", session->id_len,
           session->id);
    e->tick = tick;
    return 0;
  } else {
    WARNF("%u congestion", i);
    return 1;
  }
}
