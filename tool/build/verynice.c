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
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "third_party/getopt/getopt.internal.h"

/**
 * @fileoverview `verynice foo` launches `foo` as low priority as possible
 *
 * This is particularly useful on Linux systems with a spinning disk
 * where the classic `nice` command doesn't do much.
 */

const char *prog;

static wontreturn void PrintUsage(int rc, int fd) {
  tinyprint(fd, "Usage: ", prog, " COMMAND...\n", NULL);
  exit(rc);
}

int main(int argc, char *argv[]) {

  prog = argv[0];
  if (!prog) prog = "verynice";

  int opt;
  while ((opt = getopt(argc, argv, "h")) != -1) {
    switch (opt) {
      case 'h':
        PrintUsage(0, 1);
      default:
        PrintUsage(1, 2);
    }
  }
  if (optind == argc) {
    tinyprint(2, prog, ": missing command\n", NULL);
    exit(1);
  }

  verynice();
  execvp(argv[optind], argv + optind);
  perror(argv[optind]);
  exit(127);
}
