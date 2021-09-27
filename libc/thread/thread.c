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
#include "libc/thread/thread.h"
#include "libc/linux/mmap.h"
#include "libc/linux/clone.h"

// sched.h
#define CLONE_VM        0x00000100
#define CLONE_FS        0x00000200
#define CLONE_FILES     0x00000400
#define CLONE_SIGHAND   0x00000800
#define CLONE_PARENT    0x00008000
#define CLONE_THREAD    0x00010000
#define CLONE_SETTLS    0x00080000
#define CLONE_IO        0x80000000

// sys/mman.h
#define MAP_GROWSDOWN   0x0100
#define MAP_ANONYMOUS   0x0020
#define MAP_PRIVATE     0x0002
#define PROT_READ       0x1
#define PROT_WRITE      0x2
#define PROT_EXEC       0x4


thread_descriptor_t* allocate_thread() {
  const long pagesize = 4096;
  long stacksize = 1024*pagesize;
  // FIXME: properly count TLS size
  long tlssize = 0;
  long totalsize = stacksize + tlssize + sizeof(thread_descriptor_t);
  
  // round-up totalsize to pagesize
  totalsize = (totalsize + pagesize-1) & -pagesize;
  
  intptr_t mem = LinuxMmap(NULL, totalsize, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  if (mem == -1) return NULL;
  
  thread_descriptor_t* descriptor = (thread_descriptor_t*)(mem + totalsize - sizeof(thread_descriptor_t));
  descriptor->self = descriptor; // setup TLS (System-V ABI)
  descriptor->stack = (void*)(mem + stacksize);
  
  // FIXME: properly copy .tdata
  return descriptor;
}
long start_thread(thread_descriptor_t* descriptor, int (*func)(void*), void* arg) {
  extern wontreturn void thread_spawn(int(*func)(void*), void* arg);
  
  // Set-up thread stack
  uintptr_t stack = (uintptr_t)(descriptor->stack) - 3*sizeof(void*);
  *(void**)(stack + 2*sizeof(void*)) = func;
  *(void**)(stack + 1*sizeof(void*)) = arg;
  *(void**)(stack + 0*sizeof(void*)) = &thread_spawn;
  
  long flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_PARENT | CLONE_THREAD | CLONE_IO | CLONE_SETTLS;
  
  // It is not necessary to check the return of the syscall here as the return address for the thread is setup to to `thread_spawn`.
  // In case of success: the parent callee returns immediately to the caller forwarding the success to the callee
  //                     the child return immediately to the entry point of `thread_spawn`
  // In case of error: the parent callee returns immediately to the caller forwarding the error
  //                   the child is never created (and so cannot returned in the wrong place)
  return LinuxClone(flags, (void*)stack, NULL, NULL, descriptor);
}