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
#include "libc/assert.h"
#include "libc/bits/bits.h"
#include "libc/fmt/itoa.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/stdio/append.internal.h"
#include "libc/x/x.h"
#include "third_party/lua/cosmo.h"
#include "third_party/lua/lauxlib.h"
#include "third_party/lua/lctype.h"
#include "third_party/lua/lua.h"
#include "third_party/lua/visitor.h"

static bool IsLuaIdentifier(lua_State *L, int idx) {
  size_t i, n;
  const char *p;
  p = luaL_checklstring(L, idx, &n);
  if (!lislalpha(p[0])) return false;
  for (i = 1; i < n; ++i) {
    if (!lislalnum(p[i])) return false;
  }
  return true;
}

// TODO: Can we be smarter with lua_rawlen?
static bool IsLuaArray(lua_State *L) {
  int i;
  lua_pushnil(L);
  for (i = 1; lua_next(L, -2); ++i) {
    if (!lua_isinteger(L, -2) || lua_tointeger(L, -2) != i) {
      lua_pop(L, 2);
      return false;
    }
    lua_pop(L, 1);
  }
  return true;
}

static int LuaEncodeLuaDataImpl(lua_State *L, char **buf, int level,
                                char *numformat, int idx,
                                struct LuaVisited *visited) {
  char *s;
  bool isarray;
  lua_Integer i;
  int ktype, vtype;
  size_t tbllen, buflen, slen;
  char ibuf[24], fmt[] = "%.14g";
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
        appendf(buf, "\"func@%p\"", lua_topointer(L, idx));
        return 0;

      case LUA_TLIGHTUSERDATA:
        appendf(buf, "\"light@%p\"", lua_topointer(L, idx));
        return 0;

      case LUA_TTHREAD:
        appendf(buf, "\"thread@%p\"", lua_topointer(L, idx));
        return 0;

      case LUA_TUSERDATA:
        if (luaL_callmeta(L, idx, "__repr")) {
          if (lua_type(L, -1) == LUA_TSTRING) {
            s = lua_tolstring(L, -1, &slen);
            appendd(buf, s, slen);
          } else {
            appendf(buf, "[[error %s returned a %s value]]", "__repr",
                    luaL_typename(L, -1));
          }
          lua_pop(L, 1);
          return 0;
        }
        if (luaL_callmeta(L, idx, "__tostring")) {
          if (lua_type(L, -1) == LUA_TSTRING) {
            s = lua_tolstring(L, -1, &slen);
            EscapeLuaString(s, slen, buf);
          } else {
            appendf(buf, "[[error %s returned a %s value]]", "__tostring",
                    luaL_typename(L, -1));
          }
          lua_pop(L, 1);
          return 0;
        }
        appendf(buf, "\"udata@%p\"", lua_touserdata(L, idx));
        return 0;

      case LUA_TNUMBER:
        if (lua_isinteger(L, idx)) {
          appendd(buf, ibuf,
                  FormatFlex64(ibuf, luaL_checkinteger(L, idx), 2) - ibuf);
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
              free(visited->p);
              luaL_error(L, "numformat string not allowed");
              unreachable;
          }
          appendf(buf, fmt, lua_tonumber(L, idx));
        }
        return 0;

      case LUA_TBOOLEAN:
        appendw(buf, lua_toboolean(L, idx) ? READ32LE("true")
                                           : READ64LE("false\0\0"));
        return 0;

      case LUA_TTABLE:
        i = 0;
        if (LuaPushVisit(visited, lua_topointer(L, idx))) {
          appendw(buf, '{');
          lua_pushvalue(L, idx);  // idx becomes invalid once we change stack
          isarray = IsLuaArray(L);
          lua_pushnil(L);  // push the first key
          while (lua_next(L, -2)) {
            ktype = lua_type(L, -2);
            vtype = lua_type(L, -1);
            if (i++ > 0) appendw(buf, ',' | ' ' << 8);
            if (isarray) {
              // use {v₁′,v₂′,...} for lua-normal integer keys
            } else if (ktype == LUA_TSTRING && IsLuaIdentifier(L, -2)) {
              // use {𝑘=𝑣′} syntax when 𝑘 is legal as a lua identifier
              s = lua_tolstring(L, -2, &slen);
              appendd(buf, s, slen);
              appendw(buf, '=');
            } else {
              // use {[𝑘′]=𝑣′} otherwise
              appendw(buf, '[');
              LuaEncodeLuaDataImpl(L, buf, level - 1, numformat, -2, visited);
              appendw(buf, ']' | '=' << 010);
            }
            LuaEncodeLuaDataImpl(L, buf, level - 1, numformat, -1, visited);
            lua_pop(L, 1);  // table/-2, key/-1
          }
          lua_pop(L, 1);  // table ref
          LuaPopVisit(visited);
          appendw(buf, '}');
        } else {
          appendf(buf, "\"cyclic@%p\"", lua_topointer(L, idx));
        }
        return 0;

      default:
        free(visited->p);
        luaL_error(L, "can't serialize value of this type");
        unreachable;
    }
  } else {
    free(visited->p);
    luaL_error(L, "too many nested tables");
    unreachable;
  }
}

int LuaEncodeLuaData(lua_State *L, char **buf, char *numformat, int idx) {
  int rc;
  struct LuaVisited visited = {0};
  rc = LuaEncodeLuaDataImpl(L, buf, 64, numformat, idx, &visited);
  assert(!visited.n);
  free(visited.p);
  return rc;
}
