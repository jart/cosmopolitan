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
#include "libc/mem/mem.h"
#include "libc/serialize.h"
#include "libc/stdio/append.h"

////////////////////////////////////////////////////////////////////////////////
// unix.Rusage object

static struct rusage *GetUnixRusage(lua_State *L) {
  return luaL_checkudata(L, 1, "unix.Rusage");
}

static dontinline int ReturnTimeval(lua_State *L, struct timeval *tv) {
  lua_pushinteger(L, tv->tv_sec);
  lua_pushinteger(L, tv->tv_usec * 1000);
  return 2;
}

// unix.Rusage:utime()
//     └─→ unixts:int, nanos:int
static int LuaUnixRusageUtime(lua_State *L) {
  return ReturnTimeval(L, &GetUnixRusage(L)->ru_utime);
}

// unix.Rusage:stime()
//     └─→ unixts:int, nanos:int
static int LuaUnixRusageStime(lua_State *L) {
  return ReturnTimeval(L, &GetUnixRusage(L)->ru_stime);
}

// unix.Rusage:maxrss()
//     └─→ kilobytes:int
static int LuaUnixRusageMaxrss(lua_State *L) {
  return ReturnInteger(L, GetUnixRusage(L)->ru_maxrss);
}

// unix.Rusage:ixrss()
//     └─→ integralkilobytes:int
static int LuaUnixRusageIxrss(lua_State *L) {
  return ReturnInteger(L, GetUnixRusage(L)->ru_ixrss);
}

// unid.Rusage:idrss()
//     └─→ integralkilobytes:int
static int LuaUnixRusageIdrss(lua_State *L) {
  return ReturnInteger(L, GetUnixRusage(L)->ru_idrss);
}

// unis.Rusage:isrss()
//     └─→ integralkilobytes:int
static int LuaUnixRusageIsrss(lua_State *L) {
  return ReturnInteger(L, GetUnixRusage(L)->ru_isrss);
}

// unix.Rusage:minflt()
//     └─→ count:int
static int LuaUnixRusageMinflt(lua_State *L) {
  return ReturnInteger(L, GetUnixRusage(L)->ru_minflt);
}

// unix.Rusage:majflt()
//     └─→ count:int
static int LuaUnixRusageMajflt(lua_State *L) {
  return ReturnInteger(L, GetUnixRusage(L)->ru_majflt);
}

// unix.Rusage:nswap()
//     └─→ count:int
static int LuaUnixRusageNswap(lua_State *L) {
  return ReturnInteger(L, GetUnixRusage(L)->ru_nswap);
}

// unix.Rusage:inblock()
//     └─→ count:int
static int LuaUnixRusageInblock(lua_State *L) {
  return ReturnInteger(L, GetUnixRusage(L)->ru_inblock);
}

// unix.Rusage:oublock()
//     └─→ count:int
static int LuaUnixRusageOublock(lua_State *L) {
  return ReturnInteger(L, GetUnixRusage(L)->ru_oublock);
}

// unix.Rusage:msgsnd()
//     └─→ count:int
static int LuaUnixRusageMsgsnd(lua_State *L) {
  return ReturnInteger(L, GetUnixRusage(L)->ru_msgsnd);
}

// unix.Rusage:msgrcv()
//     └─→ count:int
static int LuaUnixRusageMsgrcv(lua_State *L) {
  return ReturnInteger(L, GetUnixRusage(L)->ru_msgrcv);
}

// unix.Rusage:nsignals()
//     └─→ count:int
static int LuaUnixRusageNsignals(lua_State *L) {
  return ReturnInteger(L, GetUnixRusage(L)->ru_nsignals);
}

// unix.Rusage:nvcsw()
//     └─→ count:int
static int LuaUnixRusageNvcsw(lua_State *L) {
  return ReturnInteger(L, GetUnixRusage(L)->ru_nvcsw);
}

// unix.Rusage:nivcsw()
//     └─→ count:int
static int LuaUnixRusageNivcsw(lua_State *L) {
  return ReturnInteger(L, GetUnixRusage(L)->ru_nivcsw);
}

static int LuaUnixRusageToString(lua_State *L) {
  char *b = 0;
  struct rusage *ru = GetUnixRusage(L);
  appends(&b, "{");
  appendf(&b, "%s={%ld, %ld}", "utime", ru->ru_utime.tv_sec,
          ru->ru_utime.tv_usec * 1000);
  if (ru->ru_stime.tv_sec || ru->ru_stime.tv_usec) {
    appendw(&b, READ16LE(", "));
    appendf(&b, "%s={%ld, %ld}", "stime", ru->ru_stime.tv_sec,
            ru->ru_stime.tv_usec * 1000);
  }
  if (ru->ru_maxrss) appendf(&b, ", %s=%ld", "maxrss", ru->ru_maxrss);
  if (ru->ru_ixrss) appendf(&b, ", %s=%ld", "ixrss", ru->ru_ixrss);
  if (ru->ru_idrss) appendf(&b, ", %s=%ld", "idrss", ru->ru_idrss);
  if (ru->ru_isrss) appendf(&b, ", %s=%ld", "isrss", ru->ru_isrss);
  if (ru->ru_minflt) appendf(&b, ", %s=%ld", "minflt", ru->ru_minflt);
  if (ru->ru_majflt) appendf(&b, ", %s=%ld", "majflt", ru->ru_majflt);
  if (ru->ru_nswap) appendf(&b, ", %s=%ld", "nswap", ru->ru_nswap);
  if (ru->ru_inblock) appendf(&b, ", %s=%ld", "inblock", ru->ru_inblock);
  if (ru->ru_oublock) appendf(&b, ", %s=%ld", "oublock", ru->ru_oublock);
  if (ru->ru_msgsnd) appendf(&b, ", %s=%ld", "msgsnd", ru->ru_msgsnd);
  if (ru->ru_msgrcv) appendf(&b, ", %s=%ld", "msgrcv", ru->ru_msgrcv);
  if (ru->ru_nsignals) appendf(&b, ", %s=%ld", "nsignals", ru->ru_nsignals);
  if (ru->ru_nvcsw) appendf(&b, ", %s=%ld", "nvcsw", ru->ru_nvcsw);
  if (ru->ru_nivcsw) appendf(&b, ", %s=%ld", "nivcsw", ru->ru_nivcsw);
  appendw(&b, '}');
  lua_pushlstring(L, b, appendz(b).i);
  return 1;
}

static const luaL_Reg kLuaUnixRusageMeth[] = {
    {"utime", LuaUnixRusageUtime},        //
    {"stime", LuaUnixRusageStime},        //
    {"maxrss", LuaUnixRusageMaxrss},      //
    {"ixrss", LuaUnixRusageIxrss},        //
    {"idrss", LuaUnixRusageIdrss},        //
    {"isrss", LuaUnixRusageIsrss},        //
    {"minflt", LuaUnixRusageMinflt},      //
    {"majflt", LuaUnixRusageMajflt},      //
    {"nswap", LuaUnixRusageNswap},        //
    {"inblock", LuaUnixRusageInblock},    //
    {"oublock", LuaUnixRusageOublock},    //
    {"msgsnd", LuaUnixRusageMsgsnd},      //
    {"msgrcv", LuaUnixRusageMsgrcv},      //
    {"nsignals", LuaUnixRusageNsignals},  //
    {"nvcsw", LuaUnixRusageNvcsw},        //
    {"nivcsw", LuaUnixRusageNivcsw},      //
    {0},                                  //
};

static const luaL_Reg kLuaUnixRusageMeta[] = {
    {"__repr", LuaUnixRusageToString},      //
    {"__tostring", LuaUnixRusageToString},  //
    {0},                                    //
};

void LuaUnixRusageObj(lua_State *L) {
  luaL_newmetatable(L, "unix.Rusage");
  luaL_setfuncs(L, kLuaUnixRusageMeta, 0);
  luaL_newlibtable(L, kLuaUnixRusageMeth);
  luaL_setfuncs(L, kLuaUnixRusageMeth, 0);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);
}
