/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Will Maier                                                    │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for        │
│ any purpose with or without fee is hereby granted, provided that the        │
│ above copyright notice and this permission notice appear in all copies.     │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL               │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED               │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE            │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL        │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR       │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER              │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR            │
│ PERFORMANCE OF THIS SOFTWARE.                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "tool/net/lfetch.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/timeval.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/intrin/atomic.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/goodsocket.internal.h"
#include "libc/sock/sock.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ai.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/limits.h"
#include "libc/sysv/consts/sock.h"
#include "libc/thread/thread.h"
#include "libc/x/x.h"
#include "net/http/escape.h"
#include "net/http/http.h"
#include "net/http/ip.h"
#include "net/http/url.h"
#include "third_party/lua/lauxlib.h"
#include "third_party/lua/lua.h"
#include "third_party/mbedtls/ctr_drbg.h"
#include "third_party/mbedtls/entropy.h"
#include "third_party/mbedtls/error.h"
#include "third_party/mbedtls/ssl.h"
#include "third_party/mbedtls/x509.h"
#include "third_party/musl/netdb.h"

// Global state for SSL client
static pthread_mutex_t g_ssl_mu;
static bool g_ssl_initialized;
static mbedtls_ssl_config g_ssl_conf;
static mbedtls_ssl_context g_ssl;
static mbedtls_ctr_drbg_context g_ssl_rng;
static mbedtls_entropy_context g_ssl_entropy;
static mbedtls_x509_crt g_ssl_cacerts;

// TLS I/O structure
struct TlsBio {
  int fd;
  size_t a;
  size_t b;
  int c;
  char buf[1400];
};

// Shared state structure (minimal version)
struct SharedState {
  struct {
    atomic_int sslhandshakes;
    atomic_int sslverifyfailed;
  } c;
};

static struct SharedState g_shared;
static struct SharedState *shared = &g_shared;

// Configuration
static const char *brand = "cosmo-fetch/1.0";
static struct timeval timeout = {.tv_sec = 60};
static bool sslinitialized;
static bool sslcliused;
static bool unsecure;
static bool evadedragnetsurveillance;
static bool logmessages;
static bool logbodies;

// Logging macros (simplified versions for standalone build)
#define DEBUGF(...) ((void)0)
#define VERBOSEF(...) ((void)0)
#define NOISEF(...) ((void)0)
#define WARNF(...) LOGF(kLogWarn, __VA_ARGS__)

// I/O macros
#define READ read
#define WRITE write

// SSL references
static mbedtls_ssl_context *sslcli = &g_ssl;
static mbedtls_ctr_drbg_context *rngcli = &g_ssl_rng;

// Forward declarations
static int LuaNilError(lua_State *, const char *, ...);
static int LuaNilTlsError(lua_State *, const char *, int);
static void LuaPushHeaders(lua_State *, struct HttpMessage *, const char *);
static void LogMessage(const char *, const char *, size_t);
static void LogBody(const char *, const char *, size_t);
static char *DescribeSslVerifyFailure(uint32_t);
static void ReseedRng(mbedtls_ctr_drbg_context *, const char *);
static int TlsSend(void *, const unsigned char *, size_t);
static int TlsRecvImpl(void *, unsigned char *, size_t, uint32_t);
static void TlsInit(void);
static void LockInc(atomic_int *);

// Helper functions
static void LockInc(atomic_int *p) {
  atomic_fetch_add(p, 1);
}

static int LuaNilError(lua_State *L, const char *fmt, ...) {
  char buf[512];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);
  lua_pushnil(L);
  lua_pushstring(L, buf);
  return 2;
}

static int LuaNilTlsError(lua_State *L, const char *s, int r) {
  char buf[300];
  mbedtls_strerror(r, buf, sizeof(buf));
  return LuaNilError(L, "%s failed: %s", s, buf);
}

static void LuaPushHeaders(lua_State *L, struct HttpMessage *msg,
                           const char *buf) {
  size_t i;
  const char *k, *v;
  size_t kn, vn;
  lua_newtable(L);
  for (i = 0; i < kHttpHeadersMax; ++i) {
    if (!msg->headers[i].a)
      continue;
    k = GetHttpHeaderName(i);
    kn = strlen(k);
    v = buf + msg->headers[i].a;
    vn = msg->headers[i].b - msg->headers[i].a;
    if (IsRepeatable(k, kn)) {
      lua_pushlstring(L, k, kn);
      lua_rawget(L, -2);
      if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        lua_newtable(L);
        lua_pushlstring(L, v, vn);
        lua_rawseti(L, -2, 1);
        lua_pushlstring(L, k, kn);
        lua_pushvalue(L, -2);
        lua_rawset(L, -4);
      } else {
        lua_pushlstring(L, v, vn);
        lua_rawseti(L, -2, lua_rawlen(L, -2) + 1);
      }
      lua_pop(L, 1);
    } else {
      lua_pushlstring(L, k, kn);
      lua_pushlstring(L, v, vn);
      lua_rawset(L, -3);
    }
  }
  for (i = 0; i < msg->xheaders.n; ++i) {
    k = buf + msg->xheaders.p[i].k.a;
    kn = msg->xheaders.p[i].k.b - msg->xheaders.p[i].k.a;
    v = buf + msg->xheaders.p[i].v.a;
    vn = msg->xheaders.p[i].v.b - msg->xheaders.p[i].v.a;
    if (IsRepeatable(k, kn)) {
      lua_pushlstring(L, k, kn);
      lua_rawget(L, -2);
      if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        lua_newtable(L);
        lua_pushlstring(L, v, vn);
        lua_rawseti(L, -2, 1);
        lua_pushlstring(L, k, kn);
        lua_pushvalue(L, -2);
        lua_rawset(L, -4);
      } else {
        lua_pushlstring(L, v, vn);
        lua_rawseti(L, -2, lua_rawlen(L, -2) + 1);
      }
      lua_pop(L, 1);
    } else {
      lua_pushlstring(L, k, kn);
      lua_pushlstring(L, v, vn);
      lua_rawset(L, -3);
    }
  }
}

static void LogMessage(const char *prefix, const char *msg, size_t len) {
  // No-op in standalone version
  (void)prefix;
  (void)msg;
  (void)len;
}

static void LogBody(const char *prefix, const char *body, size_t len) {
  // No-op in standalone version
  (void)prefix;
  (void)body;
  (void)len;
}

static char *DescribeSslVerifyFailure(uint32_t flags) {
  static char buf[512];
  mbedtls_x509_crt_verify_info(buf, sizeof(buf), "", flags);
  return buf;
}

static void ReseedRng(mbedtls_ctr_drbg_context *ctx, const char *label) {
  unsigned char seed[32];
  int i;
  for (i = 0; i < sizeof(seed); ++i) {
    seed[i] = rand();
  }
  mbedtls_ctr_drbg_reseed(ctx, seed, sizeof(seed));
}

static int TlsSend(void *ctx, const unsigned char *buf, size_t len) {
  struct TlsBio *bio = ctx;
  ssize_t rc;
  if ((rc = write(bio->fd, buf, len)) == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return MBEDTLS_ERR_SSL_WANT_WRITE;
    }
    return MBEDTLS_ERR_NET_SEND_FAILED;
  }
  return rc;
}

static int TlsRecvImpl(void *ctx, unsigned char *buf, size_t len,
                       uint32_t timeout_ms) {
  struct TlsBio *bio = ctx;
  ssize_t rc;
  size_t got;

  // Try to satisfy from buffer
  if (bio->c != -1 && bio->a < bio->b) {
    got = bio->b - bio->a;
    if (got > len)
      got = len;
    memcpy(buf, bio->buf + bio->a, got);
    bio->a += got;
    return got;
  }

  // Read from socket
  if ((rc = read(bio->fd, buf, len)) == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return MBEDTLS_ERR_SSL_WANT_READ;
    }
    return MBEDTLS_ERR_NET_RECV_FAILED;
  }
  if (rc == 0) {
    return MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY;
  }
  return rc;
}

static void TlsInit(void) {
  int rc;

  pthread_mutex_lock(&g_ssl_mu);
  if (g_ssl_initialized) {
    pthread_mutex_unlock(&g_ssl_mu);
    return;
  }

  mbedtls_ssl_config_init(&g_ssl_conf);
  mbedtls_ssl_init(&g_ssl);
  mbedtls_ctr_drbg_init(&g_ssl_rng);
  mbedtls_entropy_init(&g_ssl_entropy);
  mbedtls_x509_crt_init(&g_ssl_cacerts);

  if ((rc = mbedtls_ctr_drbg_seed(&g_ssl_rng, mbedtls_entropy_func,
                                   &g_ssl_entropy, NULL, 0)) != 0) {
    WARNF("mbedtls_ctr_drbg_seed failed: %d", rc);
    goto fail;
  }

  if ((rc = mbedtls_ssl_config_defaults(
           &g_ssl_conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM,
           MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
    WARNF("mbedtls_ssl_config_defaults failed: %d", rc);
    goto fail;
  }

  mbedtls_ssl_conf_authmode(&g_ssl_conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
  mbedtls_ssl_conf_rng(&g_ssl_conf, mbedtls_ctr_drbg_random, &g_ssl_rng);

  if ((rc = mbedtls_ssl_setup(&g_ssl, &g_ssl_conf)) != 0) {
    WARNF("mbedtls_ssl_setup failed: %d", rc);
    goto fail;
  }

  g_ssl_initialized = true;
  sslinitialized = true;
  pthread_mutex_unlock(&g_ssl_mu);
  return;

fail:
  pthread_mutex_unlock(&g_ssl_mu);
}

// Include the actual Fetch implementation
#include "tool/net/fetch.inc"

void LuaInitFetch(void) {
  static pthread_mutex_t init_mu = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_lock(&init_mu);
  if (!g_ssl_initialized) {
    pthread_mutex_init(&g_ssl_mu, NULL);
  }
  pthread_mutex_unlock(&init_mu);
}
