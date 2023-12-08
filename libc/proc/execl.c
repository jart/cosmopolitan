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
#include "libc/calls/calls.h"
#include "libc/mem/alloca.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/sysv/errfuns.h"

/**
 * Executes program, with current environment.
 *
 * The current process is replaced with the executed one.
 *
 * @param prog will not be PATH searched, see execlp()
 * @param arg[0] is the name of the program to run
 * @param arg[1,n-2] optionally specify program arguments
 * @param arg[n-1] is NULL
 * @return doesn't return on success, otherwise -1 w/ errno
 * @asyncsignalsafe
 * @vforksafe
 */
int execl(const char *exe, const char *arg, ... /*, NULL*/) {
  int i;
  va_list va, vb;
  va_copy(vb, va);
  va_start(va, arg);
  for (i = 0; va_arg(va, const char *); ++i) donothing;
  va_end(va);
#pragma GCC push_options
#pragma GCC diagnostic ignored "-Walloca-larger-than="
  int nbytes = (i + 2) * sizeof(char *);
  if (__get_safe_size(nbytes, 4096) < nbytes) return enomem();
  char **argv = alloca(nbytes);
  CheckLargeStackAllocation(argv, nbytes);
#pragma GCC pop_options
  va_start(vb, arg);
  argv[0] = (char *)arg;
  for (i = 1;; ++i) {
    if (!(argv[i] = va_arg(vb, char *))) break;
  }
  va_end(vb);
  return execv(exe, argv);
}
