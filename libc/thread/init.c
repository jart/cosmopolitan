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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/threaded.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/thread/thread.h"

static textstartup void _main_thread_init(void) {
  _Static_assert(offsetof(struct cthread_descriptor_t, self) == 0x00, "");
  _Static_assert(offsetof(struct cthread_descriptor_t, self2) == 0x30, "");
  _Static_assert(offsetof(struct cthread_descriptor_t, tid) == 0x38, "");
  _Static_assert(offsetof(struct cthread_descriptor_t, err) == 0x3c, "");
  cthread_t td;
  size_t totalsize;
  char *mem, *bottom, *top;

  totalsize = ROUNDUP(
      (uintptr_t)_tls_size + sizeof(struct cthread_descriptor_t), FRAMESIZE);

  mem = mmap(0, totalsize, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE,
             -1, 0);
  assert(mem != MAP_FAILED);

  bottom = mem;
  top = mem + totalsize;

  td = (cthread_t)(top - sizeof(struct cthread_descriptor_t));
  td->self = td;
  td->self2 = td;
  td->err = errno;
  td->tid = gettid();
  td->alloc.bottom = bottom;
  td->alloc.top = top;
  td->stack.bottom = GetStackAddr(0);
  td->stack.top = td->stack.bottom + GetStackSize();
  td->state = cthread_main;

  // Initialize TLS with content of .tdata section
  memmove((void *)((uintptr_t)td - (uintptr_t)_tls_size), _tdata_start,
          (uintptr_t)_tdata_size);

  // Set FS
  __install_tls((char *)td);
  assert(cthread_self()->tid == gettid());
  __threaded = true;
}

const void *const _main_thread_ctor[] initarray = {
    _main_thread_init,
};
