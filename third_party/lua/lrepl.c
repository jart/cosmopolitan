/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╚──────────────────────────────────────────────────────────────────────────────╝
│                                                                              │
│  Lua                                                                         │
│  Copyright © 2004-2021 Lua.org, PUC-Rio.                                     │
│                                                                              │
│  Permission is hereby granted, free of charge, to any person obtaining       │
│  a copy of this software and associated documentation files (the             │
│  "Software"), to deal in the Software without restriction, including         │
│  without limitation the rights to use, copy, modify, merge, publish,         │
│  distribute, sublicense, and/or sell copies of the Software, and to          │
│  permit persons to whom the Software is furnished to do so, subject to       │
│  the following conditions:                                                   │
│                                                                              │
│  The above copyright notice and this permission notice shall be              │
│  included in all copies or substantial portions of the Software.             │
│                                                                              │
│  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,             │
│  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF          │
│  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.      │
│  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY        │
│  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,        │
│  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE           │
│  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                      │
│                                                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#define lua_c
#include "third_party/lua/lrepl.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/errno.h"
#include "libc/intrin/nomultics.internal.h"
#include "libc/log/check.h"
#include "libc/macros.internal.h"
#include "libc/mem/alg.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/thread/thread.h"
#include "third_party/linenoise/linenoise.h"
#include "third_party/lua/cosmo.h"
#include "third_party/lua/lauxlib.h"
#include "third_party/lua/lprefix.h"
#include "third_party/lua/lua.h"
#include "third_party/lua/lualib.h"

asm(".ident\t\"\\n\\n\
Lua 5.4.3 (MIT License)\\n\
Copyright 1994–2021 Lua.org, PUC-Rio.\"");
asm(".include \"libc/disclaimer.inc\"");


static const char *const kKeywordHints[] = {
  "else",     //
  "elseif",   //
  "function", //
  "function", //
  "repeat",   //
  "then",     //
  "until",    //
  "while",    //
};

bool lua_repl_blocking;
bool lua_repl_isterminal;
linenoiseCompletionCallback *lua_repl_completions_callback;
struct linenoiseState *lua_repl_linenoise;
const char *lua_progname;
static lua_State *globalL;
static char *g_historypath;

/*
** {==================================================================
** Read-Eval-Print Loop (REPL)
** ===================================================================
*/

#if !defined(LUA_PROMPT)
#define LUA_PROMPT		">: "
#define LUA_PROMPT2		">>: "
#endif

#if !defined(LUA_MAXINPUT)
#define LUA_MAXINPUT		512
#endif


static void lua_readline_addcompletion (linenoiseCompletions *c, char *s) {
  char **p;
  if ((p = realloc(c->cvec, (c->len + 1) * sizeof(*p)))) {
    c->cvec = p;
    c->cvec[c->len++] = s;
  }
}


void lua_readline_completions (const char *p, linenoiseCompletions *c) {
  int i;
  size_t n;
  bool found;
  lua_State *L;
  const char *a;
  const char *name;
  char *b, *s, *component;

  // start searching globals
  L = globalL;
  lua_pushglobaltable(L);

  // traverse parent objects
  // split foo.bar and foo:bar
  a = p;
  b = strpbrk(a, ".:");
  while (b) {
    found = false;
    if (lua_istable(L, -1)) {
      component = strndup(a, b - a);
      lua_pushnil(L);  // search key
      while (lua_next(L, -2)) {
        if (lua_type(L, -2) == LUA_TSTRING) {
          name = lua_tostring(L, -2);
          if (!strcmp(name, component)) {
            lua_remove(L, -3);  // remove table
            lua_remove(L, -2);  // remove key
            found = true;
            break;
          }
        }
        lua_pop(L, 1);  // pop value
      }
      free(component);
    }
    if (!found) {
      lua_pop(L, 1);  // pop table
      return;
    }
    a = b + 1;
    b = strpbrk(a, ".:");
  }

  // search final object
  if (lua_type(L, -1) == LUA_TTABLE) {
    lua_pushnil(L);
    while (lua_next(L, -2)) {
      if (lua_type(L, -2) == LUA_TSTRING) {
        name = lua_tolstring(L, -2, &n);
        if (startswithi(name, a) && (s = malloc(a - p + n + 1))) {
          memcpy(s, p, a - p);
          memcpy(s + (a - p), name, n + 1);
          lua_readline_addcompletion(c, s);
        }
      }
      lua_pop(L, 1);
    }
  }

  lua_pop(L, 1); // pop table

  for (i = 0; i < ARRAYLEN(kKeywordHints); ++i) {
    if (startswithi(kKeywordHints[i], p)) {
      if ((s = strdup(kKeywordHints[i]))) {
        lua_readline_addcompletion(c, s);
      }
    }
  }
  if (lua_repl_completions_callback) {
    lua_repl_completions_callback(p, c);
  }
}


char *lua_readline_hint (const char *p, const char **ansi1, const char **ansi2) {
  char *h;
  linenoiseCompletions c = {0};
  lua_readline_completions(p, &c);
  h = c.len == 1 ? strdup(c.cvec[0] + strlen(p)) : 0;
  linenoiseFreeCompletions(&c);
  return h;
}


static void lua_freeline (lua_State *L, char *b) {
  free(b);
}


/*
** Return the string to be used as a prompt by the interpreter. Leave
** the string (or nil, if using the default value) on the stack, to keep
** it anchored.
*/
static const char *get_prompt (lua_State *L, int firstline) {
  if (lua_getglobal(L, firstline ? "_PROMPT" : "_PROMPT2") == LUA_TNIL)
    return (firstline ? LUA_PROMPT : LUA_PROMPT2);  /* use the default */
  else {  /* apply 'tostring' over the value */
    const char *p = luaL_tolstring(L, -1, NULL);
    lua_remove(L, -2);  /* remove original value */
    return p;
  }
}


/* mark in error messages for incomplete statements */
#define EOFMARK		"<eof>"
#define marklen		(sizeof(EOFMARK)/sizeof(char) - 1)


/*
** Check whether 'status' signals a syntax error and the error
** message at the top of the stack ends with the above mark for
** incomplete statements.
*/
static int incomplete (lua_State *L, int status) {
  if (status == LUA_ERRSYNTAX) {
    size_t lmsg;
    const char *msg = lua_tolstring(L, -1, &lmsg);
    if (lmsg >= marklen && strcmp(msg + lmsg - marklen, EOFMARK) == 0) {
      lua_pop(L, 1);
      return 1;
    }
  }
  return 0;  /* else... */
}


/*
** Prompt the user, read a line, and push it into the Lua stack.
*/
static ssize_t pushline (lua_State *L, int firstline) {
  char *b;
  size_t l;
  ssize_t rc;
  char *prmt;
  globalL = L;
  prmt = strdup(get_prompt(L, firstline));
  lua_pop(L, 1);  /* remove prompt */
  if (lua_repl_isterminal) {
    lua_repl_unlock();
    rc = linenoiseEdit(lua_repl_linenoise, prmt, &b, !firstline || lua_repl_blocking);
    free(prmt);
    if (rc != -1) {
      if (b && *b) {
        linenoiseHistoryLoad(g_historypath);
        linenoiseHistoryAdd(b);
        linenoiseHistorySave(g_historypath);
      }
    }
    lua_repl_lock();
  } else {
    lua_repl_unlock();
    fputs(prmt, stdout);
    free(prmt);
    fflush(stdout);
    b = linenoiseGetLine(stdin);
    if (b) {
      rc = 1;
    } else if (ferror(stdin)) {
      rc = -1;
    } else {
      rc = 0;
    }
    lua_repl_lock();
  }
  if (!(rc == -1 && errno == EAGAIN)) {
    write(1, "\n", 1);
  }
  if (rc == -1 || (!rc && !b)) {
    return rc;
  }
  l = strlen(b);
  if (l > 0 && b[l-1] == '\n')  /* line ends with newline? */
    b[--l] = '\0';  /* remove it */
  if (firstline && b[0] == '=') {  /* for compatibility with 5.2, ... */
    lua_pushfstring(L, "return %s", b + 1);  /* change '=' to 'return' */
  } else {
    lua_pushlstring(L, b, l);
  }
  lua_freeline(L, b);
  return 1;
}


/*
** Try to compile line on the stack as 'return <line>;'; on return, stack
** has either compiled chunk or original line (if compilation failed).
*/
static int addreturn (lua_State *L) {
  const char *line = lua_tostring(L, -1);  /* original line */
  const char *retline = lua_pushfstring(L, "return %s;", line);
  int status = luaL_loadbuffer(L, retline, strlen(retline), "=stdin");
  if (status == LUA_OK) {
    lua_remove(L, -2);  /* remove modified line */
  } else {
    lua_pop(L, 2);  /* pop result from 'luaL_loadbuffer' and modified line */
  }
  return status;
}


/*
** Hook set by signal function to stop the interpreter.
*/
static void lstop (lua_State *L, lua_Debug *ar) {
  (void)ar;  /* unused arg. */
  lua_sethook(L, NULL, 0, 0);  /* reset hook */
  luaL_error(L, "interrupted!");
}


/*
** Read multiple lines until a complete Lua statement
*/
static int multiline (lua_State *L) {
  for (;;) {  /* repeat until gets a complete statement */
    size_t len;
    const char *line = lua_tolstring(L, 1, &len);  /* get what it has */
    int status = luaL_loadbuffer(L, line, len, "=stdin");  /* try it */
    if (!incomplete(L, status) || pushline(L, 0) != 1)
      return status;  /* cannot or should not try to add continuation line */
    lua_pushliteral(L, "\n");  /* add newline... */
    lua_insert(L, -2);  /* ...between the two lines */
    lua_concat(L, 3);  /* join them */
  }
}


void lua_initrepl(lua_State *L) {
  const char *prompt;
  lua_repl_lock();
  if ((lua_repl_isterminal = linenoiseIsTerminal())) {
    linenoiseSetCompletionCallback(lua_readline_completions);
    linenoiseSetHintsCallback(lua_readline_hint);
    linenoiseSetFreeHintsCallback(free);
    prompt = get_prompt(L, 1);
    if ((g_historypath = linenoiseGetHistoryPath(lua_progname))) {
      if (linenoiseHistoryLoad(g_historypath) == -1) {
        fprintf(stderr, "%r%s: failed to load history: %m%n", g_historypath);
        free(g_historypath);
        g_historypath = 0;
      }
    }
    lua_repl_linenoise = linenoiseBegin(prompt, 0, 1);
    lua_pop(L, 1);  /* remove prompt */
    __ttyconf.replmode = true;
    if (isatty(2)) {
      __ttyconf.replstderr = true;
    }
  }
  lua_repl_unlock();
}


void lua_freerepl(void) {
  lua_repl_lock();
  __ttyconf.replmode = false;
  linenoiseEnd(lua_repl_linenoise);
  free(g_historypath);
  lua_repl_unlock();
}


/*
** Read a line and try to load (compile) it first as an expression (by
** adding "return " in front of it) and second as a statement. Return
** the final status of load/call with the resulting function (if any)
** in the top of the stack.
**
** returns -1 on eof
** returns -2 on error
*/
int lua_loadline (lua_State *L) {
  ssize_t rc;
  int status;
  lua_settop(L, 0);
  lua_repl_lock();
  if ((rc = pushline(L, 1)) != 1) {
    lua_repl_unlock();
    return rc - 1;  /* eof or error */
  }
  if ((status = addreturn(L)) != LUA_OK) /* 'return ...' did not work? */
    status = multiline(L);  /* try as command, maybe with continuation lines */
  lua_remove(L, 1);  /* remove line from the stack */
  lua_assert(lua_gettop(L) == 1);
  lua_repl_unlock();
  return status;
}


void lua_sigint (lua_State *L, int sig) {
  int flag = LUA_MASKCALL | LUA_MASKRET | LUA_MASKLINE | LUA_MASKCOUNT;
  lua_sethook(L, lstop, flag, 1);
}



/*
** Function to be called at a C signal. Because a C signal cannot
** just change a Lua state (as there is no proper synchronization),
** this function only sets a hook that, when called, will stop the
** interpreter.
*/
static void laction (int i) {
  lua_sigint(globalL, i);
  int flag = LUA_MASKCALL | LUA_MASKRET | LUA_MASKLINE | LUA_MASKCOUNT;
  lua_sethook(globalL, lstop, flag, 1);
}


/*
** Message handler used to run all chunks
*/
static int msghandler (lua_State *L) {
  const char *msg = lua_tostring(L, 1);
  if (msg == NULL) {  /* is error object not a string? */
    if (luaL_callmeta(L, 1, "__tostring") &&  /* does it have a metamethod */
        lua_type(L, -1) == LUA_TSTRING)  /* that produces a string? */
      return 1;  /* that is the message */
    else
      msg = lua_pushfstring(L, "(error object is a %s value)",
                               luaL_typename(L, 1));
  }
  luaL_traceback(L, L, msg, 1);  /* append a standard traceback */
  return 1;  /* return the traceback */
}


/*
** Interface to 'lua_pcall', which sets appropriate message function
** and C-signal handler. Used to run all chunks.
*/
int lua_runchunk (lua_State *L, int narg, int nres) {
  struct sigaction sa, saold;
  int status;
  int base = lua_gettop(L) - narg;  /* function index */
  lua_pushcfunction(L, msghandler);  /* push message handler */
  lua_insert(L, base);  /* put it under function and args */
  globalL = L;  /* to be available to 'laction' */
  sa.sa_flags = SA_RESETHAND; /* if another int happens, terminate */
  sa.sa_handler = laction;
  sigemptyset(&sa.sa_mask); /* do not mask any signal */
  sigaction(SIGINT, &sa, &saold);
  status = lua_pcall(L, narg, nres, base);
  sigaction(SIGINT, &saold, 0); /* restore C-signal handler */
  lua_remove(L, base);  /* remove message handler from the stack */
  return status;
}


/*
** Prints an error message, adding the program name in front of it
** (if present)
*/
void lua_l_message (const char *pname, const char *msg) {
  if (pname) lua_writestringerror("%s: ", pname);
  lua_writestringerror("%s\n", msg);
}


/*
** Prints (calling the Lua 'print' function) any values on the stack
*/
void lua_l_print (lua_State *L) {
  int n = lua_gettop(L);
  if (n > 0) {  /* any result to be printed? */
    luaL_checkstack(L, LUA_MINSTACK, "too many results to print");
    lua_getglobal(L, "print");
    lua_insert(L, 1);
    if (lua_pcall(L, n, 0, 0) != LUA_OK)
      lua_l_message(lua_progname, lua_pushfstring(L, "error calling 'print' (%s)",
                                                lua_tostring(L, -1)));
  }
}


/*
** Check whether 'status' is not OK and, if so, prints the error
** message on the top of the stack. It assumes that the error object
** is a string, as it was either generated by Lua or by 'msghandler'.
*/
int lua_report (lua_State *L, int status) {
  if (status != LUA_OK) {
    const char *msg = lua_tostring(L, -1);
    lua_l_message(lua_progname, msg);
    lua_pop(L, 1);  /* remove message */
  }
  return status;
}
