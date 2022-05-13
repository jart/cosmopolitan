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
#include "libc/calls/sigbits.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/timespec.h"
#include "libc/fmt/fmt.h"
#include "libc/fmt/itoa.h"
#include "libc/log/internal.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/append.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/dt.h"
#include "libc/sysv/consts/sig.h"
#include "libc/time/time.h"
#include "libc/x/x.h"
#include "third_party/linenoise/linenoise.h"

/**
 * @fileoverview Shell that works on Windows.
 *
 * This doesn't have script language features like UNBOURNE.COM but it
 * works on Windows and, unlike CMD.EXE, actually has CTRL-P and CTRL-R
 * which alone make it so much better.
 *
 * One day we'll have UNBOURNE.COM working on Windows but the code isn't
 * very maintainable sadly.
 */

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

int main(int argc, char *argv[]) {
  bool timeit;
  int64_t nanos;
  int n, ws, pid;
  struct rusage ru;
  struct timespec ts1, ts2;
  char *prog, path[PATH_MAX];
  sigset_t chldmask, savemask;
  struct sigaction ignore, saveint, savequit;
  char *p, *line, **args, *arg, *start, *state, prompt[64];
  linenoiseSetFreeHintsCallback(free);
  linenoiseSetHintsCallback(ShellHint);
  linenoiseSetCompletionCallback(ShellCompletion);
  stpcpy(prompt, "$ ");
  while ((line = linenoiseWithHistory(prompt, "cmd"))) {
    n = 0;
    start = line;
    if (startswith(start, "time ")) {
      timeit = true;
      start += 5;
    } else {
      timeit = false;
    }
    args = xcalloc(1, sizeof(*args));
    while ((arg = strtok_r(start, " \t\r\n", &state))) {
      args = xrealloc(args, (++n + 1) * sizeof(*args));
      args[n - 1] = arg;
      args[n - 0] = 0;
      start = 0;
    }
    if (n > 0) {
      if ((prog = commandv(args[0], path, sizeof(path)))) {
        ignore.sa_flags = 0;
        ignore.sa_handler = SIG_IGN;
        sigemptyset(&ignore.sa_mask);
        sigaction(SIGINT, &ignore, &saveint);
        sigaction(SIGQUIT, &ignore, &savequit);
        sigemptyset(&chldmask);
        sigaddset(&chldmask, SIGCHLD);
        sigprocmask(SIG_BLOCK, &chldmask, &savemask);

        if (timeit) {
          clock_gettime(CLOCK_REALTIME, &ts1);
        }
        if (!fork()) {
          sigaction(SIGINT, &saveint, 0);
          sigaction(SIGQUIT, &savequit, 0);
          sigprocmask(SIG_SETMASK, &savemask, 0);
          execv(prog, args);
          _Exit(127);
        }
        wait4(0, &ws, 0, &ru);
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

        p = prompt;
        if (WIFEXITED(ws)) {
          if (WEXITSTATUS(ws)) {
            if (!__nocolor) p = stpcpy(p, "\e[1;31m");
            p = stpcpy(p, "rc=");
            p = FormatInt32(p, WEXITSTATUS(ws));
            if (!__nocolor) p = stpcpy(p, "\e[0m");
            *p++ = ' ';
          }
        } else {
          if (!__nocolor) p = stpcpy(p, "\e[1;31m");
          p = stpcpy(p, "rc=");
          p = stpcpy(p, strsignal(WTERMSIG(ws)));
          if (!__nocolor) p = stpcpy(p, "\e[0m");
          *p++ = ' ';
        }
        p = stpcpy(p, "$ ");

        sigaction(SIGINT, &saveint, 0);
        sigaction(SIGQUIT, &savequit, 0);
        sigprocmask(SIG_SETMASK, &savemask, 0);
      } else {
        fprintf(stderr, "%s: %s: command not found\n", argv[0], args[0]);
      }
    }
    free(line);
    free(args);
  }
}
