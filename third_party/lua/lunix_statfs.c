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
#include "libc/fmt/itoa.h"

////////////////////////////////////////////////////////////////////////////////
// unix.Statfs object

static struct statfs *GetUnixStatfs(lua_State *L) {
  return luaL_checkudata(L, 1, "unix.Statfs");
}

// unix.Statfs:type()
//     └─→ bytes:int
static int LuaUnixStatfsType(lua_State *L) {
  return ReturnInteger(L, GetUnixStatfs(L)->f_type);
}

// unix.Statfs:bsize()
//     └─→ bsize:int
static int LuaUnixStatfsBsize(lua_State *L) {
  return ReturnInteger(L, GetUnixStatfs(L)->f_bsize);
}

// unix.Statfs:blocks()
//     └─→ blocks:int
static int LuaUnixStatfsBlocks(lua_State *L) {
  return ReturnInteger(L, GetUnixStatfs(L)->f_blocks);
}

// unix.Statfs:bfree()
//     └─→ bfreedeint
static int LuaUnixStatfsBfree(lua_State *L) {
  return ReturnInteger(L, GetUnixStatfs(L)->f_bfree);
}

// unix.Statfs:bavail()
//     └─→ count:int
static int LuaUnixStatfsBavail(lua_State *L) {
  return ReturnInteger(L, GetUnixStatfs(L)->f_bavail);
}

// unix.Statfs:files()
//     └─→ files:int
static int LuaUnixStatfsFiles(lua_State *L) {
  return ReturnInteger(L, GetUnixStatfs(L)->f_files);
}

// unix.Statfs:ffree()
//     └─→ ffree:int
static int LuaUnixStatfsFfree(lua_State *L) {
  return ReturnInteger(L, GetUnixStatfs(L)->f_ffree);
}

// unix.Statfs:fsid()
//     └─→ x:int, y:int
static int LuaUnixStatfsFsid(lua_State *L) {
  struct statfs *f = GetUnixStatfs(L);
  lua_pushinteger(L, f->f_fsid.__val[0]);
  lua_pushinteger(L, f->f_fsid.__val[1]);
  return 2;
}

// unix.Statfs:namelen()
//     └─→ count:int
static int LuaUnixStatfsNamelen(lua_State *L) {
  return ReturnInteger(L, GetUnixStatfs(L)->f_namelen);
}

// unix.Statfs:frsize()
//     └─→ bytes:int
static int LuaUnixStatfsFrsize(lua_State *L) {
  return ReturnInteger(L, GetUnixStatfs(L)->f_frsize);
}

// unix.Statfs:flags()
//     └─→ bytes:int
static int LuaUnixStatfsFlags(lua_State *L) {
  return ReturnInteger(L, GetUnixStatfs(L)->f_flags);
}

// unix.Statfs:owner()
//     └─→ bytes:int
static int LuaUnixStatfsOwner(lua_State *L) {
  return ReturnInteger(L, GetUnixStatfs(L)->f_owner);
}

// unix.Statfs:fstypename()
//     └─→ fstypename:str
static int LuaUnixStatfsFstypename(lua_State *L) {
  return ReturnString(L, GetUnixStatfs(L)->f_fstypename);
}

static int LuaUnixStatfsToString(lua_State *L) {
  char ibuf[21];
  luaL_Buffer b;
  struct statfs *f;
  f = GetUnixStatfs(L);
  luaL_buffinit(L, &b);
  luaL_addstring(&b, "unix.Statfs({type=");
  FormatInt64(ibuf, f->f_type);
  luaL_addstring(&b, ibuf);
  luaL_addstring(&b, ", fstypename=\"");
  luaL_addstring(&b, f->f_fstypename);
  luaL_addstring(&b, "\"");
  if (f->f_bsize) {
    luaL_addstring(&b, ", bsize=");
    FormatInt64(ibuf, f->f_bsize);
    luaL_addstring(&b, ibuf);
  }
  if (f->f_blocks) {
    luaL_addstring(&b, ", blocks=");
    FormatInt64(ibuf, f->f_blocks);
    luaL_addstring(&b, ibuf);
  }
  if (f->f_bfree) {
    luaL_addstring(&b, ", bfree=");
    FormatInt64(ibuf, f->f_bfree);
    luaL_addstring(&b, ibuf);
  }
  if (f->f_bavail) {
    luaL_addstring(&b, ", bavail=");
    FormatInt64(ibuf, f->f_bavail);
    luaL_addstring(&b, ibuf);
  }
  if (f->f_files) {
    luaL_addstring(&b, ", files=");
    FormatInt64(ibuf, f->f_files);
    luaL_addstring(&b, ibuf);
  }
  if (f->f_ffree) {
    luaL_addstring(&b, ", ffree=");
    FormatInt64(ibuf, f->f_ffree);
    luaL_addstring(&b, ibuf);
  }
  if (f->f_fsid.__val[0] || f->f_fsid.__val[1]) {
    luaL_addstring(&b, ", fsid={");
    FormatUint64(ibuf, f->f_fsid.__val[0]);
    luaL_addstring(&b, ibuf);
    luaL_addstring(&b, ", ");
    FormatUint64(ibuf, f->f_fsid.__val[1]);
    luaL_addstring(&b, ibuf);
    luaL_addstring(&b, "}");
  }
  if (f->f_namelen) {
    luaL_addstring(&b, ", namelen=");
    FormatUint64(ibuf, f->f_namelen);
    luaL_addstring(&b, ibuf);
  }
  if (f->f_flags) {
    luaL_addstring(&b, ", flags=");
    FormatHex64(ibuf, f->f_flags, 2);
    luaL_addstring(&b, ibuf);
  }
  if (f->f_owner) {
    luaL_addstring(&b, ", owner=");
    FormatUint32(ibuf, f->f_owner);
    luaL_addstring(&b, ibuf);
  }
  luaL_addstring(&b, "})");
  luaL_pushresult(&b);
  return 1;
}

static const luaL_Reg kLuaUnixStatfsMeth[] = {
    {"type", LuaUnixStatfsType},              //
    {"bsize", LuaUnixStatfsBsize},            //
    {"blocks", LuaUnixStatfsBlocks},          //
    {"bfree", LuaUnixStatfsBfree},            //
    {"bavail", LuaUnixStatfsBavail},          //
    {"files", LuaUnixStatfsFiles},            //
    {"ffree", LuaUnixStatfsFfree},            //
    {"fsid", LuaUnixStatfsFsid},              //
    {"namelen", LuaUnixStatfsNamelen},        //
    {"frsize", LuaUnixStatfsFrsize},          //
    {"flags", LuaUnixStatfsFlags},            //
    {"owner", LuaUnixStatfsOwner},            //
    {"fstypename", LuaUnixStatfsFstypename},  //
    {0},                                      //
};

static const luaL_Reg kLuaUnixStatfsMeta[] = {
    {"__tostring", LuaUnixStatfsToString},  //
    {"__repr", LuaUnixStatfsToString},      //
    {0},                                    //
};

void LuaUnixStatfsObj(lua_State *L) {
  luaL_newmetatable(L, "unix.Statfs");
  luaL_setfuncs(L, kLuaUnixStatfsMeta, 0);
  luaL_newlibtable(L, kLuaUnixStatfsMeth);
  luaL_setfuncs(L, kLuaUnixStatfsMeth, 0);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);
}
