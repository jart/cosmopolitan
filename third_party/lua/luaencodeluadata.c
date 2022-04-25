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
#include "libc/bits/bits.h"
#include "libc/fmt/itoa.h"
#include "libc/math.h"
#include "libc/stdio/append.internal.h"
#include "third_party/lua/cosmo.h"
#include "third_party/lua/lauxlib.h"
#include "third_party/lua/lua.h"

int LuaEncodeLuaData(lua_State *L, char **buf, int level, char *numformat,
                     int idx) {
  char *s;
  int ktype;
  lua_Integer i;
  size_t tbllen, buflen, slen;
  char ibuf[21], fmt[] = "%.14g";
  if (level > 0) {
    switch (lua_type(L, idx)) {
      case LUA_TNIL:
        appendw(buf, READ32LE("nil"));
        return 0;
      case LUA_TSTRING:
        s = lua_tolstring(L, idx, &slen);
        EscapeLuaString(s, slen, buf);
        return 0;
      case LUA_TFUNCTION:
        appendf(buf, "\"func@%p\"", lua_touserdata(L, idx));
        return 0;
      case LUA_TUSERDATA:
        appendf(buf, "\"udata@%p\"", lua_touserdata(L, idx));
        return 0;
      case LUA_TLIGHTUSERDATA:
        appendf(buf, "\"light@%p\"", lua_touserdata(L, idx));
        return 0;
      case LUA_TTHREAD:
        appendf(buf, "\"thread@%p\"", lua_touserdata(L, idx));
        return 0;
      case LUA_TNUMBER:
        if (lua_isinteger(L, idx)) {
          appendd(buf, ibuf,
                  FormatInt64(ibuf, luaL_checkinteger(L, idx)) - ibuf);
        } else {
          // TODO(jart): replace this api
          while (*numformat == '%' || *numformat == '.' ||
                 isdigit(*numformat)) {
            ++numformat;
          }
          switch (*numformat) {
            case 'a':
            case 'g':
            case 'f':
              fmt[4] = *numformat;
              break;
            default:
              return luaL_error(L, "numformat string not allowed");
          }
          appendf(buf, fmt, lua_tonumber(L, idx));
        }
        return 0;
      case LUA_TBOOLEAN:
        if (lua_toboolean(L, idx)) {
          appendw(buf, READ32LE("true"));
        } else {
          appendw(buf, READ64LE("false\0\0"));
        }
        return 0;
      case LUA_TTABLE:
        i = 0;
        appendw(buf, '{');
        lua_pushnil(L);  // push the first key
        while (lua_next(L, -2) != 0) {
          ktype = lua_type(L, -2);
          if (i++ > 0) appendw(buf, ',');
          if (ktype != LUA_TNUMBER || lua_tointeger(L, -2) != i) {
            appendw(buf, '[');
            lua_pushvalue(L, -2);  // table/-4, key/-3, value/-2, key/-1
            LuaEncodeLuaData(L, buf, level - 1, numformat, -1);
            lua_remove(L, -1);  // remove copied key: table/-3, key/-2, value/-1
            appendw(buf, ']' | '=' << 010);
          }
          LuaEncodeLuaData(L, buf, level - 1, numformat, -1);
          lua_pop(L, 1);  // table/-2, key/-1
        }
        // stack: table/-1, as the key was popped by lua_next
        appendw(buf, '}');
        return 0;
      default:
        luaL_error(L, "can't serialize value of this type");
        unreachable;
    }
  } else {
    luaL_error(L, "too many nested tables");
    unreachable;
  }
}
