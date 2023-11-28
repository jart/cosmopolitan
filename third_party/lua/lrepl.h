#ifndef COSMOPOLITAN_THIRD_PARTY_LUA_LREPL_H_
#define COSMOPOLITAN_THIRD_PARTY_LUA_LREPL_H_
#include "third_party/linenoise/linenoise.h"
#include "third_party/lua/lauxlib.h"
COSMOPOLITAN_C_START_

extern bool lua_repl_blocking;
extern bool lua_repl_isterminal;
extern const char *lua_progname;
extern struct linenoiseState *lua_repl_linenoise;
extern linenoiseCompletionCallback *lua_repl_completions_callback;

void lua_freerepl(void);
void lua_repl_lock(void);
void lua_repl_unlock(void);
int lua_loadline(lua_State *);
void lua_l_print(lua_State *);
void lua_initrepl(lua_State *);
void lua_sigint(lua_State *, int);
int lua_report(lua_State *, int);
int lua_runchunk(lua_State *, int, int);
void lua_l_message(const char *, const char *);
char *lua_readline_hint(const char *, const char **, const char **);
void lua_readline_completions(const char *, linenoiseCompletions *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_LUA_LREPL_H_ */
