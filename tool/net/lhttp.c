/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
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
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "tool/net/lhttp.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/timeval.h"
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/serialize.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/pollfd.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/so.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/consts/sol.h"
#include "net/http/http.h"
#include "third_party/lua/cosmo.h"
#include "third_party/lua/lauxlib.h"
#include "third_party/lua/lua.h"

#define HTTP_RECV_BUFSIZE  65536
#define HTTP_MAX_REQUEST   1048576
#define HTTP_MAX_HEADERS   100

static int IsValidHttpHeaderName(const char *s, size_t len) {
  if (len == 0) return 0;
  for (size_t i = 0; i < len; i++) {
    unsigned char c = s[i];
    if (!kHttpToken[c]) return 0;
  }
  return 1;
}

static int IsValidHttpHeaderValue(const char *s, size_t len) {
  for (size_t i = 0; i < len; i++) {
    unsigned char c = s[i];
    if (c == '\r' || c == '\n') return 0;
    if (c < 0x20 && c != '\t') return 0;
    if (c == 0x7f) return 0;
  }
  return 1;
}

static int IsValidHttpStatus(int status) {
  return status >= 100 && status <= 599;
}

// http.parse(raw_request_string)
//     ├─→ {method, uri, version, headers, body, header_size}
//     └─→ nil, error_message
static int LuaHttpParse(lua_State *L) {
  size_t len;
  const char *buf = luaL_checklstring(L, 1, &len);

  struct HttpMessage msg;
  InitHttpMessage(&msg, kHttpRequest);

  int rc = ParseHttpMessage(&msg, buf, len, len);
  if (rc <= 0) {
    DestroyHttpMessage(&msg);
    lua_pushnil(L);
    lua_pushstring(L, rc == 0 ? "incomplete message" : "parse error");
    return 2;
  }

  lua_newtable(L);

  // method
  char method[9] = {0};
  WRITE64LE(method, msg.method);
  lua_pushstring(L, method);
  lua_setfield(L, -2, "method");

  // uri
  lua_pushlstring(L, buf + msg.uri.a, msg.uri.b - msg.uri.a);
  lua_setfield(L, -2, "uri");

  // version
  lua_pushinteger(L, msg.version);
  lua_setfield(L, -2, "version");

  // headers
  lua_newtable(L);
  for (int i = 0; i < kHttpHeadersMax; i++) {
    if (msg.headers[i].a) {
      const char *name = GetHttpHeaderName(i);
      lua_pushlstring(L, buf + msg.headers[i].a,
                      msg.headers[i].b - msg.headers[i].a);
      lua_setfield(L, -2, name);
    }
  }
  for (unsigned i = 0; i < msg.xheaders.n; i++) {
    lua_pushlstring(L, buf + msg.xheaders.p[i].v.a,
                    msg.xheaders.p[i].v.b - msg.xheaders.p[i].v.a);
    lua_pushlstring(L, buf + msg.xheaders.p[i].k.a,
                    msg.xheaders.p[i].k.b - msg.xheaders.p[i].k.a);
    lua_settable(L, -3);
  }
  lua_setfield(L, -2, "headers");

  // body
  if (rc < (int)len) {
    lua_pushlstring(L, buf + rc, len - rc);
    lua_setfield(L, -2, "body");
  }

  // header_size
  lua_pushinteger(L, rc);
  lua_setfield(L, -2, "header_size");

  DestroyHttpMessage(&msg);
  return 1;
}

// http.parse_response(raw_response_string)
//     ├─→ {status, message, version, headers, body, header_size}
//     └─→ nil, error_message
static int LuaHttpParseResponse(lua_State *L) {
  size_t len;
  const char *buf = luaL_checklstring(L, 1, &len);

  struct HttpMessage msg;
  InitHttpMessage(&msg, kHttpResponse);

  int rc = ParseHttpMessage(&msg, buf, len, len);
  if (rc <= 0) {
    DestroyHttpMessage(&msg);
    lua_pushnil(L);
    lua_pushstring(L, rc == 0 ? "incomplete message" : "parse error");
    return 2;
  }

  lua_newtable(L);

  // status
  lua_pushinteger(L, msg.status);
  lua_setfield(L, -2, "status");

  // message
  lua_pushlstring(L, buf + msg.message.a, msg.message.b - msg.message.a);
  lua_setfield(L, -2, "message");

  // version
  lua_pushinteger(L, msg.version);
  lua_setfield(L, -2, "version");

  // headers
  lua_newtable(L);
  for (int i = 0; i < kHttpHeadersMax; i++) {
    if (msg.headers[i].a) {
      const char *name = GetHttpHeaderName(i);
      lua_pushlstring(L, buf + msg.headers[i].a,
                      msg.headers[i].b - msg.headers[i].a);
      lua_setfield(L, -2, name);
    }
  }
  for (unsigned i = 0; i < msg.xheaders.n; i++) {
    lua_pushlstring(L, buf + msg.xheaders.p[i].v.a,
                    msg.xheaders.p[i].v.b - msg.xheaders.p[i].v.a);
    lua_pushlstring(L, buf + msg.xheaders.p[i].k.a,
                    msg.xheaders.p[i].k.b - msg.xheaders.p[i].k.a);
    lua_settable(L, -3);
  }
  lua_setfield(L, -2, "headers");

  // body
  if (rc < (int)len) {
    lua_pushlstring(L, buf + rc, len - rc);
    lua_setfield(L, -2, "body");
  }

  // header_size
  lua_pushinteger(L, rc);
  lua_setfield(L, -2, "header_size");

  DestroyHttpMessage(&msg);
  return 1;
}

// http.format_response({status, headers, body}) -> string
static int LuaHttpFormatResponse(lua_State *L) {
  luaL_checktype(L, 1, LUA_TTABLE);

  lua_getfield(L, 1, "status");
  int status = luaL_optinteger(L, -1, 200);
  lua_pop(L, 1);

  if (!IsValidHttpStatus(status)) {
    return luaL_error(L, "invalid HTTP status code: %d (must be 100-599)", status);
  }

  // Build status line
  char status_line[64];
  snprintf(status_line, sizeof(status_line), "HTTP/1.1 %d %s\r\n",
           status, GetHttpReason(status));
  lua_pushstring(L, status_line);
  int nparts = 1;

  // Headers - collect all header strings
  // First pass: validate all headers before pushing any to the stack
  // This avoids leaving the stack in a bad state if we need to error
  lua_getfield(L, 1, "headers");
  if (lua_istable(L, -1)) {
    int headers_idx = lua_gettop(L);
    int header_count = 0;
    const char *error_msg = NULL;

    // Validation pass
    lua_pushnil(L);
    while (lua_next(L, headers_idx)) {
      size_t key_len, val_len;
      const char *key = lua_tolstring(L, -2, &key_len);
      const char *val = lua_tolstring(L, -1, &val_len);
      lua_pop(L, 1);
      if (key && val) {
        if (!IsValidHttpHeaderName(key, key_len)) {
          error_msg = "invalid header name: contains invalid characters";
          lua_pop(L, 1);  // pop key to end iteration
          break;
        }
        if (!IsValidHttpHeaderValue(val, val_len)) {
          error_msg = "invalid header value: contains CR/LF or control characters";
          lua_pop(L, 1);
          break;
        }
        if (++header_count > HTTP_MAX_HEADERS) {
          error_msg = "too many headers (max 100)";
          lua_pop(L, 1);
          break;
        }
      }
    }
    if (error_msg) {
      return luaL_error(L, "%s", error_msg);
    }

    // Collection pass - now safe to push strings
    lua_pushnil(L);
    while (lua_next(L, headers_idx)) {
      size_t key_len, val_len;
      const char *key = lua_tolstring(L, -2, &key_len);
      const char *val = lua_tolstring(L, -1, &val_len);
      lua_pop(L, 1);
      if (key && val) {
        lua_pushfstring(L, "%s: %s\r\n", key, val);
        lua_insert(L, headers_idx);
        headers_idx++;
        nparts++;
      }
    }
    lua_remove(L, headers_idx);
  } else {
    lua_pop(L, 1);
  }

  // End of headers
  lua_pushstring(L, "\r\n");
  nparts++;

  // Body
  lua_getfield(L, 1, "body");
  if (lua_isstring(L, -1)) {
    nparts++;
  } else {
    lua_pop(L, 1);
  }

  lua_concat(L, nparts);
  return 1;
}

// http.format_request({method, uri, headers, body}) -> string
static int LuaHttpFormatRequest(lua_State *L) {
  luaL_checktype(L, 1, LUA_TTABLE);

  lua_getfield(L, 1, "method");
  size_t method_len;
  const char *method = luaL_optlstring(L, -1, "GET", &method_len);
  if (!IsValidHttpHeaderName(method, method_len)) {
    return luaL_error(L, "invalid HTTP method: contains invalid characters");
  }
  lua_pop(L, 1);

  lua_getfield(L, 1, "uri");
  size_t uri_len;
  const char *uri = luaL_checklstring(L, -1, &uri_len);
  if (!IsValidHttpHeaderValue(uri, uri_len)) {
    return luaL_error(L, "invalid URI: contains CR/LF or control characters");
  }
  lua_pop(L, 1);

  // Build request line
  lua_pushfstring(L, "%s %s HTTP/1.1\r\n", method, uri);
  int nparts = 1;

  // Headers - collect all header strings
  // First pass: validate all headers before pushing any to the stack
  lua_getfield(L, 1, "headers");
  if (lua_istable(L, -1)) {
    int headers_idx = lua_gettop(L);
    int header_count = 0;
    const char *error_msg = NULL;

    // Validation pass
    lua_pushnil(L);
    while (lua_next(L, headers_idx)) {
      size_t key_len, val_len;
      const char *key = lua_tolstring(L, -2, &key_len);
      const char *val = lua_tolstring(L, -1, &val_len);
      lua_pop(L, 1);
      if (key && val) {
        if (!IsValidHttpHeaderName(key, key_len)) {
          error_msg = "invalid header name: contains invalid characters";
          lua_pop(L, 1);
          break;
        }
        if (!IsValidHttpHeaderValue(val, val_len)) {
          error_msg = "invalid header value: contains CR/LF or control characters";
          lua_pop(L, 1);
          break;
        }
        if (++header_count > HTTP_MAX_HEADERS) {
          error_msg = "too many headers (max 100)";
          lua_pop(L, 1);
          break;
        }
      }
    }
    if (error_msg) {
      return luaL_error(L, "%s", error_msg);
    }

    // Collection pass - now safe to push strings
    lua_pushnil(L);
    while (lua_next(L, headers_idx)) {
      size_t key_len, val_len;
      const char *key = lua_tolstring(L, -2, &key_len);
      const char *val = lua_tolstring(L, -1, &val_len);
      lua_pop(L, 1);
      if (key && val) {
        lua_pushfstring(L, "%s: %s\r\n", key, val);
        lua_insert(L, headers_idx);
        headers_idx++;
        nparts++;
      }
    }
    lua_remove(L, headers_idx);
  } else {
    lua_pop(L, 1);
  }

  // End of headers
  lua_pushstring(L, "\r\n");
  nparts++;

  // Body
  lua_getfield(L, 1, "body");
  if (lua_isstring(L, -1)) {
    nparts++;
  } else {
    lua_pop(L, 1);
  }

  lua_concat(L, nparts);
  return 1;
}

// http.reason(status_code) -> string
static int LuaHttpReason(lua_State *L) {
  int status = luaL_checkinteger(L, 1);
  lua_pushstring(L, GetHttpReason(status));
  return 1;
}

// http.header_name(header_constant) -> string or nil
static int LuaHttpHeaderName(lua_State *L) {
  int header = luaL_checkinteger(L, 1);
  if (header >= 0 && header < kHttpHeadersMax) {
    lua_pushstring(L, GetHttpHeaderName(header));
    return 1;
  }
  lua_pushnil(L);
  return 1;
}

static int ParsePort(const char *s, uint16_t *port) {
  if (!s || !*s) return -1;
  char *endptr;
  long p = strtol(s, &endptr, 10);
  if (*endptr != '\0') return -1;
  if (p <= 0 || p > 65535) return -1;
  *port = (uint16_t)p;
  return 0;
}

// Parse address string like ":8080" or "127.0.0.1:8080"
// Returns host IP (0 for INADDR_ANY) and port, or -1 on error
static int ParseAddr(const char *addr, uint32_t *ip, uint16_t *port) {
  const char *colon = strchr(addr, ':');
  if (!colon) {
    // Just a port number
    *ip = 0;
    return ParsePort(addr, port);
  }
  if (colon == addr) {
    // ":8080" format
    *ip = 0;
    return ParsePort(colon + 1, port);
  }
  // "host:port" format
  char host[64];
  size_t hostlen = colon - addr;
  if (hostlen >= sizeof(host)) return -1;
  memcpy(host, addr, hostlen);
  host[hostlen] = 0;

  // Parse IP address
  struct in_addr inaddr;
  if (inet_pton(AF_INET, host, &inaddr) != 1) {
    return -1;
  }
  *ip = inaddr.s_addr;
  return ParsePort(colon + 1, port);
}

// http.serve(addr, handler) or http.serve(options, handler)
// Starts an HTTP server that calls handler for each request
//
// addr: ":8080" or "127.0.0.1:8080"
// handler: function(request) -> response_table
//
// options table:
//   addr: address string (required)
//   reuseaddr: boolean (default true)
//   backlog: int (default 128)
//   timeout: request timeout in seconds (default 30)
//
static int LuaHttpServe(lua_State *L) {
  uint32_t ip = 0;
  uint16_t port = 0;
  int reuseaddr = 1;
  int backlog = 128;
  int timeout_sec = 30;
  int handler_idx;

  // Parse arguments
  if (lua_istable(L, 1)) {
    // Options table
    lua_getfield(L, 1, "addr");
    const char *addr = luaL_checkstring(L, -1);
    if (ParseAddr(addr, &ip, &port) != 0) {
      return luaL_error(L, "invalid address: %s", addr);
    }
    lua_pop(L, 1);

    lua_getfield(L, 1, "reuseaddr");
    if (!lua_isnil(L, -1)) {
      reuseaddr = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, 1, "backlog");
    if (!lua_isnil(L, -1)) {
      backlog = luaL_checkinteger(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, 1, "timeout");
    if (!lua_isnil(L, -1)) {
      int t = luaL_checkinteger(L, -1);
      if (t < 0) {
        return luaL_error(L, "timeout must be non-negative");
      }
      timeout_sec = t;
    }
    lua_pop(L, 1);

    luaL_checktype(L, 2, LUA_TFUNCTION);
    handler_idx = 2;
  } else {
    // Simple form: serve(addr, handler)
    const char *addr = luaL_checkstring(L, 1);
    if (ParseAddr(addr, &ip, &port) != 0) {
      return luaL_error(L, "invalid address: %s", addr);
    }
    luaL_checktype(L, 2, LUA_TFUNCTION);
    handler_idx = 2;
  }

  // Create socket
  int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (fd == -1) {
    return luaL_error(L, "socket: %s", strerror(errno));
  }

  // Set SO_REUSEADDR
  if (reuseaddr) {
    int optval = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
  }

  // Bind
  struct sockaddr_in saddr = {0};
  saddr.sin_family = AF_INET;
  saddr.sin_addr.s_addr = ip;
  saddr.sin_port = htons(port);
  if (bind(fd, (struct sockaddr *)&saddr, sizeof(saddr)) == -1) {
    close(fd);
    return luaL_error(L, "bind: %s", strerror(errno));
  }

  // Listen
  if (listen(fd, backlog) == -1) {
    close(fd);
    return luaL_error(L, "listen: %s", strerror(errno));
  }

  // Allocate receive buffer
  char *buf = malloc(HTTP_RECV_BUFSIZE);
  if (!buf) {
    close(fd);
    return luaL_error(L, "out of memory");
  }

  // Accept loop
  for (;;) {
    struct sockaddr_in client_addr;
    uint32_t client_len = sizeof(client_addr);
    int client = accept(fd, (struct sockaddr *)&client_addr, &client_len);
    if (client == -1) {
      if (errno == EINTR) continue;
      free(buf);
      close(fd);
      return luaL_error(L, "accept: %s", strerror(errno));
    }

    // Set receive timeout on client socket
    if (timeout_sec > 0) {
      struct timeval tv = {.tv_sec = timeout_sec, .tv_usec = 0};
      setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    }

    // Read request headers
    size_t buflen = 0;
    int parsed = 0;
    struct HttpMessage msg;
    InitHttpMessage(&msg, kHttpRequest);

    while (buflen < HTTP_RECV_BUFSIZE - 1) {
      size_t remaining = HTTP_RECV_BUFSIZE - 1 - buflen;
      if (remaining == 0) break;
      ssize_t n = recv(client, buf + buflen, remaining, 0);
      if (n <= 0) break;
      buflen += n;
      buf[buflen] = 0;

      parsed = ParseHttpMessage(&msg, buf, buflen, buflen);
      if (parsed > 0) break;  // Got complete headers
      if (parsed < 0) break;  // Parse error
    }

    if (parsed <= 0) {
      // Send error response
      const char *err_resp = "HTTP/1.1 400 Bad Request\r\n"
                             "Content-Length: 11\r\n\r\nBad Request";
      send(client, err_resp, strlen(err_resp), 0);
      DestroyHttpMessage(&msg);
      close(client);
      continue;
    }

    // Handle Content-Length: read full request body
    int64_t content_length = 0;
    if (msg.headers[kHttpContentLength].a) {
      content_length = ParseContentLength(
          buf + msg.headers[kHttpContentLength].a,
          msg.headers[kHttpContentLength].b - msg.headers[kHttpContentLength].a);
      if (content_length < 0) {
        const char *err_resp = "HTTP/1.1 400 Bad Request\r\n"
                               "Content-Length: 22\r\n\r\nInvalid Content-Length";
        send(client, err_resp, strlen(err_resp), 0);
        DestroyHttpMessage(&msg);
        close(client);
        continue;
      }
    }

    // Read remaining body if Content-Length specified
    size_t body_received = buflen - parsed;
    if (content_length > 0 && body_received < (size_t)content_length) {
      // Check if body fits in our buffer
      if (parsed + (size_t)content_length > HTTP_RECV_BUFSIZE - 1) {
        const char *err_resp = "HTTP/1.1 413 Payload Too Large\r\n"
                               "Content-Length: 17\r\n\r\nPayload Too Large";
        send(client, err_resp, strlen(err_resp), 0);
        DestroyHttpMessage(&msg);
        close(client);
        continue;
      }
      // Read remaining body
      while (body_received < (size_t)content_length) {
        size_t remaining = (size_t)content_length - body_received;
        ssize_t n = recv(client, buf + buflen, remaining, 0);
        if (n <= 0) break;
        buflen += n;
        body_received += n;
      }
      buf[buflen] = 0;
    }

    // Build request table for Lua
    lua_pushvalue(L, handler_idx);  // Push handler function

    lua_newtable(L);  // Request table

    // method
    char method[9] = {0};
    WRITE64LE(method, msg.method);
    lua_pushstring(L, method);
    lua_setfield(L, -2, "method");

    // uri
    lua_pushlstring(L, buf + msg.uri.a, msg.uri.b - msg.uri.a);
    lua_setfield(L, -2, "uri");

    // version
    lua_pushinteger(L, msg.version);
    lua_setfield(L, -2, "version");

    // headers
    lua_newtable(L);
    for (int i = 0; i < kHttpHeadersMax; i++) {
      if (msg.headers[i].a) {
        const char *name = GetHttpHeaderName(i);
        lua_pushlstring(L, buf + msg.headers[i].a,
                        msg.headers[i].b - msg.headers[i].a);
        lua_setfield(L, -2, name);
      }
    }
    for (unsigned i = 0; i < msg.xheaders.n; i++) {
      lua_pushlstring(L, buf + msg.xheaders.p[i].v.a,
                      msg.xheaders.p[i].v.b - msg.xheaders.p[i].v.a);
      lua_pushlstring(L, buf + msg.xheaders.p[i].k.a,
                      msg.xheaders.p[i].k.b - msg.xheaders.p[i].k.a);
      lua_settable(L, -3);
    }
    lua_setfield(L, -2, "headers");

    // body
    if (parsed < (int)buflen) {
      lua_pushlstring(L, buf + parsed, buflen - parsed);
      lua_setfield(L, -2, "body");
    }

    // client_ip
    char ipstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, ipstr, sizeof(ipstr));
    lua_pushstring(L, ipstr);
    lua_setfield(L, -2, "client_ip");

    // client_port
    lua_pushinteger(L, ntohs(client_addr.sin_port));
    lua_setfield(L, -2, "client_port");

    DestroyHttpMessage(&msg);

    // Call handler
    if (lua_pcall(L, 1, 1, 0) != LUA_OK) {
      // Handler error - send 500
      const char *err = lua_tostring(L, -1);
      fprintf(stderr, "http.serve handler error: %s\n", err ? err : "unknown");
      lua_pop(L, 1);
      const char *err_resp = "HTTP/1.1 500 Internal Server Error\r\n"
                             "Content-Length: 21\r\n\r\nInternal Server Error";
      send(client, err_resp, strlen(err_resp), 0);
      close(client);
      continue;
    }

    // Format and send response
    if (lua_istable(L, -1)) {
      // Get status
      lua_getfield(L, -1, "status");
      int status = luaL_optinteger(L, -1, 200);
      lua_pop(L, 1);

      // Validate status code
      if (!IsValidHttpStatus(status)) {
        status = 500;  // Default to 500 for invalid status
      }

      // Get body first to calculate Content-Length
      lua_getfield(L, -1, "body");
      size_t body_len = 0;
      const char *body = NULL;
      if (lua_isstring(L, -1)) {
        body = lua_tolstring(L, -1, &body_len);
      }

      // Build response using format_response logic
      char status_line[64];
      snprintf(status_line, sizeof(status_line), "HTTP/1.1 %d %s\r\n",
               status, GetHttpReason(status));
      send(client, status_line, strlen(status_line), 0);

      // Send headers
      lua_getfield(L, -2, "headers");
      int has_content_length = 0;
      int header_count = 0;
      if (lua_istable(L, -1)) {
        lua_pushnil(L);
        while (lua_next(L, -2)) {
          size_t key_len, val_len;
          const char *key = lua_tolstring(L, -2, &key_len);
          const char *val = lua_tolstring(L, -1, &val_len);
          lua_pop(L, 1);
          if (key && val) {
            // Skip headers with invalid names or values (security: prevent CRLF injection)
            if (!IsValidHttpHeaderName(key, key_len) ||
                !IsValidHttpHeaderValue(val, val_len)) {
              continue;
            }
            // Limit header count
            if (++header_count > HTTP_MAX_HEADERS) {
              break;
            }
            if (strcasecmp(key, "Content-Length") == 0) {
              has_content_length = 1;
            }
            // Use lua_pushfstring for dynamic allocation
            const char *header = lua_pushfstring(L, "%s: %s\r\n", key, val);
            send(client, header, strlen(header), 0);
            lua_pop(L, 1);  // pop the formatted header string
          }
        }
      }
      lua_pop(L, 1);  // pop headers

      // Add Content-Length if not present and body exists
      if (body && !has_content_length) {
        char cl[64];
        snprintf(cl, sizeof(cl), "Content-Length: %zu\r\n", body_len);
        send(client, cl, strlen(cl), 0);
      }

      // End headers
      send(client, "\r\n", 2, 0);

      // Send body
      if (body) {
        send(client, body, body_len, 0);
      }
      lua_pop(L, 1);  // pop body
    }
    lua_pop(L, 1);  // pop response table

    close(client);
  }

  // Never reached, but clean up anyway
  free(buf);
  close(fd);
  return 0;
}

static const luaL_Reg kLuaHttp[] = {
    {"parse", LuaHttpParse},
    {"parse_response", LuaHttpParseResponse},
    {"format_response", LuaHttpFormatResponse},
    {"format_request", LuaHttpFormatRequest},
    {"reason", LuaHttpReason},
    {"header_name", LuaHttpHeaderName},
    {"serve", LuaHttpServe},
    {0},
};

int LuaHttp(lua_State *L) {
  luaL_newlib(L, kLuaHttp);

  // HTTP method constants
  lua_pushinteger(L, kHttpGet);
  lua_setfield(L, -2, "GET");
  lua_pushinteger(L, kHttpPost);
  lua_setfield(L, -2, "POST");
  lua_pushinteger(L, kHttpPut);
  lua_setfield(L, -2, "PUT");
  lua_pushinteger(L, kHttpDelete);
  lua_setfield(L, -2, "DELETE");
  lua_pushinteger(L, kHttpHead);
  lua_setfield(L, -2, "HEAD");
  lua_pushinteger(L, kHttpOptions);
  lua_setfield(L, -2, "OPTIONS");
  lua_pushinteger(L, kHttpConnect);
  lua_setfield(L, -2, "CONNECT");
  lua_pushinteger(L, kHttpTrace);
  lua_setfield(L, -2, "TRACE");

  // Common status codes
  lua_pushinteger(L, 200);
  lua_setfield(L, -2, "OK");
  lua_pushinteger(L, 201);
  lua_setfield(L, -2, "CREATED");
  lua_pushinteger(L, 204);
  lua_setfield(L, -2, "NO_CONTENT");
  lua_pushinteger(L, 301);
  lua_setfield(L, -2, "MOVED_PERMANENTLY");
  lua_pushinteger(L, 302);
  lua_setfield(L, -2, "FOUND");
  lua_pushinteger(L, 304);
  lua_setfield(L, -2, "NOT_MODIFIED");
  lua_pushinteger(L, 400);
  lua_setfield(L, -2, "BAD_REQUEST");
  lua_pushinteger(L, 401);
  lua_setfield(L, -2, "UNAUTHORIZED");
  lua_pushinteger(L, 403);
  lua_setfield(L, -2, "FORBIDDEN");
  lua_pushinteger(L, 404);
  lua_setfield(L, -2, "NOT_FOUND");
  lua_pushinteger(L, 405);
  lua_setfield(L, -2, "METHOD_NOT_ALLOWED");
  lua_pushinteger(L, 500);
  lua_setfield(L, -2, "INTERNAL_SERVER_ERROR");
  lua_pushinteger(L, 502);
  lua_setfield(L, -2, "BAD_GATEWAY");
  lua_pushinteger(L, 503);
  lua_setfield(L, -2, "SERVICE_UNAVAILABLE");

  // Header constants
  lua_pushinteger(L, kHttpHost);
  lua_setfield(L, -2, "HOST");
  lua_pushinteger(L, kHttpContentType);
  lua_setfield(L, -2, "CONTENT_TYPE");
  lua_pushinteger(L, kHttpContentLength);
  lua_setfield(L, -2, "CONTENT_LENGTH");
  lua_pushinteger(L, kHttpConnection);
  lua_setfield(L, -2, "CONNECTION");
  lua_pushinteger(L, kHttpAccept);
  lua_setfield(L, -2, "ACCEPT");
  lua_pushinteger(L, kHttpUserAgent);
  lua_setfield(L, -2, "USER_AGENT");

  return 1;
}
