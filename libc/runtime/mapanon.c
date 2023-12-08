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
#include "libc/errno.h"
#include "libc/intrin/weaken.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/errfuns.h"

/**
 * Helper function for allocating anonymous mapping.
 *
 * This function is equivalent to:
 *
 *     mmap(NULL, mapsize, PROT_READ | PROT_WRITE,
 *          MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
 *
 * If mmap() fails, possibly because the parent process did this:
 *
 *     if (!vfork()) {
 *       setrlimit(RLIMIT_AS, &(struct rlimit){maxbytes, maxbytes});
 *       execv(prog, (char *const[]){prog, 0});
 *     }
 *     wait(0);
 *
 * Then this function will call:
 *
 *     __oom_hook(size);
 *
 * If it's linked. The LIBC_TESTLIB library provides an implementation,
 * which can be installed as follows:
 *
 *     int main() {
 *         InstallQuotaHandlers();
 *         // ...
 *     }
 *
 * That is performed automatically for unit test executables.
 *
 * @return memory map address on success, or null w/ errno
 */
void *_mapanon(size_t size) {
  void *m;
  m = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (m != MAP_FAILED) {
    return m;
  }
  if (errno == ENOMEM && _weaken(__oom_hook)) {
    _weaken(__oom_hook)(size);
  }
  return 0;
}
