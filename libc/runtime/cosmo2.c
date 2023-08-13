/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/strace.internal.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sig.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#include "libc/errno.h"
#include "third_party/lua/lunix.h"
#ifndef __x86_64__

/**
 * @fileoverview Cosmopolitan C Runtime, Second Edition
 */

int main(int, char **, char **) __attribute__((__weak__));

typedef int init_f(int argc, char **argv, char **envp, unsigned long *auxv);

extern long syscon_start[];
extern long syscon_end[];
extern char syscon_linux[];
extern char syscon_xnu[];
extern char syscon_freebsd[];
extern char syscon_openbsd[];
extern char syscon_netbsd[];
extern char syscon_windows[];
extern init_f __strace_init;
extern init_f *__preinit_array_start[] __attribute__((__weak__));
extern init_f *__preinit_array_end[] __attribute__((__weak__));
extern init_f *__init_array_start[] __attribute__((__weak__));
extern init_f *__init_array_end[] __attribute__((__weak__));
extern char ape_stack_vaddr[] __attribute__((__weak__));
extern char ape_stack_memsz[] __attribute__((__weak__));
extern char ape_stack_prot[] __attribute__((__weak__));
extern pthread_mutex_t __mmi_lock_obj;
extern int hostos asm("__hostos");

static const char *DecodeMagnum(const char *p, long *r) {
  int k = 0;
  unsigned long c, x = 0;
  do {
    c = *p++;
    x |= (c & 127) << k;
    k += 7;
  } while (c & 128);
  return *r = x, p;
}

textstartup void cosmo(long *sp, struct Syslib *m1) {
  int argc;
  long *mp;
  init_f **fp;
  uintptr_t *pp;
  unsigned long *auxv;
  char **argv, **envp, *magnums;

  // get startup timestamp as early as possible
  // its used by --strace and also kprintf() %T
  kStartTsc = rdtsc();

  // extracts arguments from old sysv stack abi
  argc = *sp;
  argv = (char **)(sp + 1);
  envp = (char **)(sp + 1 + argc + 1);
  auxv = (unsigned long *)(sp + 1 + argc + 1);
  while (*auxv++) donothing;

  // detect apple m1 environment
  if (SupportsXnu() && (__syslib = m1)) {
    hostos = _HOSTXNU;
    magnums = syscon_xnu;
  } else if (SupportsLinux()) {
    hostos = _HOSTLINUX;
    magnums = syscon_linux;
  } else {
    notpossible;
  }

  // setup system magic numbers
  for (mp = syscon_start; mp < syscon_end; ++mp) {
    magnums = DecodeMagnum(magnums, mp);
  }

  // check system call abi compatibility
  if (SupportsXnu() && __syslib && __syslib->version < SYSLIB_VERSION) {
    sys_write(2, "need newer ape loader\n", 22);
    _Exit(127);
  }

  // disable enosys trapping
  if (IsBsd()) {
    void *act[6] = {SIG_IGN};
    sys_sigaction(SIGSYS, act, 0, 8, 0);
  }

  // needed by kisdangerous()
  __oldstack = (intptr_t)sp;
  __pid = sys_getpid().ax;

  // initialize memory manager
  _mmi.n = ARRAYLEN(_mmi.s);
  _mmi.p = _mmi.s;
  __mmi_lock_obj._type = PTHREAD_MUTEX_RECURSIVE;

  // record system provided stack to memory manager
  // todo: how do we get the real size of the stack
  //       if `y` is too small mmap will destroy it
  //       if `x` is too high, backtraces will fail
  uintptr_t t = (uintptr_t)__builtin_frame_address(0);
  uintptr_t s = (uintptr_t)sp;
  uintptr_t z = GetStackSize() << 1;
  _mmi.i = 1;
  _mmi.p->x = MIN((s & -z) >> 16, (t & -z) >> 16);
  _mmi.p->y = MIN(((s & -z) + (z - 1)) >> 16, INT_MAX);
  _mmi.p->size = z;
  _mmi.p->prot = PROT_READ | PROT_WRITE;
  __virtualmax = -1;

#if 0
#if IsAsan()
  // TODO(jart): Figure out ASAN data model on AARCH64.
  __asan_init(argc, argv, envp, auxv);
#endif
#endif

  // initialize file system
  __init_fds(argc, argv, envp);

  // set helpful globals
  __argc = argc;
  __argv = argv;
  __envp = envp;
  __auxv = auxv;
  environ = envp;
  program_invocation_name = argv[0];

  // initialize program
  _init();
  __enable_tls();
#ifdef SYSDEBUG
  argc = __strace_init(argc, argv, envp, auxv);
#endif
  for (fp = __init_array_end; fp-- > __init_array_start;) {
    (*fp)(argc, argv, envp, auxv);
  }
#ifdef FTRACE
  argc = ftrace_init();
#endif

  // run program
  exit(main(argc, argv, envp));
}

#endif /* __x86_64__ */
