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
// unix.Stat object

static struct stat *GetUnixStat(lua_State *L) {
  return luaL_checkudata(L, 1, "unix.Stat");
}

// unix.Stat:size()
//     └─→ bytes:int
static int LuaUnixStatSize(lua_State *L) {
  return ReturnInteger(L, GetUnixStat(L)->st_size);
}

// unix.Stat:mode()
//     └─→ mode:int
static int LuaUnixStatMode(lua_State *L) {
  return ReturnInteger(L, GetUnixStat(L)->st_mode);
}

// unix.Stat:dev()
//     └─→ dev:int
static int LuaUnixStatDev(lua_State *L) {
  return ReturnInteger(L, GetUnixStat(L)->st_dev);
}

// unix.Stat:ino()
//     └─→ inodeint
static int LuaUnixStatIno(lua_State *L) {
  return ReturnInteger(L, GetUnixStat(L)->st_ino);
}

// unix.Stat:nlink()
//     └─→ count:int
static int LuaUnixStatNlink(lua_State *L) {
  return ReturnInteger(L, GetUnixStat(L)->st_nlink);
}

// unix.Stat:rdev()
//     └─→ rdev:int
static int LuaUnixStatRdev(lua_State *L) {
  return ReturnInteger(L, GetUnixStat(L)->st_rdev);
}

// unix.Stat:uid()
//     └─→ uid:int
static int LuaUnixStatUid(lua_State *L) {
  return ReturnInteger(L, GetUnixStat(L)->st_uid);
}

// unix.Stat:gid()
//     └─→ gid:int
static int LuaUnixStatGid(lua_State *L) {
  return ReturnInteger(L, GetUnixStat(L)->st_gid);
}

// unix.Stat:blocks()
//     └─→ count:int
static int LuaUnixStatBlocks(lua_State *L) {
  return ReturnInteger(L, GetUnixStat(L)->st_blocks);
}

// unix.Stat:blksize()
//     └─→ bytes:int
static int LuaUnixStatBlksize(lua_State *L) {
  return ReturnInteger(L, GetUnixStat(L)->st_blksize);
}

static dontinline int ReturnTimespec(lua_State *L, struct timespec *ts) {
  lua_pushinteger(L, ts->tv_sec);
  lua_pushinteger(L, ts->tv_nsec);
  return 2;
}

// unix.Stat:atim()
//     └─→ unixts:int, nanos:int
static int LuaUnixStatAtim(lua_State *L) {
  return ReturnTimespec(L, &GetUnixStat(L)->st_atim);
}

// unix.Stat:mtim()
//     └─→ unixts:int, nanos:int
static int LuaUnixStatMtim(lua_State *L) {
  return ReturnTimespec(L, &GetUnixStat(L)->st_mtim);
}

// unix.Stat:ctim()
//     └─→ unixts:int, nanos:int
static int LuaUnixStatCtim(lua_State *L) {
  return ReturnTimespec(L, &GetUnixStat(L)->st_ctim);
}

// unix.Stat:birthtim()
//     └─→ unixts:int, nanos:int
static int LuaUnixStatBirthtim(lua_State *L) {
  return ReturnTimespec(L, &GetUnixStat(L)->st_birthtim);
}

// unix.Stat:gen()
//     └─→ gen:int [xnu/bsd]
static int LuaUnixStatGen(lua_State *L) {
  return ReturnInteger(L, GetUnixStat(L)->st_gen);
}

// unix.Stat:flags()
//     └─→ flags:int [xnu/bsd]
static int LuaUnixStatFlags(lua_State *L) {
  return ReturnInteger(L, GetUnixStat(L)->st_flags);
}

static int LuaUnixStatToString(lua_State *L) {
  char ibuf[21];
  luaL_Buffer b;
  struct stat *st;
  st = GetUnixStat(L);
  luaL_buffinit(L, &b);
  luaL_addstring(&b, "unix.Stat({size=");
  FormatInt64(ibuf, st->st_size);
  luaL_addstring(&b, ibuf);
  if (st->st_mode) {
    luaL_addstring(&b, ", mode=");
    FormatOctal32(ibuf, st->st_mode, 1);
    luaL_addstring(&b, ibuf);
  }
  if (st->st_ino) {
    luaL_addstring(&b, ", ino=");
    FormatUint64(ibuf, st->st_ino);
    luaL_addstring(&b, ibuf);
  }
  if (st->st_nlink) {
    luaL_addstring(&b, ", nlink=");
    FormatUint64(ibuf, st->st_nlink);
    luaL_addstring(&b, ibuf);
  }
  if (st->st_uid) {
    luaL_addstring(&b, ", uid=");
    FormatUint32(ibuf, st->st_uid);
    luaL_addstring(&b, ibuf);
  }
  if (st->st_gid) {
    luaL_addstring(&b, ", gid=");
    FormatUint32(ibuf, st->st_gid);
    luaL_addstring(&b, ibuf);
  }
  if (st->st_flags) {
    luaL_addstring(&b, ", flags=");
    FormatUint32(ibuf, st->st_flags);
    luaL_addstring(&b, ibuf);
  }
  if (st->st_dev) {
    luaL_addstring(&b, ", dev=");
    FormatUint64(ibuf, st->st_dev);
    luaL_addstring(&b, ibuf);
  }
  if (st->st_rdev) {
    luaL_addstring(&b, ", rdev=");
    FormatUint64(ibuf, st->st_rdev);
    luaL_addstring(&b, ibuf);
  }
  if (st->st_blksize) {
    luaL_addstring(&b, ", blksize=");
    FormatInt64(ibuf, st->st_blksize);
    luaL_addstring(&b, ibuf);
  }
  if (st->st_blocks) {
    luaL_addstring(&b, ", blocks=");
    FormatInt64(ibuf, st->st_blocks);
    luaL_addstring(&b, ibuf);
  }
  luaL_addstring(&b, "})");
  luaL_pushresult(&b);
  return 1;
}

static const luaL_Reg kLuaUnixStatMeth[] = {
    {"atim", LuaUnixStatAtim},          //
    {"birthtim", LuaUnixStatBirthtim},  //
    {"blksize", LuaUnixStatBlksize},    //
    {"blocks", LuaUnixStatBlocks},      //
    {"ctim", LuaUnixStatCtim},          //
    {"dev", LuaUnixStatDev},            //
    {"gid", LuaUnixStatGid},            //
    {"ino", LuaUnixStatIno},            //
    {"mode", LuaUnixStatMode},          //
    {"mtim", LuaUnixStatMtim},          //
    {"nlink", LuaUnixStatNlink},        //
    {"rdev", LuaUnixStatRdev},          //
    {"size", LuaUnixStatSize},          //
    {"uid", LuaUnixStatUid},            //
    {"flags", LuaUnixStatFlags},        //
    {"gen", LuaUnixStatGen},            //
    {0},                                //
};

static const luaL_Reg kLuaUnixStatMeta[] = {
    {"__tostring", LuaUnixStatToString},  //
    {"__repr", LuaUnixStatToString},      //
    {0},                                  //
};

void LuaUnixStatObj(lua_State *L) {
  luaL_newmetatable(L, "unix.Stat");
  luaL_setfuncs(L, kLuaUnixStatMeta, 0);
  luaL_newlibtable(L, kLuaUnixStatMeth);
  luaL_setfuncs(L, kLuaUnixStatMeth, 0);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);
}
