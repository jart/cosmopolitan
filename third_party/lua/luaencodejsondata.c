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
#include "libc/runtime/gc.internal.h"
#include "libc/stdio/append.internal.h"
#include "net/http/escape.h"
#include "third_party/lua/cosmo.h"
#include "third_party/lua/lauxlib.h"
#include "third_party/lua/lua.h"

int LuaEncodeJsonData(lua_State *L, char **buf, int level, char *numformat) {
  size_t idx = -1;
  size_t tbllen, buflen;
  bool isarray;
  int t = lua_type(L, idx);
  if (level < 0) return luaL_argerror(L, 1, "too many nested tables");
  if (LUA_TSTRING == t) {
    appendw(buf, '"');
    appends(buf, gc(EscapeJsStringLiteral(lua_tostring(L, idx), -1, 0)));
    appendw(buf, '"');
  } else if (LUA_TNUMBER == t) {
    appendf(buf, numformat, lua_tonumber(L, idx));
  } else if (LUA_TBOOLEAN == t) {
    appends(buf, lua_toboolean(L, idx) ? "true" : "false");
  } else if (LUA_TTABLE == t) {
    tbllen = lua_rawlen(L, idx);
    // encode tables with numeric indices and empty tables as arrays
    isarray = tbllen > 0 ||                              // integer keys present
              (lua_pushnil(L), lua_next(L, -2) == 0) ||  // no non-integer keys
              (lua_pop(L, 2), false);  // pop key/value pushed by lua_next
    appendw(buf, isarray ? '[' : '{');
    if (isarray) {
      for (int i = 1; i <= tbllen; i++) {
        if (i > 1) appendw(buf, ',');
        lua_rawgeti(L, -1, i);  // table/-2, value/-1
        LuaEncodeJsonData(L, buf, level - 1, numformat);
        lua_pop(L, 1);
      }
    } else {
      int i = 1;
      lua_pushnil(L);  // push the first key
      while (lua_next(L, -2) != 0) {
        if (!lua_isstring(L, -2))
          return luaL_argerror(L, 1, "expected number or string as key value");
        if (i++ > 1) appendw(buf, ',');
        appendw(buf, '"');
        if (lua_type(L, -2) == LUA_TSTRING) {
          appends(buf, gc(EscapeJsStringLiteral(lua_tostring(L, -2), -1, 0)));
        } else {
          // we'd still prefer to use lua_tostring on a numeric index, but can't
          // use it in-place, as it breaks lua_next (changes numeric key to a
          // string)
          lua_pushvalue(L, -2);  // table/-4, key/-3, value/-2, key/-1
          appends(buf, lua_tostring(L, idx));  // use the copy
          lua_remove(L, -1);  // remove copied key: table/-3, key/-2, value/-1
        }
        appendw(buf, '"' | ':' << 010);
        LuaEncodeJsonData(L, buf, level - 1, numformat);
        lua_pop(L, 1);  // table/-2, key/-1
      }
      // stack: table/-1, as the key was popped by lua_next
    }
    appendw(buf, isarray ? ']' : '}');
  } else if (LUA_TNIL == t) {
    appendd(buf, "null", 4);
  } else {
    return luaL_argerror(L, 1, "can't serialize value of this type");
  }
  return 0;
}
