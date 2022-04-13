#ifndef COSMOPOLITAN_TOOL_NET_LUACHECK_H_
#define COSMOPOLITAN_TOOL_NET_LUACHECK_H_
#include "libc/log/log.h"
#include "libc/mem/mem.h"
#include "third_party/lua/cosmo.h"
#include "third_party/lua/lua.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define AssertLuaStackIsEmpty(L)                  \
  do {                                            \
    if (lua_gettop(L)) {                          \
      char *s = LuaFormatStack(L);                \
      WARNF("lua stack should be empty!\n%s", s); \
      free(s);                                    \
      lua_settop(L, 0);                           \
    }                                             \
  } while (0)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_NET_LUACHECK_H_ */
