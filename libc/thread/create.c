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
#include "libc/linux/clone.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/clone.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/nr.h"
#include "libc/sysv/consts/prot.h"
#include "libc/thread/create.h"

STATIC_YOINK("_main_thread_ctor");

// TLS boundaries
extern char _tbss_start, _tbss_end, _tdata_start, _tdata_end;

static cthread_t _thread_allocate(const cthread_attr_t* attr) {
  size_t stacksize = attr->stacksize;
  size_t guardsize = attr->guardsize;
  size_t tbsssize = &_tbss_end - &_tbss_start;
  size_t tdatasize = &_tdata_end - &_tdata_start;
  size_t tlssize = tbsssize + tdatasize;

  size_t totalsize =
      3 * guardsize + stacksize + tlssize + sizeof(struct cthread_descriptor_t);
  totalsize = (totalsize + PAGESIZE - 1) & -PAGESIZE;

  uintptr_t mem = (uintptr_t)mmap(NULL, totalsize, PROT_NONE,
                                  MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  if (mem == -1) return NULL;

  void* alloc_bottom = (void*)mem;
  void* stack_bottom = (void*)(mem + guardsize);
  void* stack_top = (void*)(mem + guardsize + stacksize);
  void* tls_bottom = (void*)(mem + guardsize + stacksize + guardsize);
  void* tls_top = (void*)(mem + totalsize - guardsize);
  void* alloc_top = (void*)(mem + totalsize);

  if (mprotect(stack_bottom, (uintptr_t)stack_top - (uintptr_t)stack_bottom,
               PROT_READ | PROT_WRITE) != 0 ||
      mprotect(tls_bottom, (uintptr_t)tls_top - (uintptr_t)tls_bottom,
               PROT_READ | PROT_WRITE) != 0) {
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
  td->state = (attr->mode & CTHREAD_CREATE_DETACHED) ? cthread_detached
                                                     : cthread_started;
  // Initialize TLS with content of .tdata section
  memmove((void*)((uintptr_t)td - tlssize), &_tdata_start, tdatasize);

  return td;
}

int cthread_create(cthread_t* restrict p, const cthread_attr_t* restrict attr,
                   int (*func)(void*), void* restrict arg) {
  extern wontreturn void _thread_run(int (*func)(void*), void* arg);

  cthread_attr_t default_attr;
  cthread_attr_init(&default_attr);
  cthread_t td = _thread_allocate(attr ? attr : &default_attr);
  cthread_attr_destroy(&default_attr);
  if (!td) return errno;

  *p = td;

  register cthread_t td_ asm("r8") = td;
  register int* ptid_ asm("rdx") = &td->tid;
  register int* ctid_ asm("r10") = &td->tid;
  register int (*func_)(void*) asm("r12") = func;
  register void* arg_ asm("r13") = arg;

  long flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND |
               CLONE_PARENT | CLONE_THREAD | /*CLONE_IO |*/ CLONE_SETTLS |
               CLONE_PARENT_SETTID | CLONE_CHILD_CLEARTID;
  int rc;
  // asm ensures the (empty) stack of the child thread is not used
  asm volatile("syscall\n\t"                    // clone
               "test\t%0, %0\n\t"               // if not child
               "jne\t.L.cthread_create.%=\n\t"  // jump to `parent` label
               "xor\t%%rbp, %%rbp\n\t"          // reset stack frame pointer
               "mov\t%2, %%rdi\n\t"
               "call\t*%1\n\t"  // call `func(arg)`
               "mov\t%%rax, %%rdi\n\t"
               "jmp\tcthread_exit\n"  // exit thread
               ".L.cthread_create.%=:"
               : "=a"(rc)
               : "r"(func_), "r"(arg_), "0"(__NR_clone), "D"(flags),
                 "S"(td->stack.top), "r"(ptid_), "r"(ctid_), "r"(td_)
               : "rcx", "r11", "cc", "memory");
  if (__builtin_expect(rc < 0, 0)) {
    // `clone` has failed. The thread must be deallocated.
    size_t size = (intptr_t)(td->alloc.top) - (intptr_t)(td->alloc.bottom);
    munmap(td->alloc.bottom, size);
    return -rc;
  }
  return 0;
}
