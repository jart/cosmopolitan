#ifndef COSMOPOLITAN_THIRD_PARTY_LUA_LREPL_H_
#define COSMOPOLITAN_THIRD_PARTY_LUA_LREPL_H_
#include "libc/dce.h"
#include "libc/intrin/spinlock.h"
#include "third_party/linenoise/linenoise.h"
#include "third_party/lua/lauxlib.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#if !defined(STATIC) && SupportsWindows()
#define LUA_REPL_LOCK _spinlock(&lua_repl_lock)
#else
#define LUA_REPL_LOCK (void)0
#endif

#if !defined(STATIC) && SupportsWindows()
#define LUA_REPL_UNLOCK _spunlock(&lua_repl_lock)
#else
#define LUA_REPL_UNLOCK (void)0
#endif

extern char lua_repl_lock;
extern bool lua_repl_blocking;
extern bool lua_repl_isterminal;
extern struct linenoiseState *lua_repl_linenoise;
extern linenoiseCompletionCallback *lua_repl_completions_callback;

void lua_freerepl(void);
int lua_loadline(lua_State *);
void lua_l_print(lua_State *);
void lua_sigint(lua_State *, int);
void lua_initrepl(lua_State *, const char *);
int lua_report(lua_State *, int);
int lua_runchunk(lua_State *, int, int);
void lua_l_message(const char *, const char *);
char *lua_readline_hint(const char *, const char **, const char **);
void lua_readline_completions(const char *, linenoiseCompletions *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_LUA_LREPL_H_ */
