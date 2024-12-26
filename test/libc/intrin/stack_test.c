/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/calls.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/cosmo.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/testlib/testlib.h"

// returns true if byte at memory address is readable
bool readable(void *addr) {
  return testlib_pokememory(addr);
}

// returns true if page is reserved by linux memory manager
// it can be true for addresses that aren't listed in /proc/PID/maps
bool occupied(void *addr) {
  int olde = errno;
  char *want = (char *)((uintptr_t)addr & -__pagesize);
  char *got =
      __sys_mmap(want, __pagesize, PROT_READ | PROT_WRITE,
                 MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, -1, 0, 0);
  if (got == MAP_FAILED) {
    unassert(errno == IsFreebsd() ? EINVAL : EEXIST);
    errno = olde;
    return true;
  }
  sys_munmap(got, __pagesize);
  return got != want;
}

TEST(stack, test) {
  if (IsWindows())
    return;

  void *vstackaddr;
  size_t stacksize = 65536;
  size_t guardsize = 4096;
  unassert(!cosmo_stack_alloc(&stacksize, &guardsize, &vstackaddr));
  char *stackaddr = vstackaddr;

  /* check memory reservation */
  unassert(occupied(stackaddr + stacksize - 1));  // top stack
  unassert(occupied(stackaddr));                  // bot stack
  unassert(occupied(stackaddr - 1));              // top guard
  unassert(occupied(stackaddr - guardsize));      // bot guard

  /* check memory accessibility */
  unassert(readable(stackaddr + stacksize - 1));  // top stack
  unassert(readable(stackaddr));                  // bot stack
  unassert(!readable(stackaddr - 1));             // top guard
  unassert(!readable(stackaddr - guardsize));     // bot guard

  unassert(!cosmo_stack_free(stackaddr, stacksize, guardsize));
}
