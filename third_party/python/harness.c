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
#include "libc/calls/calls.h"
#include "libc/calls/copyfile.h"
#include "libc/calls/sigbits.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/append.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sig.h"
#include "libc/x/x.h"
#include "third_party/getopt/getopt.h"

#define MANUAL \
  "\
SYNOPSIS\n\
\n\
  harness.com [FLAGS] COMMAND [ARGS...]\n\
\n\
OVERVIEW\n\
\n\
  Python Test Hardness\n\
\n\
FLAGS\n\
\n\
  -n           do nothing (used to prime the executable)\n\
  -h           print help\n\
\n"

bool FileExistsAndIsNewerThan(const char *filepath, const char *thanpath) {
  struct stat st1, st2;
  if (stat(filepath, &st1) == -1) return false;
  if (stat(thanpath, &st2) == -1) return false;
  if (st1.st_mtim.tv_sec < st2.st_mtim.tv_sec) return false;
  if (st1.st_mtim.tv_sec > st2.st_mtim.tv_sec) return true;
  return st1.st_mtim.tv_nsec >= st2.st_mtim.tv_nsec;
}

int main(int argc, char *argv[]) {
  size_t got;
  ssize_t rc;
  bool failed;
  sigset_t mask, savemask;
  int ws, opt, pid, exitcode, pipefds[2];
  char *cmd, *output, *comdbg, *cachedcmd, *originalcmd, buf[512];
  while ((opt = getopt(argc, argv, "hn")) != -1) {
    switch (opt) {
      case 'n':
        exit(0);
      case 'h':
        fputs(MANUAL, stdout);
        exit(0);
      default:
        fputs(MANUAL, stderr);
        exit(1);
    }
  }
  if (optind == argc) {
    fputs("error: missing arguments\n", stderr);
    exit(1);
  }
  cmd = argv[optind];
  originalcmd = NULL;
  cachedcmd = NULL;
  if (!IsWindows() && endswith(cmd, ".com")) {
    comdbg = xstrcat(cmd, ".dbg");
    cachedcmd = xstrcat("o/", cmd);
    if (fileexists(comdbg)) {
      setenv("COMDBG", comdbg, 1);
    }
    if (FileExistsAndIsNewerThan(cachedcmd, cmd)) {
      cmd = cachedcmd;
    } else {
      if (startswith(cmd, "o/")) {
        cachedcmd = NULL;
      }
      originalcmd = cmd;
      cmd = xasprintf("%s.tmp.%d", originalcmd, getpid());
      copyfile(originalcmd, cmd, 0);
    }
  }
  sigfillset(&mask);
  if (pipe2(pipefds, O_CLOEXEC) == -1) exit(errno);
  close(0);
  sigprocmask(SIG_BLOCK, &mask, &savemask);
  if ((pid = vfork()) == -1) exit(errno);
  if (!pid) {
    open("/dev/null", O_RDONLY);
    dup2(pipefds[1], 1);
    dup2(pipefds[1], 2);
    sigprocmask(SIG_SETMASK, &savemask, 0);
    execve(cmd, argv + 1, environ);
    _exit(127);
  }
  close(pipefds[1]);
  output = 0;
  failed = false;
  appends(&output, "");
  for (;;) {
    if ((rc = read(pipefds[0], buf, sizeof(buf))) != -1) {
      if ((got = rc)) {
        appendd(&output, buf, got);
      } else {
        break;
      }
    } else {
      appendf(&output, "error: %s read() failed %m\n", cmd);
      failed = true;
      break;
    }
  }
  close(pipefds[0]);
  rc = wait4(pid, &ws, 0, 0);
  if (originalcmd) {
    if (cachedcmd && WIFEXITED(ws) && !WEXITSTATUS(ws)) {
      makedirs(xdirname(cachedcmd), 0755);
      rename(cmd, cachedcmd);
    } else {
      unlink(cmd);
    }
  }
  if (rc != -1) {
    if (WIFEXITED(ws)) {
      if (!WEXITSTATUS(ws)) {
        if (!failed) {
          return 0;
        } else {
          exitcode = 77;
        }
      } else {
        exitcode = WEXITSTATUS(ws);
        appendf(&output, "error: %s exited with %d\n", cmd, exitcode);
      }
    } else {
      appendf(&output, "error: %s terminated by %s\n", cmd,
              strsignal(WTERMSIG(ws)));
      exitcode = 128 + WTERMSIG(ws);
    }
  } else {
    appendf(&output, "error: %s wait4() failed %m\n", cmd);
    exitcode = 1;
  }
  xwrite(2, output, appendz(output).i);
  return exitcode;
}
