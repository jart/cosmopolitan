/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "ltls.h"
#include "libc/calls/struct/iovec.h"
#include "third_party/mbedtls/ctr_drbg.h"
#include "third_party/mbedtls/debug.h"
#include "third_party/mbedtls/iana.h"
#include "third_party/mbedtls/net_sockets.h"
#include "third_party/mbedtls/oid.h"
#include "third_party/mbedtls/san.h"
#include "third_party/mbedtls/ssl.h"
#include "third_party/mbedtls/ssl_ticket.h"
#include "third_party/mbedtls/x509.h"
#include "third_party/mbedtls/x509_crt.h"
#include "third_party/mbedtls/entropy.h"
#include "net/https/https.h"

#ifndef MIN
#define MIN(a,b)   ( (a) < (b) ? (a) :(b) )
#endif

static const char *const tls_meta = ":mbedtls";

typedef struct {
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context ctr_drbg;
  mbedtls_ssl_context ssl;
  mbedtls_ssl_config conf;
  int ref;  // Reference to self in the Lua registry
  char *read_buffer;
  size_t read_buffer_size;
  int fd;  // File descriptor
} TlsContext;

static TlsContext **checktls(lua_State *L) {
  TlsContext **tls = (TlsContext **)luaL_checkudata(L, 1, tls_meta);
  if (tls == NULL || *tls == NULL)
    luaL_typeerror(L, 1, tls_meta);
  return tls;
}


static int TlsSend(void *c, const unsigned char *p, size_t n) {
  int rc;
  if ((rc = write(*(int *)c, p, n)) == -1) {
    perror("TlsSend");
    fprintf(stderr, "TlsSend error: rc=%d, c=%d\n", rc, *(int *)c);
    exit(1);
  }
  return rc;
}

static int TlsRecv(void *c, unsigned char *p, size_t n, uint32_t o) {
  int r;
  struct iovec v[2];
  static unsigned a, b;
  static unsigned char t[4096];
  if (a < b) {
    r = MIN(n, b - a);
    memcpy(p, t + a, r);
    if ((a += r) == b) {
      a = b = 0;
    }
    return r;
  }
  v[0].iov_base = p;
  v[0].iov_len = n;
  v[1].iov_base = t;
  v[1].iov_len = sizeof(t);
  if ((r = readv(*(int *)c, v, 2)) == -1) {
    perror("TlsRecv");
    exit(1);
  }
  if (r > n) {
    b = r - n;
  }
  return MIN(n, r);
}

static int tls_gc(lua_State *L) {
  TlsContext **tlsp = checktls(L);
  TlsContext *tls = *tlsp;

  if (tls) {
    mbedtls_ssl_free(&tls->ssl);
    mbedtls_ssl_config_free(&tls->conf);
    mbedtls_ctr_drbg_free(&tls->ctr_drbg);
    mbedtls_entropy_free(&tls->entropy);
    luaL_unref(L, LUA_REGISTRYINDEX, tls->ref);
    free(tls->read_buffer);
    free(tls);
    *tlsp = NULL;
  }
  return 0;
}

static void my_debug(void *ctx, int level, const char *file, int line,
                     const char *str) {
  ((void)level);
  fprintf((FILE *)ctx, "%s:%04d: %s", file, line, str);
  fflush((FILE *)ctx);
}
static int tls_client(lua_State *L) {
  int fd = luaL_checkinteger(L, 1);

  printf("fd: %d\n", fd);

  TlsContext **tlsp = (TlsContext **)lua_newuserdata(L, sizeof(TlsContext *));
  *tlsp = NULL;

  luaL_getmetatable(L, tls_meta);
  lua_setmetatable(L, -2);

  TlsContext *tls = (TlsContext *)malloc(sizeof(TlsContext));
  if (tls == NULL) {
    lua_pushnil(L);
    lua_pushstring(L, "Failed to allocate memory for TLS context");
    return 2;
  }
  *tlsp = tls;

  tls->read_buffer = NULL;
  tls->read_buffer_size = 0;
  tls->fd = fd;

  mbedtls_ssl_init(&tls->ssl);
  mbedtls_ssl_config_init(&tls->conf);
  mbedtls_ctr_drbg_init(&tls->ctr_drbg);
  mbedtls_entropy_init(&tls->entropy);

  int sslVerify = lua_isnone(L, 2) ? 1 : lua_toboolean(L, 2);
  if (sslVerify) {
    mbedtls_ssl_conf_ca_chain(&tls->conf, GetSslRoots(), 0);
    mbedtls_ssl_conf_authmode(&tls->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
  } else {
    mbedtls_ssl_conf_authmode(&tls->conf, MBEDTLS_SSL_VERIFY_NONE);
  }

  int timeout = lua_isnone(L, 3) ? 0 : luaL_checkinteger(L, 3);
  mbedtls_ssl_conf_read_timeout(&tls->conf, timeout);

  const char *pers = "tls_client";
  int ret;
  if ((ret = mbedtls_ctr_drbg_seed(&tls->ctr_drbg, mbedtls_entropy_func,
                                   &tls->entropy, (const unsigned char *)pers,
                                   strlen(pers))) != 0) {
    free(tls);
    *tlsp = NULL;
    lua_pushnil(L);
    lua_pushfstring(L, "mbedtls_ctr_drbg_seed returned %d", ret);
    return 2;
  }

  if ((ret = mbedtls_ssl_config_defaults(&tls->conf, MBEDTLS_SSL_IS_CLIENT,
                                         MBEDTLS_SSL_TRANSPORT_STREAM,
                                         MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
    free(tls);
    *tlsp = NULL;
    lua_pushnil(L);
    lua_pushfstring(L, "mbedtls_ssl_config_defaults failed: %d", ret);
    return 2;
  }

  mbedtls_ssl_conf_rng(&tls->conf, mbedtls_ctr_drbg_random, &tls->ctr_drbg);
  mbedtls_ssl_conf_dbg(&tls->conf, my_debug, stdout);

  if ((ret = mbedtls_ssl_setup(&tls->ssl, &tls->conf)) != 0) {
    free(tls);
    *tlsp = NULL;
    lua_pushnil(L);
    lua_pushfstring(L, "mbedtls_ssl_setup returned %d", ret);
    return 2;
  }

  mbedtls_ssl_set_bio(&tls->ssl, &tls->fd, TlsSend, 0, TlsRecv);

  if ((ret = mbedtls_ssl_handshake(&tls->ssl)) != 0) {
    lua_pushnil(L);
    lua_pushfstring(L, "SSL handshake failed: %d", ret);
    return 2;
  }

  tls->ref = luaL_ref(L, LUA_REGISTRYINDEX);
  lua_rawgeti(L, LUA_REGISTRYINDEX, tls->ref);

  return 1;
}

static int tls_write(lua_State *L) {
  TlsContext **tlsp = checktls(L);
  TlsContext *tls = *tlsp;
  size_t len;
  const char *data = luaL_checklstring(L, 2, &len);
  int ret = mbedtls_ssl_write(&tls->ssl, (const unsigned char *)data, len);

  if (ret < 0) {
    lua_pushnil(L);
    lua_pushfstring(L, "SSL write failed: %d", ret);
    return 2;
  }

  lua_pushinteger(L, ret);
  return 1;
}

static int tls_read(lua_State *L) {
  TlsContext **tlsp = checktls(L);
  TlsContext *tls = *tlsp;
  lua_Integer bufsiz = luaL_optinteger(L, 2, BUFSIZ);
  bufsiz = MIN(bufsiz, 0x7ffff000);

  if (tls->read_buffer == NULL || tls->read_buffer_size < bufsiz) {
    char *new_buffer = realloc(tls->read_buffer, bufsiz);
    if (new_buffer == NULL) {
      lua_pushnil(L);
      lua_pushstring(L, "Memory allocation failed");
      return 2;
    }
    tls->read_buffer = new_buffer;
    tls->read_buffer_size = bufsiz;
  }

  int ret =
      mbedtls_ssl_read(&tls->ssl, (unsigned char *)tls->read_buffer, bufsiz);

  if (ret > 0) {
    lua_pushlstring(L, tls->read_buffer, ret);
    return 1;
  } else if (ret == 0) {
    // End of file
    lua_pushnil(L);
    return 1;
  } else {
    // All negative values are treated as errors
    lua_pushnil(L);
    lua_pushfstring(L, "Read error: %d", ret);
    return 2;
  }
}

static int tls_tostring(lua_State *L) {
  TlsContext **tlsp = checktls(L);
  TlsContext *tls = *tlsp;

  lua_pushfstring(L, "tls.TlsClient(fd=%d)", tls->fd);
  return 1;
}

static const struct luaL_Reg tls_methods[] = {
    {"write", tls_write},
    {"read", tls_read},
    {"__gc", tls_gc},
    {"__tostring", tls_tostring},
    {"__repr", tls_tostring},
    {NULL, NULL}
};

static const struct luaL_Reg tlslib[] = {
    {"TlsClient", tls_client},
    {NULL, NULL}
};

static void create_meta(lua_State *L, const char *name,
                        const luaL_Reg *methods) {
  luaL_newmetatable(L, name);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  luaL_setfuncs(L, methods, 0);
}

LUALIB_API int luaopen_tls(lua_State *L) {
  create_meta(L, tls_meta, tls_methods);

  luaL_newlib(L, tlslib);

  lua_pushvalue(L, -1);
  lua_setmetatable(L, -2);

  return 1;
}
