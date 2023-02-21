/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/intrin/_getenv.internal.h"
#include "libc/intrin/bits.h"
#include "libc/intrin/weaken.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/timer.h"
#include "third_party/awk/cmd.h"
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
static int envi;
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
static char errbuf[512];
static char argbuf[ARG_MAX];
static bool unsupported[256];

static ssize_t Write(int fd, const char *s) {
  return write(fd, s, strlen(s));
}

static void Log(const char *s, ...) {
  va_list va;
  va_start(va, s);
  errbuf[0] = 0;
  do {
    strlcat(errbuf, s, sizeof(errbuf));
  } while ((s = va_arg(va, const char *)));
  strlcat(errbuf, "\n", sizeof(errbuf));
  Write(2, errbuf);
  va_end(va);
}

static wontreturn void Wexit(int rc, const char *s, ...) {
  va_list va;
  va_start(va, s);
  errbuf[0] = 0;
  do {
    strlcat(errbuf, s, sizeof(errbuf));
  } while ((s = va_arg(va, const char *)));
  Write(2, errbuf);
  va_end(va);
  _Exit(rc);
}

static wontreturn void UnsupportedSyntax(unsigned char c) {
  char ibuf[13], cbuf[2] = {c};
  FormatOctal32(ibuf, c, true);
  Wexit(4, prog, ": unsupported syntax '", cbuf, "' (", ibuf, "): ", cmd, "\n",
        0);
}

static wontreturn void SysExit(int rc, const char *call, const char *thing) {
  int err;
  char ibuf[12];
  const char *estr;
  err = errno;
  FormatInt32(ibuf, err);
  estr = _strerdoc(err);
  if (!estr) estr = "EUNKNOWN";
  Wexit(rc, thing, ": ", call, "() failed: ", estr, " (", ibuf, ")\n", 0);
}

static void Open(const char *path, int fd, int flags) {
  const char *err;
  close(fd);
  if (open(path, flags, 0644) == -1) {
    SysExit(7, "open", path);
  }
}

static wontreturn void Exec(void) {
  _unassert(args[0][0]);
  if (!n) Wexit(5, prog, ": error: too few args\n", 0);
  execvpe(args[0], args, envs);
  SysExit(127, "execve", args[0]);
}

static int GetSignalByName(const char *s) {
  for (int i = 0; kSignalNames[i].x != MAGNUM_TERMINATOR; ++i) {
    if (!strcmp(s, MAGNUM_STRING(kSignalNames, i) + 3)) {
      return MAGNUM_NUMBER(kSignalNames, i);
    }
  }
  return 0;
}

static void PutEnv(char **p, const char *kv) {
  struct Env e;
  e = _getenv(p, kv);
  p[e.i] = kv;
  if (!e.s) p[e.i + 1] = 0;
}

static void Append(int c) {
  _npassert(q + 1 < argbuf + sizeof(argbuf));
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

static int True(void) {
  return 0;
}

static int False(void) {
  return 1;
}

static wontreturn void Exit(void) {
  _Exit(n > 1 ? atoi(args[1]) : 0);
}

static int Wait(void) {
  char ibuf[12];
  int e, rc, ws, pid;
  if (n > 1) {
    if (waitpid(atoi(args[1]), &ws, 0) == -1) {
      SysExit(22, "waitpid", prog);
    }
    rc = WIFEXITED(ws) ? WEXITSTATUS(ws) : 128 + WTERMSIG(ws);
    exitstatus = rc;
  } else {
    for (e = errno;;) {
      if (waitpid(-1, &ws, 0) == -1) {
        if (errno == ECHILD) {
          errno = e;
          break;
        }
        SysExit(22, "waitpid", prog);
      }
    }
    rc = 0;
  }
  return rc;
}

static int Echo(void) {
  int i = 1;
  bool once = false;
  const char *l = " ";
  if (i < n && !strcmp(args[i], "-l")) {
    ++i, l = "\n";
  }
  for (; i < n; ++i) {
    if (once) {
      Write(1, l);
    } else {
      once = true;
    }
    Write(1, args[i]);
  }
  Write(1, "\n");
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
  if ((s = n > 1 ? args[1] : _getenv(envs, "HOME").s)) {
    if (!chdir(s)) {
      return 0;
    } else {
      Log("chdir: ", s, ": ", _strerdoc(errno), 0);
      return 1;
    }
  } else {
    Log("chdir: missing argument", 0);
    return 1;
  }
}

static int Mkdir(void) {
  int i = 1;
  int (*f)(const char *, unsigned) = mkdir;
  if (n >= 3 && !strcmp(args[1], "-p")) ++i, f = makedirs;
  for (; i < n; ++i) {
    if (f(args[i], 0755)) {
      Log("mkdir: ", args[i], ": ", _strerdoc(errno), 0);
      return errno;
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
      Log("kill: ", args[i], ": ", _strerdoc(errno), 0);
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
  int i;
  if (n > 1 && args[1][0] != '-') {
    for (i = 1; i < n; ++i) {
      if (unlink(args[i])) {
        Log("rm: ", args[i], ": ", _strerdoc(errno), 0);
        return 1;
      }
    }
    return 0;
  } else {
    return -1;  // fall back to system rm command
  }
}

static int Rmdir(void) {
  int i;
  if (n > 1 && args[1][0] != '-') {
    for (i = 1; i < n; ++i) {
      if (rmdir(args[i])) {
        Log("rmdir: ", args[i], ": ", _strerdoc(errno), 0);
        return 1;
      }
    }
    return 0;
  } else {
    return -1;  // fall back to system rmdir command
  }
}

static int Touch(void) {
  int i;
  if (n > 1 && args[1][0] != '-') {
    for (i = 1; i < n; ++i) {
      if (touch(args[i], 0644)) {
        Log("touch: ", args[i], ": ", _strerdoc(errno), 0);
        return 1;
      }
    }
    return 0;
  } else {
    return -1;  // fall back to system rmdir command
  }
}

static int Fake(int main(int, char **)) {
  int exitstatus, ws, pid;
  if ((pid = fork()) == -1) SysExit(21, "vfork", prog);
  if (!pid) {
    // TODO(jart): Maybe nuke stdio state somehow?
    environ = envs;
    exit(main(n, args));
  }
  if (waitpid(pid, &ws, 0) == -1) SysExit(22, "waitpid", prog);
  exitstatus = WIFEXITED(ws) ? WEXITSTATUS(ws) : 128 + WTERMSIG(ws);
  return n = 0, exitstatus;
}

static int TryBuiltin(void) {
  if (!n) return 0;
  if (!strcmp(args[0], "exit")) Exit();
  if (!strcmp(args[0], "cd")) return Cd();
  if (!strcmp(args[0], "rm")) return Rm();
  if (!strcmp(args[0], "[")) return Test();
  if (!strcmp(args[0], "wait")) return Wait();
  if (!strcmp(args[0], "echo")) return Echo();
  if (!strcmp(args[0], "read")) return Read();
  if (!strcmp(args[0], "true")) return True();
  if (!strcmp(args[0], "test")) return Test();
  if (!strcmp(args[0], "kill")) return Kill();
  if (!strcmp(args[0], "touch")) return Touch();
  if (!strcmp(args[0], "rmdir")) return Rmdir();
  if (!strcmp(args[0], "mkdir")) return Mkdir();
  if (!strcmp(args[0], "false")) return False();
  if (!strcmp(args[0], "usleep")) return Usleep();
  if (!strcmp(args[0], "toupper")) return Toupper();
  if (_weaken(_tr) && !strcmp(args[0], "tr")) return Fake(_weaken(_tr));
  if (_weaken(_sed) && !strcmp(args[0], "sed")) return Fake(_weaken(_sed));
  if (_weaken(_awk) && !strcmp(args[0], "awk")) return Fake(_weaken(_awk));
  if (_weaken(_curl) && !strcmp(args[0], "curl")) return Fake(_weaken(_curl));
  return -1;
}

static wontreturn void Launch(void) {
  int rc;
  if ((rc = TryBuiltin()) != -1) _Exit(rc);
  Exec();
}

static void Pipe(void) {
  int pid, pfds[2];
  if (pipe2(pfds, O_CLOEXEC)) SysExit(8, "pipe2", prog);
  if ((pid = fork()) == -1) SysExit(9, "vfork", prog);
  if (!pid) {
    _unassert(dup2(pfds[1], 1) == 1);
    // we can't rely on cloexec because builtins
    if (pfds[0] != 1) _unassert(!close(pfds[0]));
    if (pfds[1] != 1) _unassert(!close(pfds[1]));
    Launch();
  }
  _unassert(!dup2(pfds[0], 0));
  if (pfds[1]) _unassert(!close(pfds[1]));
  n = 0;
}

static int Run(void) {
  int exitstatus, ws, pid;
  if ((exitstatus = TryBuiltin()) == -1) {
    if ((pid = vfork()) == -1) SysExit(21, "vfork", prog);
    if (!pid) Exec();
    if (waitpid(pid, &ws, 0) == -1) SysExit(22, "waitpid", prog);
    exitstatus = WIFEXITED(ws) ? WEXITSTATUS(ws) : 128 + WTERMSIG(ws);
  }
  n = 0;
  return exitstatus;
}

static void Async(void) {
  if ((lastchild = fork()) == -1) SysExit(21, "vfork", prog);
  if (!lastchild) Launch();
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
    _npassert(getcwd(vbuf, sizeof(vbuf)));
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
    return _getenv(envs, key).s;
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
    _npassert(vari + 1 < sizeof(var));
    var[vari++] = *p;
    var[vari] = 0;
    return false;
  }
  if ((s = GetVar(var))) {
    if ((j = strlen(s))) {
      _npassert(q + j < argbuf + sizeof(argbuf));
      q = mempcpy(q, s, j);
    }
  }
  return true;
}

static char *Tokenize(void) {
  const char *s;
  int c, t, j, k, rc;
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
            rc = Run();
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
            Run();
            t = STATE_WHITESPACE;
          }
        } else if (*p == '&') {
          if (q > r) {
            return Finish();
          } else if (p[1] == '&') {
            rc = Run();
            if (!rc) {
              ++p;
              t = STATE_WHITESPACE;
            } else {
              _Exit(rc);
            }
          } else {
            Async();
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
        Wexit(6, "cmd: error: unterminated string\n", 0);

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
        unreachable;
    }
  }
}

static const char *GetRedirectArg(const char *prog, const char *arg, int n) {
  if (arg[n]) {
    return arg + n;
  } else if ((arg = Tokenize())) {
    return arg;
  } else {
    Wexit(14, prog, ": error: redirect missing path\n", 0);
  }
}

int _cocmd(int argc, char **argv, char **envp) {
  char *arg;
  size_t i, j;
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
  unsupported['}'] = true;
  if (!_weaken(glob)) {
    unsupported['*'] = true;
    unsupported['?'] = true;
  }

  if (argc != 3) {
    Wexit(10, prog, ": error: wrong number of args\n", 0);
  }

  if (strcmp(argv[1], "-c")) {
    Wexit(11, prog, ": error: argv[1] should -c\n", 0);
  }

  p = cmd = argv[2];
  if (strlen(cmd) >= ARG_MAX) {
    Wexit(12, prog, ": error: cmd too long: ", cmd, "\n", 0);
  }

  // copy environment variables
  envi = 0;
  if (envp) {
    for (; envp[envi]; ++envi) {
      _npassert(envi + 1 < ARRAYLEN(envs));
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
            Wexit(16, prog, ": error: with glob\n", 0);
          }
          globFlags |= GLOB_APPEND;
        }
        if (globrc == GLOB_NOMATCH) {
          args[n++] = arg;
        }
        args[n] = 0;
      }
    } else {
      Wexit(13, prog, ": error: too many args\n", 0);
    }
  }

  Launch();
}
