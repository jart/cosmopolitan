/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "third_party/lua/lunix.h"
#include "third_party/lua/lunix_internal.h"
#include "libc/assert.h"
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/cp.internal.h"
#include "libc/calls/makedev.h"
#include "libc/calls/pledge.h"
#include "libc/calls/struct/bpf.internal.h"
#include "libc/calls/struct/dirent.h"
#include "libc/calls/struct/flock.h"
#include "libc/calls/struct/itimerval.h"
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/statfs.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
#include "libc/calls/struct/winsize.h"
#include "libc/calls/ucontext.h"
#include "libc/calls/weirdtypes.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/itoa.h"
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/intrin/atomic.h"
#include "libc/serialize.h"
#include "libc/intrin/strace.h"
#include "libc/limits.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/runtime/clktck.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/sysconf.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/ifconf.h"
#include "libc/sock/struct/linger.h"
#include "libc/sock/struct/pollfd.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/sock/syslog.h"
#include "libc/stdio/append.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/temp.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/dt.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/ip.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/itimer.h"
#include "libc/sysv/consts/limits.h"
#include "libc/sysv/consts/log.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/msg.h"
#include "libc/sysv/consts/nr.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/ok.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/prot.h"
#include "libc/calls/struct/rlimit.h"
#include "libc/sysv/consts/rusage.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/shut.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/sio.h"
#include "libc/sysv/consts/so.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/consts/sol.h"
#include "libc/sysv/consts/st.h"
#include "libc/sysv/consts/tcp.h"
#include "libc/sysv/consts/prio.h"
#include "libc/sysv/consts/termios.h"
#include "libc/sysv/consts/utime.h"
#include "libc/proc/posix_spawn.h"
#include "libc/sysv/consts/w.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/thread.h"
#include "libc/time.h"
#include "libc/x/x.h"
#include "third_party/lua/cosmo.h"
#include "third_party/lua/lauxlib.h"
#include "third_party/lua/lgc.h"
#include "third_party/lua/lua.h"
#include "third_party/lua/luaconf.h"
#include "libc/sysv/consts/clock.h"
#include "libc/cosmo.h"
#include "libc/cosmo.h"
#include "libc/cosmotime.h"
#include "tool/net/luacheck.h"

#define DNS_NAME_MAX  253

/**
 * @fileoverview UNIX system calls thinly wrapped for Lua
 * @support Linux, Mac, Windows, FreeBSD, NetBSD, OpenBSD
 */

lua_State *GL;

void *LuaRealloc(lua_State *L, void *p, size_t n) {
  void *p2;
  if ((p2 = realloc(p, n))) {
    return p2;
  }
  if (n < 0x100000000000) {
    WARNF("reacting to malloc() failure by running lua garbage collector...");
    luaC_fullgc(L, 1);
    if ((p2 = realloc(p, n)))
      return p2;
  }
  return p;
}

void *LuaAlloc(lua_State *L, size_t n) {
  return LuaRealloc(L, 0, n);
}

static void *LuaAllocOrDie(lua_State *L, size_t n) {
  void *p;
  if ((p = LuaAlloc(L, n))) {
    return p;
  } else {
    luaL_error(L, "out of memory");
    __builtin_unreachable();
  }
}

static lua_Integer FixLimit(long x) {
  if (0 <= x && x < RLIM_INFINITY) {
    return x;
  } else {
    return -1;
  }
}

void LuaPushSigset(lua_State *L, sigset_t set) {
  sigset_t *sp = lua_newuserdatauv(L, sizeof(*sp), 1);
  luaL_setmetatable(L, "unix.Sigset");
  *sp = set;
}

void LuaPushStat(lua_State *L, struct stat *st) {
  struct stat *stp = lua_newuserdatauv(L, sizeof(*stp), 1);
  luaL_setmetatable(L, "unix.Stat");
  *stp = *st;
}

void LuaPushStatfs(lua_State *L, struct statfs *st) {
  struct statfs *stp = lua_newuserdatauv(L, sizeof(*stp), 1);
  luaL_setmetatable(L, "unix.Statfs");
  *stp = *st;
}

void LuaPushRusage(lua_State *L, struct rusage *set) {
  struct rusage *sp = lua_newuserdatauv(L, sizeof(*sp), 1);
  luaL_setmetatable(L, "unix.Rusage");
  *sp = *set;
}

void LuaSetIntField(lua_State *L, const char *k, lua_Integer v) {
  lua_pushinteger(L, v);
  lua_setfield(L, -2, k);
}

int LuaUnixSysretErrno(lua_State *L, const char *call, int olderr) {
  int unixerr, winerr;
  struct UnixErrno *ep;
  unixerr = errno;
  winerr = IsWindows() ? GetLastError() : 0;
  if (!IsTiny() && !(0 < unixerr && unixerr < (!IsWindows() ? 4096 : 65536))) {
    WARNF("errno should not be %d", unixerr);
  }
  lua_pushnil(L);
  ep = lua_newuserdatauv(L, sizeof(*ep), 1);
  luaL_setmetatable(L, "unix.Errno");
  ep->errno_ = unixerr;
  ep->winerr = winerr;
  ep->call = call;
  errno = olderr;
  return 2;
}

int SysretBool(lua_State *L, const char *call, int olderr, int rc) {
  if (!IsTiny() && (rc != 0 && rc != -1))
    WARNF("syscall supposed to return 0 / -1 but got %d", rc);
  if (rc != -1) {
    lua_pushboolean(L, true);
    return 1;
  } else {
    return LuaUnixSysretErrno(L, call, olderr);
  }
}

int SysretInteger(lua_State *L, const char *call, int olderr,
                  int64_t rc) {
  if (rc != -1) {
    if (!IsTiny() && olderr != errno) {
      WARNF("errno unexpectedly changed %d → %d", olderr, errno);
    }
    lua_pushinteger(L, rc);
    return 1;
  } else {
    return LuaUnixSysretErrno(L, call, olderr);
  }
}

static int MakeSockaddr(lua_State *L, int i, struct sockaddr_storage *ss,
                        uint32_t *salen) {
  bzero(ss, sizeof(*ss));
  if (!lua_isinteger(L, i)) {
    ((struct sockaddr_un *)ss)->sun_family = AF_UNIX;
    if (!memccpy(((struct sockaddr_un *)ss)->sun_path, luaL_checkstring(L, i),
                 0, sizeof(((struct sockaddr_un *)ss)->sun_path))) {
      luaL_error(L, "unix path too long");
      __builtin_unreachable();
    }
    *salen = sizeof(struct sockaddr_un);
    return i + 1;
  } else {
    ((struct sockaddr_in *)ss)->sin_family = AF_INET;
    ((struct sockaddr_in *)ss)->sin_addr.s_addr =
        htonl(luaL_optinteger(L, i, 0));
    ((struct sockaddr_in *)ss)->sin_port = htons(luaL_optinteger(L, i + 1, 0));
    *salen = sizeof(struct sockaddr_in);
    return i + 2;
  }
}

static int PushSockaddr(lua_State *L, const struct sockaddr_storage *ss) {
  if (ss->ss_family == AF_INET) {
    lua_pushinteger(L,
                    ntohl(((const struct sockaddr_in *)ss)->sin_addr.s_addr));
    lua_pushinteger(L, ntohs(((const struct sockaddr_in *)ss)->sin_port));
    return 2;
  } else if (ss->ss_family == AF_UNIX) {
    lua_pushstring(L, ((const struct sockaddr_un *)ss)->sun_path);
    return 1;
  } else {
    luaL_error(L, "bad family");
    __builtin_unreachable();
  }
}

static void CheckOptvalsize(lua_State *L, uint32_t want, uint32_t got) {
  if (!IsTiny()) {
    if (want == got) return;
    WARNF("getsockopt optvalsize should be %d but was %d", want, got);
  }
}

void FreeStringList(char **p) {
  int i;
  if (p) {
    for (i = 0; p[i]; ++i) {
      free(p[i]);
    }
    free(p);
  }
}

char **ConvertLuaArrayToStringList(lua_State *L, int i) {
  int j, n;
  char **p, *s;
  const char *str;
  luaL_checktype(L, i, LUA_TTABLE);
  lua_len(L, i);
  n = lua_tointeger(L, -1);
  lua_pop(L, 1);
  if ((p = LuaAlloc(L, (n + 1) * sizeof(*p)))) {
    for (j = 1; j <= n; ++j) {
      lua_geti(L, i, j);
      if ((str = lua_tostring(L, -1))) {
        s = strdup(str);
      } else {
        s = 0;
      }
      lua_pop(L, 1);
      if (s) {
        p[j - 1] = s;
      } else {
        FreeStringList(p);
        p = 0;
        break;
      }
    }
    if (p)
      p[j - 1] = 0;
  }
  return p;
}

////////////////////////////////////////////////////////////////////////////////
// System Calls

// unix.exit([exitcode:int])
//     └─→ ⊥
static wontreturn int LuaUnixExit(lua_State *L) {
  _Exit(luaL_optinteger(L, 1, 0));
}

static dontinline int LuaUnixGetid(lua_State *L, int f(void)) {
  return ReturnInteger(L, f());
}

static dontinline int LuaUnixGetUnsignedid(lua_State *L, unsigned f(void)) {
  return ReturnInteger(L, f());
}

// unix.getpid()
//     └─→ pid:int
static int LuaUnixGetpid(lua_State *L) {
  return LuaUnixGetid(L, getpid);
}

// unix.getppid()
//     └─→ pid:int
static int LuaUnixGetppid(lua_State *L) {
  return LuaUnixGetid(L, getppid);
}

// unix.getuid()
//     └─→ uid:int
static int LuaUnixGetuid(lua_State *L) {
  return LuaUnixGetUnsignedid(L, getuid);
}

// unix.getgid()
//     └─→ gid:int
static int LuaUnixGetgid(lua_State *L) {
  return LuaUnixGetUnsignedid(L, getgid);
}

// unix.geteuid()
//     └─→ uid:int
static int LuaUnixGeteuid(lua_State *L) {
  return LuaUnixGetUnsignedid(L, geteuid);
}

// unix.getegid()
//     └─→ gid:int
static int LuaUnixGetegid(lua_State *L) {
  return LuaUnixGetUnsignedid(L, getegid);
}

// unix.umask(newmask:int)
//     └─→ oldmask:int
static int LuaUnixUmask(lua_State *L) {
  return ReturnInteger(L, umask(luaL_checkinteger(L, 1)));
}

// unix.access(path:str, how:int[, flags:int[, dirfd:int]])
//     ├─→ true
//     └─→ nil, unix.Errno
static int LuaUnixAccess(lua_State *L) {
  int olderr = errno;
  return SysretBool(
      L, "access", olderr,
      faccessat(luaL_optinteger(L, 3, AT_FDCWD), luaL_checkstring(L, 1),
                luaL_checkinteger(L, 2), luaL_optinteger(L, 4, 0)));
}

// unix.mkdir(path:str[, mode:int[, dirfd:int]])
//     ├─→ true
//     └─→ nil, unix.Errno
static int LuaUnixMkdir(lua_State *L) {
  int olderr = errno;
  return SysretBool(
      L, "mkdir", olderr,
      mkdirat(luaL_optinteger(L, 3, AT_FDCWD), luaL_checkstring(L, 1),
              luaL_optinteger(L, 2, 0755)));
}

// unix.makedirs(path:str[, mode:int])
//     ├─→ true
//     └─→ nil, unix.Errno
static int LuaUnixMakedirs(lua_State *L) {
  int olderr = errno;
  return SysretBool(
      L, "makedirs", olderr,
      makedirs(luaL_checkstring(L, 1), luaL_optinteger(L, 2, 0755)));
}

// unix.rmrf(path:str)
//     ├─→ true
//     └─→ nil, unix.Errno
static int LuaUnixRmrf(lua_State *L) {
  int olderr = errno;
  return SysretBool(L, "rmrf", olderr, rmrf(luaL_checkstring(L, 1)));
}

// unix.chdir(path:str)
//     ├─→ true
//     └─→ nil, unix.Errno
static int LuaUnixChdir(lua_State *L) {
  int olderr = errno;
  return SysretBool(L, "chdir", olderr, chdir(luaL_checkstring(L, 1)));
}

// unix.unlink(path:str[, dirfd:int])
//     ├─→ true
//     └─→ nil, unix.Errno
static int LuaUnixUnlink(lua_State *L) {
  int olderr = errno;
  return SysretBool(
      L, "unlink", olderr,
      unlinkat(luaL_optinteger(L, 2, AT_FDCWD), luaL_checkstring(L, 1), 0));
}

// unix.rmdir(path:str[, dirfd:int])
//     ├─→ true
//     └─→ nil, unix.Errno
static int LuaUnixRmdir(lua_State *L) {
  int olderr = errno;
  return SysretBool(L, "rmdir", olderr,
                    unlinkat(luaL_optinteger(L, 2, AT_FDCWD),
                             luaL_checkstring(L, 1), AT_REMOVEDIR));
}

// unix.rename(oldpath:str, newpath:str[, olddirfd:int, newdirfd:int])
//     ├─→ true
//     └─→ nil, unix.Errno
static int LuaUnixRename(lua_State *L) {
  int olderr = errno;
  return SysretBool(
      L, "rename", olderr,
      renameat(luaL_optinteger(L, 3, AT_FDCWD), luaL_checkstring(L, 1),
               luaL_optinteger(L, 4, AT_FDCWD), luaL_checkstring(L, 2)));
}

// unix.link(existingpath:str, newpath:str[, flags:int[, olddirfd, newdirfd]])
//     ├─→ true
//     └─→ nil, unix.Errno
static int LuaUnixLink(lua_State *L) {
  int olderr = errno;
  return SysretBool(
      L, "link", olderr,
      linkat(luaL_optinteger(L, 4, AT_FDCWD), luaL_checkstring(L, 1),
             luaL_optinteger(L, 5, AT_FDCWD), luaL_checkstring(L, 2),
             luaL_optinteger(L, 3, 0)));
}

// unix.symlink(target:str, linkpath:str[, newdirfd:int])
//     ├─→ true
//     └─→ nil, unix.Errno
static int LuaUnixSymlink(lua_State *L) {
  int olderr = errno;
  return SysretBool(
      L, "symlink", olderr,
      symlinkat(luaL_checkstring(L, 1), luaL_optinteger(L, 3, AT_FDCWD),
                luaL_checkstring(L, 2)));
}

// unix.chown(path:str, uid:int, gid:int[, flags:int[, dirfd:int]])
//     ├─→ true
//     └─→ nil, unix.Errno
static int LuaUnixChown(lua_State *L) {
  int olderr = errno;
  return SysretBool(
      L, "chown", olderr,
      fchownat(luaL_optinteger(L, 5, AT_FDCWD), luaL_checkstring(L, 1),
               luaL_checkinteger(L, 2), luaL_checkinteger(L, 3),
               luaL_optinteger(L, 4, 0)));
}

// unix.chmod(path:str, mode:int[, flags:int[, dirfd:int]])
//     ├─→ true
//     └─→ nil, unix.Errno
static int LuaUnixChmod(lua_State *L) {
  int olderr = errno;
  return SysretBool(
      L, "chmod", olderr,
      fchmodat(luaL_optinteger(L, 4, AT_FDCWD), luaL_checkstring(L, 1),
               luaL_checkinteger(L, 2), luaL_optinteger(L, 3, 0)));
}

// unix.readlink(path:str[, dirfd:int])
//     ├─→ content:str
//     └─→ nil, unix.Errno
static int LuaUnixReadlink(lua_State *L) {
  size_t got;
  ssize_t rc;
  luaL_Buffer lb;
  int olderr = errno;
  if ((rc = readlinkat(luaL_optinteger(L, 2, AT_FDCWD), luaL_checkstring(L, 1),
                       luaL_buffinitsize(L, &lb, BUFSIZ), BUFSIZ)) != -1) {
    if ((got = rc) < BUFSIZ) {
      luaL_pushresultsize(&lb, got);
      return 1;
    } else {
      enametoolong();
    }
  }
  return LuaUnixSysretErrno(L, "readlink", olderr);
}

// unix.getcwd()
//     ├─→ path:str
//     └─→ nil, unix.Errno
static int LuaUnixGetcwd(lua_State *L) {
  char *path;
  int olderr = errno;
  if ((path = getcwd(0, 0))) {
    lua_pushstring(L, path);
    free(path);
    return 1;
  } else {
    return LuaUnixSysretErrno(L, "getcwd", olderr);
  }
}

// unix.fork()
//     ├─┬─→ 0
//     │ └─→ childpid:int
//     └─→ nil, unix.Errno
static int LuaUnixFork(lua_State *L) {
  int olderr = errno;
  return SysretInteger(L, "fork", olderr, fork());
}

// unix.environ()
//     └─→ {str,...}
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

// unix.execve(prog:str[, args:List<*>, env:List<*>])
//     └─→ nil, unix.Errno
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
          return LuaUnixSysretErrno(L, "execve", olderr);
        }
      } else {
        envp = environ;
        freeme2 = 0;
      }
    } else {
      return LuaUnixSysretErrno(L, "execve", olderr);
    }
  } else {
    ezargs[0] = (char *)prog;
    ezargs[1] = 0;
    argv = ezargs;
    envp = environ;
    freeme1 = 0;
    freeme2 = 0;
  }
  execve(prog, argv, envp);
  FreeStringList(freeme1);
  FreeStringList(freeme2);
  return LuaUnixSysretErrno(L, "execve", olderr);
}

// unix.commandv(prog:str)
//     ├─→ path:str
//     └─→ nil, unix.Errno
static int LuaUnixCommandv(lua_State *L) {
  int olderr;
  const char *prog;
  char *pathbuf, *resolved;
  olderr = errno;
  prog = luaL_checkstring(L, 1);
  pathbuf = LuaAllocOrDie(L, PATH_MAX);
  if ((resolved = commandv(prog, pathbuf, PATH_MAX))) {
    lua_pushstring(L, resolved);
    free(pathbuf);
    return 1;
  } else {
    free(pathbuf);
    return LuaUnixSysretErrno(L, "commandv", olderr);
  }
}

// unix.realpath(path:str)
//     ├─→ path:str
//     └─→ nil, unix.Errno
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
    return LuaUnixSysretErrno(L, "realpath", olderr);
  }
}

// unix.syslog(priority:str, msg:str)
static int LuaUnixSyslog(lua_State *L) {
  syslog(luaL_checkinteger(L, 1), "%s", luaL_checkstring(L, 2));
  return 0;
}

// unix.chroot(path:str)
//     ├─→ true
//     └─→ nil, unix.Errno
static int LuaUnixChroot(lua_State *L) {
  int olderr = errno;
  return SysretBool(L, "chroot", olderr, chroot(luaL_checkstring(L, 1)));
}

// unix.setrlimit(resource:int, soft:int[, hard:int])
//     ├─→ true
//     └─→ nil, unix.Errno
static int LuaUnixSetrlimit(lua_State *L) {
  int olderr = errno;
  int64_t soft = luaL_checkinteger(L, 2);
  return SysretBool(
      L, "setrlimit", olderr,
      setrlimit(luaL_checkinteger(L, 1),
                &(struct rlimit){soft, luaL_optinteger(L, 3, soft)}));
}

// unix.getrlimit(resource:int)
//     ├─→ soft:int, hard:int
//     └─→ nil, unix.Errno
static int LuaUnixGetrlimit(lua_State *L) {
  int olderr = errno;
  struct rlimit rlim;
  if (!getrlimit(luaL_checkinteger(L, 1), &rlim)) {
    lua_pushinteger(L, FixLimit(rlim.rlim_cur));
    lua_pushinteger(L, FixLimit(rlim.rlim_max));
    return 2;
  } else {
    return LuaUnixSysretErrno(L, "getrlimit", olderr);
  }
}

// unix.getrusage([who:int])
//     ├─→ unix.Rusage
//     └─→ nil, unix.Errno
static int LuaUnixGetrusage(lua_State *L) {
  struct rusage ru;
  int olderr = errno;
  if (!getrusage(luaL_optinteger(L, 1, RUSAGE_SELF), &ru)) {
    LuaPushRusage(L, &ru);
    return 1;
  } else {
    return LuaUnixSysretErrno(L, "getrusage", olderr);
  }
}

// unix.kill(pid:int, sig:int)
//     ├─→ true
//     └─→ nil, unix.Errno
static int LuaUnixKill(lua_State *L) {
  int olderr = errno;
  return SysretBool(L, "kill", olderr,
                    kill(luaL_checkinteger(L, 1), luaL_checkinteger(L, 2)));
}

// unix.raise(sig:int)
//     ├─→ rc:int
//     └─→ nil, unix.Errno
static int LuaUnixRaise(lua_State *L) {
  int olderr = errno;
  return SysretInteger(L, "raise", olderr, raise(luaL_checkinteger(L, 1)));
}

// unix.wait([pid:int, options:int])
//     ├─→ pid:int, wstatus:int, unix.Rusage
//     └─→ nil, unix.Errno
static int LuaUnixWait(lua_State *L) {
  struct rusage ru;
  int pid, wstatus, olderr = errno;
  if ((pid = wait4(luaL_optinteger(L, 1, -1), &wstatus,
                   luaL_optinteger(L, 2, 0), &ru)) != -1) {
    lua_pushinteger(L, pid);
    lua_pushinteger(L, wstatus);
    LuaPushRusage(L, &ru);
    return 3;
  } else {
    return LuaUnixSysretErrno(L, "wait", olderr);
  }
}

// unix.fcntl(fd:int, cmd:int[, ...])
//     ├─→ ...
//     └─→ nil, unix.Errno
static int LuaUnixFcntl(lua_State *L) {
  struct flock lock;
  int fd, cmd, olderr = errno;
  fd = luaL_checkinteger(L, 1);
  cmd = luaL_checkinteger(L, 2);
  if (cmd == F_SETLK || cmd == F_SETLKW || cmd == F_GETLK) {
    lock.l_type = luaL_optinteger(L, 3, F_RDLCK);
    lock.l_start = luaL_optinteger(L, 4, 0);
    lock.l_len = luaL_optinteger(L, 5, 0);
    lock.l_whence = luaL_optinteger(L, 6, SEEK_SET);
  }
  if (cmd == F_SETLK || cmd == F_SETLKW) {
    return SysretBool(L, "fcntl(F_SETLK*)", olderr, fcntl(fd, cmd, &lock));
  } else if (cmd == F_GETLK) {
    if (fcntl(fd, cmd, &lock) != -1) {
      if (lock.l_type == F_UNLCK) {
        lua_pushinteger(L, F_UNLCK);
        return 1;
      } else {
        lua_pushinteger(L, lock.l_type);
        lua_pushinteger(L, lock.l_start);
        lua_pushinteger(L, lock.l_len);
        lua_pushinteger(L, lock.l_whence);
        lua_pushinteger(L, lock.l_pid);
        return 5;
      }
    } else {
      return LuaUnixSysretErrno(L, "fcntl(F_GETLK)", olderr);
    }
  } else {
    return SysretBool(L, "fcntl", olderr,
                      fcntl(fd, cmd, luaL_optinteger(L, 3, 0)));
  }
}

// unix.dup(oldfd:int[, newfd:int[, flags:int[, lowest:int]]])
//     ├─→ newfd:int
//     └─→ nil, unix.Errno
static int LuaUnixDup(lua_State *L) {
  int rc, oldfd, newfd, flags, lowno, olderr;
  olderr = errno;
  oldfd = luaL_checkinteger(L, 1);
  newfd = luaL_optinteger(L, 2, -1);
  flags = luaL_optinteger(L, 3, 0);
  lowno = luaL_optinteger(L, 4, 0);
  if (newfd < 0) {
    if (!flags && !lowno) {
      rc = dup(oldfd);
    } else if (!flags) {
      rc = fcntl(oldfd, F_DUPFD, lowno);
    } else if (flags == O_CLOEXEC) {
      rc = fcntl(oldfd, F_DUPFD_CLOEXEC, lowno);
    } else {
      rc = einval();
    }
  } else {
    rc = dup3(oldfd, newfd, flags);
  }
  return SysretInteger(L, "dup", olderr, rc);
}

// unix.pipe([flags:int])
//     ├─→ reader:int, writer:int
//     └─→ nil, unix.Errno
static int LuaUnixPipe(lua_State *L) {
  int pipefd[2], olderr = errno;
  if (!pipe2(pipefd, luaL_optinteger(L, 1, 0))) {
    lua_pushinteger(L, pipefd[0]);
    lua_pushinteger(L, pipefd[1]);
    return 2;
  } else {
    return LuaUnixSysretErrno(L, "pipe", olderr);
  }
}

// unix.getsid(pid:int)
//     ├─→ sid:int
//     └─→ nil, unix.Errno
static int LuaUnixGetsid(lua_State *L) {
  int olderr = errno;
  return SysretInteger(L, "getsid", olderr, getsid(luaL_checkinteger(L, 1)));
}

static dontinline int LuaUnixRc0(lua_State *L, const char *call, int f(void)) {
  int olderr = errno;
  return SysretInteger(L, call, olderr, f());
}

// unix.getpgrp()
//     ├─→ pgid:int
//     └─→ nil, unix.Errno
static int LuaUnixGetpgrp(lua_State *L) {
  return LuaUnixRc0(L, "getpgrp", getpgrp);
}

// unix.setpgrp()
//     ├─→ pgid:int
//     └─→ nil, unix.Errno
static int LuaUnixSetpgrp(lua_State *L) {
  return LuaUnixRc0(L, "setpgrp", setpgrp);
}

// unix.setsid()
//     ├─→ sid:int
//     └─→ nil, unix.Errno
static int LuaUnixSetsid(lua_State *L) {
  return LuaUnixRc0(L, "setsid", setsid);
}

// unix.getpgid(pid:int)
//     ├─→ pgid:int
//     └─→ nil, unix.Errno
static int LuaUnixGetpgid(lua_State *L) {
  int olderr = errno;
  return SysretInteger(L, "getpgid", olderr, getpgid(luaL_checkinteger(L, 1)));
}

// unix.setpgid(pid:int, pgid:int)
//     ├─→ true
//     └─→ nil, unix.Errno
static int LuaUnixSetpgid(lua_State *L) {
  int olderr = errno;
  return SysretBool(L, "setpgid", olderr,
                    setpgid(luaL_checkinteger(L, 1), luaL_checkinteger(L, 2)));
}

static dontinline int LuaUnixSetid(lua_State *L, const char *call,
                                   int f(unsigned)) {
  int olderr = errno;
  return SysretBool(L, call, olderr, f(luaL_checkinteger(L, 1)));
}

// unix.setuid(uid:int)
//     ├─→ true
//     └─→ nil, unix.Errno
static int LuaUnixSetuid(lua_State *L) {
  return LuaUnixSetid(L, "setuid", setuid);
}

// unix.setgid(gid:int)
//     ├─→ true
//     └─→ nil, unix.Errno
static int LuaUnixSetgid(lua_State *L) {
  return LuaUnixSetid(L, "setgid", setgid);
}

// unix.setfsuid(fsuid:int)
//     ├─→ true
//     └─→ nil, unix.Errno
static int LuaUnixSetfsuid(lua_State *L) {
  return LuaUnixSetid(L, "setfsuid", setfsuid);
}

// unix.setfsgid(fsgid:int)
//     ├─→ true
//     └─→ nil, unix.Errno
static int LuaUnixSetfsgid(lua_State *L) {
  return LuaUnixSetid(L, "setfsgid", setfsgid);
}

static dontinline int LuaUnixSetresid(lua_State *L, const char *call,
                                      int f(uint32_t, uint32_t, uint32_t)) {
  int olderr = errno;
  return SysretBool(L, call, olderr,
                    f(luaL_checkinteger(L, 1), luaL_checkinteger(L, 2),
                      luaL_checkinteger(L, 3)));
}

// unix.setresuid(real:int, effective:int, saved:int)
//     ├─→ true
//     └─→ nil, unix.Errno
static int LuaUnixSetresuid(lua_State *L) {
  return LuaUnixSetresid(L, "setresuid", setresuid);
}

// unix.setresgid(real:int, effective:int, saved:int)
//     ├─→ true
//     └─→ nil, unix.Errno
static int LuaUnixSetresgid(lua_State *L) {
  return LuaUnixSetresid(L, "setresgid", setresgid);
}

// unix.utimensat(path[, asecs, ananos, msecs, mnanos[, dirfd[, flags]]])
//     ├─→ 0
//     └─→ nil, unix.Errno
static int LuaUnixUtimensat(lua_State *L) {
  int olderr = errno;
  return SysretInteger(
      L, "utimensat", olderr,
      utimensat(
          luaL_optinteger(L, 6, AT_FDCWD), luaL_checkstring(L, 1),
          (struct timespec[2]){
              {luaL_optinteger(L, 2, 0), luaL_optinteger(L, 3, UTIME_NOW)},
              {luaL_optinteger(L, 4, 0), luaL_optinteger(L, 5, UTIME_NOW)},
          },
          luaL_optinteger(L, 7, 0)));
}

// unix.futimens(fd:int[, asecs, ananos, msecs, mnanos])
//     ├─→ 0
//     └─→ nil, unix.Errno
static int LuaUnixFutimens(lua_State *L) {
  int olderr = errno;
  return SysretInteger(
      L, "futimens", olderr,
      futimens(luaL_checkinteger(L, 1),
               (struct timespec[2]){
                   {luaL_optinteger(L, 2, 0), luaL_optinteger(L, 3, UTIME_NOW)},
                   {luaL_optinteger(L, 4, 0), luaL_optinteger(L, 5, UTIME_NOW)},
               }));
}

// unix.clock_gettime([clock:int])
//     ├─→ seconds:int, nanos:int
//     └─→ nil, unix.Errno
static int LuaUnixGettime(lua_State *L) {
  struct timespec ts;
  int olderr = errno;
  if (!clock_gettime(luaL_optinteger(L, 1, CLOCK_REALTIME), &ts)) {
    lua_pushinteger(L, ts.tv_sec);
    lua_pushinteger(L, ts.tv_nsec);
    return 2;
  } else {
    return LuaUnixSysretErrno(L, "clock_gettime", olderr);
  }
}

// unix.nanosleep(seconds:int[, nanos:int])
//     ├─→ remseconds:int, remnanos:int
//     └─→ nil, unix.Errno
static int LuaUnixNanosleep(lua_State *L) {
  int olderr = errno;
  struct timespec req, rem;
  req.tv_sec = luaL_checkinteger(L, 1);
  req.tv_nsec = luaL_optinteger(L, 2, 0);
  if (!nanosleep(&req, &rem)) {
    lua_pushinteger(L, rem.tv_sec);
    lua_pushinteger(L, rem.tv_nsec);
    return 2;
  } else {
    return LuaUnixSysretErrno(L, "nanosleep", olderr);
  }
}

// unix.sync()
static int LuaUnixSync(lua_State *L) {
  sync();
  return 0;
}

// unix.fsync(fd:int)
//     ├─→ true
//     └─→ nil, unix.Errno
static int LuaUnixFsync(lua_State *L) {
  int olderr = errno;
  return SysretBool(L, "fsync", olderr, fsync(luaL_checkinteger(L, 1)));
}

// unix.fdatasync(fd:int)
//     ├─→ true
//     └─→ nil, unix.Errno
static int LuaUnixFdatasync(lua_State *L) {
  int olderr = errno;
  return SysretBool(L, "fdatasync", olderr, fdatasync(luaL_checkinteger(L, 1)));
}

// unix.open(path:str[, flags:int[, mode:int[, dirfd:int]]])
//     ├─→ fd:int
//     └─→ nil, unix.Errno
static int LuaUnixOpen(lua_State *L) {
  int olderr = errno;
  return SysretInteger(
      L, "open", olderr,
      openat(luaL_optinteger(L, 4, AT_FDCWD), luaL_checkstring(L, 1),
             luaL_optinteger(L, 2, O_RDONLY), luaL_optinteger(L, 3, 0644)));
}

// unix.tmpfd()
//     ├─→ fd:int
//     └─→ nil, unix.Errno
static int LuaUnixTmpfd(lua_State *L) {
  int olderr = errno;
  return SysretInteger(L, "tmpfd", olderr, tmpfd());
}

// unix.close(fd:int)
//     ├─→ true
//     └─→ nil, unix.Errno
static int LuaUnixClose(lua_State *L) {
  int olderr = errno;
  return SysretBool(L, "close", olderr, close(luaL_checkinteger(L, 1)));
}

// unix.lseek(fd:int, offset:int[, whence:int])
//     ├─→ newposbytes:int
//     └─→ nil, unix.Errno
static int LuaUnixLseek(lua_State *L) {
  int olderr = errno;
  return SysretInteger(L, "lseek", olderr,
                       lseek(luaL_checkinteger(L, 1), luaL_checkinteger(L, 2),
                             luaL_optinteger(L, 3, SEEK_SET)));
}

// unix.truncate(path:str[, length:int])
//     ├─→ true
//     └─→ nil, unix.Errno
static int LuaUnixTruncate(lua_State *L) {
  int olderr = errno;
  return SysretBool(L, "truncate", olderr,
                    truncate(luaL_checkstring(L, 1), luaL_optinteger(L, 2, 0)));
}

// unix.ftruncate(fd:int[, length:int])
//     ├─→ true
//     └─→ nil, unix.Errno
static int LuaUnixFtruncate(lua_State *L) {
  int olderr = errno;
  return SysretBool(
      L, "ftruncate", olderr,
      ftruncate(luaL_checkinteger(L, 1), luaL_optinteger(L, 2, 0)));
}

// unix.read(fd:int[, bufsiz:str[, offset:int]])
//     ├─→ data:str
//     └─→ nil, unix.Errno
static int LuaUnixRead(lua_State *L) {
  char *buf;
  size_t got;
  ssize_t rc;
  int fd, olderr;
  lua_Integer bufsiz, offset;
  olderr = errno;
  fd = luaL_checkinteger(L, 1);
  bufsiz = luaL_optinteger(L, 2, BUFSIZ);
  offset = luaL_optinteger(L, 3, -1);
  bufsiz = MIN(bufsiz, 0x7ffff000);
  buf = LuaAllocOrDie(L, bufsiz);
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
    return LuaUnixSysretErrno(L, "read", olderr);
  }
}

// unix.write(fd:int, data:str[, offset:int])
//     ├─→ wrotebytes:int
//     └─→ nil, unix.Errno
static int LuaUnixWrite(lua_State *L) {
  ssize_t rc;
  size_t size;
  int fd, olderr;
  const char *data;
  lua_Integer offset;
  olderr = errno;
  fd = luaL_checkinteger(L, 1);
  data = luaL_checklstring(L, 2, &size);
  offset = luaL_optinteger(L, 3, -1);
  if (offset == -1) {
    rc = write(fd, data, size);
  } else {
    rc = pwrite(fd, data, size, offset);
  }
  return SysretInteger(L, "write", olderr, rc);
}

// unix.stat(path:str[, flags:int[, dirfd:int]])
//     ├─→ unix.Stat
//     └─→ nil, unix.Errno
static int LuaUnixStat(lua_State *L) {
  struct stat st;
  int olderr = errno;
  if (!fstatat(luaL_optinteger(L, 3, AT_FDCWD), luaL_checkstring(L, 1), &st,
               luaL_optinteger(L, 2, 0))) {
    LuaPushStat(L, &st);
    return 1;
  } else {
    return LuaUnixSysretErrno(L, "stat", olderr);
  }
}

// unix.fstat(fd:int)
//     ├─→ unix.Stat
//     └─→ nil, unix.Errno
static int LuaUnixFstat(lua_State *L) {
  struct stat st;
  int olderr = errno;
  if (!fstat(luaL_checkinteger(L, 1), &st)) {
    LuaPushStat(L, &st);
    return 1;
  } else {
    return LuaUnixSysretErrno(L, "fstat", olderr);
  }
}

// unix.statfs(path:str)
//     ├─→ unix.Statfs
//     └─→ nil, unix.Errno
static int LuaUnixStatfs(lua_State *L) {
  struct statfs f;
  int olderr = errno;
  if (!statfs(luaL_checkstring(L, 1), &f)) {
    LuaPushStatfs(L, &f);
    return 1;
  } else {
    return LuaUnixSysretErrno(L, "statfs", olderr);
  }
}

// unix.fstatfs(fd:int)
//     ├─→ unix.Stat
//     └─→ nil, unix.Errno
static int LuaUnixFstatfs(lua_State *L) {
  struct statfs f;
  int olderr = errno;
  if (!fstatfs(luaL_checkinteger(L, 1), &f)) {
    LuaPushStatfs(L, &f);
    return 1;
  } else {
    return LuaUnixSysretErrno(L, "fstatfs", olderr);
  }
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
  } else if (l == SOL_TCP) {
    return x == TCP_NODELAY ||           //
           x == TCP_CORK ||              //
           x == TCP_QUICKACK ||          //
           x == TCP_SAVE_SYN ||          //
           x == TCP_FASTOPEN_CONNECT ||  //
           x == TCP_DEFER_ACCEPT;        //
  } else if (l == SOL_IP) {
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
  } else if (l == SOL_TCP) {
    return x == TCP_FASTOPEN ||       //
           x == TCP_KEEPCNT ||        //
           x == TCP_MAXSEG ||         //
           x == TCP_SYNCNT ||         //
           x == TCP_NOTSENT_LOWAT ||  //
           x == TCP_WINDOW_CLAMP ||   //
           x == TCP_KEEPIDLE ||       //
           x == TCP_KEEPINTVL;        //
  } else if (l == SOL_IP) {
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

static int LuaUnixSetsockopt(lua_State *L) {
  void *optval;
  struct linger l;
  uint32_t optsize;
  struct timeval tv;
  int fd, level, optname, optint, olderr = errno;
  fd = luaL_checkinteger(L, 1);
  level = luaL_checkinteger(L, 2);
  optname = luaL_checkinteger(L, 3);
  if (level == -1 || optname == 0) {
  NoProtocolOption:
    enoprotoopt();
    return LuaUnixSysretErrno(L, "setsockopt", olderr);
  }
  if (IsSockoptBool(level, optname)) {
    // unix.setsockopt(fd:int, level:int, optname:int, value:bool)
    //     ├─→ true
    //     └─→ nil, unix.Errno
    optint = lua_toboolean(L, 4);
    optval = &optint;
    optsize = sizeof(optint);
  } else if (IsSockoptInt(level, optname)) {
    // unix.setsockopt(fd:int, level:int, optname:int, value:int)
    //     ├─→ true
    //     └─→ nil, unix.Errno
    optint = luaL_checkinteger(L, 4);
    optval = &optint;
    optsize = sizeof(optint);
  } else if (IsSockoptTimeval(level, optname)) {
    // unix.setsockopt(fd:int, level:int, optname:int, secs:int[, nanos:int])
    //     ├─→ true
    //     └─→ nil, unix.Errno
    tv.tv_sec = luaL_checkinteger(L, 4);
    tv.tv_usec = luaL_optinteger(L, 5, 0) / 1000;
    optval = &tv;
    optsize = sizeof(tv);
  } else if (level == SOL_SOCKET && optname == SO_LINGER) {
    // unix.setsockopt(fd:int, level:int, optname:int, secs:int, enabled:bool)
    //     ├─→ true
    //     └─→ nil, unix.Errno
    l.l_linger = luaL_checkinteger(L, 4);
    l.l_onoff = lua_toboolean(L, 5);
    optval = &l;
    optsize = sizeof(l);
  } else {
    goto NoProtocolOption;
  }
  return SysretBool(L, "setsockopt", olderr,
                    setsockopt(fd, level, optname, optval, optsize));
}

static int LuaUnixGetsockopt(lua_State *L) {
  char *p;
  uint32_t size;
  struct linger l;
  struct timeval tv;
  int fd, level, optname, optval, olderr = errno;
  fd = luaL_checkinteger(L, 1);
  level = luaL_checkinteger(L, 2);
  optname = luaL_checkinteger(L, 3);
  if (level == -1 || optname == 0) {
  NoProtocolOption:
    enoprotoopt();
    return LuaUnixSysretErrno(L, "setsockopt", olderr);
  }
  if (IsSockoptBool(level, optname) || IsSockoptInt(level, optname)) {
    // unix.getsockopt(fd:int, level:int, optname:int)
    //     ├─→ value:int
    //     └─→ nil, unix.Errno
    size = sizeof(optval);
    if (getsockopt(fd, level, optname, &optval, &size) != -1) {
      CheckOptvalsize(L, sizeof(optval), size);
      lua_pushinteger(L, optval);
      return 1;
    }
  } else if (IsSockoptTimeval(level, optname)) {
    // unix.getsockopt(fd:int, level:int, optname:int)
    //     ├─→ secs:int, nsecs:int
    //     └─→ nil, unix.Errno
    size = sizeof(tv);
    if (getsockopt(fd, level, optname, &tv, &size) != -1) {
      CheckOptvalsize(L, sizeof(tv), size);
      lua_pushinteger(L, tv.tv_sec);
      lua_pushinteger(L, tv.tv_usec * 1000);
      return 2;
    }
  } else if (level == SOL_SOCKET && optname == SO_LINGER) {
    // unix.getsockopt(fd:int, unix.SOL_SOCKET, unix.SO_LINGER)
    //     ├─→ seconds:int, enabled:bool
    //     └─→ nil, unix.Errno
    size = sizeof(l);
    if (getsockopt(fd, level, optname, &l, &size) != -1) {
      CheckOptvalsize(L, sizeof(l), size);
      lua_pushinteger(L, l.l_linger);
      lua_pushboolean(L, !!l.l_onoff);
      return 1;
    }
  } else if (level == SOL_TCP && optname == TCP_SAVED_SYN) {
    // unix.getsockopt(fd:int, unix.SOL_TCP, unix.SO_SAVED_SYN)
    //     ├─→ syn_packet_bytes:str
    //     └─→ nil, unix.Errno
    if ((p = malloc((size = 1500)))) {
      if (getsockopt(fd, level, optname, p, &size) != -1) {
        lua_pushlstring(L, p, size);
        free(p);
        return 1;
      }
      free(p);
    }
  } else {
    goto NoProtocolOption;
  }
  return LuaUnixSysretErrno(L, "getsockopt", olderr);
}

// unix.socket([family:int[, type:int[, protocol:int]]])
//     ├─→ fd:int
//     └─→ nil, unix.Errno
static int LuaUnixSocket(lua_State *L) {
  int olderr = errno;
  int family = luaL_optinteger(L, 1, AF_INET);
  return SysretInteger(L, "socket", olderr,
                       socket(family, luaL_optinteger(L, 2, SOCK_STREAM),
                              luaL_optinteger(L, 3, 0)));
}

// unix.socketpair([family:int[, type:int[, protocol:int]]])
//     ├─→ fd1:int, fd2:int
//     └─→ nil, unix.Errno
static int LuaUnixSocketpair(lua_State *L) {
  int sv[2], olderr = errno;
  if (!socketpair(luaL_optinteger(L, 1, AF_UNIX),
                  luaL_optinteger(L, 2, SOCK_STREAM), luaL_optinteger(L, 3, 0),
                  sv)) {
    lua_pushinteger(L, sv[0]);
    lua_pushinteger(L, sv[1]);
    return 2;
  } else {
    return LuaUnixSysretErrno(L, "socketpair", olderr);
  }
}

// unix.bind(fd:int[, ip:uint32, port:uint16])
// unix.bind(fd:int[, unixpath:str])
//     ├─→ true
//     └─→ nil, unix.Errno
static int LuaUnixBind(lua_State *L) {
  uint32_t salen;
  struct sockaddr_storage ss;
  int olderr = errno;
  MakeSockaddr(L, 2, &ss, &salen);
  return SysretBool(
      L, "bind", olderr,
      bind(luaL_checkinteger(L, 1), (struct sockaddr *)&ss, salen));
}

// unix.connect(fd:int, ip:uint32, port:uint16)
// unix.connect(fd:int, unixpath:str)
//     ├─→ true
//     └─→ nil, unix.Errno
static int LuaUnixConnect(lua_State *L) {
  uint32_t salen;
  struct sockaddr_storage ss;
  int olderr = errno;
  MakeSockaddr(L, 2, &ss, &salen);
  return SysretBool(
      L, "connect", olderr,
      connect(luaL_checkinteger(L, 1), (struct sockaddr *)&ss, salen));
}

// unix.listen(fd:int[, backlog:int])
//     ├─→ true
//     └─→ nil, unix.Errno
static int LuaUnixListen(lua_State *L) {
  int olderr = errno;
  return SysretBool(L, "listen", olderr,
                    listen(luaL_checkinteger(L, 1), luaL_optinteger(L, 2, 10)));
}

static int LuaUnixGetname(lua_State *L, const char *name,
                          int func(int, struct sockaddr *, uint32_t *)) {
  int olderr;
  uint32_t addrsize;
  struct sockaddr_storage ss = {0};
  olderr = errno;
  addrsize = sizeof(ss) - 1;
  if (!func(luaL_checkinteger(L, 1), (struct sockaddr *)&ss, &addrsize)) {
    return PushSockaddr(L, &ss);
  } else {
    return LuaUnixSysretErrno(L, name, olderr);
  }
}

// unix.getsockname(fd:int)
//     ├─→ ip:uint32, port:uint16
//     ├─→ unixpath:str
//     └─→ nil, unix.Errno
static int LuaUnixGetsockname(lua_State *L) {
  return LuaUnixGetname(L, "getsockname", getsockname);
}

// unix.getpeername(fd:int)
//     ├─→ ip:uint32, port:uint16
//     ├─→ unixpath:str
//     └─→ nil, unix.Errno
static int LuaUnixGetpeername(lua_State *L) {
  return LuaUnixGetname(L, "getpeername", getpeername);
}

// unix.siocgifconf()
//     ├─→ {{name:str,ip:uint32,netmask:uint32}, ...}
//     └─→ nil, unix.Errno
static int LuaUnixSiocgifconf(lua_State *L) {
  size_t n;
  char *data;
  int i, fd, olderr;
  struct ifreq *ifr;
  struct ifconf conf;
  olderr = errno;
  data = LuaAllocOrDie(L, (n = 4096));
  if ((fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)) == -1) {
    free(data);
    return LuaUnixSysretErrno(L, "siocgifconf", olderr);
  }
  conf.ifc_buf = data;
  conf.ifc_len = n;
  if (ioctl(fd, SIOCGIFCONF, &conf) == -1) {
    close(fd);
    free(data);
    return LuaUnixSysretErrno(L, "siocgifconf", olderr);
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

// sandbox.pledge([promises:str[, execpromises:str[, mode:int]]])
//     ├─→ true
//     └─→ nil, unix.Errno
static int LuaUnixPledge(lua_State *L) {
  int olderr = errno;
  __pledge_mode = luaL_optinteger(L, 3, 0);
  return SysretBool(L, "pledge", olderr,
                    pledge(luaL_optstring(L, 1, 0), luaL_optstring(L, 2, 0)));
}

// sandbox.unveil([path:str[, permissions:str]])
//     ├─→ true
//     └─→ nil, unix.Errno
static int LuaUnixUnveil(lua_State *L) {
  int olderr = errno;
  return SysretBool(L, "unveil", olderr,
                    unveil(luaL_optstring(L, 1, 0), luaL_optstring(L, 2, 0)));
}

// unix.gethostname()
//     ├─→ host:str
//     └─→ nil, unix.Errno
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
    }
  }
  return LuaUnixSysretErrno(L, "gethostname", olderr);
}

// unix.accept(serverfd:int[, flags:int])
//     ├─→ clientfd:int, ip:uint32, port:uint16
//     ├─→ clientfd:int, unixpath:str
//     └─→ nil, unix.Errno
static int LuaUnixAccept(lua_State *L) {
  uint32_t addrsize;
  struct sockaddr_storage ss;
  int clientfd, serverfd, olderr, flags;
  olderr = errno;
  addrsize = sizeof(ss);
  serverfd = luaL_checkinteger(L, 1);
  flags = luaL_optinteger(L, 2, 0);
  clientfd = accept4(serverfd, (struct sockaddr *)&ss, &addrsize, flags);
  if (clientfd != -1) {
    lua_pushinteger(L, clientfd);
    return 1 + PushSockaddr(L, &ss);
  } else {
    return LuaUnixSysretErrno(L, "accept", olderr);
  }
}

// unix.poll({[fd:int]=events:int, ...}[, timeoutms:int[, mask:unix.Sigset]])
//     ├─→ {[fd:int]=revents:int, ...}
//     └─→ nil, unix.Errno
static int LuaUnixPoll(lua_State *L) {
  size_t nfds;
  sigset_t *mask;
  struct timespec ts, *tsp;
  struct pollfd *fds, *fds2;
  int i, events, olderr = errno;
  luaL_checktype(L, 1, LUA_TTABLE);
  if (!lua_isnoneornil(L, 2)) {
    ts = timespec_frommillis(luaL_checkinteger(L, 2));
    tsp = &ts;
  } else {
    tsp = 0;
  }
  if (!lua_isnoneornil(L, 3)) {
    mask = luaL_checkudata(L, 3, "unix.Sigset");
  } else {
    mask = 0;
  }
  lua_pushnil(L);
  for (fds = 0, nfds = 0; lua_next(L, 1);) {
    if (lua_isinteger(L, -2)) {
      if ((fds2 = LuaRealloc(L, fds, (nfds + 1) * sizeof(*fds)))) {
        fds2[nfds].fd = lua_tointeger(L, -2);
        fds2[nfds].events = lua_tointeger(L, -1);
        fds = fds2;
        ++nfds;
      } else {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuse-after-free"
        free(fds);
#pragma GCC diagnostic pop
        return LuaUnixSysretErrno(L, "poll", olderr);
      }
    } else {
      // ignore non-integer key
    }
    lua_pop(L, 1);
  }
  olderr = errno;
  if ((events = ppoll(fds, nfds, tsp, mask)) != -1) {
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
    return LuaUnixSysretErrno(L, "poll", olderr);
  }
}

// unix.recvfrom(fd:int[, bufsiz:int[, flags:int]])
//     ├─→ data:str, ip:uint32, port:uint16
//     ├─→ data:str, unixpath:str
//     └─→ nil, unix.Errno
static int LuaUnixRecvfrom(lua_State *L) {
  char *buf;
  size_t got;
  ssize_t rc;
  uint32_t addrsize;
  lua_Integer bufsiz;
  struct sockaddr_storage ss;
  int fd, flags, pushed, olderr = errno;
  addrsize = sizeof(ss);
  fd = luaL_checkinteger(L, 1);
  bufsiz = luaL_optinteger(L, 2, 1500);
  bufsiz = MIN(bufsiz, 0x7ffff000);
  flags = luaL_optinteger(L, 3, 0);
  buf = LuaAllocOrDie(L, bufsiz);
  if ((rc = recvfrom(fd, buf, bufsiz, flags, (struct sockaddr *)&ss,
                     &addrsize)) != -1) {
    got = rc;
    lua_pushlstring(L, buf, got);
    pushed = 1 + PushSockaddr(L, &ss);
    free(buf);
    return pushed;
  } else {
    free(buf);
    return LuaUnixSysretErrno(L, "recvfrom", olderr);
  }
}

// unix.recv(fd:int[, bufsiz:int[, flags:int]])
//     ├─→ data:str
//     └─→ nil, unix.Errno
static int LuaUnixRecv(lua_State *L) {
  char *buf;
  size_t got;
  ssize_t rc;
  lua_Integer bufsiz;
  int fd, flags, olderr = errno;
  fd = luaL_checkinteger(L, 1);
  bufsiz = luaL_optinteger(L, 2, 1500);
  bufsiz = MIN(bufsiz, 0x7ffff000);
  flags = luaL_optinteger(L, 3, 0);
  buf = LuaAllocOrDie(L, bufsiz);
  rc = recv(fd, buf, bufsiz, flags);
  if (rc != -1) {
    got = rc;
    lua_pushlstring(L, buf, got);
    free(buf);
    return 1;
  } else {
    free(buf);
    return LuaUnixSysretErrno(L, "recv", olderr);
  }
}

// unix.send(fd:int, data:str[, flags:int])
//     ├─→ sent:int
//     └─→ nil, unix.Errno
static int LuaUnixSend(lua_State *L) {
  size_t size;
  const char *data;
  int fd, flags, olderr = errno;
  fd = luaL_checkinteger(L, 1);
  data = luaL_checklstring(L, 2, &size);
  flags = luaL_optinteger(L, 3, 0);
  return SysretInteger(L, "send", olderr, send(fd, data, size, flags));
}

// unix.sendto(fd:int, data:str, ip:uint32, port:uint16[, flags:int])
// unix.sendto(fd:int, data:str, unixpath:str[, flags:int])
//     ├─→ sent:int
//     └─→ nil, unix.Errno
static int LuaUnixSendto(lua_State *L) {
  size_t size;
  uint32_t salen;
  const char *data;
  struct sockaddr_storage ss;
  int i, fd, flags, olderr = errno;
  fd = luaL_checkinteger(L, 1);
  data = luaL_checklstring(L, 2, &size);
  i = MakeSockaddr(L, 3, &ss, &salen);
  flags = luaL_optinteger(L, i, 0);
  return SysretInteger(
      L, "sendto", olderr,
      sendto(fd, data, size, flags, (struct sockaddr *)&ss, salen));
}

// unix.shutdown(fd:int, how:int)
//     ├─→ true
//     └─→ nil, unix.Errno
static int LuaUnixShutdown(lua_State *L) {
  int olderr = errno;
  return SysretBool(L, "shutdown", olderr,
                    shutdown(luaL_checkinteger(L, 1), luaL_checkinteger(L, 2)));
}

// unix.sigprocmask(how:int, newmask:unix.Sigset)
//     ├─→ oldmask:unix.Sigset
//     └─→ nil, unix.Errno
static int LuaUnixSigprocmask(lua_State *L) {
  sigset_t oldmask;
  int olderr = errno;
  if (!sigprocmask(luaL_checkinteger(L, 1),
                   luaL_checkudata(L, 2, "unix.Sigset"), &oldmask)) {
    LuaPushSigset(L, oldmask);
    return 1;
  } else {
    return LuaUnixSysretErrno(L, "sigprocmask", olderr);
  }
}

static void LuaUnixOnSignal(int sig, siginfo_t *si, void *ctx) {
  int type;
  lua_State *L = GL;
  STRACE("LuaUnixOnSignal(%G)", sig);
  lua_getglobal(L, "__signal_handlers");
  type = lua_rawgeti(L, -1, sig);
  lua_remove(L, -2);  // pop __signal_handlers
  if (type == LUA_TFUNCTION) {
    lua_pushinteger(L, sig);
    if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
      ERRORF("(lua) %s failed: %s", strsignal(sig), lua_tostring(L, -1));
      lua_pop(L, 1);  // pop error
    }
  } else {
    lua_pop(L, 1);  // pop handler
  }
}

// unix.sigaction(sig:int[, handler:func|int[, flags:int[, mask:unix.Sigset]]])
//     ├─→ oldhandler:func|int, flags:int, mask:unix.Sigset
//     └─→ nil, unix.Errno
static int LuaUnixSigaction(lua_State *L) {
  sigset_t *mask;
  int i, n, sig, olderr = errno;
  struct sigaction sa, oldsa, *saptr = &sa;
  sigemptyset(&sa.sa_mask);
  sig = luaL_checkinteger(L, 1);
  if (!(1 <= sig && sig <= NSIG)) {
    luaL_argerror(L, 1, "signal number invalid");
    __builtin_unreachable();
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
    __builtin_unreachable();
  }
  if (!lua_isnoneornil(L, 4)) {
    mask = luaL_checkudata(L, 4, "unix.Sigset");
    sigorset(&sa.sa_mask, &sa.sa_mask, mask);
    lua_remove(L, 4);
  }
  if (lua_isnoneornil(L, 3)) {
    sa.sa_flags = 0;
  } else {
    sa.sa_flags = lua_tointeger(L, 3);
    lua_remove(L, 3);
  }
  if (!sigaction(sig, saptr, &oldsa)) {
    lua_getglobal(L, "__signal_handlers");
    // push the old handler result to stack. if the global lua handler
    // table has a real function, then we prefer to return that. if it's
    // absent or a raw integer value, then we're better off returning
    // what the kernel gave us in &oldsa.
    if (lua_rawgeti(L, -1, sig) != LUA_TFUNCTION) {
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
      lua_rawseti(L, -3, sig);
    }
    // remove the signal handler table from stack
    lua_remove(L, -2);
    // finish pushing the last 2/3 results
    lua_pushinteger(L, oldsa.sa_flags);
    LuaPushSigset(L, oldsa.sa_mask);
    return 3;
  } else {
    return LuaUnixSysretErrno(L, "sigaction", olderr);
  }
}

// unix.sigsuspend([mask:Sigmask])
//     └─→ nil, unix.Errno
static int LuaUnixSigsuspend(lua_State *L) {
  int olderr = errno;
  sigsuspend(!lua_isnoneornil(L, 1) ? luaL_checkudata(L, 1, "unix.Sigset") : 0);
  return LuaUnixSysretErrno(L, "sigsuspend", olderr);
}

// unix.sigpending()
//     ├─→ mask:unix.Sigset
//     └─→ nil, unix.Errno
static int LuaUnixSigpending(lua_State *L) {
  sigset_t mask;
  int olderr = errno;
  if (!sigpending(&mask)) {
    LuaPushSigset(L, mask);
    return 1;
  } else {
    return LuaUnixSysretErrno(L, "sigpending", olderr);
  }
}

// unix.setitimer(which[, intervalsec, intns, valuesec, valuens])
//     ├─→ intervalsec:int, intervalns:int, valuesec:int, valuens:int
//     └─→ nil, unix.Errno
static int LuaUnixSetitimer(lua_State *L) {
  int which, olderr = errno;
  struct itimerval it, oldit, *itptr;
  which = luaL_checkinteger(L, 1);
  if (!lua_isnoneornil(L, 2)) {
    itptr = &it;
    it.it_interval.tv_sec = luaL_optinteger(L, 2, 0);
    it.it_interval.tv_usec = luaL_optinteger(L, 3, 0) / 1000;
    it.it_value.tv_sec = luaL_optinteger(L, 4, 0);
    it.it_value.tv_usec = luaL_optinteger(L, 5, 0) / 1000;
  } else {
    itptr = 0;
  }
  if (!setitimer(which, itptr, &oldit)) {
    lua_pushinteger(L, oldit.it_interval.tv_sec);
    lua_pushinteger(L, oldit.it_interval.tv_usec * 1000);
    lua_pushinteger(L, oldit.it_value.tv_sec);
    lua_pushinteger(L, oldit.it_value.tv_usec * 1000);
    return 4;
  } else {
    return LuaUnixSysretErrno(L, "setitimer", olderr);
  }
}

static int LuaUnixStr(lua_State *L, char *f(int)) {
  return ReturnString(L, f(luaL_checkinteger(L, 1)));
}

// unix.strsignal(sig:int)
//     └─→ symbol:str
static int LuaUnixStrsignal(lua_State *L) {
  return LuaUnixStr(L, strsignal);
}

// unix.WIFEXITED(wstatus)
//     └─→ bool
static int LuaUnixWifexited(lua_State *L) {
  return ReturnBoolean(L, !!WIFEXITED(luaL_checkinteger(L, 1)));
}

// unix.WEXITSTATUS(wstatus)
//     └─→ exitcode:uint8
static int LuaUnixWexitstatus(lua_State *L) {
  return ReturnInteger(L, WEXITSTATUS(luaL_checkinteger(L, 1)));
}

// unix.WIFSIGNALED(wstatus)
//     └─→ bool
static int LuaUnixWifsignaled(lua_State *L) {
  return ReturnBoolean(L, !!WIFSIGNALED(luaL_checkinteger(L, 1)));
}

// unix.WTERMSIG(wstatus)
//     └─→ sig:uint8
static int LuaUnixWtermsig(lua_State *L) {
  return ReturnInteger(L, WTERMSIG(luaL_checkinteger(L, 1)));
}

static dontinline int LuaUnixTime(lua_State *L, const char *call,
                                  struct tm *f(const time_t *, struct tm *)) {
  int64_t ts;
  struct tm tm;
  int olderr = errno;
  ts = luaL_checkinteger(L, 1);
  if (f(&ts, &tm)) {
    lua_pushinteger(L, tm.tm_year + 1900);
    lua_pushinteger(L, tm.tm_mon + 1);  // 1 ≤ mon  ≤ 12
    lua_pushinteger(L, tm.tm_mday);     // 1 ≤ mday ≤ 31
    lua_pushinteger(L, tm.tm_hour);     // 0 ≤ hour ≤ 23
    lua_pushinteger(L, tm.tm_min);      // 0 ≤ min  ≤ 59
    lua_pushinteger(L, tm.tm_sec);      // 0 ≤ sec  ≤ 60
    lua_pushinteger(L, tm.tm_gmtoff);   // ±93600 seconds
    lua_pushinteger(L, tm.tm_wday);     // 0 ≤ wday ≤ 6
    lua_pushinteger(L, tm.tm_yday);     // 0 ≤ yday ≤ 365
    lua_pushinteger(L, tm.tm_isdst);    // daylight savings
    lua_pushstring(L, tm.tm_zone);
    return 11;
  } else {
    return LuaUnixSysretErrno(L, call, olderr);
  }
}

// unix.gmtime(unixsecs:int)
//     ├─→ year,mon,mday,hour,min,sec,gmtoffsec,wday,yday,dst:int,zone:str
//     └─→ nil,unix.Errno
static int LuaUnixGmtime(lua_State *L) {
  return LuaUnixTime(L, "gmtime", gmtime_r);
}

// unix.localtime(unixts:int)
//     ├─→ year,mon,mday,hour,min,sec,gmtoffsec,wday,yday,dst:int,zone:str
//     └─→ nil,unix.Errno
static int LuaUnixLocaltime(lua_State *L) {
  return LuaUnixTime(L, "localtime", localtime_r);
}

// unix.major(rdev:int)
//     └─→ major:int
static int LuaUnixMajor(lua_State *L) {
  return ReturnInteger(L, major(luaL_checkinteger(L, 1)));
}

// unix.minor(rdev:int)
//     └─→ minor:int
static int LuaUnixMinor(lua_State *L) {
  return ReturnInteger(L, minor(luaL_checkinteger(L, 1)));
}

// unix.S_ISDIR(mode:int)
//     └─→ bool
static int LuaUnixSisdir(lua_State *L) {
  lua_pushboolean(L, S_ISDIR(luaL_checkinteger(L, 1)));
  return 1;
}

// unix.S_ISCHR(mode:int)
//     └─→ bool
static int LuaUnixSischr(lua_State *L) {
  lua_pushboolean(L, S_ISCHR(luaL_checkinteger(L, 1)));
  return 1;
}

// unix.S_ISBLK(mode:int)
//     └─→ bool
static int LuaUnixSisblk(lua_State *L) {
  lua_pushboolean(L, S_ISBLK(luaL_checkinteger(L, 1)));
  return 1;
}

// unix.S_ISREG(mode:int)
//     └─→ bool
static int LuaUnixSisreg(lua_State *L) {
  lua_pushboolean(L, S_ISREG(luaL_checkinteger(L, 1)));
  return 1;
}

// unix.S_ISFIFO(mode:int)
//     └─→ bool
static int LuaUnixSisfifo(lua_State *L) {
  lua_pushboolean(L, S_ISFIFO(luaL_checkinteger(L, 1)));
  return 1;
}

// unix.S_ISLNK(mode:int)
//     └─→ bool
static int LuaUnixSislnk(lua_State *L) {
  lua_pushboolean(L, S_ISLNK(luaL_checkinteger(L, 1)));
  return 1;
}

// unix.S_ISSOCK(mode:int)
//     └─→ bool
static int LuaUnixSissock(lua_State *L) {
  lua_pushboolean(L, S_ISSOCK(luaL_checkinteger(L, 1)));
  return 1;
}

// unix.isatty(fd:int)
//     ├─→ true
//     └─→ nil, unix.Errno
static int LuaUnixIsatty(lua_State *L) {
  int olderr = errno;
  return SysretBool(L, "isatty", olderr, isatty(luaL_checkinteger(L, 1)));
}

// unix.tiocgwinsz(fd:int)
//     ├─→ rows:int, cols:int
//     └─→ nil, unix.Errno
static int LuaUnixTiocgwinsz(lua_State *L) {
  struct winsize ws;
  int olderr = errno;
  if (!ioctl(luaL_checkinteger(L, 1), TIOCGWINSZ, &ws)) {
    lua_pushinteger(L, ws.ws_row);
    lua_pushinteger(L, ws.ws_col);
    return 2;
  } else {
    return LuaUnixSysretErrno(L, "tiocgwinsz", olderr);
  }
}

// unix.sched_yield()
static int LuaUnixSchedYield(lua_State *L) {
  pthread_yield();
  return 0;
}

// unix.verynice()
static int LuaUnixVerynice(lua_State *L) {
  verynice();
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// UNIX module

static const luaL_Reg kLuaUnix[] = {
    {"S_ISBLK", LuaUnixSisblk},           // is st:mode() a block device?
    {"S_ISCHR", LuaUnixSischr},           // is st:mode() a character device?
    {"S_ISDIR", LuaUnixSisdir},           // is st:mode() a directory?
    {"S_ISFIFO", LuaUnixSisfifo},         // is st:mode() a fifo?
    {"S_ISLNK", LuaUnixSislnk},           // is st:mode() a symbolic link?
    {"S_ISREG", LuaUnixSisreg},           // is st:mode() a regular file?
    {"S_ISSOCK", LuaUnixSissock},         // is st:mode() a socket?
    {"Sigset", LuaUnixSigset},            // creates signal bitmask
    {"WEXITSTATUS", LuaUnixWexitstatus},  // gets exit status from wait status
    {"WIFEXITED", LuaUnixWifexited},      // gets exit code from wait status
    {"WIFSIGNALED", LuaUnixWifsignaled},  // determines if died due to signal
    {"WTERMSIG", LuaUnixWtermsig},        // gets the signal code
    {"accept", LuaUnixAccept},            // create client fd for client
    {"access", LuaUnixAccess},            // check my file authorization
    {"bind", LuaUnixBind},                // reserve network interface address
    {"chdir", LuaUnixChdir},              // change directory
    {"chmod", LuaUnixChmod},              // change mode of file
    {"chown", LuaUnixChown},              // change owner of file
    {"chroot", LuaUnixChroot},            // change root directory
    {"clock_gettime", LuaUnixGettime},    // get timestamp w/ nano precision
    {"close", LuaUnixClose},              // close file or socket
    {"commandv", LuaUnixCommandv},        // resolve program on $PATH
    {"connect", LuaUnixConnect},          // connect to remote address
    {"daemon", LuaUnixDaemon},            // daemonize process
    {"dup", LuaUnixDup},                  // copy fd to lowest empty slot
    {"environ", LuaUnixEnviron},          // get environment variables
    {"execve", LuaUnixExecve},            // replace process with program
    {"execvp", LuaUnixExecvp},            // exec program using PATH
    {"execvpe", LuaUnixExecvpe},          // exec program using PATH with env
    {"exit", LuaUnixExit},                // exit w/o atexit
    {"fcntl", LuaUnixFcntl},              // manipulate file descriptor
    {"fdatasync", LuaUnixFdatasync},      // flush open file w/o metadata
    {"fdopendir", LuaUnixFdopendir},      // read directory entry list
    {"fexecve", LuaUnixFexecve},          // exec program from fd
    {"fork", LuaUnixFork},                // make child process via mitosis
    {"fstat", LuaUnixFstat},              // get file info from fd
    {"fstatfs", LuaUnixFstatfs},          // get filesystem info from fd
    {"fsync", LuaUnixFsync},              // flush open file
    {"ftruncate", LuaUnixFtruncate},      // shrink or extend file medium
    {"futimens", LuaUnixFutimens},        // change access/modified time
    {"getcwd", LuaUnixGetcwd},            // get current directory
    {"getegid", LuaUnixGetegid},          // get effective group id of process
    {"geteuid", LuaUnixGeteuid},          // get effective user id of process
    {"getgid", LuaUnixGetgid},            // get real group id of process
    {"gethostname", LuaUnixGethostname},  // get hostname of this machine
    {"getpeername", LuaUnixGetpeername},  // get address of remote end
    {"getpgid", LuaUnixGetpgid},          // get process group id of pid
    {"getpgrp", LuaUnixGetpgrp},          // get process group id
    {"getpid", LuaUnixGetpid},            // get id of this process
    {"getppid", LuaUnixGetppid},          // get parent process id
    {"getpriority", LuaUnixGetpriority},  // get process priority
    {"getrlimit", LuaUnixGetrlimit},      // query resource limits
    {"getrusage", LuaUnixGetrusage},      // query resource usages
    {"getsid", LuaUnixGetsid},            // get session id of pid
    {"getsockname", LuaUnixGetsockname},  // get address of local end
    {"getsockopt", LuaUnixGetsockopt},    // get socket tunings
    {"getuid", LuaUnixGetuid},            // get real user id of process
    {"gmtime", LuaUnixGmtime},            // destructure unix timestamp
    {"isatty", LuaUnixIsatty},            // detects pseudoteletypewriters
    {"kill", LuaUnixKill},                // signal child process
    {"killpg", LuaUnixKillpg},            // signal process group
    {"link", LuaUnixLink},                // create hard link
    {"listen", LuaUnixListen},            // begin listening for clients
    {"localtime", LuaUnixLocaltime},      // localize unix timestamp
    {"lseek", LuaUnixLseek},              // seek in file
    {"major", LuaUnixMajor},              // extract device info
    {"makedirs", LuaUnixMakedirs},        // make directory and parents too
    {"mapshared", LuaUnixMapshared},      // mmap(MAP_SHARED) w/ mutex+atomics
    {"minor", LuaUnixMinor},              // extract device info
    {"mkdir", LuaUnixMkdir},              // make directory
    {"mkdtemp", LuaUnixMkdtemp},          // create temporary directory
    {"mkstemp", LuaUnixMkstemp},          // create temporary file
    {"nanosleep", LuaUnixNanosleep},      // sleep w/ nano precision
    {"nice", LuaUnixNice},                // adjust process priority
    {"open", LuaUnixOpen},                // open file fd at lowest slot
    {"opendir", LuaUnixOpendir},          // read directory entry list
    {"pipe", LuaUnixPipe},                // create two anon fifo fds
    {"pledge", LuaUnixPledge},            // enables syscall sandbox
    {"poll", LuaUnixPoll},                // waits for file descriptor events
    {"raise", LuaUnixRaise},              // signal this process
    {"read", LuaUnixRead},                // read from file or socket
    {"readlink", LuaUnixReadlink},        // reads symbolic link
    {"realpath", LuaUnixRealpath},        // abspath without dots/symlinks
    {"recv", LuaUnixRecv},                // receive tcp from some address
    {"recvfrom", LuaUnixRecvfrom},        // receive udp from some address
    {"rename", LuaUnixRename},            // rename file or directory
    {"rmdir", LuaUnixRmdir},              // remove empty directory
    {"rmrf", LuaUnixRmrf},                // remove file recursively
    {"sched_yield", LuaUnixSchedYield},   // relinquish scheduled quantum
    {"send", LuaUnixSend},                // send tcp to some address
    {"sendto", LuaUnixSendto},            // send udp to some address
    {"setfsgid", LuaUnixSetfsgid},        // set/get group id for fs ops
    {"setfsuid", LuaUnixSetfsuid},        // set/get user id for fs ops
    {"setgid", LuaUnixSetgid},            // set real group id of process
    {"setitimer", LuaUnixSetitimer},      // set alarm clock
    {"setpgid", LuaUnixSetpgid},          // set process group id for pid
    {"setpgrp", LuaUnixSetpgrp},          // sets process group id
    {"setpriority", LuaUnixSetpriority},  // set process priority
    {"setresgid", LuaUnixSetresgid},      // sets real/effective/saved gids
    {"setresuid", LuaUnixSetresuid},      // sets real/effective/saved uids
    {"setrlimit", LuaUnixSetrlimit},      // prevent cpu memory bombs
    {"setsid", LuaUnixSetsid},            // create a new session id
    {"setsockopt", LuaUnixSetsockopt},    // tune socket options
    {"setuid", LuaUnixSetuid},            // set real user id of process
    {"shutdown", LuaUnixShutdown},        // make socket half empty or full
    {"sigaction", LuaUnixSigaction},      // install signal handler
    {"sigpending", LuaUnixSigpending},    // get pending signals
    {"sigprocmask", LuaUnixSigprocmask},  // change signal mask
    {"sigsuspend", LuaUnixSigsuspend},    // wait for signal
    {"siocgifconf", LuaUnixSiocgifconf},  // get list of network interfaces
    {"socket", LuaUnixSocket},            // create network communication fd
    {"socketpair", LuaUnixSocketpair},    // create bidirectional pipe
    {"spawn", LuaUnixSpawn},              // spawn process
    {"spawnp", LuaUnixSpawnp},            // spawn process using PATH
    {"stat", LuaUnixStat},                // get file info from path
    {"statfs", LuaUnixStatfs},            // get filesystem info from path
    {"strsignal", LuaUnixStrsignal},      // turn signal into string
    {"symlink", LuaUnixSymlink},          // create symbolic link
    {"sync", LuaUnixSync},                // flushes files and disks
    {"syslog", LuaUnixSyslog},            // logs to system log
    {"tiocgwinsz", LuaUnixTiocgwinsz},    // pseudoteletypewriter dimensions
    {"tmpfd", LuaUnixTmpfd},              // create anonymous file
    {"truncate", LuaUnixTruncate},        // shrink or extend file medium
    {"umask", LuaUnixUmask},              // set default file mask
    {"unlink", LuaUnixUnlink},            // remove file
    {"unveil", LuaUnixUnveil},            // filesystem sandboxing
    {"utimensat", LuaUnixUtimensat},      // change access/modified time
    {"verynice", LuaUnixVerynice},        // lowest priority
    {"wait", LuaUnixWait},                // wait for child to change status
    {"write", LuaUnixWrite},              // write to file or socket
    {0},                                  //
};

static void LoadMagnums(lua_State *L, const struct MagnumStr *ms,
                        const char *pfx) {
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
  LuaUnixSigsetObj(L);
  LuaUnixRusageObj(L);
  LuaUnixStatfsObj(L);
  LuaUnixMemoryObj(L);
  LuaUnixErrnoObj(L);
  LuaUnixStatObj(L);
  LuaUnixDirObj(L);
  lua_newtable(L);
  lua_setglobal(L, "__signal_handlers");

  LoadMagnums(L, kIpOptnames, "IP_");
  LoadMagnums(L, kTcpOptnames, "TCP_");
  LoadMagnums(L, kSockOptnames, "SO_");
  LoadMagnums(L, kClockNames, "CLOCK_");

  // errno
  LuaSetIntField(L, "EPERM", EPERM);
  LuaSetIntField(L, "ENOENT", ENOENT);
  LuaSetIntField(L, "ESRCH", ESRCH);
  LuaSetIntField(L, "EINTR", EINTR);
  LuaSetIntField(L, "EIO", EIO);
  LuaSetIntField(L, "ENXIO", ENXIO);
  LuaSetIntField(L, "E2BIG", E2BIG);
  LuaSetIntField(L, "ENOEXEC", ENOEXEC);
  LuaSetIntField(L, "EBADF", EBADF);
  LuaSetIntField(L, "ECHILD", ECHILD);
  LuaSetIntField(L, "EAGAIN", EAGAIN);
  LuaSetIntField(L, "ENOMEM", ENOMEM);
  LuaSetIntField(L, "EACCES", EACCES);
  LuaSetIntField(L, "EFAULT", EFAULT);
  LuaSetIntField(L, "ENOTBLK", ENOTBLK);
  LuaSetIntField(L, "EBUSY", EBUSY);
  LuaSetIntField(L, "EEXIST", EEXIST);
  LuaSetIntField(L, "EXDEV", EXDEV);
  LuaSetIntField(L, "ENODEV", ENODEV);
  LuaSetIntField(L, "ENOTDIR", ENOTDIR);
  LuaSetIntField(L, "EISDIR", EISDIR);
  LuaSetIntField(L, "EINVAL", EINVAL);
  LuaSetIntField(L, "ENFILE", ENFILE);
  LuaSetIntField(L, "EMFILE", EMFILE);
  LuaSetIntField(L, "ENOTTY", ENOTTY);
  LuaSetIntField(L, "ETXTBSY", ETXTBSY);
  LuaSetIntField(L, "EFBIG", EFBIG);
  LuaSetIntField(L, "ENOSPC", ENOSPC);
  LuaSetIntField(L, "ESPIPE", ESPIPE);
  LuaSetIntField(L, "EROFS", EROFS);
  LuaSetIntField(L, "EMLINK", EMLINK);
  LuaSetIntField(L, "EPIPE", EPIPE);
  LuaSetIntField(L, "EDOM", EDOM);
  LuaSetIntField(L, "ERANGE", ERANGE);
  LuaSetIntField(L, "EDEADLK", EDEADLK);
  LuaSetIntField(L, "ENAMETOOLONG", ENAMETOOLONG);
  LuaSetIntField(L, "ENOLCK", ENOLCK);
  LuaSetIntField(L, "ENOSYS", ENOSYS);
  LuaSetIntField(L, "ENOTEMPTY", ENOTEMPTY);
  LuaSetIntField(L, "ELOOP", ELOOP);
  LuaSetIntField(L, "ENOMSG", ENOMSG);
  LuaSetIntField(L, "EIDRM", EIDRM);
  LuaSetIntField(L, "ENOTSUP", ENOTSUP);
  LuaSetIntField(L, "ENOSTR", ENOSTR);
  LuaSetIntField(L, "ENODATA", ENODATA);
  LuaSetIntField(L, "ETIME", ETIME);
  LuaSetIntField(L, "ENOSR", ENOSR);
  LuaSetIntField(L, "ENONET", ENONET);
  LuaSetIntField(L, "EREMOTE", EREMOTE);
  LuaSetIntField(L, "ENOLINK", ENOLINK);
  LuaSetIntField(L, "EPROTO", EPROTO);
  LuaSetIntField(L, "EMULTIHOP", EMULTIHOP);
  LuaSetIntField(L, "EBADMSG", EBADMSG);
  LuaSetIntField(L, "EOVERFLOW", EOVERFLOW);
  LuaSetIntField(L, "EBADFD", EBADFD);
  LuaSetIntField(L, "EFTYPE", EFTYPE);
  LuaSetIntField(L, "EILSEQ", EILSEQ);
  LuaSetIntField(L, "ERESTART", ERESTART);
  LuaSetIntField(L, "EUSERS", EUSERS);
  LuaSetIntField(L, "ENOTSOCK", ENOTSOCK);
  LuaSetIntField(L, "EDESTADDRREQ", EDESTADDRREQ);
  LuaSetIntField(L, "EMSGSIZE", EMSGSIZE);
  LuaSetIntField(L, "EPROTOTYPE", EPROTOTYPE);
  LuaSetIntField(L, "ENOPROTOOPT", ENOPROTOOPT);
  LuaSetIntField(L, "EPROTONOSUPPORT", EPROTONOSUPPORT);
  LuaSetIntField(L, "ESOCKTNOSUPPORT", ESOCKTNOSUPPORT);
  LuaSetIntField(L, "EOPNOTSUPP", EOPNOTSUPP);
  LuaSetIntField(L, "EPFNOSUPPORT", EPFNOSUPPORT);
  LuaSetIntField(L, "EAFNOSUPPORT", EAFNOSUPPORT);
  LuaSetIntField(L, "EADDRINUSE", EADDRINUSE);
  LuaSetIntField(L, "EADDRNOTAVAIL", EADDRNOTAVAIL);
  LuaSetIntField(L, "ENETDOWN", ENETDOWN);
  LuaSetIntField(L, "ENETUNREACH", ENETUNREACH);
  LuaSetIntField(L, "ENETRESET", ENETRESET);
  LuaSetIntField(L, "ECONNABORTED", ECONNABORTED);
  LuaSetIntField(L, "ECONNRESET", ECONNRESET);
  LuaSetIntField(L, "ENOBUFS", ENOBUFS);
  LuaSetIntField(L, "EISCONN", EISCONN);
  LuaSetIntField(L, "ENOTCONN", ENOTCONN);
  LuaSetIntField(L, "ESHUTDOWN", ESHUTDOWN);
  LuaSetIntField(L, "ETOOMANYREFS", ETOOMANYREFS);
  LuaSetIntField(L, "ETIMEDOUT", ETIMEDOUT);
  LuaSetIntField(L, "ECONNREFUSED", ECONNREFUSED);
  LuaSetIntField(L, "EHOSTDOWN", EHOSTDOWN);
  LuaSetIntField(L, "EHOSTUNREACH", EHOSTUNREACH);
  LuaSetIntField(L, "EALREADY", EALREADY);
  LuaSetIntField(L, "EINPROGRESS", EINPROGRESS);
  LuaSetIntField(L, "ESTALE", ESTALE);
  LuaSetIntField(L, "EDQUOT", EDQUOT);
  LuaSetIntField(L, "ENOMEDIUM", ENOMEDIUM);
  LuaSetIntField(L, "EMEDIUMTYPE", EMEDIUMTYPE);
  LuaSetIntField(L, "ECANCELED", ECANCELED);
  LuaSetIntField(L, "EOWNERDEAD", EOWNERDEAD);
  LuaSetIntField(L, "ENOTRECOVERABLE", ENOTRECOVERABLE);
  LuaSetIntField(L, "ERFKILL", ERFKILL);
  LuaSetIntField(L, "EHWPOISON", EHWPOISON);

  // signals
  LuaSetIntField(L, "SIGHUP", SIGHUP);
  LuaSetIntField(L, "SIGINT", SIGINT);
  LuaSetIntField(L, "SIGQUIT", SIGQUIT);
  LuaSetIntField(L, "SIGILL", SIGILL);
  LuaSetIntField(L, "SIGTRAP", SIGTRAP);
  LuaSetIntField(L, "SIGABRT", SIGABRT);
  LuaSetIntField(L, "SIGBUS", SIGBUS);
  LuaSetIntField(L, "SIGFPE", SIGFPE);
  LuaSetIntField(L, "SIGKILL", SIGKILL);
  LuaSetIntField(L, "SIGUSR1", SIGUSR1);
  LuaSetIntField(L, "SIGSEGV", SIGSEGV);
  LuaSetIntField(L, "SIGUSR2", SIGUSR2);
  LuaSetIntField(L, "SIGPIPE", SIGPIPE);
  LuaSetIntField(L, "SIGALRM", SIGALRM);
  LuaSetIntField(L, "SIGTERM", SIGTERM);
  LuaSetIntField(L, "SIGCHLD", SIGCHLD);
  LuaSetIntField(L, "SIGCONT", SIGCONT);
  LuaSetIntField(L, "SIGSTOP", SIGSTOP);
  LuaSetIntField(L, "SIGTSTP", SIGTSTP);
  LuaSetIntField(L, "SIGTTIN", SIGTTIN);
  LuaSetIntField(L, "SIGTTOU", SIGTTOU);
  LuaSetIntField(L, "SIGURG", SIGURG);
  LuaSetIntField(L, "SIGXCPU", SIGXCPU);
  LuaSetIntField(L, "SIGXFSZ", SIGXFSZ);
  LuaSetIntField(L, "SIGVTALRM", SIGVTALRM);
  LuaSetIntField(L, "SIGPROF", SIGPROF);
  LuaSetIntField(L, "SIGWINCH", SIGWINCH);
  LuaSetIntField(L, "SIGSYS", SIGSYS);

  // open()
  LuaSetIntField(L, "O_RDONLY", O_RDONLY);
  LuaSetIntField(L, "O_WRONLY", O_WRONLY);
  LuaSetIntField(L, "O_RDWR", O_RDWR);
  LuaSetIntField(L, "O_ACCMODE", O_ACCMODE);
  LuaSetIntField(L, "O_CREAT", O_CREAT);
  LuaSetIntField(L, "O_EXCL", O_EXCL);
  LuaSetIntField(L, "O_NOCTTY", O_NOCTTY);
  LuaSetIntField(L, "O_TRUNC", O_TRUNC);
  LuaSetIntField(L, "O_APPEND", O_APPEND);
  LuaSetIntField(L, "O_NONBLOCK", O_NONBLOCK);
  LuaSetIntField(L, "O_SYNC", O_SYNC);
  LuaSetIntField(L, "O_DSYNC", O_DSYNC);
  LuaSetIntField(L, "O_DIRECT", O_DIRECT);
  LuaSetIntField(L, "O_DIRECTORY", O_DIRECTORY);
  LuaSetIntField(L, "O_NOFOLLOW", O_NOFOLLOW);
  LuaSetIntField(L, "O_NOATIME", O_NOATIME);
  LuaSetIntField(L, "O_CLOEXEC", O_CLOEXEC);
  LuaSetIntField(L, "O_UNLINK", O_UNLINK);

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
  LuaSetIntField(L, "F_GETLK", F_GETLK);
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

  // getpriority()/setpriority() which
  LuaSetIntField(L, "PRIO_PROCESS", PRIO_PROCESS);
  LuaSetIntField(L, "PRIO_PGRP", PRIO_PGRP);
  LuaSetIntField(L, "PRIO_USER", PRIO_USER);

  // wait() options
  LuaSetIntField(L, "WNOHANG", WNOHANG);
  LuaSetIntField(L, "WUNTRACED", WUNTRACED);
#ifdef WCONTINUED
  LuaSetIntField(L, "WCONTINUED", WCONTINUED);
#endif

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
  LuaSetIntField(L, "SOCK_NONBLOCK", SOCK_NONBLOCK);

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
  LuaSetIntField(L, "MSG_OOB", MSG_OOB);
  LuaSetIntField(L, "MSG_PEEK", MSG_PEEK);
  LuaSetIntField(L, "MSG_DONTROUTE", MSG_DONTROUTE);
  LuaSetIntField(L, "MSG_DONTWAIT", MSG_DONTWAIT);
  LuaSetIntField(L, "MSG_NOSIGNAL", MSG_NOSIGNAL);
  LuaSetIntField(L, "MSG_WAITALL", MSG_WAITALL);
  LuaSetIntField(L, "MSG_TRUNC", MSG_TRUNC);
  LuaSetIntField(L, "MSG_CTRUNC", MSG_CTRUNC);

  // readdir() type
  LuaSetIntField(L, "DT_UNKNOWN", DT_UNKNOWN);
  LuaSetIntField(L, "DT_REG", DT_REG);
  LuaSetIntField(L, "DT_DIR", DT_DIR);
  LuaSetIntField(L, "DT_BLK", DT_BLK);
  LuaSetIntField(L, "DT_LNK", DT_LNK);
  LuaSetIntField(L, "DT_CHR", DT_CHR);
  LuaSetIntField(L, "DT_FIFO", DT_FIFO);
  LuaSetIntField(L, "DT_SOCK", DT_SOCK);

  // poll() flags
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
  LuaSetIntField(L, "AT_EACCESS", AT_EACCESS);
  LuaSetIntField(L, "AT_SYMLINK_NOFOLLOW", AT_SYMLINK_NOFOLLOW);

  // sigprocmask() handlers
  LuaSetIntField(L, "SIG_BLOCK", SIG_BLOCK);
  LuaSetIntField(L, "SIG_UNBLOCK", SIG_UNBLOCK);
  LuaSetIntField(L, "SIG_SETMASK", SIG_SETMASK);

  // sigaction() handlers
  LuaSetIntField(L, "SIG_DFL", (intptr_t)SIG_DFL);
  LuaSetIntField(L, "SIG_IGN", (intptr_t)SIG_IGN);

  // utimensat() magnums
  LuaSetIntField(L, "UTIME_NOW", UTIME_NOW);
  LuaSetIntField(L, "UTIME_OMIT", UTIME_OMIT);

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

  // sigaction() flags
  LuaSetIntField(L, "SA_RESTART", SA_RESTART);
  LuaSetIntField(L, "SA_RESETHAND", SA_RESETHAND);
  LuaSetIntField(L, "SA_NODEFER", SA_NODEFER);
  LuaSetIntField(L, "SA_NOCLDWAIT", SA_NOCLDWAIT);
  LuaSetIntField(L, "SA_NOCLDSTOP", SA_NOCLDSTOP);

  // getrusage() who
  LuaSetIntField(L, "RUSAGE_SELF", RUSAGE_SELF);
  LuaSetIntField(L, "RUSAGE_THREAD", RUSAGE_THREAD);
  LuaSetIntField(L, "RUSAGE_CHILDREN", RUSAGE_CHILDREN);
  LuaSetIntField(L, "RUSAGE_BOTH", RUSAGE_BOTH);

  // more stuff
  LuaSetIntField(L, "ARG_MAX", __get_arg_max());
  LuaSetIntField(L, "BUFSIZ", BUFSIZ);
  LuaSetIntField(L, "CLK_TCK", CLK_TCK);
  LuaSetIntField(L, "NAME_MAX", _NAME_MAX);
  LuaSetIntField(L, "NSIG", _NSIG);
  LuaSetIntField(L, "PATH_MAX", _PATH_MAX);
  LuaSetIntField(L, "PIPE_BUF", PIPE_BUF);

  // pledge() flags
  LuaSetIntField(L, "PLEDGE_PENALTY_KILL_THREAD", PLEDGE_PENALTY_KILL_THREAD);
  LuaSetIntField(L, "PLEDGE_PENALTY_KILL_PROCESS", PLEDGE_PENALTY_KILL_PROCESS);
  LuaSetIntField(L, "PLEDGE_PENALTY_RETURN_EPERM", PLEDGE_PENALTY_RETURN_EPERM);
  LuaSetIntField(L, "PLEDGE_STDERR_LOGGING", PLEDGE_STDERR_LOGGING);

  // statfs::f_flags
  LuaSetIntField(L, "ST_RDONLY", ST_RDONLY);
  LuaSetIntField(L, "ST_NOSUID", ST_NOSUID);
  LuaSetIntField(L, "ST_NODEV", ST_NODEV);
  LuaSetIntField(L, "ST_NOEXEC", ST_NOEXEC);
  LuaSetIntField(L, "ST_SYNCHRONOUS", ST_SYNCHRONOUS);
  LuaSetIntField(L, "ST_NOATIME", ST_NOATIME);
  LuaSetIntField(L, "ST_RELATIME", ST_RELATIME);
  LuaSetIntField(L, "ST_APPEND", ST_APPEND);
  LuaSetIntField(L, "ST_IMMUTABLE", ST_IMMUTABLE);
  LuaSetIntField(L, "ST_MANDLOCK", ST_MANDLOCK);
  LuaSetIntField(L, "ST_NODIRATIME", ST_NODIRATIME);
  LuaSetIntField(L, "ST_WRITE", ST_WRITE);

  return 1;
}
