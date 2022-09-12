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
#include "libc/calls/calls.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/errfuns.h"

uintptr_t __break;

/**
 * Sets end of data section.
 *
 * This can be used to allocate and deallocate memory. It won't
 * conflict with malloc() and mmap(NULL, ...) allocations since
 * APE binaries load the image at 0x440000 and does allocations
 * starting at 0x100080040000. You should consult _end, or call
 * sbrk(NULL), to figure out where the existing break is first.
 *
 * @return 0 on success or -1 w/ errno
 * @see mmap(), sbrk(), _end
 */
int brk(void *end) {
  int rc;
  uintptr_t x;
  if (!__break) __break = (uintptr_t)_end;
  x = (uintptr_t)end;
  if (x < (uintptr_t)_end) x = (uintptr_t)_end;
  x = ROUNDUP(x, FRAMESIZE);
  if (x == __break) return 0;
  /* allocate one frame at a time due to nt pickiness */
  for (; x > __break; __break += FRAMESIZE) {
    if (mmap((void *)__break, FRAMESIZE, PROT_READ | PROT_WRITE,
             MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0) == MAP_FAILED) {
      return -1;
    }
  }
  for (rc = 0; x < __break; __break -= FRAMESIZE) {
    rc |= munmap((void *)(__break - FRAMESIZE), FRAMESIZE);
  }
  return 0;
}

/**
 * Adjusts end of data section.
 *
 * This shrinks or increases the program break by delta bytes. On
 * success, the previous program break is returned. It's possible
 * to pass zero to this function to get the current program break
 *
 * @return old break on success or -1 w/ errno
 * @see mmap(), brk(), _end
 */
void *sbrk(intptr_t delta) {
  uintptr_t oldbreak;
  if (!__break) __break = (uintptr_t)_end;
  oldbreak = __break;
  return (void *)(brk((void *)(__break + delta)) != -1 ? oldbreak : -1);
}
