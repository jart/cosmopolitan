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
#include "libc/calls/struct/stat.h"
#include "libc/log/check.h"
#include "libc/mem/mem.h"
#include "libc/runtime/gc.h"
#include "libc/sysv/consts/o.h"
#include "third_party/duktape/duktape.h"

STATIC_YOINK("zip_uri_support");

static duk_ret_t NativePrint(duk_context *ctx) {
  duk_push_string(ctx, " ");
  duk_insert(ctx, 0);
  duk_join(ctx, duk_get_top(ctx) - 1);
  fputs(duk_safe_to_string(ctx, -1), stdout);
  return 0;
}

static duk_ret_t NativeAdd(duk_context *ctx) {
  int i, n;
  double r;
  n = duk_get_top(ctx);
  for (r = i = 0; i < n; i++) r += duk_to_number(ctx, i);
  duk_push_number(ctx, r);
  return 1;
}

int main(int argc, char *argv[]) {
  int fd;
  struct stat st;
  const char *code;
  duk_context *ctx;
  ctx = duk_create_heap_default();
  duk_push_c_function(ctx, NativePrint, DUK_VARARGS);
  duk_put_global_string(ctx, "NativePrint");
  duk_push_c_function(ctx, NativeAdd, DUK_VARARGS);
  duk_put_global_string(ctx, "NativeAdd");

  CHECK_NE(-1, (fd = open("zip:examples/hello.js", O_RDONLY)));
  CHECK_NE(-1, fstat(fd, &st));
  CHECK_NOTNULL((code = gc(malloc(st.st_size))));
  CHECK_EQ(st.st_size, read(fd, code, st.st_size));
  CHECK_NE(-1, close(fd));

  duk_eval_string(ctx, code);
  duk_pop(ctx);
  duk_destroy_heap(ctx);
  return 0;
}
