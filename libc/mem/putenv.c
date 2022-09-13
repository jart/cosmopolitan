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
#include "libc/intrin/strace.internal.h"
#include "libc/dce.h"
#include "libc/macros.internal.h"
#include "libc/mem/alg.h"
#include "libc/mem/internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

#define ToUpper(c) ((c) >= 'a' && (c) <= 'z' ? (c) - 'a' + 'A' : (c))

static bool once;
static size_t capacity;

static size_t GetEnvironLen(char **env) {
  char **p = env;
  while (*p) ++p;
  return p - env;
}

static void RestoreOriginalEnvironment(char **envp) {
  environ = envp;
}

static void PutEnvImplAtExit(void *p) {
  free(p);
}

static void FreeEnviron(char **env) {
  char **a;
  for (a = env; *a; ++a) {
    free(*a);
  }
  free(env);
}

static void GrowEnviron(void) {
  size_t n, c;
  char **a, **b, **p;
  a = environ;
  n = a ? GetEnvironLen(a) : 0;
  c = MAX(16ul, n) << 1;
  b = calloc(c, sizeof(char *));
  if (a) {
    for (p = b; *a;) {
      *p++ = strdup(*a++);
    }
  }
  __cxa_atexit(FreeEnviron, b, 0);
  environ = b;
  capacity = c;
}

int PutEnvImpl(char *s, bool overwrite) {
  char *p;
  unsigned i, namelen;
  if (!once) {
    __cxa_atexit(RestoreOriginalEnvironment, environ, 0);
    GrowEnviron();
    once = true;
  } else if (!environ) {
    GrowEnviron();
  }
  for (p = s; *p && *p != '='; ++p) {
    if (IsWindows()) {
      *p = ToUpper(*p);
    }
  }
  if (*p != '=') goto Fail;
  namelen = p + 1 - s;
  for (i = 0; environ[i]; ++i) {
    if (!strncmp(environ[i], s, namelen)) {
      if (!overwrite) {
        free(s);
        return 0;
      }
      goto Replace;
    }
  }
  if (i + 1 >= capacity) {
    GrowEnviron();
  }
  environ[i + 1] = 0;
Replace:
  __cxa_atexit(PutEnvImplAtExit, environ[i], 0);
  environ[i] = s;
  return 0;
Fail:
  free(s);
  return einval();
}

static void UnsetenvFree(void *p) {
  free(p);
}

/* weakly called by unsetenv() when removing a pointer */
void __freeenv(void *p) {
  if (once) {
    __cxa_atexit(UnsetenvFree, p, 0);
  }
}

/**
 * Emplaces environment key=value.
 *
 * @return 0 on success or non-zero on error
 * @see setenv(), getenv()
 */
int putenv(char *s) {
  int rc;
  rc = PutEnvImpl(strdup(s), true);
  STRACE("putenv(%#s) → %d", s, rc);
  return rc;
}
