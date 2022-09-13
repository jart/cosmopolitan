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
#include "libc/calls/struct/stat.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/s.h"
#include "libc/x/x.h"
#include "third_party/musl/ftw.h"

const char *prog;
char tmpdir[PATH_MAX];
char binpath[PATH_MAX];

bool IsDirectory(const char *path) {
  int e;
  bool res;
  struct stat st;
  e = errno;
  res = stat(path, &st) != -1 && S_ISDIR(st.st_mode);
  errno = e;
  return res;
}

void Execute(char *argv[]) {
  int ws;
  if (!vfork()) {
    execv(argv[0], argv);
    _Exit(127);
  }
  wait(&ws);
  if (!WIFEXITED(ws) || WEXITSTATUS(ws)) {
    fputs(argv[0], stderr);
    fputs(": command failed\n", stderr);
    exit(1);
  }
}

int Visit(const char *fpath, const struct stat *sb, int tflag,
          struct FTW *ftwbuf) {
  if (tflag == FTW_F && _endswith(fpath, ".gz")) {
    Execute((char *[]){"build/bootstrap/gzip.com", "-d", fpath, 0});
    strcpy(binpath, fpath);
    binpath[strlen(binpath) - 3] = 0;
    chmod(binpath, 0755);
  } else if (tflag == FTW_F && _endswith(fpath, ".sym")) {
    strcpy(binpath, fpath);
    binpath[strlen(binpath) - 4] = 0;
    symlink(xslurp(fpath, 0), binpath);
  }
  return 0;
}

int main(int argc, char *argv[]) {
  if (!IsLinux()) return 0;
  prog = argc > 0 ? argv[0] : "unbundle.com";
  if (IsDirectory("o/third_party/gcc")) return 0;
  makedirs("o/third_party", 0755);
  FormatInt32(stpcpy(tmpdir, "o/third_party/gcc."), getpid());
  Execute(
      (char *[]){"build/bootstrap/cp.com", "-r", "third_party/gcc", tmpdir, 0});
  if (nftw(tmpdir, Visit, 20, 0) == -1) {
    fputs(prog, stderr);
    fputs(": nftw failed: ", stderr);
    fputs(_strerdoc(errno), stderr);
    fputs("\n", stderr);
    exit(1);
  }
  rename(tmpdir, "o/third_party/gcc");
}
