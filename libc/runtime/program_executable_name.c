/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/assert.h"
#include "libc/bits/bits.h"
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/mem/alloca.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/ok.h"

/**
 * Contains absolute path of executable.
 *
 * This variable is initialized automatically at startup. It always
 * holds a correct absolute path name which is not guaranteed to be
 * canonical.
 */
char program_executable_name[PATH_MAX];

textstartup void program_executable_name_init(int argc, char **argv,
                                              char **envp, intptr_t *auxv) {
  size_t n;
  char *p, *t;
  static bool once;
  if (!cmpxchg(&once, 0, 1)) return;
  for (p = argv[0]; auxv[0]; auxv += 2) {
    if (auxv[0] == AT_EXECFN) {
      p = (char *)auxv[1];
      break;
    }
  }
  n = 0;
  if (!_isabspath(p)) {
    if (getcwd(program_executable_name, PATH_MAX - 2)) {
      n = strlen(program_executable_name);
      program_executable_name[n++] = '/';
    }
  }
  memccpy(program_executable_name + n, p, '\0', PATH_MAX - n);
#ifndef NDEBUG
  if (IsMetal()) return; /* TODO(jart): do metal */
  if (!_isabspath(program_executable_name) ||
      !fileexists(program_executable_name)) {
    p = t = alloca(PATH_MAX + 32);
    p = stpcpy(p, "could not find executable: ");
    p = stpcpy(p, program_executable_name);
    *p++ = '\n';
    write(2, t, p - t);
    _exit(1);
  }
#endif
}

const void *const program_executable_name_init_ctor[] initarray = {
    program_executable_name_init,
};
