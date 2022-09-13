#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/calls/calls.h"
#include "libc/calls/struct/dirent.h"
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/timespec.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/fmt/itoa.h"
#include "libc/log/appendresourcereport.internal.h"
#include "libc/log/internal.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/append.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/dt.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sig.h"
#include "libc/time/time.h"
#include "libc/x/x.h"
#include "third_party/linenoise/linenoise.h"

/**
 * @fileoverview Cosmopolitan Shell
 *
 * This doesn't have script language features like UNBOURNE.COM but it
 * works on Windows and, unlike CMD.EXE, has CTRL-P, CTRL-R, and other
 * GNU Emacs / Readline keyboard shortcuts.
 *
 * One day we'll have UNBOURNE.COM working on Windows but the code isn't
 * very maintainable sadly.
 */

volatile int gotint;

static void OnInterrupt(int sig) {
  gotint = sig;
}

static void AddUniqueCompletion(linenoiseCompletions *c, char *s) {
  size_t i;
  if (!s) return;
  for (i = 0; i < c->len; ++i) {
    if (!strcmp(s, c->cvec[i])) {
      return;
    }
  }
  c->cvec = realloc(c->cvec, ++c->len * sizeof(*c->cvec));
  c->cvec[c->len - 1] = s;
}

static void CompleteFilename(const char *p, const char *q, const char *b,
                             linenoiseCompletions *c) {
  DIR *d;
  char *buf;
  const char *g;
  struct dirent *e;
  if ((buf = malloc(512))) {
    if ((g = memrchr(p, '/', q - p))) {
      *(char *)mempcpy(buf, p, MIN(g - p, 511)) = 0;
      p = ++g;
    } else {
      strcpy(buf, ".");
    }
    if ((d = opendir(buf))) {
      while ((e = readdir(d))) {
        if (!strcmp(e->d_name, ".")) continue;
        if (!strcmp(e->d_name, "..")) continue;
        if (!strncmp(e->d_name, p, q - p)) {
          snprintf(buf, 512, "%.*s%s%s", p - b, b, e->d_name,
                   e->d_type == DT_DIR ? "/" : "");
          AddUniqueCompletion(c, strdup(buf));
        }
      }
      closedir(d);
    }
    free(buf);
  }
}

static void ShellCompletion(const char *p, linenoiseCompletions *c) {
  bool slashed;
  const char *q, *b;
  for (slashed = false, b = p, q = (p += strlen(p)); p > b; --p) {
    if (p[-1] == '/' && p[-1] == '\\') slashed = true;
    if (!isalnum(p[-1]) &&
        (p[-1] != '.' && p[-1] != '_' && p[-1] != '-' && p[-1] != '+' &&
         p[-1] != '[' && p[-1] != '/' && p[-1] != '\\')) {
      break;
    }
  }
  CompleteFilename(p, q, b, c);
}

static char *ShellHint(const char *p, const char **ansi1, const char **ansi2) {
  char *h = 0;
  linenoiseCompletions c = {0};
  ShellCompletion(p, &c);
  if (c.len == 1) {
    h = strdup(c.cvec[0] + strlen(p));
  }
  linenoiseFreeCompletions(&c);
  return h;
}

static char *MakePrompt(char *p) {
  char *s, buf[256];
  if (!gethostname(buf, sizeof(buf))) {
    p = stpcpy(p, "\e[95m");
    if ((s = getenv("USER"))) {
      p = stpcpy(p, s);
      *p++ = '@';
    }
    p = stpcpy(p, buf);
    *p++ = ':';
  }
  p = stpcpy(p, "\e[96m");
  if ((s = getcwd(buf, sizeof(buf)))) {
    p = stpcpy(p, s);
  }
  return stpcpy(p, "\e[0m>: ");
}

int main(int argc, char *argv[]) {
  bool timeit;
  int64_t nanos;
  struct rusage ru;
  struct timespec ts1, ts2;
  char *prog, path[PATH_MAX];
  sigset_t chldmask, savemask;
  int stdoutflags, stderrflags;
  const char *stdoutpath, *stderrpath;
  int n, rc, ws, pid, child, killcount;
  struct sigaction sa, saveint, savequit;
  char *p, *line, **args, *arg, *start, *state, prompt[1024];
  linenoiseSetFreeHintsCallback(free);
  linenoiseSetHintsCallback(ShellHint);
  linenoiseSetCompletionCallback(ShellCompletion);
  MakePrompt(prompt);
  while ((line = linenoiseWithHistory(prompt, "cmd"))) {
    n = 0;
    start = line;
    if (_startswith(start, "time ")) {
      timeit = true;
      start += 5;
    } else {
      timeit = false;
    }
    stdoutpath = 0;
    stderrpath = 0;
    stdoutflags = 0;
    stderrflags = 0;
    args = xcalloc(1, sizeof(*args));
    while ((arg = strtok_r(start, " \t\r\n", &state))) {
      // cmd >>stdout.txt
      if (arg[0] == '>' && arg[1] == '>') {
        stdoutflags = O_WRONLY | O_APPEND | O_CREAT;
        stdoutpath = arg + 2;
      } else if (arg[0] == '>') {
        // cmd >stdout.txt
        stdoutflags = O_WRONLY | O_CREAT | O_TRUNC;
        stdoutpath = arg + 1;
      } else if (arg[0] == '2' && arg[1] == '>' && arg[2] == '>') {
        // cmd 2>>stderr.txt
        stderrflags = O_WRONLY | O_APPEND | O_CREAT;
        stderrpath = arg + 3;
      } else if (arg[0] == '2' && arg[1] == '>') {
        // cmd 2>stderr.txt
        stderrflags = O_WRONLY | O_CREAT | O_TRUNC;
        stderrpath = arg + 2;
      } else {
        // arg
        args = xrealloc(args, (++n + 1) * sizeof(*args));
        args[n - 1] = arg;
        args[n - 0] = 0;
      }
      start = 0;
    }
    if (n > 0) {
      if ((prog = commandv(args[0], path, sizeof(path)))) {

        // let keyboard interrupts kill child and not shell
        gotint = 0;
        killcount = 0;
        sa.sa_flags = 0;
        sa.sa_handler = SIG_IGN;
        sigemptyset(&sa.sa_mask);
        sigaction(SIGQUIT, &sa, &savequit);
        sa.sa_handler = OnInterrupt;
        sigaction(SIGINT, &sa, &saveint);
        sigemptyset(&chldmask);
        sigaddset(&chldmask, SIGCHLD);
        sigprocmask(SIG_BLOCK, &chldmask, &savemask);

        // record timestamp
        if (timeit) {
          clock_gettime(CLOCK_REALTIME, &ts1);
        }

        // launch process
        if (!(child = vfork())) {
          if (stdoutpath) {
            close(1);
            open(stdoutpath, stdoutflags, 0644);
          }
          if (stderrpath) {
            close(2);
            open(stderrpath, stderrflags, 0644);
          }
          sigaction(SIGINT, &saveint, 0);
          sigaction(SIGQUIT, &savequit, 0);
          sigprocmask(SIG_SETMASK, &savemask, 0);
          execv(prog, args);
          _Exit(127);
        }

        // wait for process
        for (;;) {
          if (gotint) {
            switch (killcount) {
              case 0:
                // ctrl-c
                // we do nothing
                // terminals broadcast sigint to process group
                rc = 0;
                break;
              case 1:
                // ctrl-c ctrl-c
                // we try sending sigterm
                rc = kill(child, SIGTERM);
                break;
              default:
                // ctrl-c ctrl-c ctrl-c ...
                // we use kill -9 as our last resort
                rc = kill(child, SIGKILL);
                break;
            }
            if (rc == -1) {
              fprintf(stderr, "kill failed: %m\n");
              exit(1);
            }
            ++killcount;
            gotint = 0;
          }
          rc = wait4(0, &ws, 0, &ru);
          if (rc != -1) {
            break;
          } else if (errno == EINTR) {
            errno = 0;
          } else {
            fprintf(stderr, "wait failed: %m\n");
            exit(1);
          }
        }

        // print resource consumption for `time` pseudocommand
        if (timeit) {
          clock_gettime(CLOCK_REALTIME, &ts2);
          if (ts2.tv_sec == ts1.tv_sec) {
            nanos = ts2.tv_nsec - ts1.tv_nsec;
          } else {
            nanos = (ts2.tv_sec - ts1.tv_sec) * 1000000000LL;
            nanos += 1000000000LL - ts1.tv_nsec;
            nanos += ts2.tv_nsec;
          }
          printf("took %,ldµs wall time\n", nanos / 1000);
          p = 0;
          AppendResourceReport(&p, &ru, "\n");
          fputs(p, stdout);
          free(p);
        }

        // update prompt to reflect exit status
        p = prompt;
        if (WIFEXITED(ws)) {
          if (WEXITSTATUS(ws)) {
            if (!__nocolor) p = stpcpy(p, "\e[1;31m");
            p = stpcpy(p, "rc=");
            p = FormatInt32(p, WEXITSTATUS(ws));
            if (128 < WEXITSTATUS(ws) && WEXITSTATUS(ws) <= 128 + 32) {
              *p++ = ' ';
              p = stpcpy(p, strsignal(WEXITSTATUS(ws) - 128));
            }
            if (!__nocolor) p = stpcpy(p, "\e[0m");
            *p++ = ' ';
          }
        } else {
          if (!__nocolor) p = stpcpy(p, "\e[1;31m");
          p = stpcpy(p, strsignal(WTERMSIG(ws)));
          if (!__nocolor) p = stpcpy(p, "\e[0m");
          *p++ = ' ';
        }
        MakePrompt(p);

        sigprocmask(SIG_SETMASK, &savemask, 0);
        sigaction(SIGINT, &saveint, 0);
        sigaction(SIGQUIT, &savequit, 0);
      } else {
        fprintf(stderr, "%s: %s: command not found\n", argv[0], args[0]);
      }
    }
    free(line);
    free(args);
  }
}
