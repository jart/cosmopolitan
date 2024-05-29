/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/x86.h"
#include "libc/runtime/runtime.h"
#include "third_party/getopt/getopt.internal.h"

/**
 * @fileoverview Command for printing `-march=native` flags.
 *
 * In recent years (2023) compilers have decided to remove support for
 * the `-march=native` flag, even on x86. That's unfortunate, since as
 * we can see below, grokking all the various microarchitecture is not
 * something a compiler should reasonably expect from users especially
 * not for a flag as important as this one, which can have a night and
 * day impact for apps that do scientific computing.
 *
 * This is a tiny program, that makes it easy for shell scripts to get
 * these flags.
 */

#define VERSION                       \
  "-march=native flag printer v0.2\n" \
  "copyright 2024 justine alexandra roberts tunney\n"

#define USAGE                    \
  "usage: march-native [-hvc]\n" \
  "  -h          show help\n"    \
  "  -v          show version\n" \
  "  -c          assume we're using clang (not gcc)\n"

static bool isclang;

static void GetOpts(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "hvc")) != -1) {
    switch (opt) {
      case 'c':
        isclang = true;
        break;
      case 'v':
        tinyprint(1, VERSION, NULL);
        exit(0);
      case 'h':
        tinyprint(1, VERSION, USAGE, NULL);
        exit(0);
      default:
        tinyprint(2, VERSION, USAGE, NULL);
        exit(1);
    }
  }
}

static void Puts(const char *s) {
  tinyprint(1, s, "\n", NULL);
}

int main(int argc, char *argv[]) {
  GetOpts(argc, argv);
#ifdef __x86_64__
  const char *march;
  if ((march = __cpu_march(__cpu_model.__cpu_subtype)))
    tinyprint(1, "-march=", march, "\n", NULL);
#elif defined(__aarch64__)
  // TODO(jart): How can we determine CPU features on AARCH64?
#else
  // otherwise do nothing (it's usually best)
#endif
}
