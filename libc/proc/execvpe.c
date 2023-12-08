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
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/limits.h"
#include "libc/mem/alloca.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

/**
 * Executes program, with path environment search.
 *
 * This function is a wrapper of the execve() system call that does path
 * resolution. The `PATH` environment variable is taken from your global
 * `environ` rather than the `envp` argument.
 *
 * @param prog is the program to launch
 * @param argv is [file,argv₁..argvₙ₋₁,NULL]
 * @param envp is ["key=val",...,NULL]
 * @return doesn't return on success, otherwise -1 w/ errno
 * @asyncsignalsafe
 * @vforksafe
 */
int execvpe(const char *prog, char *const argv[], char *const *envp) {
  size_t i;
  char *exe, **argv2;
  char pathbuf[PATH_MAX];

  // validate memory
  if (IsAsan() &&
      (!__asan_is_valid_str(prog) || !__asan_is_valid_strlist(argv))) {
    return efault();
  }

  if (strchr(prog, '/')) {
    return execve(prog, argv, envp);
  }

  // resolve path of executable
  if (!(exe = commandv(prog, pathbuf, sizeof(pathbuf)))) {
    return -1;
  }

  // change argv[0] to resolved path if it's ambiguous
  // otherwise the program won't have much luck finding itself
  if (argv[0] && *prog != '/' && *exe == '/' && !strcmp(prog, argv[0])) {
    for (i = 0; argv[i++];) (void)0;
#pragma GCC push_options
#pragma GCC diagnostic ignored "-Walloca-larger-than="
    int nbytes = i * sizeof(*argv);
    if (__get_safe_size(nbytes, 4096) < nbytes) return enomem();
    argv2 = alloca(nbytes);
    CheckLargeStackAllocation(argv2, nbytes);
#pragma GCC pop_options
    memcpy(argv2, argv, nbytes);
    argv2[0] = exe;
    argv = argv2;
  }

  // execute program
  // tail call shouldn't be possible
  return execve(exe, argv, envp);
}
