#include "libc/intrin/kprintf.h"
static const char *const tls_meta = ":mbedtls";

typedef enum {
  TLS_STATE_INIT,
  TLS_STATE_CONNECTED,
  TLS_STATE_CLOSED
} TlsConnectionState;

typedef struct {
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context ctr_drbg;
  mbedtls_ssl_context ssl;
  mbedtls_ssl_config conf;
  mbedtls_net_context server_fd;
  int ref;  // Reference to self in the Lua registry
  TlsConnectionState connection_state;
  char *read_buffer;
  size_t read_buffer_size;
} TlsContext;

static TlsContext **checktls(lua_State *L) {
  TlsContext **tls = (TlsContext **)luaL_checkudata(L, 1, tls_meta);
  if (tls == NULL || *tls == NULL)
    luaL_typeerror(L, 1, tls_meta);
  return tls;
}

static int tls_gc(lua_State *L) {
  TlsContext **tlsp = checktls(L);
  TlsContext *tls = *tlsp;

  if (tls) {
    if (tls->connection_state != TLS_STATE_CLOSED) {
      mbedtls_net_free(&tls->server_fd);
      mbedtls_ssl_free(&tls->ssl);
      mbedtls_ssl_config_free(&tls->conf);
      mbedtls_ctr_drbg_free(&tls->ctr_drbg);
      mbedtls_entropy_free(&tls->entropy);
    }
    mbedtls_ssl_free(&tls->ssl);
    luaL_unref(L, LUA_REGISTRYINDEX, tls->ref);
    free(tls->read_buffer);
    free(tls);
    *tlsp = NULL;
  }
  return 0;
}

static int tls_socket(lua_State *L) {
  if (!sslinitialized) {
    TlsInit();
  }

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

  tls->connection_state = TLS_STATE_INIT;
  tls->read_buffer = NULL;
  tls->read_buffer_size = 0;

  mbedtls_net_init(&tls->server_fd);
  mbedtls_ssl_init(&tls->ssl);
  mbedtls_ssl_config_init(&tls->conf);
  mbedtls_ctr_drbg_init(&tls->ctr_drbg);
  mbedtls_entropy_init(&tls->entropy);
  int sslVerify = lua_isnone(L, 1) ? 1 : lua_toboolean(L, 1);
  if (sslVerify) {
    mbedtls_ssl_conf_ca_chain(&tls->conf, GetSslRoots(), 0);
    mbedtls_ssl_conf_authmode(&tls->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
  } else {
    mbedtls_ssl_conf_authmode(&tls->conf, MBEDTLS_SSL_VERIFY_NONE);
  }

  int timeout = lua_isnone(L, 2) ? 0 : luaL_checkinteger(L, 2);
  mbedtls_ssl_conf_read_timeout(&tls->conf, timeout);

  const char *pers = "tls_socket";
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

  if ((ret = mbedtls_ssl_setup(&tls->ssl, &tls->conf)) != 0) {
    free(tls);
    *tlsp = NULL;
    lua_pushnil(L);
    lua_pushfstring(L, "mbedtls_ssl_setup returned %d", ret);
    return 2;
  }

  tls->ref = luaL_ref(L, LUA_REGISTRYINDEX);
  lua_rawgeti(L, LUA_REGISTRYINDEX, tls->ref);

  return 1;
}

static void my_debug(void *ctx, int level, const char *file, int line,
                     const char *str) {
  ((void)level);
  fprintf((FILE *)ctx, "%s:%04d: %s", file, line, str);
  fflush((FILE *)ctx);
}

static int tls_connect(lua_State *L) {
  TlsContext **tlsp = checktls(L);
  TlsContext *tls = *tlsp;
  const char *server_name = luaL_checkstring(L, 2);
  const char *server_port = luaL_checkstring(L, 3);

  int ret;
  if ((ret = mbedtls_net_connect(&tls->server_fd, server_name, server_port,
                                 MBEDTLS_NET_PROTO_TCP)) != 0) {
    lua_pushnil(L);
    lua_pushfstring(L, "connect failed: %d", ret);
    return 2;
  }

  if ((ret = mbedtls_ssl_config_defaults(&tls->conf, MBEDTLS_SSL_IS_CLIENT,
                                         MBEDTLS_SSL_TRANSPORT_STREAM,
                                         MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
    lua_pushnil(L);
    lua_pushfstring(L, "mbedtls_ssl_config_defaults failed: %d", ret);
    return 2;
  }

  // mbedtls_ssl_conf_authmode(&tls->conf, MBEDTLS_SSL_VERIFY_NONE); // only for
  // test, conf mbedtls_x509_crt_init instead

  mbedtls_ssl_conf_rng(&tls->conf, mbedtls_ctr_drbg_random, &tls->ctr_drbg);
  mbedtls_ssl_conf_dbg(&tls->conf, my_debug, stdout);

  if ((ret = mbedtls_ssl_set_hostname(&tls->ssl, server_name)) != 0) {
    lua_pushnil(L);
    lua_pushfstring(L, "mbedtls_ssl_set_hostname failed: %d", ret);
    return 2;
  }

  mbedtls_ssl_set_bio(&tls->ssl, &tls->server_fd, mbedtls_net_send, NULL,
                      mbedtls_net_recv_timeout);

  if ((ret = mbedtls_ssl_handshake(&tls->ssl)) != 0) {
    lua_pushnil(L);
    lua_pushfstring(L, "SSL handshake failed: %d", ret);
    return 2;
  }

  tls->connection_state = TLS_STATE_CONNECTED;

  lua_pushboolean(L, 1);
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
    lua_pushnil(L);
    if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
      lua_pushstring(L, "EAGAIN");
    } else {
      lua_pushfstring(L, "Read error: %d", ret);
    }
    return 2;
  }
}

static int tls_close(lua_State *L) {
  TlsContext **tlsp = checktls(L);
  TlsContext *tls = *tlsp;

  mbedtls_net_free(&tls->server_fd);
  mbedtls_ssl_free(&tls->ssl);
  mbedtls_ssl_config_free(&tls->conf);
  mbedtls_ctr_drbg_free(&tls->ctr_drbg);
  mbedtls_entropy_free(&tls->entropy);
  free(tls->read_buffer);
  tls->read_buffer = NULL;
  tls->read_buffer_size = 0;
  tls->connection_state = TLS_STATE_CLOSED;

  return 0;
}

static int tls_tostring(lua_State *L) {
  TlsContext **tlsp = checktls(L);
  TlsContext *tls = *tlsp;
  const char *state_str;

  switch (tls->connection_state) {
    case TLS_STATE_INIT:
      state_str = "initialized";
      break;
    case TLS_STATE_CONNECTED:
      state_str = "connected";
      break;
    case TLS_STATE_CLOSED:
      state_str = "closed";
      break;
    default:
      state_str = "unknown";
  }

  lua_pushfstring(L, "TLS connection (%p): %s", tls, state_str);
  return 1;
}

static const struct luaL_Reg tls_methods[] = {{"connect", tls_connect},
                                              {"write", tls_write},
                                              {"read", tls_read},
                                              {"close", tls_close},
                                              {"__gc", tls_gc},
                                              {"__tostring", tls_tostring},
                                              {NULL, NULL}};

static const struct luaL_Reg tlslib[] = {{"socket", tls_socket}, {NULL, NULL}};

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
