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
#include "libc/assert.h"
#include "libc/fmt/itoa.h"
#include "libc/serialize.h"
#include "libc/log/rop.internal.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/stdio/append.h"
#include "libc/stdio/strlist.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/x/x.h"
#include "third_party/double-conversion/wrapper.h"
#include "third_party/lua/cosmo.h"
#include "third_party/lua/lauxlib.h"
#include "third_party/lua/lctype.h"
#include "third_party/lua/lua.h"
#include "third_party/lua/visitor.h"

static int Serialize(lua_State *, char **, int, struct Serializer *, int);

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

// returns true if table at index -1 is an array
//
// for the purposes of lua serialization, we can only serialize using
// array ordering when a table is an array in the strictest sense. we
// consider a lua table an array if the following conditions are met:
//
//   1. for all 𝑘=𝑣 in table, 𝑘 is an integer ≥1
//   2. no holes exist between MIN(𝑘) and MAX(𝑘)
//   3. if non-empty, MIN(𝑘) is 1
//
// we need to do this because
//
//   "the order in which the indices are enumerated is not specified,
//    even for numeric indices" ──quoth lua 5.4 manual § next()
//
// we're able to implement this check in one pass, since lua_rawlen()
// reports the number of integers keys up until the first hole. so we
// simply need to check if any non-integers keys exist or any integer
// keys greater than the raw length.
//
// plesae note this is a more expensive check than the one we use for
// the json serializer, because lua doesn't require objects have only
// string keys. we want to be able to display mixed tables. it's just
// they won't be displayed with specified ordering, unless sorted.
static bool IsLuaArray(lua_State *L) {
  lua_Integer i;
  lua_Unsigned n;
  n = lua_rawlen(L, -1);
  lua_pushnil(L);
  while (lua_next(L, -2)) {
    if (!lua_isinteger(L, -2) || (i = lua_tointeger(L, -2)) < 1 || i > n) {
      lua_pop(L, 2);
      return false;
    }
    lua_pop(L, 1);
  }
  return true;
}

static int SerializeNil(lua_State *L, char **buf) {
  RETURN_ON_ERROR(appendw(buf, READ32LE("nil")));
  return 0;
OnError:
  return -1;
}

static int SerializeBoolean(lua_State *L, char **buf, int idx) {
  RETURN_ON_ERROR(appendw(
      buf, lua_toboolean(L, idx) ? READ32LE("true") : READ64LE("false\0\0")));
  return 0;
OnError:
  return -1;
}

static int SerializeOpaque(lua_State *L, char **buf, int idx,
                           const char *kind) {
  RETURN_ON_ERROR(appendf(buf, "\"%s@%p\"", kind, lua_topointer(L, idx)));
  return 0;
OnError:
  return -1;
}

static int SerializeNumber(lua_State *L, char **buf, int idx) {
  int64_t x;
  char ibuf[128];
  if (lua_isinteger(L, idx)) {
    x = luaL_checkinteger(L, idx);
    if (x == -9223372036854775807 - 1) {
      RETURN_ON_ERROR(appends(buf, "-9223372036854775807 - 1"));
    } else {
      RETURN_ON_ERROR(appendd(buf, ibuf, FormatFlex64(ibuf, x, 2) - ibuf));
    }
  } else {
    RETURN_ON_ERROR(appends(buf, DoubleToLua(ibuf, lua_tonumber(L, idx))));
  }
  return 0;
OnError:
  return -1;
}

#if 0
int main(int argc, char *argv[]) {
  int i, j;
  signed char tab[256] = {0};
  for (i = 0; i < 256; ++i) {
    if (i < 0x20) tab[i] = 1;   // hex
    if (i >= 0x7f) tab[i] = 2;  // hex/utf8
  }
  tab['\e'] = 'e';
  tab['\a'] = 'a';
  tab['\b'] = 'b';
  tab['\f'] = 'f';
  tab['\n'] = 'n';
  tab['\r'] = 'r';
  tab['\t'] = 't';
  tab['\v'] = 'v';
  tab['\\'] = '\\';
  tab['\"'] = '"';
  tab['\v'] = 'v';
  printf("const char kBase64[256] = {\n");
  for (i = 0; i < 16; ++i) {
    printf("  ");
    for (j = 0; j < 16; ++j) {
      if (isprint(tab[i * 16 + j])) {
        printf("'%c',", tab[i * 16 + j]);
      } else {
        printf("%d,", tab[i * 16 + j]);
      }
    }
    printf(" // 0x%02x\n", i * 16);
  }
  printf("};\n");
  return 0;
}
#endif

static const char kLuaStrXlat[256] = {
  1,1,1,1,1,1,1,'a','b','t','n','v','f','r',1,1, // 0x00
  1,1,1,1,1,1,1,1,1,1,1,'e',1,1,1,1, // 0x10
  0,0,'"',0,0,0,0,0,0,0,0,0,0,0,0,0, // 0x20
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0x30
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0x40
  0,0,0,0,0,0,0,0,0,0,0,0,'\\',0,0,0, // 0x50
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0x60
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1, // 0x70
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // 0x80
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // 0x90
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // 0xa0
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // 0xb0
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // 0xc0
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // 0xd0
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // 0xe0
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // 0xf0
};
// clang-format on

static int SerializeString(lua_State *L, char **buf, int idx) {
  int c, x;
  bool utf8;
  size_t i, n;
  const char *s;
  s = lua_tolstring(L, idx, &n);
  utf8 = isutf8(s, n);
  RETURN_ON_ERROR(appendw(buf, '"'));
  for (i = 0; i < n; i++) {
    switch ((x = kLuaStrXlat[(c = s[i] & 255)])) {
      case 0:
      EmitByte:
        RETURN_ON_ERROR(appendw(buf, c));
        break;
      case 2:
        if (utf8) goto EmitByte;
        // fallthrough
      case 1:
        RETURN_ON_ERROR(
            appendw(buf, '\\' | 'x' << 010 |
                             "0123456789abcdef"[(c & 0xF0) >> 4] << 020 |
                             "0123456789abcdef"[(c & 0x0F) >> 0] << 030));
        break;
      default:
        RETURN_ON_ERROR(appendw(buf, READ32LE("\\\x00\x00") | (x << 8)));
        break;
    }
  }
  RETURN_ON_ERROR(appendw(buf, '"'));
  return 0;
OnError:
  return -1;
}

static int SerializeUserData(lua_State *L, char **buf, int idx) {
  size_t n;
  const char *s;
  if (luaL_callmeta(L, idx, "__repr")) {
    if (lua_type(L, -1) == LUA_TSTRING) {
      s = lua_tolstring(L, -1, &n);
      RETURN_ON_ERROR(appendd(buf, s, n));
    } else {
      RETURN_ON_ERROR(appendf(buf, "[[error %s returned a %s value]]", "__repr",
                              luaL_typename(L, -1)));
    }
    lua_pop(L, 1);
    return 0;
  }
  if (luaL_callmeta(L, idx, "__tostring")) {
    if (lua_type(L, -1) == LUA_TSTRING) {
      RETURN_ON_ERROR(SerializeString(L, buf, -1));
    } else {
      RETURN_ON_ERROR(appendf(buf, "[[error %s returned a %s value]]",
                              "__tostring", luaL_typename(L, -1)));
    }
    lua_pop(L, 1);
    return 0;
  }
  return SerializeOpaque(L, buf, idx, "udata");
OnError:
  return -1;
}

static int SerializeArray(lua_State *L, char **buf, struct Serializer *z,
                          int depth) {
  size_t i, n;
  RETURN_ON_ERROR(appendw(buf, '{'));
  n = lua_rawlen(L, -1);
  for (i = 1; i <= n; i++) {
    lua_rawgeti(L, -1, i);
    if (i > 1) RETURN_ON_ERROR(appendw(buf, READ16LE(", ")));
    RETURN_ON_ERROR(Serialize(L, buf, -1, z, depth + 1));
    lua_pop(L, 1);
  }
  RETURN_ON_ERROR(appendw(buf, '}'));
  return 0;
OnError:
  return -1;
}

static int SerializeObject(lua_State *L, char **buf, struct Serializer *z,
                           int depth, bool multi) {
  size_t n;
  const char *s;
  bool comma = false;
  RETURN_ON_ERROR(SerializeObjectStart(buf, z, depth, multi));
  lua_pushnil(L);
  while (lua_next(L, -2)) {
    if (comma) {
      if (multi) {
        RETURN_ON_ERROR(appendw(buf, ','));
        RETURN_ON_ERROR(SerializeObjectIndent(buf, z, depth + 1));
      } else {
        RETURN_ON_ERROR(appendw(buf, READ16LE(", ")));
      }
    } else {
      comma = true;
    }
    if (lua_type(L, -2) == LUA_TSTRING && IsLuaIdentifier(L, -2)) {
      // use {𝑘=𝑣′} syntax when 𝑘 is a legal lua identifier
      s = lua_tolstring(L, -2, &n);
      RETURN_ON_ERROR(appendd(buf, s, n));
      RETURN_ON_ERROR(appendw(buf, '='));
    } else {
      // use {[𝑘′]=𝑣′} otherwise
      RETURN_ON_ERROR(appendw(buf, '['));
      RETURN_ON_ERROR(Serialize(L, buf, -2, z, depth + 1));
      RETURN_ON_ERROR(appendw(buf, READ16LE("]=")));
    }
    RETURN_ON_ERROR(Serialize(L, buf, -1, z, depth + 1));
    lua_pop(L, 1);
  }
  RETURN_ON_ERROR(SerializeObjectEnd(buf, z, depth, multi));
  return 0;
OnError:
  return -1;
}

static int SerializeSorted(lua_State *L, char **buf, struct Serializer *z,
                           int depth, bool multi) {
  int i;
  size_t n;
  const char *s;
  struct StrList sl = {0};
  lua_pushnil(L);
  while (lua_next(L, -2)) {
    RETURN_ON_ERROR(i = AppendStrList(&sl));
    if (lua_type(L, -2) == LUA_TSTRING && IsLuaIdentifier(L, -2)) {
      // use {𝑘=𝑣′} syntax when 𝑘 is a legal lua identifier
      s = lua_tolstring(L, -2, &n);
      RETURN_ON_ERROR(appendd(sl.p + i, s, n));
      RETURN_ON_ERROR(appendw(sl.p + i, '='));
    } else {
      // use {[𝑘′]=𝑣′} otherwise
      RETURN_ON_ERROR(appendw(sl.p + i, '['));
      RETURN_ON_ERROR(Serialize(L, sl.p + i, -2, z, depth + 1));
      RETURN_ON_ERROR(appendw(sl.p + i, ']' | '=' << 010));
    }
    RETURN_ON_ERROR(Serialize(L, sl.p + i, -1, z, depth + 1));
    lua_pop(L, 1);
  }
  SortStrList(&sl);
  RETURN_ON_ERROR(SerializeObjectStart(buf, z, depth, multi));
  for (i = 0; i < sl.i; ++i) {
    if (i) {
      if (multi) {
        RETURN_ON_ERROR(appendw(buf, ','));
        RETURN_ON_ERROR(SerializeObjectIndent(buf, z, depth + 1));
      } else {
        RETURN_ON_ERROR(appendw(buf, READ16LE(", ")));
      }
    }
    RETURN_ON_ERROR(appends(buf, sl.p[i]));
  }
  RETURN_ON_ERROR(SerializeObjectEnd(buf, z, depth, multi));
  FreeStrList(&sl);
  return 0;
OnError:
  FreeStrList(&sl);
  return -1;
}

static int SerializeTable(lua_State *L, char **buf, int idx,
                          struct Serializer *z, int depth) {
  int rc;
  bool multi;
  if (UNLIKELY(!HaveStackMemory(getauxval(AT_PAGESZ)))) {
    z->reason = "out of stack";
    return -1;
  }
  RETURN_ON_ERROR(rc = LuaPushVisit(&z->visited, lua_topointer(L, idx)));
  if (rc) return SerializeOpaque(L, buf, idx, "cyclic");
  lua_pushvalue(L, idx);  // idx becomes invalid once we change stack
  if (IsLuaArray(L)) {
    RETURN_ON_ERROR(SerializeArray(L, buf, z, depth));
  } else {
    multi = z->conf.pretty && LuaHasMultipleItems(L);
    if (z->conf.sorted) {
      RETURN_ON_ERROR(SerializeSorted(L, buf, z, depth, multi));
    } else {
      RETURN_ON_ERROR(SerializeObject(L, buf, z, depth, multi));
    }
  }
  LuaPopVisit(&z->visited);
  lua_pop(L, 1);  // table ref
  return 0;
OnError:
  return -1;
}

static int Serialize(lua_State *L, char **buf, int idx, struct Serializer *z,
                     int depth) {
  if (depth < z->conf.maxdepth) {
    switch (lua_type(L, idx)) {
      case LUA_TNIL:
        return SerializeNil(L, buf);
      case LUA_TBOOLEAN:
        return SerializeBoolean(L, buf, idx);
      case LUA_TNUMBER:
        return SerializeNumber(L, buf, idx);
      case LUA_TSTRING:
        return SerializeString(L, buf, idx);
      case LUA_TTABLE:
        return SerializeTable(L, buf, idx, z, depth);
      case LUA_TUSERDATA:
        return SerializeUserData(L, buf, idx);
      case LUA_TFUNCTION:
        return SerializeOpaque(L, buf, idx, "func");
      case LUA_TLIGHTUSERDATA:
        return SerializeOpaque(L, buf, idx, "light");
      case LUA_TTHREAD:
        return SerializeOpaque(L, buf, idx, "thread");
      default:
        return SerializeOpaque(L, buf, idx, "unsupported");
    }
  } else {
    return SerializeOpaque(L, buf, idx, "greatdepth");
  }
}

/**
 * Encodes Lua data structure as Lua code string.
 *
 * This serializer is intended primarily for describing the data
 * structure. For example, it's used by the REPL where we need to be
 * able to ignore errors when displaying data structures, since showing
 * most things imperfectly is better than crashing. Therefore this isn't
 * the kind of serializer you'd want to use to persist data in prod. Try
 * using the JSON serializer for that purpose.
 *
 * @param L is Lua interpreter state
 * @param buf receives encoded output string
 * @param idx is index of item on Lua stack
 * @param sorted is ignored (always sorted)
 * @return 0 on success, or -1 on error
 */
int LuaEncodeLuaData(lua_State *L, char **buf, int idx,
                     struct EncoderConfig conf) {
  int rc;
  struct Serializer z = {.reason = "out of memory", .conf = conf};
  if (lua_checkstack(L, conf.maxdepth * 3 + LUA_MINSTACK)) {
    rc = Serialize(L, buf, idx, &z, 0);
    free(z.visited.p);
    if (rc == -1) {
      lua_pushnil(L);
      lua_pushstring(L, z.reason);
    }
    return rc;
  } else {
    luaL_error(L, "can't set stack depth");
    __builtin_unreachable();
  }
}
