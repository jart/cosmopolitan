#include "third_party/chibicc/chibicc.h"
#include "libc/runtime/internal.h"
#include "libc/x/x.h"
#include "tool/build/lib/getargs.h"

int main(int argc, char **argv) {
  int n;
  char **p;
  const char *arg;
  struct GetArgs ga;
  n = 0;
  p = xcalloc(n + 1, sizeof(*p));
  getargs_init(&ga, argv);
  while ((arg = getargs_next(&ga))) {
    p = xrealloc(p, (++n + 1) * sizeof(*p));
    p[n - 1] = xstrdup(arg);
    p[n - 0] = 0;
  }
  getargs_destroy(&ga);
  return chibicc(n, p);
}
