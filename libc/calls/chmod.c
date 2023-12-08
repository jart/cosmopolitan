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
#include "libc/sysv/consts/at.h"

/**
 * Changes permissions on file, e.g.:
 *
 *     CHECK_NE(-1, chmod("foo/bar.txt", 0644));
 *     CHECK_NE(-1, chmod("o/default/program.com", 0755));
 *     CHECK_NE(-1, chmod("privatefolder/", 0700));
 *
 * The esoteric bits generally available on System Five are:
 *
 *     CHECK_NE(-1, chmod("/opt/", 01000));          // sticky bit
 *     CHECK_NE(-1, chmod("/usr/bin/sudo", 04755));  // setuid bit
 *     CHECK_NE(-1, chmod("/usr/bin/wall", 02755));  // setgid bit
 *
 * @param pathname must exist
 * @param mode contains octal flags (base 8)
 * @errors ENOENT, ENOTDIR, ENOSYS
 * @asyncsignalsafe
 * @see fchmod()
 */
int chmod(const char *pathname, uint32_t mode) {
  return fchmodat(AT_FDCWD, pathname, mode, 0);
}
