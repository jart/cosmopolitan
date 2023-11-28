#ifndef COSMOPOLITAN_THIRD_PARTY_LUA_LUNIX_H_
#define COSMOPOLITAN_THIRD_PARTY_LUA_LUNIX_H_
#include "third_party/lua/lauxlib.h"
COSMOPOLITAN_C_START_

struct UnixErrno {
  int errno_;
  int winerr;
  const char *call;
};

int LuaUnix(lua_State *);
int LuaUnixSysretErrno(lua_State *, const char *, int);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_LUA_LUNIX_H_ */
