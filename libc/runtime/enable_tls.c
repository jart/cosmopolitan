/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "ape/sections.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/getenv.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/maps.h"
#include "libc/intrin/weaken.h"
#include "libc/macros.h"
#include "libc/nt/files.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/stdalign.h"
#include "libc/str/locale.h"
#include "libc/str/locale.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#include "third_party/make/gnumake.h"

#define I(x) ((uintptr_t)x)

extern unsigned char __tls_mov_nt_rax[];
extern unsigned char __tls_add_nt_rax[];

alignas(TLS_ALIGNMENT) static char __static_tls[6016];

static unsigned long ParseMask(const char *str) {
  int c;
  unsigned long x = 0;
  if (str) {
    while ((c = *str++)) {
      x *= 10;
      x += c - '0';
    }
  }
  return x;
}

/**
 * Enables thread local storage for main process.
 *
 * Here's the TLS memory layout on x86_64:
 *
 *                           __get_tls()
 *                               │
 *                              %fs OpenBSD/NetBSD
 *            _Thread_local      │
 *     ┌───┬──────────┬──────────┼───┐
 *     │pad│  .tdata  │  .tbss   │tib│
 *     └───┴──────────┴──────────┼───┘
 *                               │
 *    Linux/FreeBSD/Windows/Mac %gs
 *
 * Here's the TLS memory layout on aarch64:
 *
 *            x28
 *         %tpidr_el0
 *             │
 *             │    _Thread_local
 *         ┌───┼───┬──────────┬──────────┐
 *         │tib│dtv│  .tdata  │  .tbss   │
 *         ├───┴───┴──────────┴──────────┘
 *         │
 *     __get_tls()
 *
 * This function is always called by the core runtime to guarantee TLS
 * is always available to your program. You must build your code using
 * -mno-tls-direct-seg-refs if you want to use _Thread_local.
 *
 * You can use __get_tls() to get the linear address of your tib. When
 * accessing TLS via privileged code you must use __get_tls_privileged
 * because we need code morphing to support The New Technology and XNU
 *
 * On XNU and The New Technology, this function imposes 1ms of latency
 * during startup for larger binaries like Python.
 *
 * If you don't want TLS and you're sure you're not using it, then you
 * can disable it as follows:
 *
 *     int main() {
 *       __tls_enabled_set(false);
 *       // do stuff
 *     }
 *
 * This is useful if you want to wrestle back control of %fs using the
 * arch_prctl() function. However, such programs might not be portable
 * and your `errno` variable also won't be thread safe anymore.
 */
textstartup void __enable_tls(void) {

  // Here's the layout we're currently using:
  //
  //         .balign 4096
  //     _tdata_start:
  //         .tdata
  //         _tdata_size = . - _tdata_start
  //         .balign 4096
  //     _tbss_start:
  //     _tdata_start + _tbss_offset:
  //         .tbss
  //         .balign TLS_ALIGNMENT
  //         _tbss_size = . - _tbss_start
  //     _tbss_end:
  //     _tbss_start + _tbss_size:
  //     _tdata_start + _tls_size:
  //
  // unassert(_tbss_start == _tdata_start + I(_tbss_offset));
  // unassert(_tbss_start + I(_tbss_size) == _tdata_start + I(_tls_size));

#ifdef __x86_64__

  char *mem;
  size_t siz = ROUNDUP(I(_tls_size) + sizeof(struct CosmoTib), TLS_ALIGNMENT);
  if (siz <= sizeof(__static_tls)) {
    // if tls requirement is small then use the static tls block
    // which helps avoid a system call for apes with little tls.
    // this is crucial to keeping life.com 16 kilobytes in size!
    mem = __static_tls;
  } else {
    // if a binary needs this much thread_local storage, then it
    // surely must have linked the mmap() function at some point
    // we can't call mmap() because it's too early for sig block
    mem = _weaken(mmap)(0, siz, PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  }

  struct CosmoTib *tib = (struct CosmoTib *)(mem + siz - sizeof(*tib));
  char *tls = mem + siz - sizeof(*tib) - I(_tls_size);

  // copy in initialized data section
  if (I(_tdata_size))
    memcpy(tls, _tdata_start, I(_tdata_size));

#elif defined(__aarch64__)

  uintptr_t size = ROUNDUP(sizeof(struct CosmoTib), I(_tls_align)) +  //
                   ROUNDUP(sizeof(uintptr_t) * 2, I(_tdata_align)) +  //
                   ROUNDUP(I(_tdata_size), I(_tbss_align)) +          //
                   I(_tbss_size);

  char *mem;
  if (I(_tls_align) <= TLS_ALIGNMENT && size <= sizeof(__static_tls)) {
    // if tls requirement is small then use the static tls block
    // which helps avoid a system call for apes with little tls.
    // this is crucial to keeping life.com 16 kilobytes in size!
    mem = __static_tls;
  } else {
    // if a binary needs this much thread_local storage, then it
    // surely must have linked the mmap() function at some point
    // we can't call mmap() because it's too early for sig block
    mem = _weaken(mmap)(0, size, PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  }

  struct CosmoTib *tib =
      (struct CosmoTib *)(mem +
                          ROUNDUP(sizeof(struct CosmoTib), I(_tls_align)) -
                          sizeof(struct CosmoTib));

  uintptr_t *dtv = (uintptr_t *)(tib + 1);
  size_t dtv_size = sizeof(uintptr_t) * 2;

  char *tdata = (char *)dtv + ROUNDUP(dtv_size, I(_tdata_align));
  if (I(_tdata_size))
    memmove(tdata, _tdata_start, I(_tdata_size));

  // Set the DTV.
  //
  // We don't support dynamic shared objects at the moment. The APE
  // linker script will only produce a single PT_TLS program header
  // therefore our job is relatively simple.
  //
  // @see musl/src/env/__init_tls.c
  // @see https://chao-tic.github.io/blog/2018/12/25/tls
  dtv[0] = 1;
  dtv[1] = (uintptr_t)tdata;

#else
#error "unsupported architecture"
#endif /* __x86_64__ */

  // initialize main thread tls memory
  tib->tib_self = tib;
  tib->tib_self2 = tib;
  tib->tib_errno = __errno;
  tib->tib_strace = __strace;
  tib->tib_ftrace = __ftrace;
  tib->tib_pthread = (pthread_t)&_pthread_static;
  if (IsWindows()) {
    intptr_t hThread;
    DuplicateHandle(GetCurrentProcess(), GetCurrentThread(),
                    GetCurrentProcess(), &hThread, 0, false,
                    kNtDuplicateSameAccess);
    atomic_init(&tib->tib_syshand, hThread);
  } else if (IsXnuSilicon()) {
    tib->tib_syshand = __syslib->__pthread_self();
  }

  int tid;
  if (IsLinux() || IsXnuSilicon()) {
    // gnu/systemd guarantees pid==tid for the main thread so we can
    // avoid issuing a superfluous system call at startup in program
    tid = __pid;
  } else {
    tid = sys_gettid();
  }
  atomic_init(&tib->tib_ptid, tid);
  atomic_init(&tib->tib_ctid, tid);
  // TODO(jart): set_tid_address?

  // inherit signal mask
  if (IsWindows())
    atomic_init(&tib->tib_sigmask, ParseMask(__getenv(environ, "_MASK").s));

  // initialize posix threads
  _pthread_static.tib = tib;
  _pthread_static.pt_flags = PT_STATIC;
  _pthread_static.pt_locale = &__global_locale;
  _pthread_static.pt_attr.__stackaddr = __maps.stack.addr;
  _pthread_static.pt_attr.__stacksize = __maps.stack.size;
  dll_init(&_pthread_static.list);
  _pthread_list = &_pthread_static.list;

  // ask the operating system to change the x86 segment register
  if (IsWindows())
    __tls_index = TlsAlloc();
  __set_tls(tib);

#ifdef __x86_64__
  // rewrite the executable tls opcodes in memory
  if (IsWindows() || IsOpenbsd() || IsNetbsd())
    __morph_tls();
#endif

  // we are now allowed to use tls
  __tls_enabled_set(true);
}
