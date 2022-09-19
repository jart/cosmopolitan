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
#include "libc/mem/copyfd.internal.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/o.h"
#include "third_party/getopt/getopt.h"

char buf[512];

static void Write(const char *s, ...) {
  va_list va;
  va_start(va, s);
  do {
    write(2, s, strlen(s));
  } while ((s = va_arg(va, const char *)));
  va_end(va);
}

wontreturn void SysExit(int rc, const char *call, const char *thing) {
  int err;
  char ibuf[12];
  const char *estr;
  err = errno;
  FormatInt32(ibuf, err);
  estr = _strerdoc(err);
  if (!estr) estr = "EUNKNOWN";
  Write(thing, ": ", call, "() failed: ", estr, " (", ibuf, ")\n", 0);
  exit(rc);
}

int main(int argc, char *argv[]) {
  int i, opt;
  const char *outpath = "/dev/stdout";
  while ((opt = getopt(argc, argv, "o:")) != -1) {
    switch (opt) {
      case 'o':
        outpath = optarg;
        break;
      default:
        return 1;
    }
  }
  int out = open(outpath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (out == -1) SysExit(2, "open", outpath);
  for (i = optind; i < argc; ++i) {
    int in = open(argv[i], O_RDONLY);
    if (in == -1) SysExit(3, "open", argv[i]);
    for (;;) {
      ssize_t rc = read(in, buf, 512);
      if (rc == -1) SysExit(3, "read", argv[i]);
      if (!rc) break;
      ssize_t rc2 = write(out, buf, rc);
      if (rc2 != rc) SysExit(4, "write", outpath);
    }
    if (close(in) == -1) SysExit(5, "close", argv[i]);
  }
  if (close(out) == -1) SysExit(6, "close", outpath);
}
