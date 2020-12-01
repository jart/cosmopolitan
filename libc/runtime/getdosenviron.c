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
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"

/**
 * Transcodes NT environment variable block from UTF-16 to UTF-8.
 *
 * @param env is a double NUL-terminated block of key=values
 * @param buf is the new environment which gets double-nul'd
 * @param size is the byte capacity of buf
 * @param envp stores NULL-terminated string pointer list (optional)
 * @param max is the pointer count capacity of envp
 * @return number of variables decoded, excluding NULL-terminator
 */
textwindows int GetDosEnviron(const char16_t *env, char *buf, size_t size,
                              char **envp, size_t max) {
  int i;
  axdx_t r;
  i = 0;
  if (size) {
    --size;
    while (*env) {
      if (i + 1 < max) envp[i++] = buf;
      r = tprecode16to8(buf, size, env);
      size -= r.ax + 1;
      buf += r.ax + 1;
      env += r.dx;
    }
    *buf = '\0';
  }
  if (i < max) envp[i] = NULL;
  return i;
}
