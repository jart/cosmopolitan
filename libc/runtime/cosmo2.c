/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/strace.internal.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/nr.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/rlim.h"
#include "libc/sysv/consts/rlimit.h"
#include "libc/sysv/consts/sig.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
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

wontreturn textstartup void cosmo(long *sp, struct Syslib *m1, char *exename,
                                  int os, long *is_freebsd) {

  // get startup timestamp as early as possible
  // its used by --strace and also kprintf() %T
  kStartTsc = rdtsc();

  // enable enough tls to survive until it can be allocated properly
  struct CosmoTib tib = {
      .tib_self = &tib,
      .tib_self2 = &tib,
      .tib_sigmask = -1,
      .tib_sigstack_size = 57344,
      .tib_sigstack_addr = (char *)__builtin_frame_address(0) - 57344,
      .tib_tid = 1,
  };
  __set_tls(&tib);

  // check for freebsd
  if (is_freebsd) {
    sp = is_freebsd;
  }

  // extracts arguments from old sysv stack abi
  int argc = *sp;
  char **argv = (char **)(sp + 1);
  char **envp = (char **)(sp + 1 + argc + 1);
  unsigned long *auxv = (unsigned long *)(sp + 1 + argc + 1);
  while (*auxv++) donothing;

  // set helpful globals
  __argc = argc;
  __argv = argv;
  __envp = envp;
  __auxv = auxv;
  environ = envp;
  __program_executable_name = exename;
  program_invocation_name = argv[0];
  __oldstack = (intptr_t)sp;
  if (!(hostos = os)) {
    if (SupportsFreebsd() && is_freebsd) {
      hostos = _HOSTFREEBSD;
    } else if (SupportsXnu() && m1) {
      hostos = _HOSTXNU;
    } else if (SupportsLinux()) {
      hostos = _HOSTLINUX;
    } else {
      notpossible;
    }
  }

  const char *magnums;
  if (IsFreebsd()) {
    magnums = syscon_freebsd;
  } else if (IsXnu()) {
    if (!(__syslib = m1)) {
      notpossible;
    }
    magnums = syscon_xnu;
  } else if (IsLinux()) {
    magnums = syscon_linux;
  } else {
    notpossible;
  }

  // setup system magic numbers
  for (long *mp = syscon_start; mp < syscon_end; ++mp) {
    magnums = DecodeMagnum(magnums, mp);
  }

  // check system call abi compatibility
  if (IsXnu() && __syslib->__version < SYSLIB_VERSION_MANDATORY) {
    sys_write(2, "need newer ape loader\n", 22);
    _Exit(127);
  }

  // disable enosys trapping
  if (IsBsd()) {
    void *act[6] = {SIG_IGN};
    sys_sigaction(SIGSYS, act, 0, 8, 0);
  }

  // needed by kisdangerous()
  __pid = sys_getpid().ax;

  // initialize memory manager
  _mmi.i = 0;
  _mmi.p = _mmi.s;
  _mmi.n = ARRAYLEN(_mmi.s);
  __virtualmax = -1;

  // initialize file system
  __init_fds(argc, argv, envp);

  // prepend cwd to executable path
  __init_program_executable_name();

  __enable_tls();

#if 0
#if IsAsan()
  // TODO(jart): Figure out ASAN data model on AARCH64.
  __asan_init(argc, argv, envp, auxv);
#endif
#endif

  _init();
  // initialize program
#if SYSDEBUG
  argc = __strace_init(argc, argv, envp, auxv);
#endif
  for (init_f **fp = __init_array_end; fp-- > __init_array_start;) {
    (*fp)(argc, argv, envp, auxv);
  }
#ifdef FTRACE
  argc = ftrace_init();
#endif

  // run program
  exit(main(argc, argv, envp));
}

#endif /* __x86_64__ */
