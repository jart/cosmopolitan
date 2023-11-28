#ifndef COSMOPOLITAN_TOOL_NET_LUACHECK_H_
#define COSMOPOLITAN_TOOL_NET_LUACHECK_H_
#include "libc/log/log.h"
#include "libc/mem/mem.h"
#include "third_party/lua/cosmo.h"
#include "third_party/lua/lua.h"
COSMOPOLITAN_C_START_

#define AssertLuaStackIsAt(L, level)      \
  do {                                    \
    if (lua_gettop(L) > level) {          \
      char *s = LuaFormatStack(L);        \
      WARNF("lua stack should be at %d;"  \
            " extra values ignored:\n%s", \
            level, s);                    \
      free(s);                            \
    }                                     \
  } while (0)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_TOOL_NET_LUACHECK_H_ */
