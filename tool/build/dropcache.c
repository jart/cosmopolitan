/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
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
