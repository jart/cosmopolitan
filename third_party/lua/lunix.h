#ifndef COSMOPOLITAN_THIRD_PARTY_LUA_LUNIX_H_
#define COSMOPOLITAN_THIRD_PARTY_LUA_LUNIX_H_
#include "third_party/lua/lauxlib.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct UnixErrno {
  int errno_;
  int winerr;
  const char *call;
};

int LuaUnix(lua_State *);
int LuaUnixSysretErrno(lua_State *, const char *, int);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_LUA_LUNIX_H_ */
