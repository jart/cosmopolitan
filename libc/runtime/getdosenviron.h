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
#ifndef __STRICT_ANSI__
#include "libc/bits/safemacros.h"
#include "libc/str/appendchar.h"
#include "libc/str/str.h"
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
static inline int GetDosEnviron(const char16_t *env, char *buf, size_t size,
                                char **envp, size_t max) {
  wint_t wc;
  size_t envc;
  char *p, *pe;
  bool endstring;
  const char16_t *s;
  s = env;
  envc = 0;
  if (size) {
    p = buf;
    pe = buf + size - 1;
    if (p < pe) {
      wc = DecodeNtsUtf16(&s);
      while (wc) {
        if (++envc < max) {
          envp[envc - 1] = p < pe ? p : NULL;
        }
        do {
          AppendChar(&p, pe, wc);
          endstring = !wc;
          wc = DecodeNtsUtf16(&s);
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
#endif /* !ANSI */
#endif /* COSMOPOLITAN_LIBC_DOSENVIRON_H_ */
