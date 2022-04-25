/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/ioctl.h"
#include "libc/calls/sigbits.h"
#include "libc/calls/strace.internal.h"
#include "libc/calls/struct/dirent.h"
#include "libc/calls/struct/itimerval.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
#include "libc/calls/ucontext.h"
#include "libc/dns/dns.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/fmt.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/sock/syslog.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/dt.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/ip.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/itimer.h"
#include "libc/sysv/consts/log.h"
#include "libc/sysv/consts/msg.h"
#include "libc/sysv/consts/nr.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/ok.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/rlim.h"
#include "libc/sysv/consts/rlimit.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/shut.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/sio.h"
#include "libc/sysv/consts/so.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/consts/sol.h"
#include "libc/sysv/consts/tcp.h"
#include "libc/sysv/consts/w.h"
#include "libc/sysv/errfuns.h"
#include "libc/time/time.h"
#include "libc/x/x.h"
#include "third_party/lua/cosmo.h"
#include "third_party/lua/lauxlib.h"
#include "third_party/lua/lua.h"
#include "third_party/lua/luaconf.h"
#include "tool/net/luacheck.h"

/**
 * @fileoverview UNIX system calls thinly wrapped for Lua
 * @support Linux, Mac, Windows, FreeBSD, NetBSD, OpenBSD
 */

struct UnixDir {
  int refs;
  DIR *dir;
};

struct UnixStat {
  int refs;
  struct stat st;
};

struct UnixErrno {
  int refs;
  int errno;
};

static lua_State *GL;

static void LuaSetIntField(lua_State *L, const char *k, lua_Integer v) {
  lua_pushinteger(L, v);
  lua_setfield(L, -2, k);
}

static dontinline int ReturnInteger(lua_State *L, lua_Integer x) {
  lua_pushinteger(L, x);
  return 1;
}

static dontinline int ReturnString(lua_State *L, const char *x) {
  lua_pushstring(L, x);
  return 1;
}

static void LuaUnixPushErrno(lua_State *L, int err) {
  // TODO: How do we solve "error object is a userdata value"?
  lua_pushinteger(L, err);
  return;
  ////////////////////////////////////////////////////////////
  struct UnixErrno *ue, **uep;
  ue = xcalloc(1, sizeof(struct UnixErrno));
  ue->refs = 1;
  ue->errno = err;
  uep = lua_newuserdatauv(L, sizeof(*uep), 1);
  luaL_setmetatable(L, "unix.Errno");
  *uep = ue;
}

static dontinline int SysretErrnoImpl(lua_State *L, int olderr, bool usebool) {
  int i, newerr = errno;
  if (!IsTiny() && !(0 < newerr && newerr < (!IsWindows() ? 4096 : 65536))) {
    WARNF("errno should not be %d", newerr);
  }
  if (usebool) {
    lua_pushboolean(L, false);
  } else {
    lua_pushnil(L);
  }
  LuaUnixPushErrno(L, newerr);
  errno = olderr;
  return 2;
}

static dontinline int SysretErrnoNil(lua_State *L, int olderr) {
  return SysretErrnoImpl(L, olderr, false);
}

static dontinline int SysretErrnoBool(lua_State *L, int olderr) {
  return SysretErrnoImpl(L, olderr, true);
}

static int SysretBool(lua_State *L, int rc, int olderr) {
  if (!IsTiny() && (rc != 0 && rc != -1)) {
    WARNF("syscall supposed to return 0 / -1 but got %d", rc);
  }
  if (rc != -1) {
    lua_pushboolean(L, true);
    return 0;
  } else {
    return SysretErrnoBool(L, olderr);
  }
}

static int SysretInteger(lua_State *L, int64_t rc, int olderr) {
  if (rc != -1) {
    if (!IsTiny() && olderr != errno) {
      WARNF("errno unexpectedly changed %d → %d", olderr, errno);
    }
    lua_pushinteger(L, rc);
    return 1;
  } else {
    return SysretErrnoNil(L, olderr);
  }
}

static void CheckOptvalsize(lua_State *L, uint32_t want, uint32_t got) {
  if (!IsTiny()) {
    if (want == got) return;
    WARNF("getsockopt optvalsize should be %d but was %d", want, got);
  }
}

static void FreeStringList(char **p) {
  int i;
  if (p) {
    for (i = 0; p[i]; ++i) {
      free(p[i]);
    }
    free(p);
  }
}

static char **ConvertLuaArrayToStringList(lua_State *L, int i) {
  int j, n;
  char **p, *s;
  luaL_checktype(L, i, LUA_TTABLE);
  lua_len(L, i);
  n = lua_tointeger(L, -1);
  lua_pop(L, 1);
  if ((p = calloc(n + 1, sizeof(*p)))) {
    for (j = 1; j <= n; ++j) {
      lua_geti(L, i, j);
      s = strdup(lua_tostring(L, -1));
      lua_pop(L, 1);
      if (s) {
        p[j - 1] = s;
      } else {
        FreeStringList(p);
        p = 0;
        break;
      }
    }
  }
  return p;
}

////////////////////////////////////////////////////////////////////////////////
// System Calls

static dontinline int LuaUnixGetid(lua_State *L, int f(void)) {
  return ReturnInteger(L, f());
}

// unix.getpid() → pid:int
static int LuaUnixGetpid(lua_State *L) {
  return LuaUnixGetid(L, getpid);
}

// unix.getppid() → pid:int
static int LuaUnixGetppid(lua_State *L) {
  return LuaUnixGetid(L, getppid);
}

// unix.getuid() → uid:int
static int LuaUnixGetuid(lua_State *L) {
  return LuaUnixGetid(L, getuid);
}

// unix.getgid() → gid:int
static int LuaUnixGetgid(lua_State *L) {
  return LuaUnixGetid(L, getgid);
}

// unix.geteuid() → uid:int
static int LuaUnixGeteuid(lua_State *L) {
  return LuaUnixGetid(L, geteuid);
}

// unix.getegid() → gid:int
static int LuaUnixGetegid(lua_State *L) {
  return LuaUnixGetid(L, getegid);
}

// unix.umask(mask:int) → oldmask:int
static int LuaUnixUmask(lua_State *L) {
  return ReturnInteger(L, umask(luaL_checkinteger(L, 1)));
}

// unix.exit([exitcode:int]) → ⊥
static wontreturn int LuaUnixExit(lua_State *L) {
  _Exit(luaL_optinteger(L, 1, 0));
}

// unix.access(path:str, how:int) → ok:bool, unix.Errno
// how can be: R_OK, W_OK, X_OK, F_OK
static int LuaUnixAccess(lua_State *L) {
  int olderr = errno;
  return SysretBool(L, access(luaL_checkstring(L, 1), luaL_checkinteger(L, 2)),
                    olderr);
}

// unix.mkdir(path:str[, mode:int]) → ok:bool, unix.Errno
// mode should be octal
static int LuaUnixMkdir(lua_State *L) {
  int olderr = errno;
  return SysretBool(
      L, mkdir(luaL_checkstring(L, 1), luaL_optinteger(L, 2, 0755)), olderr);
}

// unix.makedirs(path:str[, mode:int]) → ok:bool, unix.Errno
// mode should be octal
static int LuaUnixMakedirs(lua_State *L) {
  int olderr = errno;
  return SysretBool(
      L, makedirs(luaL_checkstring(L, 1), luaL_optinteger(L, 2, 0755)), olderr);
}

// unix.chdir(path:str) → ok:bool, unix.Errno
static int LuaUnixChdir(lua_State *L) {
  int olderr = errno;
  return SysretBool(L, chdir(luaL_checkstring(L, 1)), olderr);
}

// unix.unlink(path:str) → ok:bool, unix.Errno
static int LuaUnixUnlink(lua_State *L) {
  int olderr = errno;
  return SysretBool(L, unlink(luaL_checkstring(L, 1)), olderr);
}

// unix.rmdir(path:str) → ok:bool, unix.Errno
static int LuaUnixRmdir(lua_State *L) {
  int olderr = errno;
  return SysretBool(L, rmdir(luaL_checkstring(L, 1)), olderr);
}

// unix.rename(oldpath:str, newpath:str) → ok:bool, unix.Errno
static int LuaUnixRename(lua_State *L) {
  int olderr = errno;
  return SysretBool(L, rename(luaL_checkstring(L, 1), luaL_checkstring(L, 2)),
                    olderr);
}

// unix.link(existingpath:str, newpath:str) → ok:bool, unix.Errno
static int LuaUnixLink(lua_State *L) {
  int olderr = errno;
  return SysretBool(L, link(luaL_checkstring(L, 1), luaL_checkstring(L, 2)),
                    olderr);
}

// unix.symlink(target:str, linkpath:str) → ok:bool, unix.Errno
static int LuaUnixSymlink(lua_State *L) {
  int olderr = errno;
  return SysretBool(L, symlink(luaL_checkstring(L, 1), luaL_checkstring(L, 2)),
                    olderr);
}

// unix.chown(path:str, uid:int, gid:int) → ok:bool, unix.Errno
static int LuaUnixChown(lua_State *L) {
  int olderr = errno;
  return SysretBool(L,
                    chown(luaL_checkstring(L, 1), luaL_checkinteger(L, 2),
                          luaL_checkinteger(L, 3)),
                    olderr);
}

// unix.chmod(path:str, mode:int) → ok:bool, unix.Errno
static int LuaUnixChmod(lua_State *L) {
  int olderr = errno;
  return SysretBool(L, chmod(luaL_checkstring(L, 1), luaL_checkinteger(L, 2)),
                    olderr);
}

// unix.getcwd() → path:str, unix.Errno
static int LuaUnixGetcwd(lua_State *L) {
  int olderr;
  char *path;
  olderr = errno;
  if ((path = getcwd(0, 0))) {
    lua_pushstring(L, path);
    free(path);
    return 1;
  } else {
    return SysretErrnoNil(L, olderr);
  }
}

// unix.fork() → childpid|0:int, unix.Errno
static int LuaUnixFork(lua_State *L) {
  int olderr = errno;
  return SysretInteger(L, fork(), olderr);
}

// unix.environ() → {str,...}
static int LuaUnixEnviron(lua_State *L) {
  int i;
  char **e;
  lua_newtable(L);
  for (i = 0, e = environ; *e; ++e) {
    lua_pushstring(L, *e);
    lua_rawseti(L, -2, ++i);
  }
  return 1;
}

// unix.execve(prog:str[, args:List<*>, env:List<*>]) → false, unix.Errno
static int LuaUnixExecve(lua_State *L) {
  int olderr;
  const char *prog;
  char **argv, **envp, **freeme1, **freeme2, *ezargs[2];
  olderr = errno;
  prog = luaL_checkstring(L, 1);
  if (!lua_isnoneornil(L, 2)) {
    if ((argv = ConvertLuaArrayToStringList(L, 2))) {
      freeme1 = argv;
      if (!lua_isnoneornil(L, 3)) {
        if ((envp = ConvertLuaArrayToStringList(L, 3))) {
          freeme2 = envp;
        } else {
          FreeStringList(argv);
          return SysretErrnoBool(L, olderr);
        }
      } else {
        envp = environ;
        freeme2 = 0;
      }
    } else {
      return SysretErrnoBool(L, olderr);
    }
  } else {
    ezargs[0] = prog;
    ezargs[1] = 0;
    argv = ezargs;
    envp = environ;
    freeme1 = 0;
    freeme2 = 0;
  }
  execve(prog, argv, envp);
  FreeStringList(freeme1);
  FreeStringList(freeme2);
  return SysretErrnoBool(L, olderr);
}

// unix.commandv(prog:str) → path:str, unix.Errno
static int LuaUnixCommandv(lua_State *L) {
  int olderr;
  const char *prog;
  char *pathbuf, *resolved;
  olderr = errno;
  prog = luaL_checkstring(L, 1);
  if ((pathbuf = malloc(PATH_MAX + 1))) {
    if ((resolved = commandv(prog, pathbuf, PATH_MAX + 1))) {
      lua_pushstring(L, resolved);
      free(pathbuf);
      return 1;
    } else {
      free(pathbuf);
      return SysretErrnoNil(L, olderr);
    }
  } else {
    return SysretErrnoNil(L, olderr);
  }
}

// unix.realpath(path:str) → path:str, unix.Errno
static int LuaUnixRealpath(lua_State *L) {
  char *resolved;
  int olderr;
  const char *path;
  olderr = errno;
  path = luaL_checkstring(L, 1);
  if ((resolved = realpath(path, 0))) {
    lua_pushstring(L, resolved);
    free(resolved);
    return 1;
  } else {
    return SysretErrnoNil(L, olderr);
  }
}

// unix.syslog(priority:str, msg:str)
static int LuaUnixSyslog(lua_State *L) {
  syslog(luaL_checkinteger(L, 1), "%s", luaL_checkstring(L, 2));
  return 0;
}

// unix.chroot(path:str) → ok:bool, unix.Errno
static int LuaUnixChroot(lua_State *L) {
  int olderr = errno;
  return SysretBool(L, chroot(luaL_checkstring(L, 1)), olderr);
}

// unix.setrlimit(resource:int, soft:int[, hard:int]) → ok:bool, unix.Errno
static int LuaUnixSetrlimit(lua_State *L) {
  int olderr = errno;
  int64_t soft = luaL_checkinteger(L, 2);
  return SysretBool(
      L,
      setrlimit(luaL_checkinteger(L, 1),
                &(struct rlimit){soft, luaL_optinteger(L, 3, soft)}),
      olderr);
}

// unix.getrlimit(resource:int) → soft:int, unix.Errno, hard:int
static int LuaUnixGetrlimit(lua_State *L) {
  struct rlimit rlim;
  int rc, olderr, resource;
  olderr = errno;
  resource = luaL_checkinteger(L, 1);
  if (!getrlimit(resource, &rlim)) {
    lua_pushinteger(L, rlim.rlim_cur < RLIM_INFINITY ? rlim.rlim_cur : -1);
    lua_pushnil(L);
    lua_pushinteger(L, rlim.rlim_max < RLIM_INFINITY ? rlim.rlim_max : -1);
    return 3;
  } else {
    return SysretErrnoNil(L, olderr);
  }
}

// unix.kill(pid:int, sig:int) → ok:bool, unix.Errno
static int LuaUnixKill(lua_State *L) {
  int olderr = errno;
  return SysretBool(L, kill(luaL_checkinteger(L, 1), luaL_checkinteger(L, 2)),
                    olderr);
}

// unix.raise(sig:int) → rc:int, unix.Errno
static int LuaUnixRaise(lua_State *L) {
  int olderr = errno;
  return SysretInteger(L, raise(luaL_checkinteger(L, 1)), olderr);
}

// unix.wait([pid:int, options:int]) → pid:int, unix.Errno, wstatus:int
static int LuaUnixWait(lua_State *L) {
  int pid, wstatus, olderr = errno;
  if ((pid = wait4(luaL_optinteger(L, 1, -1), &wstatus,
                   luaL_optinteger(L, 2, 0), 0)) != -1) {
    lua_pushinteger(L, pid);
    lua_pushnil(L);
    lua_pushinteger(L, wstatus);
    return 3;
  } else {
    return SysretErrnoNil(L, olderr);
  }
}

// unix.fcntl(fd:int, cmd:int[, arg:int]) → rc:int, unix.Errno
static int LuaUnixFcntl(lua_State *L) {
  int olderr = errno;
  return SysretInteger(L,
                       fcntl(luaL_checkinteger(L, 1), luaL_checkinteger(L, 2),
                             luaL_optinteger(L, 3, 0)),
                       olderr);
}

// unix.dup(oldfd:int[, newfd:int[, flags:int]]) → newfd:int, unix.Errno
static int LuaUnixDup(lua_State *L) {
  int rc, oldfd, newfd, flags, olderr;
  olderr = errno;
  oldfd = luaL_checkinteger(L, 1);
  newfd = luaL_optinteger(L, 2, -1);
  flags = luaL_optinteger(L, 3, 0);
  if (newfd == -1) {
    rc = dup(oldfd);
  } else {
    rc = dup3(oldfd, newfd, flags);
  }
  return SysretInteger(L, rc, olderr);
}

// unix.pipe([flags:int]) → reader:int, unix.Errno, writer:int
static int LuaUnixPipe(lua_State *L) {
  int pipefd[2], olderr = errno;
  if (!pipe2(pipefd, luaL_optinteger(L, 1, 0))) {
    lua_pushinteger(L, pipefd[0]);
    lua_pushnil(L);
    lua_pushinteger(L, pipefd[1]);
    return 2;
  } else {
    return SysretErrnoNil(L, olderr);
  }
}

// unix.getsid(pid) → sid:int, unix.Errno
static int LuaUnixGetsid(lua_State *L) {
  int olderr = errno;
  return SysretInteger(L, getsid(luaL_checkinteger(L, 1)), olderr);
}

static dontinline int LuaUnixRc0(lua_State *L, int f(void)) {
  int olderr = errno;
  return SysretInteger(L, f(), olderr);
}

// unix.getpgrp() → pgid:int, unix.Errno
static int LuaUnixGetpgrp(lua_State *L) {
  return LuaUnixRc0(L, getpgrp);
}

// unix.setpgrp() → pgid:int, unix.Errno
static int LuaUnixSetpgrp(lua_State *L) {
  return LuaUnixRc0(L, setpgrp);
}

// unix.setsid() → sid:int, unix.Errno
static int LuaUnixSetsid(lua_State *L) {
  return LuaUnixRc0(L, setsid);
}

// unix.getpgid(pid:int) → pgid:int, unix.Errno
static int LuaUnixGetpgid(lua_State *L) {
  int olderr = errno;
  return SysretInteger(L, getpgid(luaL_checkinteger(L, 1)), olderr);
}

// unix.setpgid(pid:int, pgid:int) → pgid:int, unix.Errno
static int LuaUnixSetpgid(lua_State *L) {
  int olderr = errno;
  return SysretInteger(
      L, setpgid(luaL_checkinteger(L, 1), luaL_checkinteger(L, 2)), olderr);
}

static dontinline int LuaUnixSetid(lua_State *L, int f(int)) {
  int olderr = errno;
  return SysretBool(L, f(luaL_checkinteger(L, 1)), olderr);
}

// unix.setuid(uid:int) → ok:bool, unix.Errno
static int LuaUnixSetuid(lua_State *L) {
  return LuaUnixSetid(L, setuid);
}

// unix.setgid(gid:int) → ok:bool, unix.Errno
static int LuaUnixSetgid(lua_State *L) {
  return LuaUnixSetid(L, setgid);
}

static dontinline int LuaUnixSetresid(lua_State *L,
                                      int f(uint32_t, uint32_t, uint32_t)) {
  int olderr = errno;
  return SysretBool(L,
                    f(luaL_checkinteger(L, 1), luaL_checkinteger(L, 2),
                      luaL_checkinteger(L, 3)),
                    olderr);
}

// unix.setresuid(real:int, effective:int, saved:int) → ok:bool, unix.Errno
static int LuaUnixSetresuid(lua_State *L) {
  return LuaUnixSetresid(L, setresuid);
}

// unix.setresgid(real:int, effective:int, saved:int) → ok:bool, unix.Errno
static int LuaUnixSetresgid(lua_State *L) {
  return LuaUnixSetresid(L, setresgid);
}

// unix.clock_gettime([clock:int]) → seconds:int, unix.Errno, nanos:int
static int LuaUnixGettime(lua_State *L) {
  struct timespec ts;
  int rc, olderr = errno;
  if (!clock_gettime(luaL_optinteger(L, 1, CLOCK_REALTIME), &ts)) {
    lua_pushinteger(L, ts.tv_sec);
    lua_pushnil(L);
    lua_pushinteger(L, ts.tv_nsec);
    return 3;
  } else {
    return SysretErrnoNil(L, olderr);
  }
}

// unix.nanosleep(seconds:int, nanos:int)
//     → remseconds:int, unix.Errno, remnanos:int
static int LuaUnixNanosleep(lua_State *L) {
  int olderr = errno;
  struct timespec req, rem;
  req.tv_sec = luaL_checkinteger(L, 1);
  req.tv_nsec = luaL_optinteger(L, 2, 0);
  if (!nanosleep(&req, &rem)) {
    lua_pushinteger(L, rem.tv_sec);
    lua_pushnil(L);
    lua_pushinteger(L, rem.tv_nsec);
    return 3;
  } else {
    return SysretErrnoNil(L, olderr);
  }
}

// unix.sync()
static int LuaUnixSync(lua_State *L) {
  sync();
  return 0;
}

// unix.fsync(fd:int) → ok:bool, unix.Errno
static int LuaUnixFsync(lua_State *L) {
  int olderr = errno;
  return SysretBool(L, fsync(luaL_checkinteger(L, 1)), olderr);
}

// unix.fdatasync(fd:int) → ok:bool, unix.Errno
static int LuaUnixFdatasync(lua_State *L) {
  int olderr = errno;
  return SysretBool(L, fdatasync(luaL_checkinteger(L, 1)), olderr);
}

// unix.open(path:str, flags:int[, mode:int]) → fd:int, unix.Errno
static int LuaUnixOpen(lua_State *L) {
  int olderr = errno;
  return SysretInteger(L,
                       open(luaL_checkstring(L, 1), luaL_checkinteger(L, 2),
                            luaL_optinteger(L, 3, 0)),
                       olderr);
}

// unix.close(fd:int) → ok:bool, unix.Errno
static int LuaUnixClose(lua_State *L) {
  int olderr = errno;
  return SysretBool(L, close(luaL_checkinteger(L, 1)), olderr);
}

// unix.lseek(fd:int, offset:int[, whence:int]) → newpos:int, unix.Errno
static int LuaUnixLseek(lua_State *L) {
  int olderr = errno;
  return SysretInteger(L,
                       lseek(luaL_checkinteger(L, 1), luaL_checkinteger(L, 2),
                             luaL_optinteger(L, 3, SEEK_SET)),
                       olderr);
}

// unix.truncate(path:str[, length:int]) → ok:bool, unix.Errno
static int LuaUnixTruncate(lua_State *L) {
  int olderr = errno;
  return SysretBool(
      L, truncate(luaL_checkstring(L, 1), luaL_optinteger(L, 2, 0)), olderr);
}

// unix.ftruncate(fd:int[, length:int])  → ok:bool, unix.Errno
static int LuaUnixFtruncate(lua_State *L) {
  int olderr = errno;
  return SysretBool(
      L, ftruncate(luaL_checkinteger(L, 1), luaL_optinteger(L, 2, 0)), olderr);
}

// unix.read(fd:int[, bufsiz:str, offset:int]) → data:str, unix.Errno
static int LuaUnixRead(lua_State *L) {
  char *buf;
  size_t got;
  int fd, olderr;
  int64_t rc, bufsiz, offset;
  olderr = errno;
  fd = luaL_checkinteger(L, 1);
  bufsiz = luaL_optinteger(L, 2, BUFSIZ);
  offset = luaL_optinteger(L, 3, -1);
  bufsiz = MIN(bufsiz, 0x7ffff000);
  if ((buf = malloc(bufsiz))) {
    if (offset == -1) {
      rc = read(fd, buf, bufsiz);
    } else {
      rc = pread(fd, buf, bufsiz, offset);
    }
    if (rc != -1) {
      got = rc;
      lua_pushlstring(L, buf, got);
      free(buf);
      return 1;
    } else {
      free(buf);
      return SysretErrnoNil(L, olderr);
    }
  } else {
    return SysretErrnoNil(L, olderr);
  }
}

// unix.write(fd:int, data:str[, offset:int]) → rc:int, unix.Errno
static int LuaUnixWrite(lua_State *L) {
  size_t size;
  int fd, olderr;
  const char *data;
  int64_t rc, offset;
  olderr = errno;
  fd = luaL_checkinteger(L, 1);
  data = luaL_checklstring(L, 2, &size);
  offset = luaL_optinteger(L, 3, -1);
  size = MIN(size, 0x7ffff000);
  if (offset == -1) {
    rc = write(fd, data, size);
  } else {
    rc = pwrite(fd, data, size, offset);
  }
  return SysretInteger(L, rc, olderr);
}

static int ReturnStat(lua_State *L, struct UnixStat *ust) {
  struct UnixStat **ustp;
  ust->refs = 1;
  ustp = lua_newuserdatauv(L, sizeof(*ustp), 1);
  luaL_setmetatable(L, "unix.Stat");
  *ustp = ust;
  return 1;
}

// unix.stat(path:str) → unix.Stat, unix.Errno
static int LuaUnixStat(lua_State *L) {
  const char *path;
  int olderr = errno;
  struct UnixStat *ust;
  path = luaL_checkstring(L, 1);
  if ((ust = malloc(sizeof(*ust)))) {
    if (!stat(path, &ust->st)) {
      return ReturnStat(L, ust);
    }
    free(ust);
  }
  return SysretErrnoNil(L, olderr);
}

// unix.fstat(fd:int) → unix.Stat, unix.Errno
static int LuaUnixFstat(lua_State *L) {
  int fd, olderr = errno;
  struct UnixStat *ust;
  olderr = errno;
  fd = luaL_checkinteger(L, 1);
  if ((ust = malloc(sizeof(*ust)))) {
    if (!fstat(fd, &ust->st)) {
      return ReturnStat(L, ust);
    }
    free(ust);
  }
  return SysretErrnoNil(L, olderr);
}

static int ReturnDir(lua_State *L, struct UnixDir *udir) {
  struct UnixDir **udirp;
  udir->refs = 1;
  udirp = lua_newuserdatauv(L, sizeof(*udirp), 1);
  luaL_setmetatable(L, "unix.Dir");
  *udirp = udir;
  return 1;
}

// unix.opendir(path:str) → unix.Dir, unix.Errno
static int LuaUnixOpendir(lua_State *L) {
  int olderr = errno;
  const char *path;
  struct UnixDir *udir;
  path = luaL_checkstring(L, 1);
  if ((udir = calloc(1, sizeof(*udir)))) {
    if ((udir->dir = opendir(path))) {
      return ReturnDir(L, udir);
    }
    free(udir);
  }
  return SysretErrnoNil(L, olderr);
}

// unix.fdopendir(fd:int) → unix.Dir, unix.Errno
static int LuaUnixFdopendir(lua_State *L) {
  int fd, olderr = errno;
  struct UnixDir *udir;
  fd = luaL_checkinteger(L, 1);
  if ((udir = calloc(1, sizeof(*udir)))) {
    if ((udir->dir = fdopendir(fd))) {
      return ReturnDir(L, udir);
    }
    free(udir);
  }
  return SysretErrnoNil(L, olderr);
}

static bool IsSockoptBool(int l, int x) {
  if (l == SOL_SOCKET) {
    return x == SO_TYPE ||        //
           x == SO_DEBUG ||       //
           x == SO_ERROR ||       //
           x == SO_BROADCAST ||   //
           x == SO_REUSEADDR ||   //
           x == SO_REUSEPORT ||   //
           x == SO_KEEPALIVE ||   //
           x == SO_ACCEPTCONN ||  //
           x == SO_DONTROUTE;     //
  } else if (l = SOL_TCP) {
    return x == TCP_NODELAY ||           //
           x == TCP_CORK ||              //
           x == TCP_QUICKACK ||          //
           x == TCP_FASTOPEN_CONNECT ||  //
           x == TCP_DEFER_ACCEPT;        //
  } else if (l = SOL_IP) {
    return x == IP_HDRINCL;  //
  } else {
    return false;
  }
}

static bool IsSockoptInt(int l, int x) {
  if (l == SOL_SOCKET) {
    return x == SO_SNDBUF ||    //
           x == SO_RCVBUF ||    //
           x == SO_RCVLOWAT ||  //
           x == SO_SNDLOWAT;    //
  } else if (l = SOL_TCP) {
    return x == TCP_FASTOPEN ||       //
           x == TCP_KEEPCNT ||        //
           x == TCP_MAXSEG ||         //
           x == TCP_SYNCNT ||         //
           x == TCP_NOTSENT_LOWAT ||  //
           x == TCP_WINDOW_CLAMP ||   //
           x == TCP_KEEPIDLE ||       //
           x == TCP_KEEPINTVL;        //
  } else if (l = SOL_IP) {
    return x == IP_TOS ||  //
           x == IP_MTU ||  //
           x == IP_TTL;    //
  } else {
    return false;
  }
}

static bool IsSockoptTimeval(int l, int x) {
  if (l == SOL_SOCKET) {
    return x == SO_RCVTIMEO ||  //
           x == SO_SNDTIMEO;    //
  } else {
    return false;
  }
}

// unix.setsockopt(fd:int, level:int, optname:int, ...)
//     → ok:bool, unix.Errno
static int LuaUnixSetsockopt(lua_State *L) {
  struct linger l;
  struct timeval tv;
  int rc, fd, level, optname, optval, olderr = errno;
  fd = luaL_checkinteger(L, 1);
  level = luaL_checkinteger(L, 2);
  optname = luaL_checkinteger(L, 3);
  if (IsSockoptBool(level, optname)) {
    optval = lua_toboolean(L, 4);
    return SysretBool(
        L, setsockopt(fd, level, optname, &optval, sizeof(optval)), olderr);
  } else if (IsSockoptInt(level, optname)) {
    optval = luaL_checkinteger(L, 4);
    return SysretBool(
        L, setsockopt(fd, level, optname, &optval, sizeof(optval)), olderr);
  } else if (IsSockoptTimeval(level, optname)) {
    tv.tv_sec = luaL_checkinteger(L, 4);
    tv.tv_usec = luaL_optinteger(L, 5, 0);
    return SysretBool(L, setsockopt(fd, level, optname, &tv, sizeof(tv)),
                      olderr);
  } else if (level == SOL_SOCKET && optname == SO_LINGER) {
    l.l_onoff = lua_toboolean(L, 4);
    l.l_linger = luaL_checkinteger(L, 5);
    return SysretBool(L, setsockopt(fd, level, optname, &l, sizeof(l)), olderr);
  } else {
    einval();
    return SysretErrnoBool(L, olderr);
  }
}

static int LuaUnixGetsockopt(lua_State *L) {
  struct linger l;
  struct timeval tv;
  uint32_t lsize, tvsize, optvalsize;
  int rc, fd, level, optname, optval, olderr = errno;
  fd = luaL_checkinteger(L, 1);
  level = luaL_checkinteger(L, 2);
  optname = luaL_checkinteger(L, 3);
  if (IsSockoptBool(level, optname)) {
    // unix.getsockopt(fd:int, level:int, optname:int)
    //     → bool, unix.Errno
    optvalsize = sizeof(optval);
    if (getsockopt(fd, level, optname, &optval, &optvalsize) != -1) {
      CheckOptvalsize(L, sizeof(optval), optvalsize);
      lua_pushboolean(L, optval);
      return 1;
    } else {
      return SysretErrnoNil(L, olderr);
    }
  } else if (IsSockoptInt(level, optname)) {
    optvalsize = sizeof(optval);
    if (getsockopt(fd, level, optname, &optval, &optvalsize) != -1) {
      CheckOptvalsize(L, sizeof(optval), optvalsize);
      lua_pushinteger(L, optval);
      return 1;
    } else {
      return SysretErrnoNil(L, olderr);
    }
  } else if (IsSockoptTimeval(level, optname)) {
    tvsize = sizeof(tv);
    if (getsockopt(fd, level, optname, &tv, &tvsize) != -1) {
      CheckOptvalsize(L, sizeof(tv), tvsize);
      lua_pushinteger(L, tv.tv_sec);
      lua_pushnil(L);
      lua_pushinteger(L, tv.tv_usec);
      return 3;
    } else {
      return SysretErrnoNil(L, olderr);
    }
  } else if (level == SOL_SOCKET && optname == SO_LINGER) {
    lsize = sizeof(l);
    if (getsockopt(fd, level, optname, &l, &lsize) != -1) {
      CheckOptvalsize(L, sizeof(l), lsize);
      lua_pushinteger(L, l.l_onoff);
      lua_pushnil(L);
      lua_pushinteger(L, l.l_linger);
      return 3;
    } else {
      return SysretErrnoNil(L, olderr);
    }
  } else {
    einval();
    return SysretErrnoNil(L, olderr);
  }
}

// unix.socket([family:int[, type:int[, protocol:int]]]) → fd:int[,
// unix.Errno]
static int LuaUnixSocket(lua_State *L) {
  int olderr = errno;
  return SysretInteger(
      L,
      socket(luaL_optinteger(L, 1, AF_INET), luaL_optinteger(L, 2, SOCK_STREAM),
             luaL_optinteger(L, 3, IPPROTO_TCP)),
      olderr);
}

// unix.socketpair([family:int[, type:int[, protocol:int]]])
//     → fd1:int, unix.Errno, fd2:int
static int LuaUnixSocketpair(lua_State *L) {
  int sv[2], olderr = errno;
  if (!socketpair(luaL_optinteger(L, 1, AF_INET),
                  luaL_optinteger(L, 2, SOCK_STREAM),
                  luaL_optinteger(L, 3, IPPROTO_TCP), sv)) {
    lua_pushinteger(L, sv[0]);
    lua_pushnil(L);
    lua_pushinteger(L, sv[1]);
    return 2;
  } else {
    return SysretErrnoNil(L, olderr);
  }
}

// unix.bind(fd:int[, ip:uint32, port:uint16]) → ok:bool, unix.Errno
static int LuaUnixBind(lua_State *L) {
  int olderr = errno;
  return SysretBool(L,
                    bind(luaL_checkinteger(L, 1),
                         &(struct sockaddr_in){
                             .sin_family = AF_INET,
                             .sin_addr.s_addr = htonl(luaL_optinteger(L, 2, 0)),
                             .sin_port = htons(luaL_optinteger(L, 3, 0)),
                         },
                         sizeof(struct sockaddr_in)),
                    olderr);
}

// unix.connect(fd:int, ip:uint32, port:uint16) → ok:bool, unix.Errno
static int LuaUnixConnect(lua_State *L) {
  int olderr = errno;
  return SysretBool(
      L,
      connect(luaL_checkinteger(L, 1),
              &(struct sockaddr_in){
                  .sin_addr.s_addr = htonl(luaL_checkinteger(L, 2)),
                  .sin_port = htons(luaL_checkinteger(L, 3)),
              },
              sizeof(struct sockaddr_in)),
      olderr);
}

// unix.listen(fd:int[, backlog:int]) → ok:bool, unix.Errno
static int LuaUnixListen(lua_State *L) {
  int olderr = errno;
  return SysretBool(
      L, listen(luaL_checkinteger(L, 1), luaL_optinteger(L, 2, 10)), olderr);
}

// unix.getsockname(fd:int) → ip:uint32, unix.Errno, port:uint16
static int LuaUnixGetsockname(lua_State *L) {
  int fd, olderr;
  uint32_t addrsize;
  struct sockaddr_in sa;
  olderr = errno;
  addrsize = sizeof(sa);
  fd = luaL_checkinteger(L, 1);
  if (!getsockname(fd, &sa, &addrsize)) {
    lua_pushinteger(L, ntohl(sa.sin_addr.s_addr));
    lua_pushnil(L);
    lua_pushinteger(L, ntohs(sa.sin_port));
    return 3;
  } else {
    return SysretErrnoNil(L, olderr);
  }
}

// unix.getpeername(fd:int) → ip:uint32, unix.Errno, port:uint16
static int LuaUnixGetpeername(lua_State *L) {
  int fd, olderr;
  uint32_t addrsize;
  struct sockaddr_in sa;
  olderr = errno;
  addrsize = sizeof(sa);
  fd = luaL_checkinteger(L, 1);
  if (!getpeername(fd, &sa, &addrsize)) {
    lua_pushinteger(L, ntohl(sa.sin_addr.s_addr));
    lua_pushnil(L);
    lua_pushinteger(L, ntohs(sa.sin_port));
    return 3;
  } else {
    return SysretErrnoNil(L, olderr);
  }
}

// unix.siocgifconf()
//     → {{name:str,ip:uint32,netmask:uint32}, ...}, unix.Errno
static int LuaUnixSiocgifconf(lua_State *L) {
  size_t n;
  char *data;
  int i, fd, olderr;
  struct ifreq *ifr;
  struct ifconf conf;
  olderr = errno;
  if (!(data = malloc((n = 4096)))) {
    return SysretErrnoNil(L, olderr);
  }
  if ((fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)) == -1) {
    free(data);
    return SysretErrnoNil(L, olderr);
  }
  conf.ifc_buf = data;
  conf.ifc_len = n;
  if (ioctl(fd, SIOCGIFCONF, &conf) == -1) {
    close(fd);
    free(data);
    return SysretErrnoNil(L, olderr);
  }
  lua_newtable(L);
  i = 0;
  for (ifr = (struct ifreq *)data; (char *)ifr < data + conf.ifc_len; ++ifr) {
    if (ifr->ifr_addr.sa_family != AF_INET) continue;
    lua_createtable(L, 0, 3);
    lua_pushliteral(L, "name");
    lua_pushstring(L, ifr->ifr_name);
    lua_settable(L, -3);
    lua_pushliteral(L, "ip");
    lua_pushinteger(
        L, ntohl(((struct sockaddr_in *)&ifr->ifr_addr)->sin_addr.s_addr));
    lua_settable(L, -3);
    if (ioctl(fd, SIOCGIFNETMASK, ifr) != -1) {
      lua_pushliteral(L, "netmask");
      lua_pushinteger(
          L, ntohl(((struct sockaddr_in *)&ifr->ifr_addr)->sin_addr.s_addr));
      lua_settable(L, -3);
    }
    lua_rawseti(L, -2, ++i);
  }
  close(fd);
  free(data);
  return 1;
}

// unix.gethostname() → host:str, unix.Errno
static int LuaUnixGethostname(lua_State *L) {
  int rc, olderr;
  char buf[DNS_NAME_MAX + 1];
  olderr = errno;
  if ((rc = gethostname(buf, sizeof(buf))) != -1) {
    if (strnlen(buf, sizeof(buf)) < sizeof(buf)) {
      lua_pushstring(L, buf);
      return 1;
    } else {
      enomem();
      return SysretErrnoNil(L, olderr);
    }
  } else {
    return SysretErrnoNil(L, olderr);
  }
}

// unix.accept(serverfd:int[, flags:int])
//     → clientfd:int, unix.Errno, ip:uint32, port:uint16
static int LuaUnixAccept(lua_State *L) {
  uint32_t addrsize;
  struct sockaddr_in sa;
  int clientfd, serverfd, olderr, flags;
  olderr = errno;
  addrsize = sizeof(sa);
  serverfd = luaL_checkinteger(L, 1);
  flags = luaL_optinteger(L, 2, 0);
  clientfd = accept4(serverfd, &sa, &addrsize, flags);
  if (clientfd != -1) {
    lua_pushinteger(L, clientfd);
    lua_pushnil(L);
    lua_pushinteger(L, ntohl(sa.sin_addr.s_addr));
    lua_pushinteger(L, ntohs(sa.sin_port));
    return 4;
  } else {
    return SysretErrnoNil(L, olderr);
  }
}

// unix.poll({fd:int=events:int, ...}[, timeoutms:int])
//     → {fd:int=revents:int, ...}, unix.Errno
static int LuaUnixPoll(lua_State *L) {
  size_t nfds;
  struct pollfd *fds;
  int i, fd, olderr, events, timeoutms;
  luaL_checktype(L, 1, LUA_TTABLE);
  lua_pushnil(L);
  for (fds = 0, nfds = 0; lua_next(L, 1);) {
    if (lua_isinteger(L, -2)) {
      fds = xrealloc(fds, ++nfds * sizeof(*fds));
      fds[nfds - 1].fd = lua_tointeger(L, -2);
      fds[nfds - 1].events = lua_tointeger(L, -1);
    }
    lua_pop(L, 1);
  }
  timeoutms = luaL_optinteger(L, 2, -1);
  olderr = errno;
  if ((events = poll(fds, nfds, timeoutms)) != -1) {
    lua_createtable(L, events, 0);
    for (i = 0; i < nfds; ++i) {
      if (fds[i].revents && fds[i].fd >= 0) {
        lua_pushinteger(L, fds[i].revents);
        lua_rawseti(L, -2, fds[i].fd);
      }
    }
    free(fds);
    return 1;
  } else {
    free(fds);
    return SysretErrnoNil(L, olderr);
  }
}

// unix.recvfrom(fd:int[, bufsiz:str[, flags:int]])
//     → data:str, unix.Errno, ip:uint32, port:uint16
static int LuaUnixRecvfrom(lua_State *L) {
  char *buf;
  size_t got;
  ssize_t rc;
  uint32_t addrsize;
  struct sockaddr_in sa;
  int fd, flags, bufsiz, olderr;
  olderr = errno;
  addrsize = sizeof(sa);
  fd = luaL_checkinteger(L, 1);
  bufsiz = luaL_optinteger(L, 2, 1500);
  flags = luaL_optinteger(L, 3, 0);
  bufsiz = MIN(bufsiz, 0x7ffff000);
  if ((buf = malloc(bufsiz))) {
    rc = recvfrom(fd, buf, bufsiz, flags, &sa, &addrsize);
    if (rc != -1) {
      got = rc;
      lua_pushlstring(L, buf, got);
      lua_pushnil(L);
      lua_pushinteger(L, ntohl(sa.sin_addr.s_addr));
      lua_pushinteger(L, ntohs(sa.sin_port));
      free(buf);
      return 4;
    } else {
      free(buf);
      return SysretErrnoNil(L, olderr);
    }
  } else {
    return SysretErrnoNil(L, olderr);
  }
}

// unix.recv(fd:int[, bufsiz:int[, flags:int]]) → data:str, unix.Errno
static int LuaUnixRecv(lua_State *L) {
  char *buf;
  size_t got;
  ssize_t rc;
  int fd, flags, bufsiz, olderr, pushed;
  olderr = errno;
  fd = luaL_checkinteger(L, 1);
  bufsiz = luaL_optinteger(L, 2, 1500);
  flags = luaL_optinteger(L, 3, 0);
  bufsiz = MIN(bufsiz, 0x7ffff000);
  if ((buf = malloc(bufsiz))) {
    rc = recv(fd, buf, bufsiz, flags);
    if (rc != -1) {
      got = rc;
      lua_pushlstring(L, buf, got);
      free(buf);
      return 1;
    } else {
      free(buf);
      return SysretErrnoNil(L, olderr);
    }
  } else {
    return SysretErrnoNil(L, olderr);
  }
}

// unix.send(fd:int, data:str[, flags:int]) → sent:int, unix.Errno
static int LuaUnixSend(lua_State *L) {
  char *data;
  ssize_t rc;
  size_t sent, size;
  int fd, flags, bufsiz, olderr;
  olderr = errno;
  fd = luaL_checkinteger(L, 1);
  data = luaL_checklstring(L, 2, &size);
  size = MIN(size, 0x7ffff000);
  flags = luaL_optinteger(L, 5, 0);
  return SysretInteger(L, send(fd, data, size, flags), olderr);
}

// unix.sendto(fd:int, data:str, ip:uint32, port:uint16[, flags:int])
//     → sent:int, unix.Errno
static int LuaUnixSendto(lua_State *L) {
  char *data;
  size_t sent, size;
  struct sockaddr_in sa;
  int fd, flags, bufsiz, olderr;
  olderr = errno;
  bzero(&sa, sizeof(sa));
  fd = luaL_checkinteger(L, 1);
  data = luaL_checklstring(L, 2, &size);
  size = MIN(size, 0x7ffff000);
  sa.sin_addr.s_addr = htonl(luaL_checkinteger(L, 3));
  sa.sin_port = htons(luaL_checkinteger(L, 4));
  flags = luaL_optinteger(L, 5, 0);
  return SysretInteger(L, sendto(fd, data, size, flags, &sa, sizeof(sa)),
                       olderr);
}

// unix.shutdown(fd:int, how:int) → ok:bool, unix.Errno
static int LuaUnixShutdown(lua_State *L) {
  int olderr = errno;
  return SysretBool(
      L, shutdown(luaL_checkinteger(L, 1), luaL_checkinteger(L, 2)), olderr);
}

// unix.sigprocmask(how:int[, mask:int]) → oldmask:int, unix.Errno
static int LuaUnixSigprocmask(lua_State *L) {
  uint64_t imask;
  int how, olderr;
  sigset_t mask, oldmask, *maskptr;
  olderr = errno;
  how = luaL_checkinteger(L, 1);
  if (lua_isnoneornil(L, 2)) {
    // if mask isn't passed then we're querying
    maskptr = 0;
  } else {
    maskptr = &mask;
    imask = luaL_checkinteger(L, 2);
    bzero(&mask, sizeof(mask));
    if (how == SIG_SETMASK) {
      sigprocmask(how, 0, &mask);
    }
    mask.__bits[0] = imask;
  }
  if (!sigprocmask(how, maskptr, &oldmask)) {
    lua_pushinteger(L, oldmask.__bits[0]);
    return 1;
  } else {
    return SysretErrnoNil(L, olderr);
  }
}

static void LuaUnixOnSignal(int sig, siginfo_t *si, ucontext_t *ctx) {
  STRACE("LuaUnixOnSignal(%G)", sig);
  lua_getglobal(GL, "__signal_handlers");
  CHECK_EQ(LUA_TFUNCTION, lua_geti(GL, -1, sig));
  lua_remove(GL, -2);
  lua_pushinteger(GL, sig);
  if (lua_pcall(GL, 1, 0, 0) != LUA_OK) {
    ERRORF("(lua) %s failed: %s", strsignal(sig), lua_tostring(GL, -1));
    lua_pop(GL, 1);  // pop error
  }
}

// unix.sigaction(sig:int[, handler:func|int[, flags:int[, mask:int]]])
//     → oldhandler:func|int, unix.Errno, flags:int, mask:int
//
//     unix = require "unix"
//     unix.sigaction(unix.SIGUSR1, function(sig)
//        print(string.format("got %s", unix.strsignal(sig)))
//     end)
//     unix.sigprocmask(unix.SIG_SETMASK, -1)
//     unix.raise(unix.SIGUSR1)
//     unix.sigsuspend()
//
// handler can be SIG_IGN, SIG_DFL, intptr_t, or a Lua function
// sig can be SIGINT, SIGQUIT, SIGTERM, SIGUSR1, etc.
static int LuaUnixSigaction(lua_State *L) {
  int i, n, sig, olderr;
  struct sigaction sa, oldsa, *saptr;
  saptr = &sa;
  olderr = errno;
  sigemptyset(&sa.sa_mask);
  sig = luaL_checkinteger(L, 1);
  if (!(1 <= sig && sig <= NSIG)) {
    luaL_argerror(L, 1, "signal number invalid");
    unreachable;
  }
  if (lua_isnoneornil(L, 2)) {
    // if handler/flags/mask aren't passed,
    // then we're quering the current state
    saptr = 0;
  } else if (lua_isinteger(L, 2)) {
    // bypass handling signals using lua code if possible
    sa.sa_sigaction = (void *)luaL_checkinteger(L, 2);
  } else if (lua_isfunction(L, 2)) {
    sa.sa_sigaction = LuaUnixOnSignal;
    // lua probably isn't async signal safe, so...
    // let's mask all the lua handlers during handling
    sigaddset(&sa.sa_mask, sig);
    lua_getglobal(L, "__signal_handlers");
    luaL_checktype(L, -1, LUA_TTABLE);
    lua_len(L, -1);
    n = lua_tointeger(L, -1);
    lua_pop(L, 1);
    for (i = 1; i <= MIN(n, NSIG); ++i) {
      if (lua_geti(L, -1, i) == LUA_TFUNCTION) {
        sigaddset(&sa.sa_mask, i);
      }
      lua_pop(L, 1);
    }
    lua_pop(L, 1);
  } else {
    luaL_argerror(L, 2, "sigaction handler not integer or function");
    unreachable;
  }
  sa.sa_flags = luaL_optinteger(L, 3, SA_RESTART);
  sa.sa_mask.__bits[0] |= luaL_optinteger(L, 4, 0);
  if (!sigaction(sig, saptr, &oldsa)) {
    lua_getglobal(L, "__signal_handlers");
    // push the old handler result to stack. if the global lua handler
    // table has a real function, then we prefer to return that. if it's
    // absent or a raw integer value, then we're better off returning
    // what the kernel gave us in &oldsa.
    if (lua_geti(L, -1, sig) != LUA_TFUNCTION) {
      lua_pop(L, 1);
      lua_pushinteger(L, (intptr_t)oldsa.sa_handler);
    }
    if (saptr) {
      // update the global lua table
      if (sa.sa_sigaction == LuaUnixOnSignal) {
        lua_pushvalue(L, -3);
      } else {
        lua_pushnil(L);
      }
      lua_seti(L, -3, sig);
    }
    // remove the signal handler table from stack
    lua_remove(L, -2);
    // finish pushing the last 3/4 results
    lua_pushnil(L);
    lua_pushinteger(L, oldsa.sa_flags);
    lua_pushinteger(L, oldsa.sa_mask.__bits[0]);
    return 4;
  } else {
    return SysretErrnoNil(L, olderr);
  }
}

// unix.sigsuspend([mask]) → false, unix.Errno
static int LuaUnixSigsuspend(lua_State *L) {
  int olderr = errno;
  sigsuspend(&(struct sigset){{luaL_optinteger(L, 1, 0)}});
  return SysretErrnoBool(L, olderr);
}

// unix.setitimer(which[, intsec, intmicros, valsec, valmicros])
//   → intsec:int, unix.Errno, intns:int, valsec:int, valns:int
static int LuaUnixSetitimer(lua_State *L) {
  int which, olderr;
  struct itimerval it, oldit, *itptr;
  olderr = errno;
  which = luaL_checkinteger(L, 1);
  if (!lua_isnoneornil(L, 2)) {
    itptr = &it;
    it.it_interval.tv_sec = luaL_optinteger(L, 2, 0);
    it.it_interval.tv_usec = luaL_optinteger(L, 3, 0);
    it.it_value.tv_sec = luaL_optinteger(L, 4, 0);
    it.it_value.tv_usec = luaL_optinteger(L, 5, 0);
  } else {
    itptr = 0;
  }
  if (!setitimer(which, itptr, &oldit)) {
    lua_pushinteger(L, oldit.it_interval.tv_sec);
    lua_pushnil(L);
    lua_pushinteger(L, oldit.it_interval.tv_usec);
    lua_pushinteger(L, oldit.it_value.tv_sec);
    lua_pushinteger(L, oldit.it_value.tv_usec);
    return 5;
  } else {
    return SysretErrnoNil(L, olderr);
  }
}

static dontinline int LuaUnixStr(lua_State *L, char *f(int)) {
  return ReturnString(L, f(luaL_checkinteger(L, 1)));
}

// unix.strerdoc(errno) → str
static int LuaUnixStrerdoc(lua_State *L) {
  return LuaUnixStr(L, strerdoc);
}

// unix.strsignal(sig) → str
static int LuaUnixStrsignal(lua_State *L) {
  return LuaUnixStr(L, strsignal);
}

// unix.strerror(errno) → str
static int LuaUnixStrerror(lua_State *L) {
  return LuaUnixStr(L, strerror);
}

// unix.strerrno(errno) → str|nil
static int LuaUnixStrerrno(lua_State *L) {
  return LuaUnixStr(L, strerrno);
}

// unix.WIFEXITED(wstatus) → int
static int LuaUnixWifexited(lua_State *L) {
  return ReturnInteger(L, WIFEXITED(luaL_checkinteger(L, 1)));
}

// unix.WEXITSTATUS(wstatus) → int
static int LuaUnixWexitstatus(lua_State *L) {
  return ReturnInteger(L, WEXITSTATUS(luaL_checkinteger(L, 1)));
}

// unix.WIFSIGNALED(wstatus) → int
static int LuaUnixWifsignaled(lua_State *L) {
  return ReturnInteger(L, WIFSIGNALED(luaL_checkinteger(L, 1)));
}

// unix.WTERMSIG(wstatus) → int
static int LuaUnixWtermsig(lua_State *L) {
  return ReturnInteger(L, WTERMSIG(luaL_checkinteger(L, 1)));
}

////////////////////////////////////////////////////////////////////////////////
// unix.Stat object

static dontinline struct stat *GetUnixStat(lua_State *L) {
  struct UnixStat **ust;
  ust = luaL_checkudata(L, 1, "unix.Stat");
  return &(*ust)->st;
}

static int LuaUnixStatSize(lua_State *L) {
  return ReturnInteger(L, GetUnixStat(L)->st_size);
}

static int LuaUnixStatMode(lua_State *L) {
  return ReturnInteger(L, GetUnixStat(L)->st_mode);
}

static int LuaUnixStatDev(lua_State *L) {
  return ReturnInteger(L, GetUnixStat(L)->st_dev);
}

static int LuaUnixStatIno(lua_State *L) {
  return ReturnInteger(L, GetUnixStat(L)->st_ino);
}

static int LuaUnixStatNlink(lua_State *L) {
  return ReturnInteger(L, GetUnixStat(L)->st_nlink);
}

static int LuaUnixStatRdev(lua_State *L) {
  return ReturnInteger(L, GetUnixStat(L)->st_rdev);
}

static int LuaUnixStatUid(lua_State *L) {
  return ReturnInteger(L, GetUnixStat(L)->st_uid);
}

static int LuaUnixStatGid(lua_State *L) {
  return ReturnInteger(L, GetUnixStat(L)->st_gid);
}

static int LuaUnixStatBlocks(lua_State *L) {
  return ReturnInteger(L, GetUnixStat(L)->st_blocks);
}

static int LuaUnixStatBlksize(lua_State *L) {
  return ReturnInteger(L, GetUnixStat(L)->st_blksize);
}

static dontinline int UnixStatTim(lua_State *L, struct timespec *ts) {
  lua_pushinteger(L, ts->tv_sec);
  lua_pushinteger(L, ts->tv_nsec);
  return 2;
}

static int LuaUnixStatAtim(lua_State *L) {
  return UnixStatTim(L, &GetUnixStat(L)->st_atim);
}

static int LuaUnixStatMtim(lua_State *L) {
  return UnixStatTim(L, &GetUnixStat(L)->st_mtim);
}

static int LuaUnixStatCtim(lua_State *L) {
  return UnixStatTim(L, &GetUnixStat(L)->st_ctim);
}

static int LuaUnixStatBirthtim(lua_State *L) {
  return UnixStatTim(L, &GetUnixStat(L)->st_birthtim);
}

static void FreeUnixStat(struct UnixStat *stat) {
  if (!--stat->refs) {
    free(stat);
  }
}

static int LuaUnixStatToString(lua_State *L) {
  struct stat *st = GetUnixStat(L);
  lua_pushstring(L, "unix.Stat{}");
  return 1;
}

static int LuaUnixStatGc(lua_State *L) {
  struct UnixStat **ust;
  ust = luaL_checkudata(L, 1, "unix.Stat");
  if (*ust) {
    FreeUnixStat(*ust);
    *ust = 0;
  }
  return 0;
}

static const luaL_Reg kLuaUnixStatMeth[] = {
    {"atim", LuaUnixStatAtim},          //
    {"birthtim", LuaUnixStatBirthtim},  //
    {"blksize", LuaUnixStatBlksize},    //
    {"blocks", LuaUnixStatBlocks},      //
    {"ctim", LuaUnixStatCtim},          //
    {"dev", LuaUnixStatDev},            //
    {"gid", LuaUnixStatGid},            //
    {"ino", LuaUnixStatIno},            //
    {"mode", LuaUnixStatMode},          //
    {"mtim", LuaUnixStatMtim},          //
    {"nlink", LuaUnixStatNlink},        //
    {"rdev", LuaUnixStatRdev},          //
    {"size", LuaUnixStatSize},          //
    {"uid", LuaUnixStatUid},            //
    {0},                                //
};

static const luaL_Reg kLuaUnixStatMeta[] = {
    {"__tostring", LuaUnixStatToString},  //
    {"__gc", LuaUnixStatGc},              //
    {0},                                  //
};

static void LuaUnixStatObj(lua_State *L) {
  luaL_newmetatable(L, "unix.Stat");
  luaL_setfuncs(L, kLuaUnixStatMeta, 0);
  luaL_newlibtable(L, kLuaUnixStatMeth);
  luaL_setfuncs(L, kLuaUnixStatMeth, 0);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);
}

////////////////////////////////////////////////////////////////////////////////
// unix.Errno object

static dontinline struct UnixErrno *GetUnixErrno(lua_State *L) {
  struct UnixErrno **ue;
  ue = luaL_checkudata(L, 1, "unix.Errno");
  return *ue;
}

static int LuaUnixErrnoName(lua_State *L) {
  struct UnixErrno *e = GetUnixErrno(L);
  lua_pushstring(L, strerrno(e->errno));
  return 1;
}

static int LuaUnixErrnoDoc(lua_State *L) {
  struct UnixErrno *e = GetUnixErrno(L);
  lua_pushstring(L, strerdoc(e->errno));
  return 1;
}

static int LuaUnixErrnoError(lua_State *L) {
  struct UnixErrno *e = GetUnixErrno(L);
  lua_pushstring(L, strerror(e->errno));
  return 1;
}

static int LuaUnixErrnoToString(lua_State *L) {
  struct UnixErrno *e = GetUnixErrno(L);
  lua_pushfstring(L, "error: system call failed: %s", strerror(e->errno));
  return 1;
}

static void FreeUnixErrno(struct UnixErrno *errno) {
  if (!--errno->refs) {
    free(errno);
  }
}

static int LuaUnixErrnoGc(lua_State *L) {
  struct UnixErrno **ue;
  ue = luaL_checkudata(L, 1, "unix.Errno");
  if (*ue) {
    FreeUnixErrno(*ue);
    *ue = 0;
  }
  return 0;
}

static const luaL_Reg kLuaUnixErrnoMeth[] = {
    {"error", LuaUnixErrnoError},  //
    {"name", LuaUnixErrnoName},    //
    {"doc", LuaUnixErrnoDoc},      //
    {0},                           //
};

static const luaL_Reg kLuaUnixErrnoMeta[] = {
    {"__tostring", LuaUnixErrnoToString},  //
    {"__gc", LuaUnixErrnoGc},              //
    {0},                                   //
};

static void LuaUnixErrnoObj(lua_State *L) {
  luaL_newmetatable(L, "unix.Errno");
  luaL_setfuncs(L, kLuaUnixErrnoMeta, 0);
  luaL_newlibtable(L, kLuaUnixErrnoMeth);
  luaL_setfuncs(L, kLuaUnixErrnoMeth, 0);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);
}

////////////////////////////////////////////////////////////////////////////////
// unix.Dir object

static struct UnixDir **GetUnixDirSelf(lua_State *L) {
  return luaL_checkudata(L, 1, "unix.Dir");
}

static DIR *GetDirOrDie(lua_State *L) {
  struct UnixDir **udir;
  udir = GetUnixDirSelf(L);
  assert((*udir)->dir);
  if (*udir) return (*udir)->dir;
  luaL_argerror(L, 1, "unix.UnixDir is closed");
  unreachable;
}

static int FreeUnixDir(struct UnixDir *dir) {
  if (--dir->refs) return 0;
  return closedir(dir->dir);
}

// unix.Dir:close() → ok:bool, unix.Errno
// may be called multiple times
// called by the garbage collector too
static int LuaUnixDirClose(lua_State *L) {
  int rc, olderr;
  struct UnixDir **udir;
  udir = GetUnixDirSelf(L);
  if (!*udir) return 0;
  olderr = 0;
  rc = FreeUnixDir(*udir);
  *udir = 0;
  return SysretBool(L, rc, olderr);
}

// unix.Dir:read() → name:str, unix.Errno, kind:int, ino:int, off:int
static int LuaUnixDirRead(lua_State *L) {
  int olderr = errno;
  struct dirent *ent;
  errno = 0;
  if ((ent = readdir(GetDirOrDie(L)))) {
    lua_pushlstring(L, ent->d_name, strnlen(ent->d_name, sizeof(ent->d_name)));
    lua_pushnil(L);
    lua_pushinteger(L, ent->d_type);
    lua_pushinteger(L, ent->d_ino);
    lua_pushinteger(L, ent->d_off);
    return 5;
  } else if (!ent && !errno) {
    return 0;  // end of listing
  } else {
    return SysretErrnoNil(L, olderr);
  }
}

// unix.Dir:fd() → fd:int, unix.Errno
static int LuaUnixDirFd(lua_State *L) {
  int fd, olderr;
  olderr = errno;
  fd = dirfd(GetDirOrDie(L));
  if (fd != -1) {
    lua_pushinteger(L, fd);
    return 1;
  } else {
    return SysretErrnoNil(L, olderr);
  }
}

// unix.Dir:tell() → off:int
static int LuaUnixDirTell(lua_State *L) {
  long off;
  off = telldir(GetDirOrDie(L));
  lua_pushinteger(L, off);
  return 1;
}

// unix.Dir:rewind()
static int LuaUnixDirRewind(lua_State *L) {
  rewinddir(GetDirOrDie(L));
  return 0;
}

static const luaL_Reg kLuaUnixDirMeth[] = {
    {"close", LuaUnixDirClose},    //
    {"read", LuaUnixDirRead},      //
    {"fd", LuaUnixDirFd},          //
    {"tell", LuaUnixDirTell},      //
    {"rewind", LuaUnixDirRewind},  //
    {0},                           //
};

static const luaL_Reg kLuaUnixDirMeta[] = {
    {"__gc", LuaUnixDirClose},  //
    {0},                        //
};

static void LuaUnixDirObj(lua_State *L) {
  luaL_newmetatable(L, "unix.Dir");
  luaL_setfuncs(L, kLuaUnixDirMeta, 0);
  luaL_newlibtable(L, kLuaUnixDirMeth);
  luaL_setfuncs(L, kLuaUnixDirMeth, 0);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);
}

////////////////////////////////////////////////////////////////////////////////
// UNIX module

static const luaL_Reg kLuaUnix[] = {
    {"exit", LuaUnixExit},                // exit w/o atexit
    {"stat", LuaUnixStat},                // get file info from path
    {"fstat", LuaUnixFstat},              // get file info from fd
    {"open", LuaUnixOpen},                // open file fd at lowest slot
    {"close", LuaUnixClose},              // close file or socket
    {"lseek", LuaUnixLseek},              // seek in file
    {"read", LuaUnixRead},                // read from file or socket
    {"write", LuaUnixWrite},              // write to file or socket
    {"access", LuaUnixAccess},            // check my file authorization
    {"fcntl", LuaUnixFcntl},              // manipulate file descriptor
    {"chdir", LuaUnixChdir},              // change directory
    {"chown", LuaUnixChown},              // change owner of file
    {"chmod", LuaUnixChmod},              // change mode of file
    {"getcwd", LuaUnixGetcwd},            // get current directory
    {"fork", LuaUnixFork},                // make child process via mitosis
    {"execve", LuaUnixExecve},            // replace process with program
    {"environ", LuaUnixEnviron},          // get environment variables
    {"commandv", LuaUnixCommandv},        // resolve program on $PATH
    {"realpath", LuaUnixRealpath},        // abspath without dots/symlinks
    {"syslog", LuaUnixSyslog},            // logs to system log
    {"kill", LuaUnixKill},                // signal child process
    {"raise", LuaUnixRaise},              // signal this process
    {"wait", LuaUnixWait},                // wait for child to change status
    {"pipe", LuaUnixPipe},                // create two anon fifo fds
    {"dup", LuaUnixDup},                  // copy fd to lowest empty slot
    {"mkdir", LuaUnixMkdir},              // make directory
    {"makedirs", LuaUnixMakedirs},        // make directory and parents too
    {"rmdir", LuaUnixRmdir},              // remove empty directory
    {"opendir", LuaUnixOpendir},          // read directory entry list
    {"fdopendir", LuaUnixFdopendir},      // read directory entry list
    {"rename", LuaUnixRename},            // rename file or directory
    {"link", LuaUnixLink},                // create hard link
    {"unlink", LuaUnixUnlink},            // remove file
    {"symlink", LuaUnixSymlink},          // create symbolic link
    {"sync", LuaUnixSync},                // flushes files and disks
    {"fsync", LuaUnixFsync},              // flush open file
    {"fdatasync", LuaUnixFdatasync},      // flush open file w/o metadata
    {"truncate", LuaUnixTruncate},        // shrink or extend file medium
    {"ftruncate", LuaUnixFtruncate},      // shrink or extend file medium
    {"umask", LuaUnixUmask},              // set default file mask
    {"chroot", LuaUnixChroot},            // change root directory
    {"setrlimit", LuaUnixSetrlimit},      // prevent cpu memory bombs
    {"getrlimit", LuaUnixGetrlimit},      // query resource limits
    {"getppid", LuaUnixGetppid},          // get parent process id
    {"getpgrp", LuaUnixGetpgrp},          // get process group id
    {"getpgid", LuaUnixGetpgid},          // get process group id of pid
    {"setpgid", LuaUnixSetpgid},          // set process group id for pid
    {"setpgrp", LuaUnixSetpgrp},          // sets process group id
    {"getsid", LuaUnixGetsid},            // get session id of pid
    {"setsid", LuaUnixSetsid},            // create a new session id
    {"getpid", LuaUnixGetpid},            // get id of this process
    {"getuid", LuaUnixGetuid},            // get real user id of process
    {"geteuid", LuaUnixGeteuid},          // get effective user id of process
    {"setuid", LuaUnixSetuid},            // set real user id of process
    {"setresuid", LuaUnixSetresuid},      // sets real/effective/saved uids
    {"getgid", LuaUnixGetgid},            // get real group id of process
    {"getegid", LuaUnixGetegid},          // get effective group id of process
    {"setgid", LuaUnixSetgid},            // set real group id of process
    {"setresgid", LuaUnixSetresgid},      // sets real/effective/saved gids
    {"gethostname", LuaUnixGethostname},  // get hostname of this machine
    {"clock_gettime", LuaUnixGettime},    // get timestamp w/ nano precision
    {"nanosleep", LuaUnixNanosleep},      // sleep w/ nano precision
    {"socket", LuaUnixSocket},            // create network communication fd
    {"socketpair", LuaUnixSocketpair},    // create bidirectional pipe
    {"setsockopt", LuaUnixSetsockopt},    // tune socket options
    {"getsockopt", LuaUnixGetsockopt},    // get socket tunings
    {"poll", LuaUnixPoll},                // waits for file descriptor events
    {"bind", LuaUnixBind},                // reserve network interface address
    {"listen", LuaUnixListen},            // begin listening for clients
    {"accept", LuaUnixAccept},            // create client fd for client
    {"connect", LuaUnixConnect},          // connect to remote address
    {"recv", LuaUnixRecv},                // receive tcp from some address
    {"recvfrom", LuaUnixRecvfrom},        // receive udp from some address
    {"send", LuaUnixSend},                // send tcp to some address
    {"sendto", LuaUnixSendto},            // send udp to some address
    {"shutdown", LuaUnixShutdown},        // make socket half empty or full
    {"getpeername", LuaUnixGetpeername},  // get address of remote end
    {"getsockname", LuaUnixGetsockname},  // get address of local end
    {"siocgifconf", LuaUnixSiocgifconf},  // get list of network interfaces
    {"sigaction", LuaUnixSigaction},      // install signal handler
    {"sigprocmask", LuaUnixSigprocmask},  // change signal mask
    {"sigsuspend", LuaUnixSigsuspend},    // wait for signal
    {"setitimer", LuaUnixSetitimer},      // set alarm clock
    {"strerror", LuaUnixStrerror},        // turn errno into string
    {"strerrno", LuaUnixStrerrno},        // turn errno into string
    {"strerdoc", LuaUnixStrerdoc},        // turn errno into string
    {"strsignal", LuaUnixStrsignal},      // turn signal into string
    {"WIFEXITED", LuaUnixWifexited},      // gets exit code from wait status
    {"WEXITSTATUS", LuaUnixWexitstatus},  // gets exit status from wait status
    {"WIFSIGNALED", LuaUnixWifsignaled},  // determines if died due to signal
    {"WTERMSIG", LuaUnixWtermsig},        // gets the signal code
    {0},                                  //
};

static void LoadMagnums(lua_State *L, struct MagnumStr *ms, const char *pfx) {
  int i;
  char b[64], *p;
  p = stpcpy(b, pfx);
  for (i = 0; ms[i].x != MAGNUM_TERMINATOR; ++i) {
    stpcpy(p, MAGNUM_STRING(ms, i));
    LuaSetIntField(L, b, MAGNUM_NUMBER(ms, i));
  }
}

int LuaUnix(lua_State *L) {
  GL = L;
  luaL_newlib(L, kLuaUnix);
  LuaUnixStatObj(L);
  LuaUnixDirObj(L);
  lua_newtable(L);
  lua_setglobal(L, "__signal_handlers");

  LoadMagnums(L, kErrnoNames, "");
  LoadMagnums(L, kOpenFlags, "O_");
  LoadMagnums(L, kSignalNames, "SIG");
  LoadMagnums(L, kIpOptnames, "IP_");
  LoadMagnums(L, kTcpOptnames, "TCP_");
  LoadMagnums(L, kSockOptnames, "SO_");
  LoadMagnums(L, kClockNames, "CLOCK_");

  // seek() whence
  LuaSetIntField(L, "SEEK_SET", SEEK_SET);
  LuaSetIntField(L, "SEEK_CUR", SEEK_CUR);
  LuaSetIntField(L, "SEEK_END", SEEK_END);

  // fcntl() stuff
  LuaSetIntField(L, "F_GETFD", F_GETFD);
  LuaSetIntField(L, "F_SETFD", F_SETFD);
  LuaSetIntField(L, "F_GETFL", F_GETFL);
  LuaSetIntField(L, "F_SETFL", F_SETFL);
  LuaSetIntField(L, "F_UNLCK", F_UNLCK);
  LuaSetIntField(L, "F_RDLCK", F_RDLCK);
  LuaSetIntField(L, "F_WRLCK", F_WRLCK);
  LuaSetIntField(L, "F_SETLK", F_SETLK);
  LuaSetIntField(L, "F_SETLKW", F_SETLKW);
  LuaSetIntField(L, "FD_CLOEXEC", FD_CLOEXEC);

  // access() mode
  LuaSetIntField(L, "R_OK", R_OK);
  LuaSetIntField(L, "W_OK", W_OK);
  LuaSetIntField(L, "X_OK", X_OK);
  LuaSetIntField(L, "F_OK", F_OK);

  // rlimit() resources
  LuaSetIntField(L, "RLIMIT_AS", RLIMIT_AS);
  LuaSetIntField(L, "RLIMIT_RSS", RLIMIT_RSS);
  LuaSetIntField(L, "RLIMIT_CPU", RLIMIT_CPU);
  LuaSetIntField(L, "RLIMIT_FSIZE", RLIMIT_FSIZE);
  LuaSetIntField(L, "RLIMIT_NPROC", RLIMIT_NPROC);
  LuaSetIntField(L, "RLIMIT_NOFILE", RLIMIT_NOFILE);

  // wait() options
  LuaSetIntField(L, "WNOHANG", WNOHANG);
  LuaSetIntField(L, "WNOHANG", WNOHANG);

  // socket() family
  LuaSetIntField(L, "AF_UNSPEC", AF_UNSPEC);
  LuaSetIntField(L, "AF_UNIX", AF_UNIX);
  LuaSetIntField(L, "AF_INET", AF_INET);

  // socket() type
  LuaSetIntField(L, "SOCK_STREAM", SOCK_STREAM);
  LuaSetIntField(L, "SOCK_DGRAM", SOCK_DGRAM);
  LuaSetIntField(L, "SOCK_RAW", SOCK_RAW);
  LuaSetIntField(L, "SOCK_RDM", SOCK_RDM);
  LuaSetIntField(L, "SOCK_SEQPACKET", SOCK_SEQPACKET);
  LuaSetIntField(L, "SOCK_CLOEXEC", SOCK_CLOEXEC);

  // socket() protocol
  LuaSetIntField(L, "IPPROTO_IP", IPPROTO_IP);
  LuaSetIntField(L, "IPPROTO_ICMP", IPPROTO_ICMP);
  LuaSetIntField(L, "IPPROTO_TCP", IPPROTO_TCP);
  LuaSetIntField(L, "IPPROTO_UDP", IPPROTO_UDP);
  LuaSetIntField(L, "IPPROTO_RAW", IPPROTO_RAW);

  // shutdown() how
  LuaSetIntField(L, "SHUT_RD", SHUT_RD);
  LuaSetIntField(L, "SHUT_WR", SHUT_WR);
  LuaSetIntField(L, "SHUT_RDWR", SHUT_RDWR);

  // recvfrom() / sendto() flags
  LuaSetIntField(L, "MSG_WAITALL", MSG_WAITALL);
  LuaSetIntField(L, "MSG_DONTROUTE", MSG_DONTROUTE);
  LuaSetIntField(L, "MSG_PEEK", MSG_PEEK);
  LuaSetIntField(L, "MSG_OOB", MSG_OOB);
  LuaSetIntField(L, "MSG_NOSIGNAL", MSG_NOSIGNAL);

  // readdir() type
  LuaSetIntField(L, "DT_UNKNOWN", DT_UNKNOWN);
  LuaSetIntField(L, "DT_REG", DT_REG);
  LuaSetIntField(L, "DT_DIR", DT_DIR);
  LuaSetIntField(L, "DT_BLK", DT_BLK);
  LuaSetIntField(L, "DT_LNK", DT_LNK);
  LuaSetIntField(L, "DT_CHR", DT_CHR);
  LuaSetIntField(L, "DT_FIFO", DT_FIFO);
  LuaSetIntField(L, "DT_SOCK", DT_SOCK);

  // readdir() type
  LuaSetIntField(L, "POLLIN", POLLIN);
  LuaSetIntField(L, "POLLPRI", POLLPRI);
  LuaSetIntField(L, "POLLOUT", POLLOUT);
  LuaSetIntField(L, "POLLERR", POLLERR);
  LuaSetIntField(L, "POLLHUP", POLLHUP);
  LuaSetIntField(L, "POLLNVAL", POLLNVAL);
  LuaSetIntField(L, "POLLRDBAND", POLLRDBAND);
  LuaSetIntField(L, "POLLRDNORM", POLLRDNORM);
  LuaSetIntField(L, "POLLWRBAND", POLLWRBAND);
  LuaSetIntField(L, "POLLWRNORM", POLLWRNORM);
  LuaSetIntField(L, "POLLRDHUP", POLLRDHUP);

  // i/o options
  LuaSetIntField(L, "AT_FDCWD", AT_FDCWD);
  LuaSetIntField(L, "AT_SYMLINK_NOFOLLOW", AT_SYMLINK_NOFOLLOW);

  // sigprocmask() handlers
  LuaSetIntField(L, "SIG_BLOCK", SIG_BLOCK);
  LuaSetIntField(L, "SIG_UNBLOCK", SIG_UNBLOCK);
  LuaSetIntField(L, "SIG_SETMASK", SIG_SETMASK);

  // sigaction() handlers
  LuaSetIntField(L, "SIG_DFL", (intptr_t)SIG_DFL);
  LuaSetIntField(L, "SIG_IGN", (intptr_t)SIG_IGN);

  // setitimer() which
  LuaSetIntField(L, "ITIMER_REAL", ITIMER_REAL);  // portable
  LuaSetIntField(L, "ITIMER_PROF", ITIMER_PROF);
  LuaSetIntField(L, "ITIMER_VIRTUAL", ITIMER_VIRTUAL);

  // syslog() stuff
  LuaSetIntField(L, "LOG_EMERG", LOG_EMERG);
  LuaSetIntField(L, "LOG_ALERT", LOG_ALERT);
  LuaSetIntField(L, "LOG_CRIT", LOG_CRIT);
  LuaSetIntField(L, "LOG_ERR", LOG_ERR);
  LuaSetIntField(L, "LOG_WARNING", LOG_WARNING);
  LuaSetIntField(L, "LOG_NOTICE", LOG_NOTICE);
  LuaSetIntField(L, "LOG_INFO", LOG_INFO);
  LuaSetIntField(L, "LOG_DEBUG", LOG_DEBUG);

  // setsockopt() level
  LuaSetIntField(L, "SOL_IP", SOL_IP);
  LuaSetIntField(L, "SOL_SOCKET", SOL_SOCKET);
  LuaSetIntField(L, "SOL_TCP", SOL_TCP);
  LuaSetIntField(L, "SOL_UDP", SOL_UDP);

  return 1;
}
