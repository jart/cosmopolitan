#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/mem/alg.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/x/xasprintf.h"

/**
 * @fileoverview Process Initialization Stack Explorer
 */

struct Thing {
  intptr_t i;
  char *s;
};

struct Things {
  size_t n;
  struct Thing *p;
} things;

void Append(intptr_t i, char *s) {
  things.p = realloc(things.p, ++things.n * sizeof(*things.p));
  things.p[things.n - 1].i = i;
  things.p[things.n - 1].s = s;
}

int Compare(const void *a, const void *b) {
  struct Thing *x = (struct Thing *)a;
  struct Thing *y = (struct Thing *)b;
  if (x->i < y->i) return +1;
  if (x->i > y->i) return -1;
  return 0;
}

int main(int argc, char *argv[]) {
  Append((uintptr_t)__builtin_frame_address(0), "__builtin_frame_address(0)");
  Append((uintptr_t)__oldstack, "__oldstack");
  for (int i = 0;; ++i) {
    Append((uintptr_t)&argv[i], xasprintf("&argv[%d] = %`'s", i, argv[i]));
    if (!argv[i]) break;
    Append((uintptr_t)argv[i], xasprintf("argv[%d] = %`'s", i, argv[i]));
  }
  for (int i = 0;; ++i) {
    Append((uintptr_t)&environ[i],
           xasprintf("&environ[%d] = %`'s", i, environ[i]));
    if (!environ[i]) break;
    Append((uintptr_t)environ[i],
           xasprintf("environ[%d] = %`'s", i, environ[i]));
  }
  for (int i = 0;; i += 2) {
    Append((uintptr_t)&__auxv[i], xasprintf("&auxv[%d] = %ld", i, __auxv[i]));
    if (!__auxv[i]) break;
    Append((uintptr_t)&__auxv[i + 1],
           xasprintf("&auxv[%d] = %#lx", i + 1, __auxv[i + 1]));
  }
  qsort(things.p, things.n, sizeof(*things.p), Compare);
  for (int i = 0; i < things.n; ++i) {
    printf("%012lx %s\n", things.p[i].i, things.p[i].s);
  }
}
