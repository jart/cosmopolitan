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
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/itoa.h"
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/append.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/consts/sig.h"
#include "libc/x/x.h"
#include "third_party/double-conversion/wrapper.h"

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

static char *p;
static char *q;
static int vari;
static size_t n;
static char *cmd;
static char var[32];
static int lastchild;
static int exitstatus;
static char *args[8192];
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
    strlcat(errbuf, s, sizeof(argbuf));
  } while ((s = va_arg(va, const char *)));
  strlcat(errbuf, "\n", sizeof(argbuf));
  Write(2, errbuf);
  va_end(va);
}

static wontreturn void Wexit(int rc, const char *s, ...) {
  va_list va;
  va_start(va, s);
  errbuf[0] = 0;
  do {
    strlcat(errbuf, s, sizeof(argbuf));
  } while ((s = va_arg(va, const char *)));
  Write(2, errbuf);
  va_end(va);
  _Exit(rc);
}

static wontreturn void UnsupportedSyntax(unsigned char c) {
  char cbuf[2];
  char ibuf[13];
  cbuf[0] = c;
  cbuf[1] = 0;
  FormatOctal32(ibuf, c, true);
  Wexit(4, prog, ": unsupported shell syntax '", cbuf, "' (", ibuf, "): ", cmd,
        "\n", 0);
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
  _npassert(args[0][0]);
  if (!n) Wexit(5, prog, ": error: too few args\n", 0);
  execvp(args[0], args);
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
  char *b = 0;
  unsigned char c;
  int a = 1, rc = 1;
  if (n >= 3 && !strcmp(args[1], "-p")) {
    Write(1, args[2]);
    a = 3;
  }
  appendr(&b, 0);
  while (read(0, &c, 1) > 0) {
    if (c == '\n') {
      rc = 0;
      break;
    }
    appendw(&b, c);
  }
  if (a < n) {
    setenv(args[1], b, true);
  }
  free(b);
  return rc;
}

static int Cd(void) {
  const char *s = n > 1 ? args[1] : getenv("HOME");
  if (s) {
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
    if (!sig) return 1;
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
  struct timespec t, *p = 0;
  if (n > 1) {
    t = _timespec_frommicros(atoi(args[1]));
    p = &t;
  }
  return clock_nanosleep(0, 0, p, 0);
}

static int Test(void) {
  struct stat st;
  if (n && !strcmp(args[n - 1], "]")) --n;
  if (n == 4 && !strcmp(args[2], "=")) {
    return !!strcmp(args[1], args[3]);
  } else if (n == 4 && !strcmp(args[2], "!=")) {
    return !strcmp(args[1], args[3]);
  } else if (n == 3 && !strcmp(args[1], "-n")) {
    return !(strlen(args[2]) > 0);
  } else if (n == 3 && !strcmp(args[1], "-z")) {
    return !(strlen(args[2]) == 0);
  } else if (n == 3 && !strcmp(args[1], "-e")) {
    return !!stat(args[2], &st);
  } else if (n == 3 && !strcmp(args[1], "-f")) {
    return !stat(args[2], &st) && S_ISREG(st.st_mode);
  } else if (n == 3 && !strcmp(args[1], "-d")) {
    return !stat(args[2], &st) && S_ISDIR(st.st_mode);
  } else if (n == 3 && !strcmp(args[1], "-h")) {
    return !stat(args[2], &st) && S_ISLNK(st.st_mode);
  } else {
    return 1;
  }
}

static int TryBuiltin(void) {
  if (!n) return 0;
  if (!strcmp(args[0], "exit")) Exit();
  if (!strcmp(args[0], "cd")) return Cd();
  if (!strcmp(args[0], "[")) return Test();
  if (!strcmp(args[0], "wait")) return Wait();
  if (!strcmp(args[0], "echo")) return Echo();
  if (!strcmp(args[0], "read")) return Read();
  if (!strcmp(args[0], "true")) return True();
  if (!strcmp(args[0], "test")) return Test();
  if (!strcmp(args[0], "kill")) return Kill();
  if (!strcmp(args[0], "mkdir")) return Mkdir();
  if (!strcmp(args[0], "false")) return False();
  if (!strcmp(args[0], "usleep")) return Usleep();
  if (!strcmp(args[0], "toupper")) return Toupper();
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

static const char *GetEnv(const char *key) {
  if (key[0] == '$' && !key[1]) {
    return IntToStr(getpid());
  } else if (key[0] == '!' && !key[1]) {
    return IntToStr(lastchild);
  } else if (key[0] == '?' && !key[1]) {
    return IntToStr(exitstatus);
  } else {
    return getenv(key);
  }
}

static bool IsVarName(int c) {
  return isalnum(c) || c == '_' ||
         (!vari && (c == '?' || c == '!' || c == '$'));
}

static inline void Append(int c) {
  _unassert(q + 1 < argbuf + sizeof(argbuf));
  *q++ = c;
}

static char *Tokenize(void) {
  char *r;
  const char *s;
  int c, t, j, k, rc;
  for (r = q, t = STATE_WHITESPACE;; ++p) {
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
          Append(0);
          return r;
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
        } else if (*p == '|') {
          if (q > r) {
            Append(0);
            return r;
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
        } else if (*p == ';') {
          if (q > r) {
            Append(0);
            return r;
          } else {
            Run();
            t = STATE_WHITESPACE;
          }
        } else if (*p == '&') {
          if (q > r) {
            Append(0);
            return r;
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
        if (IsVarName(*p)) {
          _unassert(vari + 1 < sizeof(var));
          var[vari++] = *p;
          var[vari] = 0;
        } else {
          // XXX: we need to find a simple elegant way to break up
          //      unquoted variable expansions into multiple args.
          if ((s = GetEnv(var))) {
            if ((j = strlen(s))) {
              _unassert(q + j < argbuf + sizeof(argbuf));
              q = mempcpy(q, s, j);
            }
          }
          --p;
          t = STATE_CMD;
        }
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

      case STATE_QUOTED_VAR:
        if (!*p) goto UnterminatedString;
        if (IsVarName(*p)) {
          _unassert(vari + 1 < sizeof(var));
          var[vari++] = *p;
          var[vari] = 0;
        } else {
          if ((s = GetEnv(var))) {
            if ((j = strlen(s))) {
              _unassert(q + j < argbuf + sizeof(argbuf));
              q = mempcpy(q, s, j);
            }
          }
          --p;
          t = STATE_QUOTED;
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

int cocmd(int argc, char *argv[]) {
  char *arg;
  size_t i, j;
  prog = argc > 0 ? argv[0] : "cocmd.com";

  for (i = 1; i < 32; ++i) {
    unsupported[i] = true;
  }
  unsupported['\t'] = false;
  unsupported[0177] = true;
  unsupported['~'] = true;
  unsupported['`'] = true;
  unsupported['#'] = true;
  unsupported['*'] = true;
  unsupported['('] = true;
  unsupported[')'] = true;
  unsupported['{'] = true;
  unsupported['}'] = true;
  unsupported['?'] = true;

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

  n = 0;
  q = argbuf;
  while ((arg = Tokenize())) {
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
        args[n++] = arg;
        args[n] = 0;
      }
    } else {
      Wexit(13, prog, ": error: too many args\n", 0);
    }
  }

  Launch();
}
