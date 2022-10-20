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
#include "libc/intrin/_getenv.internal.h"
#include "libc/intrin/kmalloc.h"
#include "libc/intrin/strace.internal.h"
#include "libc/macros.internal.h"
#include "libc/mem/internal.h"
#include "libc/runtime/runtime.h"

#define ToUpper(c) ((c) >= 'a' && (c) <= 'z' ? (c) - 'a' + 'A' : (c))

static char **expected;
static size_t capacity;

static size_t GetEnvironLen(char **env) {
  char **p = env;
  while (*p) ++p;
  return p - env;
}

static char **GrowEnviron(char **a) {
  size_t n, c;
  char **b, **p;
  if (!a) a = environ;
  n = a ? GetEnvironLen(a) : 0;
  c = MAX(16ul, n) << 1;
  if ((b = kmalloc(c * sizeof(char *)))) {
    if (a) {
      for (p = b; *a;) {
        *p++ = *a++;
      }
    }
    environ = b;
    expected = b;
    capacity = c;
    return b;
  } else {
    return 0;
  }
}

int PutEnvImpl(char *s, bool overwrite) {
  char **p;
  struct Env e;
  if (!(p = environ)) {
    if (!(p = GrowEnviron(0))) {
      return -1;
    }
  }
  e = _getenv(p, s);
  if (e.s && !overwrite) {
    return 0;
  }
  if (e.s) {
    p[e.i] = s;
    return 0;
  }
  if (p != expected) {
    capacity = e.i;
  }
  if (e.i + 1 >= capacity) {
    if (!(p = GrowEnviron(p))) {
      return -1;
    }
  }
  p[e.i + 1] = 0;
  p[e.i] = s;
  return 0;
}

/**
 * Emplaces environment key=value.
 *
 * @param s should be a string that looks like `"name=value"` and it'll
 *     become part of the environment; changes to its memory will change
 *     the environment too
 * @return 0 on success, or non-zero w/ errno on error
 * @raise ENOMEM if we require more vespene gas
 * @see setenv(), getenv()
 */
int putenv(char *s) {
  int rc;
  rc = PutEnvImpl(s, true);
  STRACE("putenv(%#s) → %d% m", s, rc);
  return rc;
}
