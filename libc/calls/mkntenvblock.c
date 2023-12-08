/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/assert.h"
#include "libc/intrin/getenv.internal.h"
#include "libc/mem/alloca.h"
#include "libc/proc/ntspawn.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

#define ToUpper(c) ((c) >= 'a' && (c) <= 'z' ? (c) - 'a' + 'A' : (c))

struct EnvBuilder {
  char *buf;
  char **var;
  int bufi;
  int vari;
};

static inline int IsAlpha(int c) {
  return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

static textwindows int Compare(const char *l, const char *r) {
  int a, b;
  size_t i = 0;
  for (;;) {
    a = l[i] & 255;
    b = r[i] & 255;
    if (a == '=') a = 0;
    if (b == '=') b = 0;
    if (a != b || !b) break;
    ++i;
  }
  return a - b;
}

static textwindows int InsertString(struct EnvBuilder *env, const char *str) {
  int c, i, cmp;
  char *var, *path = 0;

  if (!str) return 0;

  // copy key=val to buf
  var = env->buf + env->bufi;
  do {
    c = *str++;
    if (env->bufi + 2 > 32767) return e2big();
    env->buf[env->bufi++] = c;
    if (c == '=' && str[0] == '/' && IsAlpha(str[1]) && str[2] == '/') {
      path = env->buf + env->bufi;
    }
  } while (c);

  // fixup key=/c/... → key=c:\...
  if (path) mungentpath(path);

  // append key=val to sorted list using insertion sort technique
  for (i = env->vari;; --i) {
    if (!i || (cmp = Compare(var, env->var[i - 1])) > 0) {
      // insert entry for new key
      env->var[i] = var;
      env->vari++;
      break;
    }
    if (!cmp) {
      // deduplicate preferring latter
      env->var[i - 1] = var;
      for (; i < env->vari; ++i) {
        env->var[i] = env->var[i + 1];
      }
      break;
    }
    // sift items right to create empty slot at insertion point
    env->var[i] = env->var[i - 1];
  }
  return 0;
}

static textwindows int InsertStrings(struct EnvBuilder *env,
                                     char *const strs[]) {
  if (strs) {
    for (int i = 0; strs[i]; ++i) {
      if (InsertString(env, strs[i]) == -1) {
        return -1;
      }
    }
  }
  return 0;
}

static textwindows int CountStrings(char *const strs[]) {
  int n = 0;
  if (strs) {
    while (*strs++) {
      ++n;
    }
  }
  return n;
}

/**
 * Copies sorted environment variable block for Windows.
 *
 * This is designed to meet the requirements of CreateProcess().
 *
 * @param envblock receives sorted double-NUL terminated string list
 * @param envp is an a NULL-terminated array of UTF-8 strings
 * @param extravar is a VAR=val string we consider part of envp or NULL
 * @return 0 on success, or -1 w/ errno
 * @error E2BIG if total number of shorts (including nul) exceeded 32767
 * @asyncsignalsafe
 */
textwindows int mkntenvblock(char16_t envblock[32767], char *const envp[],
                             char *const extravars[], char buf[32767]) {
  int i, k, n;
  struct Env e;
  struct EnvBuilder env = {buf};

  // allocate string pointer array for sorting purposes
  n = (CountStrings(envp) + CountStrings(extravars) + 1) * sizeof(char *);
#pragma GCC push_options
#pragma GCC diagnostic ignored "-Walloca-larger-than="
  env.var = alloca(n);
  CheckLargeStackAllocation(env.var, n);
#pragma GCC pop_options

  // load new environment into string pointer array and fix file paths
  if (InsertStrings(&env, envp) == -1) return -1;
  if (InsertStrings(&env, extravars) == -1) return -1;
  if (environ) {
    // https://jpassing.com/2009/12/28/the-hidden-danger-of-forgetting-to-specify-systemroot-in-a-custom-environment-block/
    e = __getenv(environ, "SYSTEMROOT");
    if (e.s && InsertString(&env, environ[e.i]) == -1) {
      return -1;
    }
  }

  // copy utf-8 sorted string pointer array into contiguous utf-16 block
  // in other words, we're creating a double-nul terminated string list!
  for (k = i = 0; i < env.vari; ++i) {
    k += tprecode8to16(envblock + k, -1, env.var[i]).ax + 1;
  }
  unassert(k <= env.bufi);
  envblock[k] = 0;

  return 0;
}
