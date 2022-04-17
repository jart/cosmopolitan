#ifndef COSMOPOLITAN_THIRD_PARTY_LUA_LREPL_H_
#define COSMOPOLITAN_THIRD_PARTY_LUA_LREPL_H_
#include "third_party/linenoise/linenoise.h"
#include "third_party/lua/lauxlib.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int lua_loadline(lua_State *);
void lua_l_print(lua_State *);
void lua_initrepl(const char *);
int lua_report(lua_State *, int);
int lua_runchunk(lua_State *, int, int);
void lua_l_message(const char *, const char *);
char *lua_readline_hint(const char *, const char **, const char **);
void lua_readline_completions(const char *, linenoiseCompletions *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_LUA_LREPL_H_ */
