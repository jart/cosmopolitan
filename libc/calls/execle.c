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
#include "libc/calls/calls.h"
#include "libc/mem/alloca.h"

/**
 * Executes program, with custom environment.
 *
 * The current process is replaced with the executed one.
 *
 * @param prog will not be PATH searched, see commandv()
 * @param arg[0] is the name of the program to run
 * @param arg[1,n-3] optionally specify program arguments
 * @param arg[n-2] is NULL
 * @param arg[n-1] is a pointer to a ["key=val",...,NULL] array
 * @return doesn't return on success, otherwise -1 w/ errno
 * @asyncsignalsafe
 * @vforksafe
 */
int execle(const char *exe, const char *arg,
           ... /*, NULL, char *const envp[] */) {
  int i;
  va_list va, vb;
  char **argv, **envp;
  va_copy(vb, va);
  va_start(va, arg);
  for (i = 0; va_arg(va, const char *); ++i) donothing;
  envp = va_arg(va, char **);
  va_end(va);
  argv = alloca((i + 2) * sizeof(char *));
  va_start(vb, arg);
  argv[0] = arg;
  for (i = 1;; ++i) {
    if (!(argv[i] = va_arg(vb, const char *))) break;
  }
  va_end(vb);
  return execve(exe, argv, envp);
}
