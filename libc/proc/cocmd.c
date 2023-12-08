/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/timespec.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/itoa.h"
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/serialize.h"
#include "libc/intrin/getenv.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/lock.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/ok.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/timer.h"
#include "libc/temp.h"
#include "third_party/awk/cmd.h"
#include "third_party/getopt/getopt.internal.h"
#include "third_party/musl/glob.h"
#include "third_party/sed/cmd.h"
#include "third_party/tr/cmd.h"
#include "tool/curl/cmd.h"

/**
 * @fileoverview Cosmopolitan Command Interpreter
 *
 * This is a lightweight command interpreter for GNU Make. It has just
 * enough shell script language support to support our build config.
 */

#define STATE_CMD        0
#define STATE_VAR        1
#define STATE_SINGLE     2
#define STATE_QUOTED     3
#define STATE_QUOTED_VAR 4
#define STATE_WHITESPACE 5

#define TOMBSTONE ((char *)-1)
#define READ24(s) READ32LE(s "\0")

static char *p;
static char *q;
static char *r;
static int vari;
static size_t n;
static char *cmd;
static char *assign;
static char var[32];
static int lastchild;
static int exitstatus;
static char *envs[500];
static char *args[3000];
static const char *prog;
static char argbuf[ARG_MAX];
static bool unsupported[256];

static int ShellSpawn(void);
static int ShellExec(void);

static ssize_t Write(int fd, const char *s) {
  return write(fd, s, strlen(s));
}

static wontreturn void UnsupportedSyntax(unsigned char c) {
  char ibuf[13], cbuf[2] = {c};
  FormatOctal32(ibuf, c, true);
  tinyprint(2, prog, ": unsupported syntax '", cbuf, "' (", ibuf, "): ", cmd,
            "\n", NULL);
  _Exit(4);
}

static void Open(const char *path, int fd, int flags) {
  int tmpfd;
  // use open+dup2 to support things like >/dev/stdout
  if ((tmpfd = open(path, flags, 0644)) == -1) {
    perror(path);
    _Exit(1);
  }
  if (tmpfd != fd) {
    if (dup2(tmpfd, fd) == -1) {
      perror("dup2");
      _Exit(1);
    }
    close(tmpfd);
  }
}

static int SystemExec(void) {
  execvpe(args[0], args, envs);
  perror(args[0]);
  return (n = 0), 127;
}

static int GetSignalByName(const char *s) {
  for (int i = 0; kSignalNames[i].x != MAGNUM_TERMINATOR; ++i) {
    if (!strcmp(s, MAGNUM_STRING(kSignalNames, i) + 3)) {
      return MAGNUM_NUMBER(kSignalNames, i);
    }
  }
  return 0;
}

static void PutEnv(char **p, char *kv) {
  struct Env e;
  e = __getenv(p, kv);
  p[e.i] = kv;
  if (!e.s) p[e.i + 1] = 0;
}

static void UnsetEnv(char **p, const char *k) {
  int i;
  struct Env e;
  if ((e = __getenv(p, k)).s) {
    p[e.i] = 0;
    for (i = e.i + 1; p[i]; ++i) {
      p[i - 1] = p[i];
      p[i] = 0;
    }
  }
}

static void Append(int c) {
  npassert(q + 1 < argbuf + sizeof(argbuf));
  *q++ = c;
}

static char *Finish(void) {
  char *s = r;
  Append(0);
  r = q;
  if (!assign) {
    return s;
  } else {
    PutEnv(envs, s);
    assign = 0;
    return TOMBSTONE;
  }
}

static int Pause(void) {
  pause();
  return 0;
}

static int True(void) {
  return 0;
}

static int False(void) {
  return 1;
}

static wontreturn void Exit(void) {
  _Exit(n > 1 ? atoi(args[1]) : 0);
}

static int Waiter(int pid) {
  int ws;
  n = 0;
  if (waitpid(pid, &ws, 0) == -1) {
    perror("wait");
    return 1;
  }
  if (WIFEXITED(ws)) {
    return WEXITSTATUS(ws);
  } else {
    return 128 + WTERMSIG(ws);
  }
}

static int Wait(void) {
  int e, ws, pid;
  if (n > 1) {
    if ((pid = atoi(args[1])) <= 0) {
      tinyprint(2, "wait: bad pid\n", NULL);
      return 1;
    }
    return Waiter(pid);
  } else {
    for (n = 0, e = errno;;) {
      if (waitpid(-1, &ws, 0) == -1) {
        if (errno == ECHILD) {
          errno = e;
          break;
        }
        perror("wait");
        return 1;
      }
    }
    return 0;
  }
}

static const char *GetOptArg(int c, int *i, int j) {
  if (args[*i][j] == c) {
    if (args[*i][j + 1]) {
      return args[*i] + j + 1;
    } else if (*i + 1 < n) {
      return args[++*i];
    }
  }
  return 0;
}

static int Echo(void) {
  int i = 1;
  bool once = false;
  bool print_newline = true;
  if (i < n && args[i][0] == '-' && args[i][1] == 'n' && !args[i][2]) {
    ++i, print_newline = false;
  }
  for (; i < n; ++i) {
    if (once) {
      Write(1, " ");
    } else {
      once = true;
    }
    Write(1, args[i]);
  }
  if (print_newline) {
    Write(1, "\n");
  }
  return 0;
}

static int NeedArgument(const char *prog) {
  tinyprint(2, prog, ": missing argument\n", NULL);
  return 1;
}

static int Flock(void) {
  int fd;
  int i = 1;
  char *endptr;
  int mode = LOCK_EX;
  if (i < n && args[i][0] == '-' && args[i][1] == 'x' && !args[i][2]) {
    ++i, mode = LOCK_EX;
  }
  if (i < n && args[i][0] == '-' && args[i][1] == 's' && !args[i][2]) {
    ++i, mode = LOCK_SH;
  }
  if (i == n) {
    return NeedArgument("flock");
  }
  if (i + 1 != n) {
    tinyprint(2, "flock: too many arguments\n", NULL);
    return 1;
  }
  fd = strtol(args[i], &endptr, 10);
  if (*endptr) {
    tinyprint(2, "flock: need a number\n", NULL);
    return 1;
  }
  if (flock(fd, mode)) {
    perror("flock");
    return 1;
  }
  return 0;
}

static int Chmod(void) {
  int i, mode;
  char *endptr;
  if (n < 3) {
    return NeedArgument("chmod");
  }
  mode = strtol(args[1], &endptr, 8);
  if (*endptr) {
    tinyprint(2, "chmod: bad octal mode\n", NULL);
    return 1;
  }
  for (i = 2; i < n; ++i) {
    if (chmod(args[i], mode)) {
      perror(args[i]);
      return 1;
    }
  }
  return 0;
}

static int Pwd(void) {
  int got;
  char path[PATH_MAX];
  if ((got = __getcwd(path, PATH_MAX - 1)) != -1) {
    path[got - 1] = '\n';
    path[got] = 0;
    Write(1, path);
    return 0;
  } else {
    perror("pwd");
    return 1;
  }
}

static int CatDump(const char *path, int fd, bool dontclose) {
  ssize_t rc;
  char buf[512];
  for (;;) {
    rc = read(fd, buf, sizeof(buf));
    if (rc == -1) {
      perror(path);
      if (!dontclose) {
        close(fd);
      }
      return 1;
    }
    if (!rc) break;
    rc = write(1, buf, rc);
    if (rc == -1) {
      perror("write");
      if (!dontclose) {
        close(fd);
      }
      return 1;
    }
  }
  if (!dontclose && close(fd)) {
    perror(path);
    return 1;
  }
  return 0;
}

static int Cat(void) {
  int i, fd, rc;
  if (n < 2) {
    return CatDump("<stdin>", 0, true);
  } else {
    for (i = 1; i < n; ++i) {
      bool dontclose = false;
      if (args[i][0] == '-' && !args[i][1]) {
        dontclose = true;
        fd = 0;
      } else if ((fd = open(args[i], O_RDONLY)) == -1) {
        perror(args[i]);
        return 1;
      }
      if ((rc = CatDump(args[i], fd, dontclose))) {
        return rc;
      }
    }
  }
  return 0;
}

static int Mktemp(void) {
  int fd;
  char template[PATH_MAX + 1];
  if (n == 2) {
    strlcpy(template, args[1], sizeof(template));
  } else {
    strlcpy(template, __get_tmpdir(), sizeof(template));
    strlcat(template, "tmp.XXXXXX", sizeof(template));
  }
  if ((fd = mkstemp(template)) == -1) {
    perror("mkstemp");
    return 1;
  }
  strlcat(template, "\n", sizeof(template));
  Write(1, template);
  close(fd);
  return 0;
}

static int Read(void) {
  unsigned char c;
  int i, j, rc = 1;
  for (i = 1; i < n; ++i) {
    if (args[i][0] != '-') break;
    if (args[i][1] == 'p' && !args[i][2] && i + 1 < n) {
      Write(1, args[++i]);
    }
  }
  if (i >= n) return 1;
  for (j = 0; args[i][j]; ++j) {
    Append(args[i][j]);
  }
  Append('=');
  while (read(0, &c, 1) > 0) {
    if (c == '\n') break;
    Append(c);
    rc = 0;
  }
  PutEnv(envs, Finish());
  return rc;
}

static int Cd(void) {
  const char *s;
  if ((s = n > 1 ? args[1] : __getenv(envs, "HOME").s)) {
    if (!chdir(s)) {
      return 0;
    } else {
      perror(s);
      return 1;
    }
  } else {
    return NeedArgument("cd");
  }
}

static int Mkdir(void) {
  int i, j;
  int mode = 0755;
  const char *arg;
  int (*mkdir_impl)(const char *, unsigned) = mkdir;
  for (i = 1; i < n; ++i) {
    if (args[i][0] == '-' && args[i][1]) {
      if (args[i][1] == '-' && !args[i][2]) {
        ++i;  // rm -- terminates option parsing
        break;
      }
      for (j = 1; j < args[i][j]; ++j) {
        if (args[i][j] == 'p') {
          mkdir_impl = makedirs;  // mkdir -p creates parents
          continue;
        }
        if ((arg = GetOptArg('m', &i, j))) {
          mode = strtol(arg, 0, 8);  // mkdir -m OCTAL sets mode
          break;
        }
        char option[2] = {args[i][j]};
        tinyprint(2, "mkdir", ": illegal option -- ", option, "\n", NULL);
        return 1;
      }
    } else {
      break;
    }
  }
  if (i == n) {
    return NeedArgument("mkdir");
  }
  for (; i < n; ++i) {
    if (mkdir_impl(args[i], mode)) {
      perror(args[i]);
      return 1;
    }
  }
  return 0;
}

static int Kill(void) {
  int sig, rc = 0, i = 1;
  if (i < n && args[i][0] == '-') {
    sig = GetSignalByName(args[i++] + 1);
    if (!sig) return -1;  // fallback to system kill command
  } else {
    sig = SIGTERM;
  }
  for (; i < n; ++i) {
    if (kill(atoi(args[i]), sig)) {
      perror("kill");
      rc = 1;
    }
  }
  return rc;
}

static int Toupper(void) {
  int i, n;
  char b[512];
  while ((n = read(0, b, 512)) > 0) {
    for (i = 0; i < n; ++i) {
      b[i] = toupper(b[i]);
    }
    write(1, b, n);
  }
  return 0;
}

static int Usleep(void) {
  int f;
  struct timespec t;
  if (n > 1) {
    f = 0;
    t = timespec_frommicros(atoi(args[1]));
  } else {
    f = TIMER_ABSTIME;
    t = timespec_max;
  }
  return clock_nanosleep(0, f, &t, 0);
}

static int Test(void) {
  int w, m = n;
  struct stat st;
  if (m && READ16LE(args[m - 1]) == READ16LE("]")) --m;
  if (m == 4) {
    w = READ32LE(args[2]) & 0x00ffffff;
    if ((w & 65535) == READ16LE("=")) return !!strcmp(args[1], args[3]);
    if (w == READ24("==")) return !!strcmp(args[1], args[3]);
    if (w == READ24("!=")) return !strcmp(args[1], args[3]);
  } else if (m == 3) {
    w = READ32LE(args[1]) & 0x00ffffff;
    if (w == READ24("-n")) return !(strlen(args[2]) > 0);
    if (w == READ24("-z")) return !(strlen(args[2]) == 0);
    if (w == READ24("-e")) return !!stat(args[2], &st);
    if (w == READ24("-f")) return !(!stat(args[2], &st) && S_ISREG(st.st_mode));
    if (w == READ24("-d")) return !(!stat(args[2], &st) && S_ISDIR(st.st_mode));
    if (w == READ24("-h")) return !(!stat(args[2], &st) && S_ISLNK(st.st_mode));
  }
  return -1;  // fall back to system test command
}

static int Rm(void) {
  int i, j;
  bool force = false;
  for (i = 1; i < n; ++i) {
    if (args[i][0] == '-' && args[i][1]) {
      if (args[i][1] == '-' && !args[i][2]) {
        ++i;  // rm -- terminates option parsing
        break;
      }
      for (j = 1; j < args[i][j]; ++j) {
        if (args[i][j] == 'f') {
          force = true;  // rm -f forces removal
          continue;
        }
        char option[2] = {args[i][j]};
        tinyprint(2, "rm", ": illegal option -- ", option, "\n", NULL);
        return 1;
      }
    } else {
      break;
    }
  }
  if (i == n) {
    return NeedArgument("rm");
  }
  for (; i < n; ++i) {
    struct stat st;
    if ((!force && (lstat(args[i], &st) ||
                    (!S_ISLNK(st.st_mode) && access(args[i], W_OK)))) ||
        unlink(args[i])) {
      if (force && errno == ENOENT) continue;
      perror(args[i]);
      return 1;
    }
  }
  return 0;
}

static int Rmdir(void) {
  int i;
  for (i = 1; i < n; ++i) {
    if (rmdir(args[i])) {
      perror(args[i]);
      return 1;
    }
  }
  return 0;
}

static int Touch(void) {
  int i;
  if (n > 1 && args[1][0] != '-') {
    for (i = 1; i < n; ++i) {
      if (touch(args[i], 0644)) {
        perror(args[i]);
        return 1;
      }
    }
    return 0;
  } else {
    return -1;  // fall back to system rmdir command
  }
}

static int Shift(int i) {
  if (i <= n) {
    memmove(args, args + i, (n - i + 1) * sizeof(*args));
    n -= i;
  }
  return 0;
}

static int Fake(int main(int, char **), bool wantexec) {
  int pid;
  if (wantexec) {
    goto RunProgram;
  }
  if ((pid = fork()) == -1) {
    perror("fork");
    return 127;
  }
  if (!pid) {
  RunProgram:
    // TODO(jart): Maybe nuke stdio too?
    if (_weaken(optind)) {
      *_weaken(optind) = 1;
    }
    environ = envs;
    exit(main(n, args));
  }
  return Waiter(pid);
}

static int Env(void) {
  int i, j;
  const char *arg;
  char term = '\n';
  for (i = 1; i < n; ++i) {
    if (args[i][0] == '-') {
      if (!args[i][1]) {
        envs[0] = 0;  // env - clears environment
        continue;
      }
      for (j = 1; j < args[i][j]; ++j) {
        if (args[i][j] == 'i') {
          envs[0] = 0;  // env -i clears environment
          continue;
        }
        if (args[i][j] == '0') {
          term = 0;  // env -0 uses '\0' line separator
          continue;
        }
        if ((arg = GetOptArg('u', &i, j))) {
          UnsetEnv(envs, arg);  // env -u VAR removes variable
          break;
        }
        char option[2] = {args[i][j]};
        tinyprint(2, "env", ": illegal option -- ", option, "\n", NULL);
        return 1;
      }
      continue;
    }
    if (strchr(args[i], '=')) {
      PutEnv(envs, args[i]);
    } else {
      Shift(i);
      return ShellSpawn();
    }
  }
  for (i = 0; envs[i]; ++i) {
    Write(1, envs[i]);
    write(1, &term, 1);
  }
  return 0;
}

static wontreturn void Exec(void) {
  Shift(1);
  if (!ShellExec()) {
    _Exit(0);  // can happen for builtins
  } else {
    perror("exec");
    _Exit(127);  // sh exec never returns
  }
}

static int TryBuiltin(bool wantexec) {
  if (!n) return exitstatus;
  if (!strcmp(args[0], "exit")) Exit();
  if (!strcmp(args[0], "exec")) Exec();
  if (!strcmp(args[0], "cd")) return Cd();
  if (!strcmp(args[0], "rm")) return Rm();
  if (!strcmp(args[0], "[")) return Test();
  if (!strcmp(args[0], "cat")) return Cat();
  if (!strcmp(args[0], "env")) return Env();
  if (!strcmp(args[0], "pwd")) return Pwd();
  if (!strcmp(args[0], "wait")) return Wait();
  if (!strcmp(args[0], "echo")) return Echo();
  if (!strcmp(args[0], "read")) return Read();
  if (!strcmp(args[0], "true")) return True();
  if (!strcmp(args[0], "test")) return Test();
  if (!strcmp(args[0], "kill")) return Kill();
  if (!strcmp(args[0], "pause")) return Pause();
  if (!strcmp(args[0], "flock")) return Flock();
  if (!strcmp(args[0], "chmod")) return Chmod();
  if (!strcmp(args[0], "touch")) return Touch();
  if (!strcmp(args[0], "rmdir")) return Rmdir();
  if (!strcmp(args[0], "mkdir")) return Mkdir();
  if (!strcmp(args[0], "false")) return False();
  if (!strcmp(args[0], "mktemp")) return Mktemp();
  if (!strcmp(args[0], "usleep")) return Usleep();
  if (!strcmp(args[0], "toupper")) return Toupper();
  if (_weaken(_tr) && !strcmp(args[0], "tr")) {
    return Fake(_weaken(_tr), wantexec);
  }
  if (_weaken(_sed) && !strcmp(args[0], "sed")) {
    return Fake(_weaken(_sed), wantexec);
  }
  if (_weaken(_awk) && !strcmp(args[0], "awk")) {
    return Fake(_weaken(_awk), wantexec);
  }
  if (_weaken(_curl) && !strcmp(args[0], "curl")) {
    return Fake(_weaken(_curl), wantexec);
  }
  return -1;
}

static int ShellExec(void) {
  int rc;
  if ((rc = TryBuiltin(true)) == -1) {
    rc = SystemExec();
  }
  return (n = 0), rc;
}

static void Pipe(void) {
  int pid, pfds[2];
  if (pipe2(pfds, O_CLOEXEC)) {
    perror("pipe");
    _Exit(127);
  }
  if ((pid = fork()) == -1) {
    perror("fork");
    _Exit(127);
  }
  if (!pid) {
    unassert(dup2(pfds[1], 1) == 1);
    // we can't rely on cloexec because builtins
    if (pfds[0] != 1) unassert(!close(pfds[0]));
    if (pfds[1] != 1) unassert(!close(pfds[1]));
    _Exit(ShellExec());
  }
  unassert(dup2(pfds[0], 0) == 0);
  if (pfds[0] != 0) unassert(!close(pfds[0]));
  if (pfds[1] != 0) unassert(!close(pfds[1]));
  n = 0;
}

static int ShellSpawn(void) {
  int rc, pid;
  if ((rc = TryBuiltin(false)) == -1) {
    switch ((pid = fork())) {
      case 0:
        _Exit(SystemExec());
      default:
        rc = Waiter(pid);
        break;
      case -1:
        perror("fork");
        rc = 127;
        break;
    }
  }
  return (n = 0), rc;
}

static void ShellSpawnAsync(void) {
  switch ((lastchild = fork())) {
    case 0:
      _Exit(ShellExec());
    default:
      break;
    case -1:
      perror("fork");
      break;
  }
  n = 0;
}

static const char *IntToStr(int x) {
  static char ibuf[12];
  FormatInt32(ibuf, x);
  return ibuf;
}

static const char *GetVar(const char *key) {
  static char vbuf[PATH_MAX];
  if (key[0] == '$' && !key[1]) {
    return IntToStr(getpid());
  } else if (key[0] == '!' && !key[1]) {
    return IntToStr(lastchild);
  } else if (key[0] == '?' && !key[1]) {
    return IntToStr(exitstatus);
  } else if (!strcmp(key, "PWD")) {
    npassert(__getcwd(vbuf, sizeof(vbuf)) != -1);
    return vbuf;
  } else if (!strcmp(key, "UID")) {
    FormatInt32(vbuf, getuid());
    return vbuf;
  } else if (!strcmp(key, "GID")) {
    FormatInt32(vbuf, getgid());
    return vbuf;
  } else if (!strcmp(key, "EUID")) {
    FormatInt32(vbuf, geteuid());
    return vbuf;
  } else {
    return __getenv(envs, key).s;
  }
}

static bool IsVarName(int c) {
  return isalnum(c) || c == '_' ||
         (!vari && (c == '?' || c == '!' || c == '$'));
}

static bool CopyVar(void) {
  size_t j;
  const char *s;
  if (IsVarName(*p)) {
    npassert(vari + 1 < sizeof(var));
    var[vari++] = *p;
    var[vari] = 0;
    return false;
  }
  if ((s = GetVar(var))) {
    if ((j = strlen(s))) {
      npassert(q + j < argbuf + sizeof(argbuf));
      q = mempcpy(q, s, j);
    }
  }
  return true;
}

static char *Tokenize(void) {
  int c, t, rc;
  for (t = STATE_WHITESPACE;; ++p) {
    switch (t) {

      case STATE_WHITESPACE:
        if (!*p) return 0;
        if (*p == ' ' || *p == '\t' || *p == '\n' ||
            (p[0] == '\\' && p[1] == '\n')) {
          continue;
        }
        t = STATE_CMD;
        // fallthrough

      case STATE_CMD:
        if (unsupported[*p & 255]) {
          UnsupportedSyntax(*p);
        }
        if (!*p || *p == ' ' || *p == '\t') {
          return Finish();
        } else if (*p == '"') {
          t = STATE_QUOTED;
        } else if (*p == '\'') {
          t = STATE_SINGLE;
        } else if (*p == '$') {
          t = STATE_VAR;
          var[(vari = 0)] = 0;
        } else if (*p == '\\') {
          if (!p[1]) UnsupportedSyntax(*p);
          Append(*++p);
        } else if (*p == '=') {
          if (!n && q > r) assign = r;
          Append(*p);
        } else if (*p == '|') {
          if (q > r) {
            return Finish();
          } else if (p[1] == '|') {
            rc = ShellSpawn();
            if (!rc) {
              _Exit(0);
            } else {
              ++p;
              t = STATE_WHITESPACE;
            }
          } else {
            Pipe();
            t = STATE_WHITESPACE;
          }
        } else if (*p == ';' || *p == '\n') {
          if (q > r) {
            return Finish();
          } else {
            exitstatus = ShellSpawn();
            t = STATE_WHITESPACE;
          }
        } else if (*p == '>') {
          Append(*p);
          if (p[1] == '&') {
            Append(*++p);
          }
        } else if (*p == '&') {
          if (q > r) {
            return Finish();
          } else if (p[1] == '&') {
            rc = ShellSpawn();
            if (!rc) {
              ++p;
              t = STATE_WHITESPACE;
            } else {
              _Exit(rc);
            }
          } else {
            ShellSpawnAsync();
            t = STATE_WHITESPACE;
          }
        } else {
          Append(*p);
        }
        break;

      case STATE_VAR:
        // XXX: we need to find a simple elegant way to break up
        //      unquoted variable expansions into multiple args.
        if (CopyVar()) t = STATE_CMD, --p;
        break;

      case STATE_SINGLE:
        if (!*p) goto UnterminatedString;
        if (*p == '\'') {
          t = STATE_CMD;
        } else {
          *q++ = *p;
        }
        break;

      UnterminatedString:
        tinyprint(2, "cmd: error: unterminated string\n", NULL);
        _Exit(6);

      case STATE_QUOTED_VAR:
        if (!*p) goto UnterminatedString;
        if (CopyVar()) t = STATE_QUOTED, --p;
        break;

      case STATE_QUOTED:
        if (!*p) goto UnterminatedString;
        if (*p == '"') {
          t = STATE_CMD;
        } else if (p[0] == '$') {
          t = STATE_QUOTED_VAR;
          var[(vari = 0)] = 0;
        } else if (p[0] == '\\') {
          switch ((c = *++p)) {
            case 0:
              UnsupportedSyntax('\\');
            case '\n':
              break;
            case '$':
            case '`':
            case '"':
              *q++ = c;
              break;
            default:
              *q++ = '\\';
              *q++ = c;
              break;
          }
        } else {
          *q++ = *p;
        }
        break;

      default:
        __builtin_unreachable();
    }
  }
}

static const char *GetRedirectArg(const char *prog, const char *arg, int n) {
  if (arg[n]) {
    return arg + n;
  } else if ((arg = Tokenize())) {
    return arg;
  } else {
    tinyprint(2, prog, ": error: redirect missing path\n", NULL);
    _Exit(14);
  }
}

int _cocmd(int argc, char **argv, char **envp) {
  size_t i;
  char *arg;
  size_t globCount = 0;
  int globFlags = 0;
  glob_t globTheBuilder;
  prog = argc > 0 ? argv[0] : "cocmd.com";

  for (i = 1; i < 32; ++i) {
    unsupported[i] = true;
  }
  unsupported['\t'] = false;
  unsupported['\n'] = false;
  unsupported[0177] = true;
  unsupported['~'] = true;
  unsupported['`'] = true;
  unsupported['#'] = true;
  unsupported['('] = true;
  unsupported[')'] = true;
  unsupported['{'] = true;
  unsupported['}'] = false;  // Perl t/op/exec.t depends on unpaired } being
                             // passed from the shell to Perl
  if (!_weaken(glob)) {
    unsupported['*'] = true;
    unsupported['?'] = true;
  }

  if (argc >= 3 && !strcmp(argv[1], "--")) {
    for (i = 2; i < argc; ++i) {
      args[n++] = argv[i];
    }
    _Exit(ShellExec());
  }

  if (argc != 3) {
    tinyprint(2, prog, ": error: wrong number of args\n", NULL);
    _Exit(10);
  }

  if (strcmp(argv[1], "-c")) {
    tinyprint(2, prog, ": error: argv[1] should -c\n", NULL);
    _Exit(11);
  }

  p = cmd = argv[2];
  if (strlen(cmd) >= ARG_MAX) {
    tinyprint(2, prog, ": error: cmd too long: ", cmd, "\n", NULL);
    _Exit(12);
  }

  // copy environment variables
  int envi = 0;
  if (envp) {
    for (; envp[envi]; ++envi) {
      npassert(envi + 1 < ARRAYLEN(envs));
      envs[envi] = envp[envi];
    }
  }
  envs[envi] = 0;

  // get command line arguments
  n = 0;
  r = q = argbuf;
  while ((arg = Tokenize())) {
    if (arg == TOMBSTONE) continue;
    if (n + 1 < ARRAYLEN(args)) {
      if (isdigit(arg[0]) && arg[1] == '>' && arg[2] == '&' &&
          isdigit(arg[3])) {
        dup2(arg[3] - '0', arg[0] - '0');
      } else if (arg[0] == '>' && arg[1] == '&' && isdigit(arg[2])) {
        dup2(arg[2] - '0', 1);
      } else if (isdigit(arg[0]) && arg[1] == '>' && arg[2] == '>') {
        Open(GetRedirectArg(prog, arg, 3), arg[0] - '0',
             O_WRONLY | O_CREAT | O_APPEND);
      } else if (arg[0] == '>' && arg[1] == '>') {
        Open(GetRedirectArg(prog, arg, 2), 1, O_WRONLY | O_CREAT | O_APPEND);
      } else if (isdigit(arg[0]) && arg[1] == '>') {
        Open(GetRedirectArg(prog, arg, 2), arg[0] - '0',
             O_WRONLY | O_CREAT | O_TRUNC);
      } else if (arg[0] == '>') {
        Open(GetRedirectArg(prog, arg, 1), 1, O_WRONLY | O_CREAT | O_TRUNC);
      } else if (arg[0] == '<') {
        Open(GetRedirectArg(prog, arg, 1), 0, O_RDONLY);
      } else {
        int globrc = GLOB_NOMATCH;
        if (_weaken(glob)) {
          globrc = _weaken(glob)(arg, globFlags, NULL, &globTheBuilder);
          if (globrc == 0) {
            for (; globCount < globTheBuilder.gl_pathc; globCount++) {
              args[n++] = globTheBuilder.gl_pathv[globCount];
            }
          } else if (globrc != GLOB_NOMATCH) {
            tinyprint(2, prog, ": error: with glob\n", NULL);
            _Exit(16);
          }
          globFlags |= GLOB_APPEND;
        }
        if (globrc == GLOB_NOMATCH) {
          args[n++] = arg;
        }
        args[n] = 0;
      }
    } else {
      tinyprint(2, prog, ": error: too many args\n", NULL);
      _Exit(13);
    }
  }

  return ShellExec();
}
