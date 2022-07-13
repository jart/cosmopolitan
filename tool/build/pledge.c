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
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/intrin/kprintf.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "third_party/getopt/getopt.h"

// options used: hpugc
// letters not used: ABCDEFGHIJKLMNOPQRSTUVWXYZabdefijklmnoqrstvwxyz
// digits not used: 0123456789
// puncts not used: !"#$%&'()*+,-./;<=>@[\]^_`{|}~
// letters duplicated: none
#define GETOPTS "hp:u:g:c:"

#define USAGE \
  "\
usage: pledge.com [-h] PROG ARGS...\n\
  -h         show help\n\
  -g GID     call setgid()\n\
  -u UID     call setuid()\n\
  -c PATH    call chroot()\n\
  -p PLEDGE  may contain any of following separated by spaces\n\
     - stdio: allow stdio and benign system calls\n\
     - rpath: read-only path ops\n\
     - wpath: write path ops\n\
     - cpath: create path ops\n\
     - dpath: create special files\n\
     - flock: file locks\n\
     - tty: terminal ioctls\n\
     - recvfd: allow SCM_RIGHTS\n\
     - fattr: allow changing some struct stat bits\n\
     - inet: allow IPv4 and IPv6\n\
     - unix: allow local sockets\n\
     - dns: allow dns\n\
     - proc: allow fork, clone and friends\n\
     - thread: allow clone\n\
     - id: allow setuid and friends\n\
     - exec: allow executing ape binaries\n\
"

int g_gflag;
int g_uflag;
int g_hflag;
const char *g_pflag;
const char *g_cflag;

static void GetOpts(int argc, char *argv[]) {
  int opt;
  g_pflag = "";
  while ((opt = getopt(argc, argv, GETOPTS)) != -1) {
    switch (opt) {
      case 'p':
        g_pflag = optarg;
        break;
      case 'c':
        g_cflag = optarg;
        break;
      case 'g':
        g_gflag = atoi(optarg);
        break;
      case 'u':
        g_uflag = atoi(optarg);
        break;
      case 'h':
      case '?':
        write(1, USAGE, sizeof(USAGE) - 1);
        exit(0);
      default:
        write(2, USAGE, sizeof(USAGE) - 1);
        exit(64);
    }
  }
}

const char *prog;
char pledges[1024];
char pathbuf[PATH_MAX];

int main(int argc, char *argv[]) {
  GetOpts(argc, argv);
  if (optind == argc) {
    kprintf("error: too few args\n", g_pflag);
    write(2, USAGE, sizeof(USAGE) - 1);
    exit(64);
  }
  if (g_cflag) {
    if (chroot(g_cflag) == -1) {
      kprintf("error: chroot(%`'s) failed: %s\n", g_cflag, strerror(errno));
      return 1;
    }
  }
  if (!(prog = commandv(argv[optind], pathbuf, sizeof(pathbuf)))) {
    kprintf("error: command not found: %s\n", argv[optind]);
    return 2;
  }
  if (g_gflag) {
    if (setgid(g_gflag) == -1) {
      kprintf("error: setgid(%d) failed: %s\n", g_gflag, strerror(errno));
      return 3;
    }
  }
  if (g_uflag) {
    if (setuid(g_uflag) == -1) {
      kprintf("error: setuid(%d) failed: %s\n", g_uflag, strerror(errno));
      return 4;
    }
  }
  ksnprintf(pledges, sizeof(pledges), "%s execnative", g_pflag);
  if (pledge(pledges, 0) == -1) {
    kprintf("error: pledge(%`'s) failed: %s\n", pledges, strerror(errno));
    return 5;
  }
  execv(prog, argv + optind);
  kprintf("error: execve(%`'s) failed: %s\n", prog, strerror(errno));
  return 127;
}
