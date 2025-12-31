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
#include "libc/calls/struct/dirent.h"
#include "libc/errno.h"
#include "libc/str/str.h"

////////////////////////////////////////////////////////////////////////////////
// unix.Dir object

static DIR **GetUnixDirSelf(lua_State *L) {
  return luaL_checkudata(L, 1, "unix.Dir");
}

static DIR *GetDirOrDie(lua_State *L) {
  DIR **dirp;
  dirp = GetUnixDirSelf(L);
  if (*dirp) return *dirp;
  luaL_argerror(L, 1, "unix.UnixDir is closed");
  __builtin_unreachable();
}

// unix.Dir:close()
//     ├─→ true
//     └─→ nil, unix.Errno
static int LuaUnixDirClose(lua_State *L) {
  DIR **dirp;
  int rc, olderr;
  dirp = GetUnixDirSelf(L);
  if (*dirp) {
    olderr = errno;
    rc = closedir(*dirp);
    *dirp = 0;
    return SysretBool(L, "closedir", olderr, rc);
  } else {
    lua_pushboolean(L, true);
    return 1;
  }
}

// unix.Dir:read()
//     ├─→ name:str, kind:int, ino:int, off:int
//     └─→ nil
static int LuaUnixDirRead(lua_State *L) {
  struct dirent *ent;
  if ((ent = readdir(GetDirOrDie(L)))) {
    lua_pushlstring(L, ent->d_name, strnlen(ent->d_name, sizeof(ent->d_name)));
    lua_pushinteger(L, ent->d_type);
    lua_pushinteger(L, ent->d_ino);
    lua_pushinteger(L, ent->d_off);
    return 4;
  } else {
    // end of directory stream condition
    // we make the assumption getdents() won't fail
    lua_pushnil(L);
    return 1;
  }
}

// unix.Dir:fd()
//     ├─→ fd:int
//     └─→ nil, unix.Errno
static int LuaUnixDirFd(lua_State *L) {
  int fd, olderr = errno;
  fd = dirfd(GetDirOrDie(L));
  if (fd != -1) {
    lua_pushinteger(L, fd);
    return 1;
  } else {
    return LuaUnixSysretErrno(L, "dirfd", olderr);
  }
}

// unix.Dir:tell()
//     ├─→ off:int
//     └─→ nil, unix.Errno
static int LuaUnixDirTell(lua_State *L) {
  int olderr = errno;
  return SysretInteger(L, "telldir", olderr, telldir(GetDirOrDie(L)));
}

// unix.Dir:rewind()
static int LuaUnixDirRewind(lua_State *L) {
  rewinddir(GetDirOrDie(L));
  return 0;
}

static int ReturnDir(lua_State *L, DIR *dir) {
  DIR **dirp;
  dirp = lua_newuserdatauv(L, sizeof(*dirp), 1);
  luaL_setmetatable(L, "unix.Dir");
  *dirp = dir;
  return 1;
}

// unix.opendir(path:str)
//     ├─→ state:unix.Dir
//     └─→ nil, unix.Errno
int LuaUnixOpendir(lua_State *L) {
  DIR *dir;
  int olderr = errno;
  if ((dir = opendir(luaL_checkstring(L, 1)))) {
    return ReturnDir(L, dir);
  } else {
    return LuaUnixSysretErrno(L, "opendir", olderr);
  }
}

// unix.fdopendir(fd:int)
//     ├─→ next:function, state:unix.Dir
//     └─→ nil, unix.Errno
int LuaUnixFdopendir(lua_State *L) {
  DIR *dir;
  int olderr = errno;
  if ((dir = fdopendir(luaL_checkinteger(L, 1)))) {
    return ReturnDir(L, dir);
  } else {
    return LuaUnixSysretErrno(L, "fdopendir", olderr);
  }
}

static const luaL_Reg kLuaUnixDirMeth[] = {
    {"close", LuaUnixDirClose},    //
    {"read", LuaUnixDirRead},      //
    {"fd", LuaUnixDirFd},          //
    {"tell", LuaUnixDirTell},      //
    {"rewind", LuaUnixDirRewind},  //
    {0},                           //
};

static const luaL_Reg kLuaUnixDirMeta[] = {
    {"__call", LuaUnixDirRead},  //
    {"__gc", LuaUnixDirClose},   //
    {0},                         //
};

void LuaUnixDirObj(lua_State *L) {
  luaL_newmetatable(L, "unix.Dir");
  luaL_setfuncs(L, kLuaUnixDirMeta, 0);
  luaL_newlibtable(L, kLuaUnixDirMeth);
  luaL_setfuncs(L, kLuaUnixDirMeth, 0);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);
}
