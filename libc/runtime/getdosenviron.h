/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=8 sts=2 sw=2 fenc=utf-8                                :vi│
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
#ifndef COSMOPOLITAN_LIBC_DOSENVIRON_H_
#define COSMOPOLITAN_LIBC_DOSENVIRON_H_
#include "libc/bits/safemacros.h"
#include "libc/str/appendchar.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/**
 * Transcodes NT environment variable block from UTF-16 to UTF-8.
 *
 * @param env is a double NUL-terminated block of key=values
 * @param buf is the new environment
 * @param size is the byte capacity of buf
 * @param envp stores NULL-terminated string pointer list
 * @param max is the pointer count capacity of envp
 * @return number of variables decoded, excluding NULL-terminator
 */
static inline int getdosenviron(const char16_t *env, char *buf, size_t size,
                                char **envp, size_t max) {
  const char16_t *s = env;
  size_t envc = 0;
  if (size) {
    wint_t wc;
    char *p = buf;
    char *pe = buf + size - 1;
    if (p < pe) {
      s += getutf16(s, &wc);
      while (wc) {
        if (++envc < max) {
          envp[envc - 1] = p < pe ? p : NULL;
        }
        bool endstring;
        do {
          AppendChar(&p, pe, wc);
          endstring = !wc;
          s += getutf16(s, &wc);
        } while (!endstring);
        buf[min(p - buf, size - 2)] = u'\0';
      }
    }
    AppendChar(&p, pe, '\0');
    buf[min(p - buf, size - 1)] = u'\0';
  }
  if (max) envp[min(envc, max - 1)] = NULL;
  return envc;
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_DOSENVIRON_H_ */
