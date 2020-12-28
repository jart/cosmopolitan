/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
