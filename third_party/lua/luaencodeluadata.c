/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/math.h"
#include "libc/stdio/append.internal.h"
#include "third_party/lua/cosmo.h"
#include "third_party/lua/lauxlib.h"
#include "third_party/lua/lua.h"

int LuaEncodeLuaData(lua_State *L, char **buf, int level, char *numformat) {
  size_t idx = -1;
  size_t tbllen, buflen, slen;
  char *s;
  int ktype;
  int t = lua_type(L, idx);
  if (level < 0) return luaL_argerror(L, 1, "too many nested tables");
  if (LUA_TSTRING == t) {
    s = lua_tolstring(L, idx, &slen);
    EscapeLuaString(s, slen, buf);
  } else if (LUA_TNUMBER == t) {
    appendf(buf, numformat, lua_tonumber(L, idx));
  } else if (LUA_TBOOLEAN == t) {
    appends(buf, lua_toboolean(L, idx) ? "true" : "false");
  } else if (LUA_TTABLE == t) {
    appendw(buf, '{');
    int i = 0;
    lua_pushnil(L);  // push the first key
    while (lua_next(L, -2) != 0) {
      ktype = lua_type(L, -2);
      if (ktype == LUA_TTABLE)
        return luaL_argerror(L, 1, "can't serialize key of this type");
      if (i++ > 0) appendd(buf, ",", 1);
      if (ktype != LUA_TNUMBER || floor(lua_tonumber(L, -2)) != i) {
        appendw(buf, '[');
        lua_pushvalue(L, -2);  // table/-4, key/-3, value/-2, key/-1
        LuaEncodeLuaData(L, buf, level, numformat);
        lua_remove(L, -1);  // remove copied key: table/-3, key/-2, value/-1
        appendw(buf, ']' | '=' << 010);
      }
      LuaEncodeLuaData(L, buf, level - 1, numformat);
      lua_pop(L, 1);  // table/-2, key/-1
    }
    // stack: table/-1, as the key was popped by lua_next
    appendw(buf, '}');
  } else if (LUA_TNIL == t) {
    appendd(buf, "nil", 3);
  } else {
    return luaL_argerror(L, 1, "can't serialize value of this type");
  }
  return 0;
}
