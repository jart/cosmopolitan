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
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/nr.h"
#include "libc/sysv/consts/prot.h"
#include "libc/thread/descriptor.h"

// TLS boundaries
extern char _tbss_start, _tbss_end, _tdata_start, _tdata_end;

static textstartup void _main_thread_init(void) {
  if (!IsLinux()) return; /* TODO */
  size_t tbsssize = &_tbss_end - &_tbss_start;
  size_t tdatasize = &_tdata_end - &_tdata_start;
  size_t tlssize = tbsssize + tdatasize;
  size_t totalsize = tlssize + sizeof(struct cthread_descriptor_t);
  totalsize = (totalsize + PAGESIZE - 1) & -PAGESIZE;

  uintptr_t mem = (uintptr_t)mmap(NULL, totalsize, PROT_READ | PROT_WRITE,
                                  MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  if (mem == -1) {
    abort();
  }

  void* bottom = (void*)mem;
  void* top = (void*)(mem + totalsize);

  cthread_t td = (cthread_t)top - 1;
  td->self = td;
  td->stack.top = NULL;
  td->stack.bottom = NULL;
  td->tls.top = top;
  td->tls.bottom = bottom;
  td->alloc.top = top;
  td->alloc.bottom = bottom;
  td->state = cthread_main;

  // Initialize TLS with content of .tdata section
  memmove((void*)((uintptr_t)td - tlssize), &_tdata_start, tdatasize);

  // Get TID of main thread
  int gettid = __NR_gettid;
  if (gettid == 0xfff) gettid = __NR_getpid;
  td->tid = syscall(gettid);

  // Set FS
  if (arch_prctl(ARCH_SET_FS, td) != 0) {
    abort();
  }
}

const void* const _main_thread_ctor[] initarray = {
    _main_thread_init,
};
