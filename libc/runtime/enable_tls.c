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
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/asancodes.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/getenv.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/macros.internal.h"
#include "libc/nt/files.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/str/locale.h"
#include "libc/str/str.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#include "third_party/make/gnumake.h"

#define I(x) ((uintptr_t)x)

extern unsigned char __tls_mov_nt_rax[];
extern unsigned char __tls_add_nt_rax[];

_Alignas(TLS_ALIGNMENT) static char __static_tls[6016];

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
 *                              %fs Linux/BSDs
 *            _Thread_local      │
 *     ┌───┬──────────┬──────────┼───┐
 *     │pad│  .tdata  │  .tbss   │tib│
 *     └───┴──────────┴──────────┼───┘
 *                               │
 *                  Windows/Mac %gs
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
  int tid;
  size_t siz;
  char *mem, *tls;
  struct CosmoTib *tib;

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

  siz = ROUNDUP(I(_tls_size) + sizeof(*tib), TLS_ALIGNMENT);
  if (siz <= sizeof(__static_tls)) {
    // if tls requirement is small then use the static tls block
    // which helps avoid a system call for appes with little tls
    // this is crucial to keeping life.com 16 kilobytes in size!
    mem = __static_tls;
  } else {
    // if this binary needs a hefty tls block then we'll bank on
    // malloc() being linked, which links _mapanon().  otherwise
    // if you exceed this, you need to __static_yoink("_mapanon").
    // please note that it's probably too early to call calloc()
    mem = _weaken(_mapanon)(siz);
  }

  if (IsAsan()) {
    // poison the space between .tdata and .tbss
    __asan_poison(mem + I(_tdata_size), I(_tbss_offset) - I(_tdata_size),
                  kAsanProtected);
  }

  tib = (struct CosmoTib *)(mem + siz - sizeof(*tib));
  tls = mem + siz - sizeof(*tib) - I(_tls_size);

#elif defined(__aarch64__)

  size_t hiz = ROUNDUP(sizeof(*tib) + 2 * sizeof(void *), I(_tls_align));
  siz = hiz + I(_tls_size);
  if (siz <= sizeof(__static_tls)) {
    mem = __static_tls;
  } else {
    mem = _weaken(_mapanon)(siz);
  }

  if (IsAsan()) {
    // there's a roundup(pagesize) gap between .tdata and .tbss
    // poison that empty space
    __asan_poison(mem + hiz + I(_tdata_size), I(_tbss_offset) - I(_tdata_size),
                  kAsanProtected);
  }

  tib = (struct CosmoTib *)mem;
  tls = mem + hiz;

  // Set the DTV.
  //
  // We don't support dynamic shared objects at the moment. The APE
  // linker script will only produce a single PT_TLS program header
  // therefore our job is relatively simple.
  //
  // @see musl/src/env/__init_tls.c
  // @see https://chao-tic.github.io/blog/2018/12/25/tls
  ((uintptr_t *)tls)[-2] = 1;
  ((void **)tls)[-1] = tls;

#else
#error "unsupported architecture"
#endif /* __x86_64__ */

  // initialize main thread tls memory
  tib->tib_self = tib;
  tib->tib_self2 = tib;
  tib->tib_errno = __errno;
  tib->tib_strace = __strace;
  tib->tib_ftrace = __ftrace;
  tib->tib_locale = (intptr_t)&__c_dot_utf8_locale;
  tib->tib_pthread = (pthread_t)&_pthread_static;
  if (IsWindows()) {
    intptr_t hThread;
    DuplicateHandle(GetCurrentProcess(), GetCurrentThread(),
                    GetCurrentProcess(), &hThread, 0, false,
                    kNtDuplicateSameAccess);
    atomic_store_explicit(&tib->tib_syshand, hThread, memory_order_relaxed);
  } else if (IsXnuSilicon()) {
    tib->tib_syshand = __syslib->__pthread_self();
  }
  if (IsLinux() || IsXnuSilicon()) {
    // gnu/systemd guarantees pid==tid for the main thread so we can
    // avoid issuing a superfluous system call at startup in program
    tid = __pid;
  } else {
    tid = sys_gettid();
  }
  atomic_store_explicit(&tib->tib_tid, tid, memory_order_relaxed);
  // TODO(jart): set_tid_address?

  // inherit signal mask
  if (IsWindows()) {
    atomic_store_explicit(&tib->tib_sigmask,
                          ParseMask(__getenv(environ, "_MASK").s),
                          memory_order_relaxed);
  }

  // initialize posix threads
  _pthread_static.tib = tib;
  _pthread_static.pt_flags = PT_STATIC;
  dll_init(&_pthread_static.list);
  _pthread_list = &_pthread_static.list;
  atomic_store_explicit(&_pthread_static.ptid, tid, memory_order_relaxed);

  // copy in initialized data section
  if (I(_tdata_size)) {
    if (IsAsan()) {
      __asan_memcpy(tls, _tdata_start, I(_tdata_size));
    } else {
      memcpy(tls, _tdata_start, I(_tdata_size));
    }
  }

  // ask the operating system to change the x86 segment register
  __set_tls(tib);

#ifdef __x86_64__
  // rewrite the executable tls opcodes in memory
  if (IsWindows() || IsXnu()) {
    __morph_tls();
  }
#endif

  // we are now allowed to use tls
  __tls_enabled_set(true);
}
