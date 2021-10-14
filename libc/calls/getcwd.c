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
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/sysdebug.internal.h"
#include "libc/dce.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

/**
 * Returns current working directory, e.g.
 *
 *     const char *dirname = gc(getcwd(0,0)); // if malloc is linked
 *     const char *dirname = getcwd(alloca(PATH_MAX),PATH_MAX);
 *
 * @param buf is where UTF-8 NUL-terminated path string gets written,
 *     which may be NULL to ask this function to malloc a buffer
 * @param size is number of bytes available in buf, e.g. PATH_MAX+1,
 *     which may be 0 if buf is NULL
 * @return buf containing system-normative path or NULL w/ errno
 * @error ERANGE, EINVAL, ENOMEM
 */
char *getcwd(char *buf, size_t size) {
  char *p, *r;
  if (buf) {
    p = buf;
    if (!size) {
      SYSDEBUG("getcwd(%p, %x) EINVAL", buf, size);
      einval();
      return 0;
    }
  } else if (weaken(malloc)) {
    if (!size) size = PATH_MAX + 1;
    if (!(p = weaken(malloc)(size))) {
      SYSDEBUG("getcwd(%p, %x) ENOMEM", buf, size);
      return 0;
    }
  } else {
    SYSDEBUG("getcwd() EINVAL needs buf≠0 or STATIC_YOINK(\"malloc\")");
    einval();
    return 0;
  }
  *p = '\0';
  if (!IsWindows()) {
    if (IsMetal()) {
      r = size >= 5 ? strcpy(p, "/zip") : 0;
    } else if (IsXnu()) {
      r = sys_getcwd_xnu(p, size);
    } else if (sys_getcwd(p, size) != (void *)-1) {
      r = p;
    } else {
      r = 0;
    }
  } else {
    r = sys_getcwd_nt(p, size);
  }
  if (!buf) {
    if (!r) {
      if (weaken(free)) {
        weaken(free)(p);
      }
    } else {
      if (weaken(realloc)) {
        if ((p = weaken(realloc)(r, strlen(r) + 1))) {
          r = p;
        }
      }
    }
  }
  SYSDEBUG("getcwd(%p, %x) -> %s", buf, size, r);
  return r;
}
