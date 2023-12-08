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
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/termios.h"
#include "libc/calls/struct/winsize.h"
#include "libc/calls/termios.h"
#include "libc/calls/ttydefaults.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/termios.h"
#include "libc/sysv/consts/w.h"
#include "third_party/getopt/getopt.internal.h"

#define GETOPTS "o:"

#define USAGE \
  "\
Usage: unbuffer.com [FLAGS] PROG ARGS...\n\
  -o PATH     output file\n\
"

int outfd;
struct winsize wsz;
struct termios tio, oldterm;
sigset_t chldmask, savemask;
struct sigaction ignore, saveint, savequit;
char databuf[512];
char pathbuf[PATH_MAX];
const char *outputpath;

static void GetOpts(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, GETOPTS)) != -1) {
    switch (opt) {
      case 'o':
        outputpath = optarg;
        break;
      case '?':
        write(1, USAGE, sizeof(USAGE) - 1);
        exit(0);
      default:
        write(2, USAGE, sizeof(USAGE) - 1);
        exit(64);
    }
  }
}

int main(int argc, char *argv[]) {
  ssize_t rc;
  const char *prog;
  size_t i, got, wrote;
  int e, ws, mfd, sfd, pid;

  GetOpts(argc, argv);
  if (optind == argc) {
    write(2, USAGE, sizeof(USAGE) - 1);
    exit(64);
  }

  if (!(prog = commandv(argv[optind], pathbuf, sizeof(pathbuf)))) {
    kprintf("%s: command not found\n", argv[optind]);
    exit(1);
  }

  if (outputpath) {
    if ((outfd = creat(outputpath, 0644)) == -1) {
      perror(outputpath);
      exit(1);
    }
  }

  bzero(&wsz, sizeof(wsz));
  wsz.ws_col = 80;
  wsz.ws_row = 45;

  if (tcgetattr(1, &tio)) {
    perror("tcgetattr");
    exit(1);
  }

  if (openpty(&mfd, &sfd, 0, &tio, &wsz)) {
    perror("openpty");
    exit(1);
  }

  ignore.sa_flags = 0;
  ignore.sa_handler = SIG_IGN;
  sigemptyset(&ignore.sa_mask);
  sigaction(SIGINT, &ignore, &saveint);
  sigaction(SIGQUIT, &ignore, &savequit);
  sigemptyset(&chldmask);
  sigaddset(&chldmask, SIGCHLD);
  sigprocmask(SIG_BLOCK, &chldmask, &savemask);

  if ((pid = fork()) == -1) {
    perror("fork");
    exit(1);
  }

  if (!pid) {
    close(0);
    dup(sfd);
    close(1);
    dup(sfd);
    close(2);
    dup(sfd);
    close(mfd);
    sigaction(SIGINT, &saveint, 0);
    sigaction(SIGQUIT, &savequit, 0);
    sigprocmask(SIG_SETMASK, &savemask, 0);
    execv(prog, argv + optind);
    perror("execve");
    return 127;
  }
  close(sfd);

  for (;;) {
    if (waitpid(pid, &ws, WNOHANG) == pid) {
      break;
    }
    e = errno;
    rc = read(mfd, databuf, sizeof(databuf));
    if (rc == -1) {
      if (errno == EIO) {
        errno = e;
        rc = 0;
      } else {
        perror("read");
        exit(1);
      }
    }
    if (!(got = rc)) {
      if (waitpid(pid, &ws, 0) == -1) {
        perror("waitpid");
        exit(1);
      }
      break;
    }
    for (i = 0; i < got; i += wrote) {
      rc = write(1, databuf + i, got - i);
      if (rc != -1) {
        wrote = rc;
      } else {
        perror("write");
        exit(1);
      }
    }
    if (outputpath) {
      for (i = 0; i < got; i += wrote) {
        rc = write(outfd, databuf + i, got - i);
        if (rc != -1) {
          wrote = rc;
        } else {
          perror("write");
          exit(1);
        }
      }
    }
  }

  sigaction(SIGINT, &saveint, 0);
  sigaction(SIGQUIT, &savequit, 0);
  sigprocmask(SIG_SETMASK, &savemask, 0);

  if (WIFEXITED(ws)) {
    return WEXITSTATUS(ws);
  } else {
    raise(WTERMSIG(ws));
    exit(128 + WTERMSIG(ws));
  }
}
