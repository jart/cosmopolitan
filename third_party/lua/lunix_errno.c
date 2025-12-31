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
#include "third_party/lua/lunix_internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/nt/runtime.h"
#include "libc/str/str.h"

////////////////////////////////////////////////////////////////////////////////
// unix.Errno object

static struct UnixErrno *GetUnixErrno(lua_State *L) {
  return luaL_checkudata(L, 1, "unix.Errno");
}

// unix.Errno:errno()
//     └─→ errno:int
static int LuaUnixErrnoErrno(lua_State *L) {
  return ReturnInteger(L, GetUnixErrno(L)->errno_);
}

static int LuaUnixErrnoWinerr(lua_State *L) {
  return ReturnInteger(L, GetUnixErrno(L)->winerr);
}

static int LuaUnixErrnoName(lua_State *L) {
  return ReturnString(L, _strerrno(GetUnixErrno(L)->errno_));
}

static int LuaUnixErrnoDoc(lua_State *L) {
  return ReturnString(L, _strerdoc(GetUnixErrno(L)->errno_));
}

static int LuaUnixErrnoCall(lua_State *L) {
  return ReturnString(L, GetUnixErrno(L)->call);
}

static int LuaUnixErrnoToString(lua_State *L) {
  char msg[256];
  struct UnixErrno *e;
  e = GetUnixErrno(L);
  if (e->call) {
    if (IsWindows()) SetLastError(e->winerr);
    strerror_r(e->errno_, msg, sizeof(msg));
    lua_pushfstring(L, "%s() failed: %s", e->call, msg);
  } else {
    lua_pushstring(L, _strerrno(e->errno_));
  }
  return 1;
}

static const luaL_Reg kLuaUnixErrnoMeth[] = {
    {"errno", LuaUnixErrnoErrno},    //
    {"winerr", LuaUnixErrnoWinerr},  //
    {"name", LuaUnixErrnoName},      //
    {"call", LuaUnixErrnoCall},      //
    {"doc", LuaUnixErrnoDoc},        //
    {0},                             //
};

static const luaL_Reg kLuaUnixErrnoMeta[] = {
    {"__tostring", LuaUnixErrnoToString},  //
    {0},                                   //
};

void LuaUnixErrnoObj(lua_State *L) {
  luaL_newmetatable(L, "unix.Errno");
  luaL_setfuncs(L, kLuaUnixErrnoMeta, 0);
  luaL_newlibtable(L, kLuaUnixErrnoMeth);
  luaL_setfuncs(L, kLuaUnixErrnoMeth, 0);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);
}
