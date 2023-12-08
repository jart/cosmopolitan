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
#include "libc/calls/struct/stat.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/sysv/errfuns.h"

/**
 * Reads file metadata from αcτµαlly pδrταblε εxεcµταblε object store.
 *
 * @param name is obtained via __zipos_parseuri()
 * @asyncsignalsafe
 */
int __zipos_stat(struct ZiposUri *name, struct stat *st) {
  ssize_t cf;
  struct Zipos *zipos;
  if (!(zipos = __zipos_get())) return enoexec();
  if ((cf = __zipos_find(zipos, name)) == -1) return -1;
  if (__zipos_stat_impl(zipos, cf, st)) return -1;
  st->st_ino = __zipos_inode(zipos, cf, name->path, name->len);
  return 0;
}
