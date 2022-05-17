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
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/intrin/threaded.h"
#include "libc/nt/thread.h"

/**
 * Global variable for last error.
 *
 * The system call wrappers update this with WIN32 error codes.
 * Unlike traditional libraries, Cosmopolitan error codes are
 * defined as variables. By convention, system calls and other
 * functions do not update this variable when nothing's broken.
 *
 * @see	libc/sysv/consts.sh
 * @see	libc/sysv/errfuns.h
 * @see	__errno_location() stable abi
 */
errno_t __errno;

/**
 * Returns address of thread information block.
 * @see __install_tls()
 * @see clone()
 */
privileged nocallersavedregisters char *__get_tls(void) {
  char *tib, *linear = (char *)0x30;
  if (IsLinux() || IsFreebsd() || IsNetbsd() || IsOpenbsd()) {
    asm("mov\t%%fs:(%1),%0" : "=a"(tib) : "r"(linear));
  } else {
    asm("mov\t%%gs:(%1),%0" : "=a"(tib) : "r"(linear));
    if (IsWindows()) tib = *(char **)(tib + 0x1480 + __tls_index * 8);
  }
  return tib;
}

/**
 * Returns address of errno variable.
 * @see __initialize_tls()
 * @see __install_tls()
 */
privileged nocallersavedregisters errno_t *(__errno_location)(void) {
  if (!__tls_enabled) return &__errno;
  return (errno_t *)(__get_tls() + 0x3c);
}
