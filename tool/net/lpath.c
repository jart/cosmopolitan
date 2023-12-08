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
#include "libc/calls/struct/stat.h"
#include "libc/errno.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/s.h"
#include "third_party/lua/lauxlib.h"

/**
 * @fileoverview redbean unix path manipulation module
 */

// path.basename(str)
//     └─→ str
static int LuaPathBasename(lua_State *L) {
  size_t i, n;
  const char *p;
  if ((p = luaL_optlstring(L, 1, 0, &n)) && n) {
    while (n > 1 && p[n - 1] == '/') --n;
    i = n - 1;
    while (i && p[i - 1] != '/') --i;
    lua_pushlstring(L, p + i, n - i);
  } else {
    lua_pushlstring(L, ".", 1);
  }
  return 1;
}

// path.dirname(str)
//     └─→ str
static int LuaPathDirname(lua_State *L) {
  size_t n;
  const char *p;
  if ((p = luaL_optlstring(L, 1, 0, &n)) && n--) {
    for (; p[n] == '/'; n--) {
      if (!n) goto ReturnSlash;
    }
    for (; p[n] != '/'; n--) {
      if (!n) goto ReturnDot;
    }
    for (; p[n] == '/'; n--) {
      if (!n) goto ReturnSlash;
    }
    lua_pushlstring(L, p, n + 1);
    return 1;
  }
ReturnDot:
  lua_pushlstring(L, ".", 1);
  return 1;
ReturnSlash:
  lua_pushlstring(L, "/", 1);
  return 1;
}

// path.join(str, ...)
//     └─→ str
static int LuaPathJoin(lua_State *L) {
  int i, n;
  size_t z;
  bool gotstr;
  const char *c;
  luaL_Buffer b;
  bool needslash;
  if ((n = lua_gettop(L))) {
    luaL_buffinit(L, &b);
    gotstr = false;
    needslash = false;
    for (i = 1; i <= n; ++i) {
      if (lua_isnoneornil(L, i)) continue;
      gotstr = true;
      c = luaL_checklstring(L, i, &z);
      if (z) {
        if (c[0] == '/') {
          luaL_buffsub(&b, luaL_bufflen(&b));
        } else if (needslash) {
          luaL_addchar(&b, '/');
        }
        luaL_addlstring(&b, c, z);
        needslash = c[z - 1] != '/';
      } else if (needslash) {
        luaL_addchar(&b, '/');
        needslash = false;
      }
    }
    if (gotstr) {
      luaL_pushresult(&b);
    } else {
      lua_pushnil(L);
    }
    return 1;
  } else {
    luaL_error(L, "missing argument");
    __builtin_unreachable();
  }
}

static int CheckPath(lua_State *L, int type, int flags) {
  int olderr;
  struct stat st;
  const char *path;
  path = luaL_checkstring(L, 1);
  olderr = errno;
  if (fstatat(AT_FDCWD, path, &st, flags) != -1) {
    lua_pushboolean(L, !type || (st.st_mode & S_IFMT) == type);
  } else {
    errno = olderr;
    lua_pushboolean(L, false);
  }
  return 1;
}

// path.exists(str)
//     └─→ bool
static int LuaPathExists(lua_State *L) {
  return CheckPath(L, 0, 0);
}

// path.isfile(str)
//     └─→ bool
static int LuaPathIsfile(lua_State *L) {
  return CheckPath(L, S_IFREG, AT_SYMLINK_NOFOLLOW);
}

// path.islink(str)
//     └─→ bool
static int LuaPathIslink(lua_State *L) {
  return CheckPath(L, S_IFLNK, AT_SYMLINK_NOFOLLOW);
}

// path.isdir(str)
//     └─→ bool
static int LuaPathIsdir(lua_State *L) {
  return CheckPath(L, S_IFDIR, AT_SYMLINK_NOFOLLOW);
}

static const luaL_Reg kLuaPath[] = {
    {"basename", LuaPathBasename},  //
    {"dirname", LuaPathDirname},    //
    {"exists", LuaPathExists},      //
    {"isdir", LuaPathIsdir},        //
    {"isfile", LuaPathIsfile},      //
    {"islink", LuaPathIslink},      //
    {"join", LuaPathJoin},          //
    {0},                            //
};

int LuaPath(lua_State *L) {
  luaL_newlib(L, kLuaPath);
  return 1;
}
