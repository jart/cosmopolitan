#ifndef COSMOPOLITAN_TOOL_NET_LFETCH_H_
#define COSMOPOLITAN_TOOL_NET_LFETCH_H_
#include "third_party/lua/lauxlib.h"
COSMOPOLITAN_C_START_

int LuaFetch(lua_State *);
void LuaInitFetch(void);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_TOOL_NET_LFETCH_H_ */
