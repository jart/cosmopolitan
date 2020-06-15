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
#include "libc/sysv/consts/o.h"

/**
 * Creates new file or changes modified time on existing one.
 *
 * @param file is a UTF-8 string, which may or may not exist
 * @param mode is an octal user/group/other permission, e.g. 0755
 * @return 0 on success, or -1 w/ errno
 * @see creat()
 */
int touch(const char *file, uint32_t mode) {
  int fd;
  if ((fd = open(file, O_CREAT | O_WRONLY, mode)) == -1) return -1;
  fsync(fd); /* TODO(jart): do we need it? */
  return close(fd);
}
