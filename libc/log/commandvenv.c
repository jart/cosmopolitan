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
#include "libc/intrin/safemacros.internal.h"
#include "libc/limits.h"
#include "libc/log/log.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/ok.h"

/**
 * Finds full executable path in overridable way.
 *
 * This is a higher level version of the commandv() function. Programs
 * that spawn subprocesses can use this function to determine the path
 * at startup. Here's an example how you could use it:
 *
 *     if ((strace = commandvenv("STRACE", "strace"))) {
 *       strace = strdup(strace);
 *     } else {
 *       fprintf(stderr, "error: please install strace\n");
 *       exit(1);
 *     }
 *
 * @param var is environment variable which may be used to override
 *     PATH search, and it can force a NULL result if it's empty
 * @param cmd is name of program, which is returned asap if it's an
 *     absolute path
 * @return pointer to exe path string, or NULL if it couldn't be found
 *     or the environment variable was empty; noting that the caller
 *     should copy this string before saving it
 */
const char *commandvenv(const char *var, const char *cmd) {
  const char *exepath;
  static char pathbuf[PATH_MAX];
  if (*cmd == '/' || *cmd == '\\') return cmd;
  if ((exepath = getenv(var))) {
    if (isempty(exepath)) return NULL;
    if (access(exepath, X_OK) != -1) {
      return exepath;
    } else {
      return NULL;
    }
  }
  return commandv(cmd, pathbuf, sizeof(pathbuf));
}
