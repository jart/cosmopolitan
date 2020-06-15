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
#include "libc/calls/internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/sysv/errfuns.h"

/**
 * Returns current working directory.
 *
 * @param buf is where UTF-8 NUL-terminated path string gets written,
 *     which may be NULL to ask this function to malloc a buffer
 * @param size is number of bytes available in buf, e.g. PATH_MAX,
 *     which may be 0 if buf NULL
 * @return buf containing system-normative path or NULL w/ errno
 * @see get_current_dir_name() which is better
 * @error ERANGE, EINVAL
 */
char *(getcwd)(char *buf, size_t size) {
  buf[0] = '\0';
  if (!IsWindows()) {
    int olderr = errno;
    if (getcwd$sysv(buf, size) != NULL) {
      return buf;
    } else if (IsXnu() && errno == ENOSYS) {
      if (size >= 2) {
        buf[0] = '.'; /* XXX: could put forth more effort */
        buf[1] = '\0';
        errno = olderr;
        return buf;
      } else {
        erange();
      }
    }
    return NULL;
  } else {
    return getcwd$nt(buf, size);
  }
}
