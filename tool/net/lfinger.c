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
#include "net/finger/finger.h"
#include "third_party/lua/lauxlib.h"

// finger.DescribeSyn(syn_packet_bytes:str)
//     ├─→ description:str
//     └─→ nil, error:str
static int LuaDescribeSyn(lua_State *L) {
  char *q, *r;
  size_t n, m;
  const char *p;
  luaL_Buffer b;
  if (!lua_isnoneornil(L, 1)) {
    p = luaL_checklstring(L, 1, &n);
    m = 128;
    q = luaL_buffinitsize(L, &b, m);
    if ((r = DescribeSyn(q, m, p, n))) {
      luaL_pushresultsize(&b, r - q);
      return 1;
    } else {
      lua_pushnil(L);
      lua_pushstring(L, "bad syn packet");
      return 2;
    }
  } else {
    return lua_gettop(L);
  }
}

// finger.FingerSyn(syn_packet_bytes:str)
//     ├─→ synfinger:uint32
//     └─→ nil, error:str
static int LuaFingerSyn(lua_State *L) {
  size_t n;
  uint32_t x;
  const char *p;
  if (!lua_isnoneornil(L, 1)) {
    p = luaL_checklstring(L, 1, &n);
    if ((x = FingerSyn(p, n))) {
      lua_pushinteger(L, x);
      return 1;
    } else {
      lua_pushnil(L);
      lua_pushstring(L, "bad syn packet");
      return 2;
    }
  } else {
    return lua_gettop(L);
  }
}

// finger.GetSynFingerOs(synfinger:uint32)
//     ├─→ osname:str
//     └─→ nil, error:str
static int LuaGetSynFingerOs(lua_State *L) {
  int os;
  if (!lua_isnoneornil(L, 1)) {
    if ((os = GetSynFingerOs(luaL_checkinteger(L, 1)))) {
      lua_pushstring(L, GetOsName(os));
      return 1;
    } else {
      lua_pushnil(L);
      lua_pushstring(L, "unknown syn os fingerprint");
      return 2;
    }
  } else {
    return lua_gettop(L);
  }
}

static const luaL_Reg kLuaFinger[] = {
    {"DescribeSyn", LuaDescribeSyn},        //
    {"FingerSyn", LuaFingerSyn},            //
    {"GetSynFingerOs", LuaGetSynFingerOs},  //
    {0},                                    //
};

int LuaFinger(lua_State *L) {
  luaL_newlib(L, kLuaFinger);
  return 1;
}
