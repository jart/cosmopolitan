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
#include "libc/calls/sigbits.h"
#include "libc/calls/strace.internal.h"
#include "libc/calls/struct/dirent.h"
#include "libc/calls/struct/itimerval.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/ucontext.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/fmt/kerrornames.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/dt.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/itimer.h"
#include "libc/sysv/consts/msg.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/ok.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/shut.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/consts/w.h"
#include "libc/time/time.h"
#include "libc/x/x.h"
#include "third_party/lua/lauxlib.h"
#include "third_party/lua/lua.h"
#include "third_party/lua/luaconf.h"
#include "tool/net/luacheck.h"

/**
 * @fileoverview UNIX system calls thinly wrapped for Lua
 * @support Linux, Mac, Windows, FreeBSD, NetBSD, OpenBSD
 */

struct UnixStat {
  int refs;
  struct stat st;
};

struct UnixDir {
  int refs;
  DIR *dir;
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

static dontinline int ReturnTimespec(lua_State *L, struct timespec *ts) {
  lua_pushinteger(L, ts->tv_sec);
  lua_pushinteger(L, ts->tv_nsec);
  return 2;
}

static int ReturnRc(lua_State *L, int64_t rc, int olderr) {
  lua_pushinteger(L, rc);
  if (rc != -1) return 1;
  lua_pushinteger(L, errno);
  errno = olderr;
  return 2;
}

static char **ConvertLuaArrayToStringList(lua_State *L, int i) {
  int j, n;
  char **p;
  luaL_checktype(L, i, LUA_TTABLE);
  lua_len(L, i);
  n = lua_tointeger(L, -1);
  lua_pop(L, 1);
  p = xcalloc(n + 1, sizeof(*p));
  for (j = 1; j <= n; ++j) {
    lua_geti(L, i, j);
    p[j - 1] = strdup(lua_tostring(L, -1));
    lua_pop(L, 1);
  }
  return p;
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

////////////////////////////////////////////////////////////////////////////////
// System Calls

// unix.exit([exitcode]) → ⊥
static int LuaUnixExit(lua_State *L) {
  _Exit(luaL_optinteger(L, 1, 0));
  unreachable;
}

// unix.access(path, mode) → rc, errno
// mode can be: R_OK, W_OK, X_OK, F_OK
static int LuaUnixAccess(lua_State *L) {
  const char *file;
  int rc, mode, olderr;
  olderr = errno;
  file = luaL_checklstring(L, 1, 0);
  mode = luaL_checkinteger(L, 2);
  rc = access(file, mode);
  return ReturnRc(L, rc, olderr);
}

// unix.mkdir(path, mode) → rc, errno
// mode should be octal
static int LuaUnixMkdir(lua_State *L) {
  const char *file;
  int rc, mode, olderr;
  olderr = errno;
  file = luaL_checklstring(L, 1, 0);
  mode = luaL_checkinteger(L, 2);
  rc = mkdir(file, mode);
  return ReturnRc(L, rc, olderr);
}

// unix.chdir(path) → rc, errno
static int LuaUnixChdir(lua_State *L) {
  int rc, olderr;
  const char *file;
  olderr = errno;
  file = luaL_checklstring(L, 1, 0);
  rc = chdir(file);
  return ReturnRc(L, rc, olderr);
}

// unix.unlink(path) → rc, errno
static int LuaUnixUnlink(lua_State *L) {
  int rc, olderr;
  const char *file;
  olderr = errno;
  file = luaL_checklstring(L, 1, 0);
  rc = unlink(file);
  return ReturnRc(L, rc, olderr);
}

// unix.rmdir(path) → rc, errno
static int LuaUnixRmdir(lua_State *L) {
  const char *file;
  int rc, olderr;
  olderr = errno;
  file = luaL_checklstring(L, 1, 0);
  rc = rmdir(file);
  return ReturnRc(L, rc, olderr);
}

// unix.rename(oldpath, newpath) → rc, errno
static int LuaUnixRename(lua_State *L) {
  const char *oldpath, *newpath;
  int rc, olderr;
  olderr = errno;
  oldpath = luaL_checklstring(L, 1, 0);
  newpath = luaL_checklstring(L, 2, 0);
  rc = rename(oldpath, newpath);
  return ReturnRc(L, rc, olderr);
}

// unix.link(existingpath, newpath) → rc, errno
static int LuaUnixLink(lua_State *L) {
  const char *existingpath, *newpath;
  int rc, olderr;
  olderr = errno;
  existingpath = luaL_checklstring(L, 1, 0);
  newpath = luaL_checklstring(L, 2, 0);
  rc = link(existingpath, newpath);
  return ReturnRc(L, rc, olderr);
}

// unix.symlink(target, linkpath) → rc, errno
static int LuaUnixSymlink(lua_State *L) {
  const char *target, *linkpath;
  int rc, olderr;
  olderr = errno;
  target = luaL_checklstring(L, 1, 0);
  linkpath = luaL_checklstring(L, 2, 0);
  rc = symlink(target, linkpath);
  return ReturnRc(L, rc, olderr);
}

// unix.chown(path, uid, gid) → rc, errno
static int LuaUnixChown(lua_State *L) {
  const char *file;
  int rc, uid, gid, olderr;
  olderr = errno;
  file = luaL_checklstring(L, 1, 0);
  uid = luaL_checkinteger(L, 2);
  gid = luaL_checkinteger(L, 3);
  rc = chown(file, uid, gid);
  return ReturnRc(L, rc, olderr);
}

// unix.chmod(path, mode) → rc, errno
static int LuaUnixChmod(lua_State *L) {
  const char *file;
  int rc, mode, olderr;
  olderr = errno;
  file = luaL_checklstring(L, 1, 0);
  mode = luaL_checkinteger(L, 2);
  rc = chmod(file, mode);
  return ReturnRc(L, rc, olderr);
}

// unix.getcwd(path, mode) → rc, errno
static int LuaUnixGetcwd(lua_State *L) {
  char *path;
  path = getcwd(0, 0);
  assert(path);
  lua_pushstring(L, path);
  free(path);
  return 1;
}

// unix.fork() → childpid|0, errno
static int LuaUnixFork(lua_State *L) {
  int rc, olderr;
  olderr = errno;
  rc = fork();
  return ReturnRc(L, rc, olderr);
}

// unix.execve(prog, argv[, envp]) → errno
// unix.exit(127)
//
//     unix = require "unix"
//     prog = unix.commandv("ls")
//     unix.execve(prog, {prog, "-hal", "."})
//     unix.exit(127)
//
// prog needs to be absolute, see commandv()
// envp defaults to environ
static int LuaUnixExecve(lua_State *L) {
  int olderr;
  const char *prog;
  char **argv, **envp, **freeme;
  olderr = errno;
  prog = luaL_checkstring(L, 1);
  argv = ConvertLuaArrayToStringList(L, 2);
  if (!lua_isnoneornil(L, 3)) {
    envp = ConvertLuaArrayToStringList(L, 3);
    freeme = envp;
  } else {
    envp = environ;
    freeme = 0;
  }
  execve(prog, argv, envp);
  FreeStringList(freeme);
  FreeStringList(argv);
  lua_pushinteger(L, errno);
  errno = olderr;
  return 1;
}

// unix.commandv(prog) → path, errno
static int LuaUnixCommandv(lua_State *L) {
  int olderr;
  const char *prog;
  char *pathbuf, *resolved;
  olderr = errno;
  pathbuf = xmalloc(PATH_MAX);
  prog = luaL_checkstring(L, 1);
  if ((resolved = commandv(prog, pathbuf))) {
    lua_pushstring(L, resolved);
    lua_pushnil(L);
  } else {
    lua_pushnil(L);
    lua_pushinteger(L, errno);
    errno = olderr;
  }
  free(pathbuf);
  return 2;
}

// unix.getpid() → pid
static int LuaUnixGetpid(lua_State *L) {
  lua_pushinteger(L, getpid());
  return 1;
}

// unix.getppid() → pid
static int LuaUnixGetppid(lua_State *L) {
  lua_pushinteger(L, getppid());
  return 1;
}

// unix.kill(pid, sig) → rc, errno
static int LuaUnixKill(lua_State *L) {
  int rc, pid, sig, olderr;
  olderr = errno;
  pid = luaL_checkinteger(L, 1);
  sig = luaL_checkinteger(L, 2);
  rc = kill(pid, sig);
  return ReturnRc(L, rc, olderr);
}

// unix.raise(sig) → rc, errno
static int LuaUnixRaise(lua_State *L) {
  int rc, sig, olderr;
  olderr = errno;
  sig = luaL_checkinteger(L, 1);
  rc = raise(sig);
  return ReturnRc(L, rc, olderr);
}

// unix.wait(pid[, options]) → pid, wstatus, nil, errno
static int LuaUnixWait(lua_State *L) {
  int rc, pid, olderr, options, wstatus;
  olderr = errno;
  pid = luaL_checkinteger(L, 1);
  options = luaL_optinteger(L, 2, 0);
  rc = wait4(pid, &wstatus, options, 0);
  if (rc != -1) {
    lua_pushinteger(L, rc);
    lua_pushinteger(L, wstatus);
    return 2;
  } else {
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);  // for future use
    lua_pushinteger(L, errno);
    errno = olderr;
    return 4;
  }
}

// unix.fcntl(fd, cmd[, arg]) → rc, errno
static int LuaUnixFcntl(lua_State *L) {
  intptr_t arg;
  int rc, fd, cmd, olderr;
  olderr = errno;
  fd = luaL_checkinteger(L, 1);
  cmd = luaL_checkinteger(L, 2);
  arg = luaL_optinteger(L, 3, 0);
  rc = fcntl(fd, cmd, arg);
  return ReturnRc(L, rc, olderr);
}

// unix.dup(oldfd[, newfd[, flags]]) → newfd, errno
// flags can have O_CLOEXEC
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
  return ReturnRc(L, rc, olderr);
}

// unix.pipe([flags]) → reader, writer, errno
// flags can have O_CLOEXEC
static int LuaUnixPipe(lua_State *L) {
  int flags, olderr, pipefd[2];
  olderr = errno;
  flags = luaL_optinteger(L, 1, 0);
  if (!pipe2(pipefd, flags)) {
    lua_pushinteger(L, pipefd[0]);
    lua_pushinteger(L, pipefd[1]);
    return 2;
  } else {
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushinteger(L, errno);
    errno = olderr;
    return 3;
  }
}

// unix.getsid(pid) → sid, errno
static int LuaUnixGetsid(lua_State *L) {
  int rc, pid, olderr;
  olderr = errno;
  pid = luaL_checkinteger(L, 1);
  rc = getsid(pid);
  return ReturnRc(L, rc, olderr);
}

// unix.getpgid(pid) → pgid, errno
static int LuaUnixGetpgid(lua_State *L) {
  int rc, pid, olderr;
  olderr = errno;
  pid = luaL_checkinteger(L, 1);
  rc = getpgid(pid);
  return ReturnRc(L, rc, olderr);
}

// unix.umask(mask) → rc, errno
static int LuaUnixUmask(lua_State *L) {
  int rc, mask, olderr;
  olderr = errno;
  mask = luaL_checkinteger(L, 1);
  rc = umask(mask);
  return ReturnRc(L, rc, olderr);
}

// unix.setpgid(pid, pgid) → pgid, errno
static int LuaUnixSetpgid(lua_State *L) {
  int rc, pid, pgid, olderr;
  olderr = errno;
  pid = luaL_checkinteger(L, 1);
  pgid = luaL_checkinteger(L, 2);
  rc = setpgid(pid, pgid);
  return ReturnRc(L, rc, olderr);
}

// unix.setsid() → sid, errno
static int LuaUnixSetsid(lua_State *L) {
  int rc, olderr;
  olderr = errno;
  rc = setsid();
  return ReturnRc(L, rc, olderr);
}

// unix.getuid() → uid, errno
static int LuaUnixGetuid(lua_State *L) {
  int rc, olderr;
  olderr = errno;
  rc = getuid();
  return ReturnRc(L, rc, olderr);
}

// unix.getgid() → gid, errno
static int LuaUnixGetgid(lua_State *L) {
  int rc, olderr;
  olderr = errno;
  rc = getgid();
  return ReturnRc(L, rc, olderr);
}

// unix.gettime([clock]) → seconds, nanos, errno
static int LuaUnixGettime(lua_State *L) {
  struct timespec ts;
  int rc, clock, olderr;
  olderr = errno;
  clock = luaL_optinteger(L, 1, CLOCK_REALTIME);
  rc = clock_gettime(clock, &ts);
  if (rc != -1) {
    lua_pushinteger(L, ts.tv_sec);
    lua_pushinteger(L, ts.tv_nsec);
    return 2;
  } else {
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushinteger(L, errno);
    errno = olderr;
    return 3;
  }
}

// unix.nanosleep(seconds, nanos) → remseconds, remnanos, errno
static int LuaUnixNanosleep(lua_State *L) {
  int rc, olderr;
  struct timespec req, rem;
  olderr = errno;
  req.tv_sec = luaL_checkinteger(L, 1);
  req.tv_nsec = luaL_optinteger(L, 2, 0);
  rc = nanosleep(&req, &rem);
  if (rc != -1) {
    lua_pushinteger(L, rem.tv_sec);
    lua_pushinteger(L, rem.tv_nsec);
    return 2;
  } else {
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushinteger(L, errno);
    errno = olderr;
    return 3;
  }
}

// unix.sync(fd)
static int LuaUnixSync(lua_State *L) {
  sync();
  return 0;
}

// unix.fsync(fd) → rc, errno
static int LuaUnixFsync(lua_State *L) {
  int rc, fd, olderr;
  olderr = errno;
  fd = luaL_checkinteger(L, 1);
  rc = fsync(fd);
  return ReturnRc(L, rc, olderr);
}

// unix.fdatasync(fd) → rc, errno
static int LuaUnixFdatasync(lua_State *L) {
  int rc, fd, olderr;
  olderr = errno;
  fd = luaL_checkinteger(L, 1);
  rc = fdatasync(fd);
  return ReturnRc(L, rc, olderr);
}

// unix.open(path, flags[, mode]) → fd, errno
static int LuaUnixOpen(lua_State *L) {
  const char *file;
  int rc, flags, mode, olderr;
  olderr = errno;
  file = luaL_checklstring(L, 1, 0);
  flags = luaL_checkinteger(L, 2);
  mode = luaL_optinteger(L, 3, 0);
  rc = open(file, flags, mode);
  return ReturnRc(L, rc, olderr);
}

// unix.close(fd) → rc, errno
static int LuaUnixClose(lua_State *L) {
  int rc, fd, olderr;
  olderr = errno;
  fd = luaL_checkinteger(L, 1);
  rc = close(fd);
  return ReturnRc(L, rc, olderr);
}

// unix.seek(fd, offset, whence) → newpos, errno
// where whence ∈ {SEEK_SET, SEEK_CUR, SEEK_END}
//       whence defaults to SEEK_SET
static int LuaUnixSeek(lua_State *L) {
  int64_t newpos, offset;
  int fd, olderr, whence;
  olderr = errno;
  fd = luaL_checkinteger(L, 1);
  offset = luaL_checkinteger(L, 2);
  whence = luaL_optinteger(L, 3, SEEK_SET);
  newpos = lseek(fd, offset, whence);
  return ReturnRc(L, newpos, olderr);
}

// unix.truncate(path, length) → rc, errno
// unix.truncate(fd, length)   → rc, errno
static int LuaUnixTruncate(lua_State *L) {
  int64_t length;
  const char *path;
  int rc, fd, olderr, whence;
  olderr = errno;
  if (lua_isinteger(L, 1)) {
    fd = luaL_checkinteger(L, 1);
    length = luaL_checkinteger(L, 2);
    rc = ftruncate(fd, length);
  } else if (lua_isstring(L, 1)) {
    path = luaL_checkstring(L, 1);
    length = luaL_checkinteger(L, 2);
    rc = truncate(path, length);
  } else {
    luaL_argerror(L, 1, "not integer or string");
    unreachable;
  }
  return ReturnRc(L, rc, olderr);
}

// unix.read(fd[, bufsiz, offset]) → data, errno
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
  buf = xmalloc(bufsiz);
  if (offset == -1) {
    rc = read(fd, buf, bufsiz);
  } else {
    rc = pread(fd, buf, bufsiz, offset);
  }
  if (rc != -1) {
    got = rc;
    lua_pushlstring(L, buf, got);
    lua_pushnil(L);
  } else {
    lua_pushnil(L);
    lua_pushinteger(L, errno);
    errno = olderr;
  }
  free(buf);
  return 2;
}

// unix.write(fd, data[, offset]) → rc, errno
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
  return ReturnRc(L, rc, olderr);
}

// unix.stat(path) → UnixStat*, errno
// unix.stat(fd)   → UnixStat*, errno
static int LuaUnixStat(lua_State *L) {
  const char *path;
  int rc, fd, olderr;
  struct UnixStat **ust, *st;
  olderr = errno;
  st = xmalloc(sizeof(struct UnixStat));
  if (lua_isinteger(L, 1)) {
    fd = luaL_checkinteger(L, 1);
    rc = fstat(fd, &st->st);
  } else if (lua_isstring(L, 1)) {
    path = luaL_checkstring(L, 1);
    rc = stat(path, &st->st);
  } else {
    luaL_argerror(L, 1, "not integer or string");
    unreachable;
  }
  if (rc == -1) {
    lua_pushnil(L);
    lua_pushinteger(L, errno);
    errno = olderr;
    free(st);
    return 2;
  }
  st->refs = 1;
  ust = lua_newuserdatauv(L, sizeof(st), 1);
  luaL_setmetatable(L, "UnixStat*");
  *ust = st;
  return 1;
}

// unix.opendir(path) → UnixDir*, errno
// unix.opendir(fd)   → UnixDir*, errno
static int LuaUnixOpendir(lua_State *L) {
  DIR *rc;
  int fd, olderr;
  const char *path;
  struct UnixDir **udir, *dir;
  olderr = errno;
  dir = xcalloc(1, sizeof(struct UnixDir));
  if (lua_isinteger(L, 1)) {
    fd = luaL_checkinteger(L, 1);
    rc = fdopendir(fd);
  } else if (lua_isstring(L, 1)) {
    path = luaL_checkstring(L, 1);
    rc = opendir(path);
  } else {
    luaL_argerror(L, 1, "not integer or string");
    unreachable;
  }
  if (!rc) {
    lua_pushnil(L);
    lua_pushinteger(L, errno);
    errno = olderr;
    free(dir);
    return 2;
  }
  dir->refs = 1;
  dir->dir = rc;
  udir = lua_newuserdatauv(L, sizeof(dir), 1);
  luaL_setmetatable(L, "UnixDir*");
  *udir = dir;
  return 1;
}

// unix.socket([family[, type[, protocol]]]) → fd, errno
// SOCK_CLOEXEC may be or'd into type
// family defaults to AF_INET
// type defaults to SOCK_STREAM
// protocol defaults to IPPROTO_TCP
static int LuaUnixSocket(lua_State *L) {
  const char *file;
  int rc, olderr, family, type, protocol;
  olderr = errno;
  family = luaL_optinteger(L, 1, AF_INET);
  type = luaL_optinteger(L, 2, SOCK_STREAM);
  protocol = luaL_optinteger(L, 3, IPPROTO_TCP);
  rc = socket(family, type, protocol);
  return ReturnRc(L, rc, olderr);
}

// unix.socketpair([family[, type[, protocol]]]) → fd1, fd2, errno
// SOCK_CLOEXEC may be or'd into type
// family defaults to AF_INET
// type defaults to SOCK_STREAM
// protocol defaults to IPPROTO_TCP
static int LuaUnixSocketpair(lua_State *L) {
  int olderr, family, type, protocol, sv[2];
  olderr = errno;
  family = luaL_optinteger(L, 1, AF_INET);
  type = luaL_optinteger(L, 2, SOCK_STREAM);
  protocol = luaL_optinteger(L, 3, IPPROTO_TCP);
  if (!socketpair(family, type, protocol, sv)) {
    lua_pushinteger(L, sv[0]);
    lua_pushinteger(L, sv[1]);
    return 2;
  } else {
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushinteger(L, errno);
    errno = olderr;
    return 3;
  }
}

// unix.bind(fd, ip, port) → rc, errno
// SOCK_CLOEXEC may be or'd into type
// family defaults to AF_INET
// type defaults to SOCK_STREAM
// protocol defaults to IPPROTO_TCP
static int LuaUnixBind(lua_State *L) {
  int rc, olderr, fd;
  struct sockaddr_in sa;
  bzero(&sa, sizeof(sa));
  olderr = errno;
  fd = luaL_checkinteger(L, 1);
  sa.sin_addr.s_addr = htonl(luaL_checkinteger(L, 2));
  sa.sin_port = htons(luaL_checkinteger(L, 3));
  rc = bind(fd, &sa, sizeof(sa));
  return ReturnRc(L, rc, olderr);
}

// unix.connect(fd, ip, port) → rc, errno
// SOCK_CLOEXEC may be or'd into type
// family defaults to AF_INET
// type defaults to SOCK_STREAM
// protocol defaults to IPPROTO_TCP
static int LuaUnixConnect(lua_State *L) {
  int rc, olderr, fd;
  struct sockaddr_in sa;
  bzero(&sa, sizeof(sa));
  olderr = errno;
  fd = luaL_checkinteger(L, 1);
  sa.sin_addr.s_addr = htonl(luaL_checkinteger(L, 2));
  sa.sin_port = htons(luaL_checkinteger(L, 3));
  rc = connect(fd, &sa, sizeof(sa));
  return ReturnRc(L, rc, olderr);
}

// unix.listen(fd[, backlog]) → rc, errno
static int LuaUnixListen(lua_State *L) {
  int rc, fd, olderr, backlog;
  olderr = errno;
  fd = luaL_checkinteger(L, 1);
  backlog = luaL_optinteger(L, 2, 10);
  rc = listen(fd, backlog);
  return ReturnRc(L, rc, olderr);
}

// unix.getsockname(fd) → ip, port, errno
static int LuaUnixGetsockname(lua_State *L) {
  int fd, olderr;
  uint32_t addrsize;
  struct sockaddr_in sa;
  olderr = errno;
  bzero(&sa, sizeof(sa));
  addrsize = sizeof(sa);
  fd = luaL_checkinteger(L, 1);
  if (!getsockname(fd, &sa, &addrsize)) {
    lua_pushinteger(L, ntohl(sa.sin_addr.s_addr));
    lua_pushinteger(L, ntohs(sa.sin_port));
    return 2;
  } else {
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushinteger(L, errno);
    errno = olderr;
    return 3;
  }
}

// unix.getpeername(fd) → ip, port, errno
static int LuaUnixGetpeername(lua_State *L) {
  int fd, olderr;
  uint32_t addrsize;
  struct sockaddr_in sa;
  olderr = errno;
  bzero(&sa, sizeof(sa));
  addrsize = sizeof(sa);
  fd = luaL_checkinteger(L, 1);
  if (!getpeername(fd, &sa, &addrsize)) {
    lua_pushinteger(L, ntohl(sa.sin_addr.s_addr));
    lua_pushinteger(L, ntohs(sa.sin_port));
    return 2;
  } else {
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushinteger(L, errno);
    errno = olderr;
    return 3;
  }
}

// unix.accept(serverfd) → clientfd, ip, port, errno
static int LuaUnixAccept(lua_State *L) {
  uint32_t addrsize;
  struct sockaddr_in sa;
  int clientfd, serverfd, olderr;
  olderr = errno;
  bzero(&sa, sizeof(sa));
  addrsize = sizeof(sa);
  serverfd = luaL_checkinteger(L, 1);
  clientfd = accept(serverfd, &sa, &addrsize);
  if (clientfd != -1) {
    lua_pushinteger(L, clientfd);
    lua_pushinteger(L, ntohl(sa.sin_addr.s_addr));
    lua_pushinteger(L, ntohs(sa.sin_port));
    return 3;
  } else {
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushinteger(L, errno);
    errno = olderr;
    return 4;
  }
}

// unix.recvfrom(fd[, bufsiz[, flags]]) → data, ip, port, errno
// flags can have MSG_{WAITALL,DONTROUTE,PEEK,OOB}, etc.
static int LuaUnixRecvfrom(lua_State *L) {
  char *buf;
  size_t got;
  ssize_t rc;
  uint32_t addrsize;
  struct sockaddr_in sa;
  int fd, flags, bufsiz, olderr;
  olderr = errno;
  bzero(&sa, sizeof(sa));
  addrsize = sizeof(sa);
  fd = luaL_checkinteger(L, 1);
  bufsiz = luaL_optinteger(L, 2, 1500);
  flags = luaL_optinteger(L, 3, 0);
  bufsiz = MIN(bufsiz, 0x7ffff000);
  buf = xmalloc(bufsiz);
  rc = recvfrom(fd, buf, bufsiz, flags, &sa, &addrsize);
  if (rc != -1) {
    got = rc;
    lua_pushlstring(L, buf, got);
    lua_pushinteger(L, ntohl(sa.sin_addr.s_addr));
    lua_pushinteger(L, ntohs(sa.sin_port));
    lua_pushnil(L);
  } else {
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushinteger(L, errno);
    errno = olderr;
  }
  free(buf);
  return 4;
}

// unix.recv(fd[, bufsiz[, flags]]) → data, errno
static int LuaUnixRecv(lua_State *L) {
  char *buf;
  size_t got;
  ssize_t rc;
  int fd, flags, bufsiz, olderr;
  olderr = errno;
  fd = luaL_checkinteger(L, 1);
  bufsiz = luaL_optinteger(L, 2, 1500);
  flags = luaL_optinteger(L, 3, 0);
  bufsiz = MIN(bufsiz, 0x7ffff000);
  buf = xmalloc(bufsiz);
  rc = recv(fd, buf, bufsiz, flags);
  if (rc != -1) {
    got = rc;
    lua_pushlstring(L, buf, got);
    lua_pushnil(L);
  } else {
    lua_pushnil(L);
    lua_pushinteger(L, errno);
    errno = olderr;
  }
  free(buf);
  return 4;
}

// unix.send(fd, data[, flags]) → sent, errno
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
  rc = send(fd, data, size, flags);
  return ReturnRc(L, rc, olderr);
}

// unix.sendto(fd, data, ip, port[, flags]) → sent, errno
// flags MSG_OOB, MSG_DONTROUTE, MSG_NOSIGNAL, etc.
static int LuaUnixSendto(lua_State *L) {
  char *data;
  ssize_t rc;
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
  rc = sendto(fd, data, size, flags, &sa, sizeof(sa));
  return ReturnRc(L, rc, olderr);
}

// unix.shutdown(fd, how) → rc, errno
// how can be SHUT_RD, SHUT_WR, or SHUT_RDWR
static int LuaUnixShutdown(lua_State *L) {
  int rc, fd, how, olderr;
  olderr = errno;
  fd = luaL_checkinteger(L, 1);
  how = luaL_checkinteger(L, 2);
  rc = shutdown(fd, how);
  return ReturnRc(L, rc, olderr);
}

// unix.sigprocmask(how[, mask]) → oldmask, errno
// how can be SIG_BLOCK, SIG_UNBLOCK, SIG_SETMASK
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
    lua_pushnil(L);
    lua_pushinteger(L, errno);
    errno = olderr;
    return 2;
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

// unix.sigaction(sig[, handler[, flags[, mask]]]) → handler, flags, mask, errno
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
    // finish pushing the last 2/3 results
    lua_pushinteger(L, oldsa.sa_flags);
    lua_pushinteger(L, oldsa.sa_mask.__bits[0]);
    return 3;
  } else {
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushinteger(L, errno);
    errno = olderr;
    return 2;
  }
}

// unix.sigsuspend([mask]) → errno
static int LuaUnixSigsuspend(lua_State *L) {
  int olderr;
  sigset_t mask;
  olderr = errno;
  mask.__bits[0] = luaL_optinteger(L, 1, 0);
  mask.__bits[1] = 0;
  sigsuspend(&mask);
  lua_pushinteger(L, errno);
  errno = olderr;
  return 1;
}

// unix.setitimer(which[, intsec, intmicros, valsec, valmicros])
//   → intsec, intns, valsec, valns, errno
//
//     ticks = 0
//     unix.sigaction(unix.SIGALRM, function(sig)
//        print(string.format("tick no. %d", ticks))
//        ticks = ticks + 1
//     end)
//     unix.setitimer(unix.ITIMER_REAL, 0, 400000, 0, 400000)
//     while true do
//        unix.sigsuspend()
//     end
//
// which should be ITIMER_REAL
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
    lua_pushinteger(L, oldit.it_interval.tv_usec);
    lua_pushinteger(L, oldit.it_value.tv_sec);
    lua_pushinteger(L, oldit.it_value.tv_usec);
    return 4;
  } else {
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushinteger(L, errno);
    errno = olderr;
    return 5;
  }
}

// unix.strerror(errno) → str
static int LuaUnixStrerror(lua_State *L) {
  lua_pushstring(L, strerror(luaL_checkinteger(L, 1)));
  return 1;
}

// unix.strsignal(sig) → str
static int LuaUnixStrsignal(lua_State *L) {
  lua_pushstring(L, strsignal(luaL_checkinteger(L, 1)));
  return 1;
}

////////////////////////////////////////////////////////////////////////////////
// UnixStat* object

static dontinline struct stat *GetUnixStat(lua_State *L) {
  struct UnixStat **ust;
  ust = luaL_checkudata(L, 1, "UnixStat*");
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

static int LuaUnixStatAtim(lua_State *L) {
  return ReturnTimespec(L, &GetUnixStat(L)->st_atim);
}

static int LuaUnixStatMtim(lua_State *L) {
  return ReturnTimespec(L, &GetUnixStat(L)->st_mtim);
}

static int LuaUnixStatCtim(lua_State *L) {
  return ReturnTimespec(L, &GetUnixStat(L)->st_ctim);
}

static void FreeUnixStat(struct UnixStat *stat) {
  if (!--stat->refs) {
    free(stat);
  }
}

static int LuaUnixStatGc(lua_State *L) {
  struct UnixStat **ust;
  ust = luaL_checkudata(L, 1, "UnixStat*");
  if (*ust) {
    FreeUnixStat(*ust);
    *ust = 0;
  }
  return 0;
}

static const luaL_Reg kLuaUnixStatMeth[] = {
    {"size", LuaUnixStatSize},        //
    {"mode", LuaUnixStatMode},        //
    {"dev", LuaUnixStatDev},          //
    {"ino", LuaUnixStatIno},          //
    {"nlink", LuaUnixStatNlink},      //
    {"rdev", LuaUnixStatRdev},        //
    {"uid", LuaUnixStatUid},          //
    {"gid", LuaUnixStatGid},          //
    {"atim", LuaUnixStatAtim},        //
    {"mtim", LuaUnixStatMtim},        //
    {"ctim", LuaUnixStatCtim},        //
    {"blocks", LuaUnixStatBlocks},    //
    {"blksize", LuaUnixStatBlksize},  //
    {0},                              //
};

static const luaL_Reg kLuaUnixStatMeta[] = {
    {"__gc", LuaUnixStatGc},  //
    {0},                      //
};

static void LuaUnixStatObj(lua_State *L) {
  luaL_newmetatable(L, "UnixStat*");
  luaL_setfuncs(L, kLuaUnixStatMeta, 0);
  luaL_newlibtable(L, kLuaUnixStatMeth);
  luaL_setfuncs(L, kLuaUnixStatMeth, 0);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);
}

////////////////////////////////////////////////////////////////////////////////
// UnixDir* object

static struct UnixDir **GetUnixDirSelf(lua_State *L) {
  return luaL_checkudata(L, 1, "UnixDir*");
}

static DIR *GetDirOrDie(lua_State *L) {
  struct UnixDir **udir;
  udir = GetUnixDirSelf(L);
  assert((*udir)->dir);
  if (*udir) return (*udir)->dir;
  luaL_argerror(L, 1, "UnixDir* is closed");
  unreachable;
}

static void FreeUnixDir(struct UnixDir *dir) {
  if (!--dir->refs) {
    closedir(dir->dir);
  }
}

// UnixDir:close()
// may be called multiple times
// called by the garbage collector too
static int LuaUnixDirClose(lua_State *L) {
  struct UnixDir **udir;
  udir = GetUnixDirSelf(L);
  if (*udir) {
    FreeUnixDir(*udir);
    *udir = 0;
  }
  return 0;
}

// UnixDir:read() → name, kind, ino, off
// returns nil if no more entries
// kind can be DT_UNKNOWN/REG/DIR/BLK/LNK/CHR/FIFO/SOCK
static int LuaUnixDirRead(lua_State *L) {
  struct dirent *ent;
  if ((ent = readdir(GetDirOrDie(L)))) {
    lua_pushlstring(L, ent->d_name, strnlen(ent->d_name, sizeof(ent->d_name)));
    lua_pushinteger(L, ent->d_type);
    lua_pushinteger(L, ent->d_ino);
    lua_pushinteger(L, ent->d_off);
    return 4;
  } else {
    return 0;
  }
}

// UnixDir:fd() → fd, errno
// EOPNOTSUPP if using /zip/
// EOPNOTSUPP if IsWindows()
static int LuaUnixDirFd(lua_State *L) {
  int fd, olderr;
  olderr = errno;
  fd = dirfd(GetDirOrDie(L));
  if (fd != -1) {
    lua_pushinteger(L, fd);
    return 1;
  } else {
    lua_pushnil(L);
    lua_pushinteger(L, errno);
    errno = olderr;
    return 2;
  }
}

// UnixDir:tell() → off
static int LuaUnixDirTell(lua_State *L) {
  long off;
  off = telldir(GetDirOrDie(L));
  lua_pushinteger(L, off);
  return 1;
}

// UnixDir:rewind()
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
  luaL_newmetatable(L, "UnixDir*");
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
    {"stat", LuaUnixStat},                // get file info
    {"open", LuaUnixOpen},                // open file fd at lowest slot
    {"close", LuaUnixClose},              // close file or socket
    {"seek", LuaUnixSeek},                // seek in file
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
    {"commandv", LuaUnixCommandv},        // resolve program on $PATH
    {"kill", LuaUnixKill},                // signal child process
    {"raise", LuaUnixRaise},              // signal this process
    {"wait", LuaUnixWait},                // wait for child to change status
    {"pipe", LuaUnixPipe},                // create two anon fifo fds
    {"dup", LuaUnixDup},                  // copy fd to lowest empty slot
    {"mkdir", LuaUnixMkdir},              // make directory
    {"rmdir", LuaUnixRmdir},              // remove empty directory
    {"opendir", LuaUnixOpendir},          // read directory entry list
    {"rename", LuaUnixRename},            // rename file or directory
    {"link", LuaUnixLink},                // create hard link
    {"unlink", LuaUnixUnlink},            // remove file
    {"symlink", LuaUnixSymlink},          // create symbolic link
    {"sync", LuaUnixSync},                // flushes files and disks
    {"fsync", LuaUnixFsync},              // flush open file
    {"fdatasync", LuaUnixFdatasync},      // flush open file w/o metadata
    {"truncate", LuaUnixTruncate},        // shrink or extend file medium
    {"umask", LuaUnixUmask},              // set file mode creation mask
    {"getppid", LuaUnixGetppid},          // get parent process id
    {"getpgid", LuaUnixGetpgid},          // get process group id of pid
    {"setpgid", LuaUnixSetpgid},          // set process group id for pid
    {"getsid", LuaUnixGetsid},            // get session id of pid
    {"setsid", LuaUnixSetsid},            // create a new session id
    {"getpid", LuaUnixGetpid},            // get id of this process
    {"getuid", LuaUnixGetuid},            // get real user id of process
    {"getgid", LuaUnixGetgid},            // get real group id of process
    {"gettime", LuaUnixGettime},          // get timestamp w/ nano precision
    {"nanosleep", LuaUnixNanosleep},      // sleep w/ nano precision
    {"socket", LuaUnixSocket},            // create network communication fd
    {"socketpair", LuaUnixSocketpair},    // create bidirectional pipe
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
    {"sigaction", LuaUnixSigaction},      // install signal handler
    {"sigprocmask", LuaUnixSigprocmask},  // change signal mask
    {"sigsuspend", LuaUnixSigsuspend},    // wait for signal
    {"setitimer", LuaUnixSetitimer},      // set alarm clock
    {"strerror", LuaUnixStrerror},        // turn errno into string
    {"strsignal", LuaUnixStrsignal},      // turn signal into string
    {0},                                  //
};

int LuaUnix(lua_State *L) {
  int i;
  char sigbuf[12];

  GL = L;
  luaL_newlib(L, kLuaUnix);
  LuaUnixStatObj(L);
  LuaUnixDirObj(L);
  lua_newtable(L);
  lua_setglobal(L, "__signal_handlers");

  // errnos
  for (i = 0; kErrorNames[i].x; ++i) {
    LuaSetIntField(L, (const char *)((uintptr_t)kErrorNames + kErrorNames[i].s),
                   *(const int *)((uintptr_t)kErrorNames + kErrorNames[i].x));
  }

  // signals
  strcpy(sigbuf, "SIG");
  for (i = 0; kStrSignal[i].x; ++i) {
    strcpy(sigbuf + 3, (const char *)((uintptr_t)kStrSignal + kStrSignal[i].s));
    LuaSetIntField(L, sigbuf,
                   *(const int *)((uintptr_t)kStrSignal + kStrSignal[i].x));
  }

  // open() flags
  LuaSetIntField(L, "O_RDONLY", O_RDONLY);          //
  LuaSetIntField(L, "O_WRONLY", O_WRONLY);          //
  LuaSetIntField(L, "O_RDWR", O_RDWR);              //
  LuaSetIntField(L, "O_ACCMODE", O_ACCMODE);        // mask of prev three
  LuaSetIntField(L, "O_CREAT", O_CREAT);            //
  LuaSetIntField(L, "O_EXCL", O_EXCL);              //
  LuaSetIntField(L, "O_TRUNC", O_TRUNC);            //
  LuaSetIntField(L, "O_CLOEXEC", O_CLOEXEC);        //
  LuaSetIntField(L, "O_APPEND", O_APPEND);          // weird on nt
  LuaSetIntField(L, "O_TMPFILE", O_TMPFILE);        // linux, windows
  LuaSetIntField(L, "O_NOFOLLOW", O_NOFOLLOW);      // unix
  LuaSetIntField(L, "O_SYNC", O_SYNC);              // unix
  LuaSetIntField(L, "O_ASYNC", O_ASYNC);            // unix
  LuaSetIntField(L, "O_NOCTTY", O_NOCTTY);          // unix
  LuaSetIntField(L, "O_NOATIME", O_NOATIME);        // linux
  LuaSetIntField(L, "O_EXEC", O_EXEC);              // free/openbsd
  LuaSetIntField(L, "O_SEARCH", O_SEARCH);          // free/netbsd
  LuaSetIntField(L, "O_DSYNC", O_DSYNC);            // linux/xnu/open/netbsd
  LuaSetIntField(L, "O_RSYNC", O_RSYNC);            // linux/open/netbsd
  LuaSetIntField(L, "O_PATH", O_PATH);              // linux
  LuaSetIntField(L, "O_VERIFY", O_VERIFY);          // freebsd
  LuaSetIntField(L, "O_SHLOCK", O_SHLOCK);          // bsd
  LuaSetIntField(L, "O_EXLOCK", O_EXLOCK);          // bsd
  LuaSetIntField(L, "O_RANDOM", O_RANDOM);          // windows
  LuaSetIntField(L, "O_SEQUENTIAL", O_SEQUENTIAL);  // windows
  LuaSetIntField(L, "O_COMPRESSED", O_COMPRESSED);  // windows
  LuaSetIntField(L, "O_INDEXED", O_INDEXED);        // windows

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

  // wait() options
  LuaSetIntField(L, "WNOHANG", WNOHANG);

  // gettime() clocks
  LuaSetIntField(L, "CLOCK_REALTIME", CLOCK_REALTIME);            // portable
  LuaSetIntField(L, "CLOCK_MONOTONIC", CLOCK_MONOTONIC);          // portable
  LuaSetIntField(L, "CLOCK_MONOTONIC_RAW", CLOCK_MONOTONIC_RAW);  // portable
  LuaSetIntField(L, "CLOCK_REALTIME_COARSE", CLOCK_REALTIME_COARSE);
  LuaSetIntField(L, "CLOCK_MONOTONIC_COARSE", CLOCK_MONOTONIC_COARSE);
  LuaSetIntField(L, "CLOCK_PROCESS_CPUTIME_ID", CLOCK_PROCESS_CPUTIME_ID);
  LuaSetIntField(L, "CLOCK_TAI", CLOCK_TAI);
  LuaSetIntField(L, "CLOCK_PROF", CLOCK_PROF);
  LuaSetIntField(L, "CLOCK_BOOTTIME", CLOCK_BOOTTIME);
  LuaSetIntField(L, "CLOCK_REALTIME_ALARM", CLOCK_REALTIME_ALARM);
  LuaSetIntField(L, "CLOCK_BOOTTIME_ALARM", CLOCK_BOOTTIME_ALARM);

  // socket() family
  LuaSetIntField(L, "AF_UNSPEC", AF_UNSPEC);
  LuaSetIntField(L, "AF_UNIX", AF_UNIX);
  LuaSetIntField(L, "AF_INET", AF_INET);

  // socket() type
  LuaSetIntField(L, "SOCK_STREAM", SOCK_STREAM);
  LuaSetIntField(L, "SOCK_DGRAM", SOCK_DGRAM);
  LuaSetIntField(L, "SOCK_CLOEXEC", SOCK_CLOEXEC);

  // socket() protocol
  LuaSetIntField(L, "IPPROTO_TCP", IPPROTO_TCP);
  LuaSetIntField(L, "IPPROTO_UDP", IPPROTO_UDP);

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

  return 1;
}
