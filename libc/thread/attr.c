/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/errno.h"
#include "libc/thread/attr.h"

#define MIN_STACKSIZE (8 * PAGESIZE)
#define MIN_GUARDSIZE PAGESIZE

// CTOR/DTOR
int cthread_attr_init(cthread_attr_t* attr) {
  attr->stacksize = 1024 * PAGESIZE;  // 4 MiB
  attr->guardsize = 16 * PAGESIZE;    // 64 KiB
  attr->mode = CTHREAD_CREATE_JOINABLE;
  return 0;
}
int cthread_attr_destroy(cthread_attr_t* attr) {
  (void)attr;
  return 0;
}

// stacksize
int cthread_attr_setstacksize(cthread_attr_t* attr, size_t size) {
  if (size & (PAGESIZE - 1)) return EINVAL;
  if (size < MIN_STACKSIZE) return EINVAL;
  attr->stacksize = size;
  return 0;
}
size_t cthread_attr_getstacksize(const cthread_attr_t* attr) {
  return attr->stacksize;
}

// guardsize
int cthread_attr_setguardsize(cthread_attr_t* attr, size_t size) {
  if (size & (PAGESIZE - 1)) return EINVAL;
  if (size < MIN_GUARDSIZE) return EINVAL;
  attr->guardsize = size;
  return 0;
}
size_t cthread_attr_getguardsize(const cthread_attr_t* attr) {
  return attr->guardsize;
}

// detachstate
int cthread_attr_setdetachstate(cthread_attr_t* attr, int mode) {
  if (mode & ~(CTHREAD_CREATE_JOINABLE | CTHREAD_CREATE_DETACHED))
    return EINVAL;
  attr->mode = mode;
  return 0;
}
int cthread_attr_getdetachstate(const cthread_attr_t* attr) {
  return attr->mode;
}
