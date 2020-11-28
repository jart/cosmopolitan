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
#include "libc/calls/struct/iovec.h"
#include "libc/sock/sock.h"

/**
 * Reads data from file descriptor.
 *
 * @param fd is something open()'d earlier
 * @param buf is copied into, cf. copy_file_range(), sendfile(), etc.
 * @param size in range [1..0x7ffff000] is reasonable
 * @return [1..size] bytes on success, 0 on EOF, or -1 w/ errno; with
 *     exception of size==0, in which case return zero means no error
 * @see write(), pread(), readv()
 * @asyncsignalsafe
 */
ssize_t read(int fd, void *buf, size_t size) {
  return readv(fd, &(struct iovec){buf, size}, 1);
}
