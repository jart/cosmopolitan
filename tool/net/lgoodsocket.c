/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
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
#include "tool/net/lgoodsocket.h"
#include "libc/calls/struct/timeval.h"
#include "libc/errno.h"
#include "libc/sock/goodsocket.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/sock.h"
#include "third_party/lua/lauxlib.h"
#include "third_party/lua/lua.h"

static int SysError(lua_State *L, const char *what) {
  lua_pushnil(L);
  lua_pushfstring(L, "%s: %s", what, strerror(errno));
  return 2;
}

// goodsocket.socket(family, type, protocol, isserver, [timeout])
//   -> fd | nil, error
//
// Creates a socket with modern goodness enabled.
//
// Parameters:
//   family: Socket family (e.g., AF_INET, AF_INET6, AF_UNIX)
//   type: Socket type (e.g., SOCK_STREAM, SOCK_DGRAM)
//   protocol: Protocol (e.g., IPPROTO_TCP, IPPROTO_UDP, 0 for default)
//   isserver: Boolean indicating if this is a server socket
//   timeout: Optional timeout in seconds (number) or nil
//            - positive: sets SO_RCVTIMEO and SO_SNDTIMEO
//            - negative: enables SO_KEEPALIVE with -timeout as interval
//            - nil: no timeout settings
//
// Returns:
//   fd: File descriptor on success
//   nil, error: On failure
static int LuaGoodSocketSocket(lua_State *L) {
  int family = luaL_checkinteger(L, 1);
  int type = luaL_checkinteger(L, 2);
  int protocol = luaL_checkinteger(L, 3);
  bool isserver = lua_toboolean(L, 4);

  struct timeval tv;
  struct timeval *timeout_ptr = NULL;

  if (!lua_isnoneornil(L, 5)) {
    lua_Number timeout_num = luaL_checknumber(L, 5);
    tv.tv_sec = (int64_t)timeout_num;
    tv.tv_usec = (int64_t)((timeout_num - tv.tv_sec) * 1000000);
    timeout_ptr = &tv;
  }

  int fd = GoodSocket(family, type, protocol, isserver, timeout_ptr);
  if (fd == -1)
    return SysError(L, "socket");

  lua_pushinteger(L, fd);
  return 1;
}

static const luaL_Reg kLuaGoodSocket[] = {
    {"socket", LuaGoodSocketSocket},
    {0},
};

int LuaGoodSocket(lua_State *L) {
  luaL_newlib(L, kLuaGoodSocket);

  // Socket families (AF_*)
  lua_pushinteger(L, AF_INET);
  lua_setfield(L, -2, "AF_INET");
  lua_pushinteger(L, AF_INET6);
  lua_setfield(L, -2, "AF_INET6");
  lua_pushinteger(L, AF_UNIX);
  lua_setfield(L, -2, "AF_UNIX");
  lua_pushinteger(L, AF_UNSPEC);
  lua_setfield(L, -2, "AF_UNSPEC");

  // Socket types (SOCK_*)
  lua_pushinteger(L, SOCK_STREAM);
  lua_setfield(L, -2, "SOCK_STREAM");
  lua_pushinteger(L, SOCK_DGRAM);
  lua_setfield(L, -2, "SOCK_DGRAM");
  lua_pushinteger(L, SOCK_RAW);
  lua_setfield(L, -2, "SOCK_RAW");
  lua_pushinteger(L, SOCK_RDM);
  lua_setfield(L, -2, "SOCK_RDM");
  lua_pushinteger(L, SOCK_SEQPACKET);
  lua_setfield(L, -2, "SOCK_SEQPACKET");

  // IP protocols (IPPROTO_*)
  lua_pushinteger(L, IPPROTO_IP);
  lua_setfield(L, -2, "IPPROTO_IP");
  lua_pushinteger(L, IPPROTO_TCP);
  lua_setfield(L, -2, "IPPROTO_TCP");
  lua_pushinteger(L, IPPROTO_UDP);
  lua_setfield(L, -2, "IPPROTO_UDP");
  lua_pushinteger(L, IPPROTO_ICMP);
  lua_setfield(L, -2, "IPPROTO_ICMP");
  lua_pushinteger(L, IPPROTO_IPV6);
  lua_setfield(L, -2, "IPPROTO_IPV6");
  lua_pushinteger(L, IPPROTO_ICMPV6);
  lua_setfield(L, -2, "IPPROTO_ICMPV6");
  lua_pushinteger(L, IPPROTO_RAW);
  lua_setfield(L, -2, "IPPROTO_RAW");

  return 1;
}
