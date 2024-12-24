/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/calls.h"
#include "libc/dlopen/dlfcn.h"
#include "libc/limits.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

struct big {
  long x1;
  long x2;
  long x3;
  long x4;
  long x5;
  long x6;
  long x7;
  long x8;
  long x9;
  double f0;
  double f1;
};

int main(int argc, char *argv[]) {

  void *lib = cosmo_dlopen(argv[1], RTLD_LAZY);
  if (!lib) {
    tinyprint(2, "cosmo_dlopen() failed: ", cosmo_dlerror(), "\n", NULL);
    exit(1);
  }

  fprintf(stderr, "opened library!\n");

  long (*big)(struct big *, long, long, long, long, long, long, long, long,
              long, double, double) = cosmo_dlsym(lib, "big");
  if (!big) {
    tinyprint(2, "cosmo_dlsym() failed: ", cosmo_dlerror(), "\n", NULL);
    exit(1);
  }

  fprintf(stderr, "imported symbol!\n");

  struct big args = {0};
  long res = big(&args, 1, 2, 3, 4, 5, 6, 7, 8, 9, 3.14, 2.3);

  int fails = 0;

  if (res != 31337) {
    fprintf(stderr, "error: bad result: %016lx\n", res);
    ++fails;
  }

  if (args.x1 != 1) {
    fprintf(stderr, "error: bad x1: %016lx\n", args.x1);
    ++fails;
  }

  if (args.x2 != 2) {
    fprintf(stderr, "error: bad x2: %016lx\n", args.x2);
    ++fails;
  }

  if (args.x3 != 3) {
    fprintf(stderr, "error: bad x3: %016lx\n", args.x3);
    ++fails;
  }

  if (args.x4 != 4) {
    fprintf(stderr, "error: bad x4: %016lx\n", args.x4);
    ++fails;
  }

  if (args.x5 != 5) {
    fprintf(stderr, "error: bad x5: %016lx\n", args.x5);
    ++fails;
  }

  if (args.x6 != 6) {
    fprintf(stderr, "error: bad x6: %016lx\n", args.x6);
    ++fails;
  }

  if (args.x7 != 7) {
    fprintf(stderr, "error: bad x7: %016lx\n", args.x7);
    ++fails;
  }

  if (args.x8 != 8) {
    fprintf(stderr, "error: bad x8: %016lx\n", args.x8);
    ++fails;
  }

  if (args.x9 != 9) {
    fprintf(stderr, "error: bad x9: %016lx\n", args.x9);
    ++fails;
  }

  if (args.f0 != 3.14) {
    fprintf(stderr, "error: bad f0: %g\n", args.f0);
    ++fails;
  }

  if (args.f1 != 2.3) {
    fprintf(stderr, "error: bad f1: %g\n", args.f1);
    ++fails;
  }

  cosmo_dlclose(lib);
  return fails;
}
