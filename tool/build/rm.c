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
#include "libc/calls/struct/stat.h"
#include "libc/errno.h"
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/ftw.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ok.h"
#include "libc/sysv/consts/s.h"
#include "third_party/getopt/getopt.internal.h"

#define USAGE \
  " FILE...\n\
\n\
SYNOPSIS\n\
\n\
  Removes Files\n\
\n\
FLAGS\n\
\n\
  -h        help\n\
  -f        force\n\
  -f or -R  recursive\n\
  -d        remove empty dirs\n\
\n"

static bool force;
static bool recursive;
static bool doemptydirs;
static const char *prog;

static wontreturn void PrintUsage(int rc, int fd) {
  tinyprint(fd, "USAGE\n\n  ", prog, USAGE, NULL);
  exit(rc);
}

static void GetOpts(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "rRdhf")) != -1) {
    switch (opt) {
      case 'f':
        force = true;
        break;
      case 'd':
        doemptydirs = true;
        break;
      case 'r':
      case 'R':
        recursive = true;
        break;
      case 'h':
        PrintUsage(0, 1);
      default:
        PrintUsage(1, 2);
    }
  }
}

static int OnFile(const char *fpath, const struct stat *st, int typeflag,
                  struct FTW *ftwbuf) {
  int rc;
  if (!force && typeflag == FTW_DNR) {
    rc = -1;
  } else if (!force && !S_ISLNK(st->st_mode) && access(fpath, W_OK)) {
    rc = -1;
  } else if (typeflag == FTW_DNR) {
    if (!(rc = chmod(fpath, 0700))) {
      rc = nftw(fpath, OnFile, 128 - ftwbuf->level, FTW_PHYS | FTW_DEPTH);
    }
  } else if (typeflag == FTW_DP) {
    rc = rmdir(fpath);
  } else {
    rc = unlink(fpath);
  }
  if (rc == -1) {
    if (force && errno == ENOENT) return 0;
    perror(fpath);
    exit(1);
  }
  return 0;
}

static void Remove(const char *path) {
  int rc;
  struct stat st;
  if (path[0] == '/' && !path[1]) {
    tinyprint(2, prog, ": add a dot if you want\n", NULL);
    exit(1);
  }
  if (recursive) {
    rc = nftw(path, OnFile, 128, FTW_PHYS | FTW_DEPTH);
  } else {
    if (lstat(path, &st)) {
      if (force && errno == ENOENT) return;
      perror(path);
      exit(1);
    }
    if (!force && !S_ISLNK(st.st_mode) && access(path, W_OK)) {
      perror(path);
      exit(1);
    }
    if (S_ISDIR(st.st_mode) && doemptydirs) {
      rc = rmdir(path);
    } else {
      rc = unlink(path);
    }
  }
  if (rc == -1) {
    if (force && errno == ENOENT) return;
    perror(path);
    exit(1);
  }
}

int main(int argc, char *argv[]) {
  int i;
  prog = argv[0];
  if (!prog) prog = "rm";
  GetOpts(argc, argv);
  if (optind == argc) {
    tinyprint(2, prog, ": missing operand\n", NULL);
    exit(1);
  }
  for (i = optind; i < argc; ++i) {
    Remove(argv[i]);
  }
}
