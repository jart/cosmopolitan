/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/log/check.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"

/**
 * Removes file system caches from RAM.
 *
 * make o//tool/build/dropcache.com
 * sudo mv o//tool/build/dropcache.com /usr/local/bin/
 * sudo chown root /usr/local/bin/dropcache.com
 * sudo chmod u+s /usr/local/bin/dropcache.com
 */

static void Write(int fd, const char *s) {
  write(fd, s, strlen(s));
}

int main(int argc, char *argv[]) {
  int fd;
  sync();
  fd = open("/proc/sys/vm/drop_caches", O_WRONLY);
  if (fd == -1) {
    if (errno == EACCES) {
      Write(1, "error: need root privileges\n");
    } else if (errno == ENOENT) {
      Write(1, "error: need /proc filesystem\n");
    }
    exit(1);
  }
  Write(fd, "3\n");
  close(fd);
  return 0;
}
