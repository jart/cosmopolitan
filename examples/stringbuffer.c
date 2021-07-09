#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/calls/calls.h"
#include "libc/fmt/fmt.h"

/**
 * @fileoverview Fast Growable Strings Tutorial
 */

struct Buffer {
  size_t i, n;
  char *p;
};

int AppendFmt(struct Buffer *b, const char *fmt, ...) {
  int n;
  char *p;
  va_list va, vb;
  va_start(va, fmt);
  va_copy(vb, va);
  n = vsnprintf(b->p + b->i, b->n - b->i, fmt, va);
  if (n >= b->n - b->i) {
    do {
      if (b->n) {
        b->n += b->n >> 1; /* this is the important line */
      } else {
        b->n = 16;
      }
    } while (b->i + n + 1 > b->n);
    b->p = realloc(b->p, b->n);
    vsnprintf(b->p + b->i, b->n - b->i, fmt, vb);
  }
  va_end(vb);
  va_end(va);
  b->i += n;
  return n;
}

int main(int argc, char *argv[]) {
  struct Buffer b = {0};
  AppendFmt(&b, "hello ");
  AppendFmt(&b, " world\n");
  AppendFmt(&b, "%d arg%s\n", argc, argc == 1 ? "" : "s");
  AppendFmt(&b, "%s\n", "have a nice day");
  write(1, b.p, b.i);
  free(b.p);
  return 0;
}
