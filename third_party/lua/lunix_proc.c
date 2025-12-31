/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
│ Copyright 2024 Will Hipschman                                                │
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
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/proc/posix_spawn.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/prio.h"
#include "libc/temp.h"

////////////////////////////////////////////////////////////////////////////////
// Process management extensions

// unix.execvp(prog:str[, argv:table])
//     ├─→ ⊥
//     └─→ nil, unix.Errno
int LuaUnixExecvp(lua_State *L) {
  int olderr;
  const char *prog;
  char **argv, **freeme, *ezargs[2];
  olderr = errno;
  prog = luaL_checkstring(L, 1);
  if (!lua_isnoneornil(L, 2)) {
    if ((argv = ConvertLuaArrayToStringList(L, 2))) {
      freeme = argv;
    } else {
      return LuaUnixSysretErrno(L, "execvp", olderr);
    }
  } else {
    ezargs[0] = (char *)prog;
    ezargs[1] = 0;
    argv = ezargs;
    freeme = 0;
  }
  execvp(prog, argv);
  FreeStringList(freeme);
  return LuaUnixSysretErrno(L, "execvp", olderr);
}

// unix.execvpe(prog:str, argv:table[, envp:table])
//     ├─→ ⊥
//     └─→ nil, unix.Errno
int LuaUnixExecvpe(lua_State *L) {
  int olderr;
  const char *prog;
  char **argv, **envp, **freeme1, **freeme2;
  olderr = errno;
  prog = luaL_checkstring(L, 1);
  if ((argv = ConvertLuaArrayToStringList(L, 2))) {
    freeme1 = argv;
    if (!lua_isnoneornil(L, 3)) {
      if ((envp = ConvertLuaArrayToStringList(L, 3))) {
        freeme2 = envp;
      } else {
        FreeStringList(argv);
        return LuaUnixSysretErrno(L, "execvpe", olderr);
      }
    } else {
      envp = environ;
      freeme2 = 0;
    }
  } else {
    return LuaUnixSysretErrno(L, "execvpe", olderr);
  }
  execvpe(prog, argv, envp);
  FreeStringList(freeme1);
  FreeStringList(freeme2);
  return LuaUnixSysretErrno(L, "execvpe", olderr);
}

// unix.fexecve(fd:int, argv:table[, envp:table])
//     ├─→ ⊥
//     └─→ nil, unix.Errno
int LuaUnixFexecve(lua_State *L) {
  int olderr, fd;
  char **argv, **envp, **freeme1, **freeme2;
  olderr = errno;
  fd = luaL_checkinteger(L, 1);
  if ((argv = ConvertLuaArrayToStringList(L, 2))) {
    freeme1 = argv;
    if (!lua_isnoneornil(L, 3)) {
      if ((envp = ConvertLuaArrayToStringList(L, 3))) {
        freeme2 = envp;
      } else {
        FreeStringList(argv);
        return LuaUnixSysretErrno(L, "fexecve", olderr);
      }
    } else {
      envp = environ;
      freeme2 = 0;
    }
  } else {
    return LuaUnixSysretErrno(L, "fexecve", olderr);
  }
  fexecve(fd, argv, envp);
  FreeStringList(freeme1);
  FreeStringList(freeme2);
  return LuaUnixSysretErrno(L, "fexecve", olderr);
}

// unix.spawn(prog:str, argv:table[, envp:table])
//     ├─→ pid:int
//     └─→ nil, unix.Errno
int LuaUnixSpawn(lua_State *L) {
  int olderr, rc;
  pid_t pid;
  const char *prog;
  char **argv, **envp, **freeme1, **freeme2;
  posix_spawn_file_actions_t fa;
  olderr = errno;
  prog = luaL_checkstring(L, 1);
  if ((argv = ConvertLuaArrayToStringList(L, 2))) {
    freeme1 = argv;
    if (!lua_isnoneornil(L, 3)) {
      if ((envp = ConvertLuaArrayToStringList(L, 3))) {
        freeme2 = envp;
      } else {
        FreeStringList(argv);
        return LuaUnixSysretErrno(L, "spawn", olderr);
      }
    } else {
      envp = environ;
      freeme2 = 0;
    }
  } else {
    return LuaUnixSysretErrno(L, "spawn", olderr);
  }
  posix_spawn_file_actions_init(&fa);
  rc = posix_spawn(&pid, prog, &fa, NULL, argv, envp);
  posix_spawn_file_actions_destroy(&fa);
  FreeStringList(freeme1);
  FreeStringList(freeme2);
  if (rc == 0) {
    lua_pushinteger(L, pid);
    return 1;
  } else {
    errno = rc;
    return LuaUnixSysretErrno(L, "spawn", olderr);
  }
}

// unix.spawnp(prog:str, argv:table[, envp:table])
//     ├─→ pid:int
//     └─→ nil, unix.Errno
int LuaUnixSpawnp(lua_State *L) {
  int olderr, rc;
  pid_t pid;
  const char *prog;
  char **argv, **envp, **freeme1, **freeme2;
  posix_spawn_file_actions_t fa;
  olderr = errno;
  prog = luaL_checkstring(L, 1);
  if ((argv = ConvertLuaArrayToStringList(L, 2))) {
    freeme1 = argv;
    if (!lua_isnoneornil(L, 3)) {
      if ((envp = ConvertLuaArrayToStringList(L, 3))) {
        freeme2 = envp;
      } else {
        FreeStringList(argv);
        return LuaUnixSysretErrno(L, "spawnp", olderr);
      }
    } else {
      envp = environ;
      freeme2 = 0;
    }
  } else {
    return LuaUnixSysretErrno(L, "spawnp", olderr);
  }
  posix_spawn_file_actions_init(&fa);
  rc = posix_spawnp(&pid, prog, &fa, NULL, argv, envp);
  posix_spawn_file_actions_destroy(&fa);
  FreeStringList(freeme1);
  FreeStringList(freeme2);
  if (rc == 0) {
    lua_pushinteger(L, pid);
    return 1;
  } else {
    errno = rc;
    return LuaUnixSysretErrno(L, "spawnp", olderr);
  }
}

// unix.killpg(pgrp:int, sig:int)
//     ├─→ true
//     └─→ nil, unix.Errno
int LuaUnixKillpg(lua_State *L) {
  int olderr = errno;
  return SysretBool(L, "killpg", olderr,
                    killpg(luaL_checkinteger(L, 1), luaL_checkinteger(L, 2)));
}

// unix.daemon([nochdir:bool[, noclose:bool]])
//     ├─→ true
//     └─→ nil, unix.Errno
int LuaUnixDaemon(lua_State *L) {
  int olderr = errno;
  int nochdir = lua_toboolean(L, 1);
  int noclose = lua_toboolean(L, 2);
  int rc = daemon(nochdir, noclose);
  if (rc != -1) {
    lua_pushboolean(L, 1);
    return 1;
  } else {
    return LuaUnixSysretErrno(L, "daemon", olderr);
  }
}

// unix.nice(inc:int)
//     ├─→ priority:int
//     └─→ nil, unix.Errno
int LuaUnixNice(lua_State *L) {
  int olderr = errno;
  int inc = luaL_checkinteger(L, 1);
  errno = 0;
  int rc = nice(inc);
  if (rc == -1 && errno != 0) {
    return LuaUnixSysretErrno(L, "nice", olderr);
  }
  errno = olderr;
  lua_pushinteger(L, rc);
  return 1;
}

// unix.getpriority(which:int, who:int)
//     ├─→ priority:int
//     └─→ nil, unix.Errno
//
// which can be:
//   - unix.PRIO_PROCESS (0) - who is process id (0 = calling process)
//   - unix.PRIO_PGRP (1) - who is process group id (0 = calling process group)
//   - unix.PRIO_USER (2) - who is user id (0 = calling user)
int LuaUnixGetpriority(lua_State *L) {
  int olderr = errno;
  int which = luaL_checkinteger(L, 1);
  int who = luaL_checkinteger(L, 2);
  errno = 0;
  int rc = getpriority(which, who);
  if (rc == -1 && errno != 0) {
    return LuaUnixSysretErrno(L, "getpriority", olderr);
  }
  errno = olderr;
  lua_pushinteger(L, rc);
  return 1;
}

// unix.setpriority(which:int, who:int, prio:int)
//     ├─→ true
//     └─→ nil, unix.Errno
//
// which can be:
//   - unix.PRIO_PROCESS (0) - who is process id (0 = calling process)
//   - unix.PRIO_PGRP (1) - who is process group id (0 = calling process group)
//   - unix.PRIO_USER (2) - who is user id (0 = calling user)
int LuaUnixSetpriority(lua_State *L) {
  int olderr = errno;
  int which = luaL_checkinteger(L, 1);
  int who = luaL_checkinteger(L, 2);
  int prio = luaL_checkinteger(L, 3);
  int rc = setpriority(which, who, prio);
  if (rc != -1) {
    lua_pushboolean(L, 1);
    return 1;
  } else {
    return LuaUnixSysretErrno(L, "setpriority", olderr);
  }
}

// unix.mkdtemp(template:str)
//     ├─→ path:str
//     └─→ nil, unix.Errno
int LuaUnixMkdtemp(lua_State *L) {
  char *path;
  int olderr = errno;
  const char *template = luaL_checkstring(L, 1);
  size_t len = strlen(template);
  path = malloc(len + 1);
  if (!path) {
    return LuaUnixSysretErrno(L, "mkdtemp", olderr);
  }
  memcpy(path, template, len + 1);
  if (mkdtemp(path)) {
    lua_pushstring(L, path);
    free(path);
    return 1;
  } else {
    free(path);
    return LuaUnixSysretErrno(L, "mkdtemp", olderr);
  }
}

// unix.mkstemp(template:str)
//     ├─→ fd:int, path:str
//     └─→ nil, unix.Errno
int LuaUnixMkstemp(lua_State *L) {
  char *path;
  int fd, olderr = errno;
  const char *template = luaL_checkstring(L, 1);
  size_t len = strlen(template);
  path = malloc(len + 1);
  if (!path) {
    return LuaUnixSysretErrno(L, "mkstemp", olderr);
  }
  memcpy(path, template, len + 1);
  if ((fd = mkstemp(path)) != -1) {
    lua_pushinteger(L, fd);
    lua_pushstring(L, path);
    free(path);
    return 2;
  } else {
    free(path);
    return LuaUnixSysretErrno(L, "mkstemp", olderr);
  }
}
