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
#include "libc/thread/create.h"
#include "libc/linux/clone.h"
#include "libc/runtime/runtime.h"
#include "libc/linux/mmap.h"
#include "libc/sysv/consts/clone.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/errno.h"


static cthread_t _thread_allocate(const cthread_attr_t* attr) {
  size_t stacksize = attr->stacksize;
  size_t guardsize = 0;//attr->guardsize;
  // FIXME: properly count TLS size
  size_t tlssize = 0;
  
  size_t totalsize = 3*guardsize + stacksize + tlssize + sizeof(struct cthread_descriptor_t);
  totalsize = (totalsize + PAGESIZE-1) & -PAGESIZE;
  
  uintptr_t mem = (uintptr_t)mmap(NULL, totalsize, PROT_NONE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  if (mem == -1) return NULL;
  
  void* alloc_bottom = (void*) mem;
  void* stack_bottom = (void*)(mem + guardsize);
  void* stack_top = (void*)(mem + guardsize + stacksize);
  void* tls_bottom = (void*)(mem + guardsize + stacksize + guardsize);
  void* tls_top = (void*)(mem + totalsize - guardsize);
  void* alloc_top = (void*)(mem + totalsize);
  
  if (mprotect(stack_bottom, (uintptr_t)stack_top - (uintptr_t)stack_bottom, PROT_READ | PROT_WRITE) != 0 ||
      mprotect(tls_bottom, (uintptr_t)tls_top - (uintptr_t)tls_bottom, PROT_READ | PROT_WRITE) != 0) {
    munmap(alloc_bottom, totalsize);
    return NULL;
  }
  
  cthread_t td = (cthread_t)tls_top - 1;
  td->self = td;
  td->stack.top = stack_top;
  td->stack.bottom = stack_bottom;
  td->tls.top = tls_top;
  td->tls.bottom = tls_bottom;
  td->alloc.top = alloc_top;
  td->alloc.bottom = alloc_bottom;
  td->tid = 0;
  
  return td;
}

int cthread_create(cthread_t*restrict p, const cthread_attr_t*restrict attr, int (*func)(void*), void*restrict arg) {
  extern wontreturn void _thread_run(int(*func)(void*), void* arg);
  
  cthread_attr_t default_attr;
  if (!attr) cthread_attr_init(&default_attr);
  cthread_t td = _thread_allocate(attr ? attr : &default_attr);
  if (!attr) cthread_attr_destroy(&default_attr);
  if (!td) return errno;
  
  *p = td;
  
  uintptr_t stack = (uintptr_t)(td->stack.top);
  
  stack -= sizeof(void*); *(void**)stack = func;
  stack -= sizeof(void*); *(void**)stack = arg;
  
  long flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_PARENT | CLONE_THREAD | /*CLONE_IO |*/ CLONE_SETTLS;
  
  // It is not necessary to check the return of the syscall here as the return address for the thread is setup to to `_thread_run`.
  // In case of success: the parent callee returns immediately to the caller forwarding the success to the callee
  //                     the child return immediately to the entry point of `thread_spawn`
  // In case of error: the parent callee returns immediately to the caller forwarding the error
  //                   the child is never created (and so cannot returned in the wrong place)
  int rc = LinuxClone(flags, (void*)stack, NULL, NULL, (void*)td);
  if (!rc) {
    // child
    asm volatile(
      "xor %rbp,%rbp\n\t"
      /* pop arguments */
      "pop %rdi\n\t"
      "pop %rax\n\t"
      /* call function */
      "call *%rax\n\t"
      /* thread exit */
      "mov %rax, %rdi\n\t"
      "jmp cthread_exit"
    );
    unreachable;
  }
  if (rc < 0) return rc;
  return 0;
}
