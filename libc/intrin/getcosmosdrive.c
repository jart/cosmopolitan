/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2025 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/ctype.h"
#include "libc/intrin/getenv.h"
#include "libc/nt/process.h"
#include "libc/runtime/runtime.h"

/**
 * Returns root drive letter on Windows.
 *
 * Normally this returns `'C'` but you may want to setup a dev drive on
 * Windows 11 for Cosmopolitan where your `~/.profile` defines like:
 *
 *     export SYSTEMDRIVE=B:
 *     export PROGRAMDATA=/B/ProgramData
 *     export PATH=/B/bin
 *     export TMPDIR=/tmp
 *     export TEMP=$TMPDIR
 *     export TMP=$TMPDIR
 *
 * This means whenever you say something like `/bin/sh` it's actually
 * going to mean `/B/bin/sh` rather than the regular `/C/bin/sh` that
 * would normally be assumed.
 *
 * Please be brave when changing this environment variable. It's very
 * deeply tied into the behavior of pretty much all i/o system calls.
 * Even the system for delivering signals between procesess needs it.
 * Therefore the smartest thing to do after putenv("SYSTEMDRIVE=B:"),
 * would probably be execve(GetProgramExecutableName()) to re-launch.
 */
textwindows char __getcosmosdrive(void) {
  char *p;
  uint32_t n;
  struct Env e;
  char16_t b[3];
  if (environ)
    if ((p = (e = __getenv(environ, "COSMOSDRIVE")).s))
      if (isalpha(p[0]) && p[1] == ':' && !p[2])
        return p[0];
  n = GetEnvironmentVariable(u"COSMOSDRIVE", b, 3);
  if (n == 2 && isalpha(b[0]) && b[1] == ':')
    return b[0];
  n = GetEnvironmentVariable(u"SYSTEMDRIVE", b, 3);
  if (n == 2 && isalpha(b[0]) && b[1] == ':')
    return b[0];
  return 'C';
}
