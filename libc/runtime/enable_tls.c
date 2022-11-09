/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/assert.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/asancodes.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/weaken.h"
#include "libc/log/libfatal.internal.h"
#include "libc/macros.internal.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

#define _TLSZ ((intptr_t)_tls_size)
#define _TLDZ ((intptr_t)_tdata_size)
#define _TIBZ sizeof(struct CosmoTib)

extern unsigned char __tls_mov_nt_rax[];
extern unsigned char __tls_add_nt_rax[];

nsync_dll_list_ _pthread_list;
pthread_spinlock_t _pthread_lock;
static struct PosixThread _pthread_main;
_Alignas(TLS_ALIGNMENT) static char __static_tls[5008];

/**
 * Enables thread local storage for main process.
 *
 *                              %fs Linux/BSDs
 *                               │
 *            _Thread_local      │ __get_tls()
 *     ┌───┬──────────┬──────────┼───┐
 *     │pad│  .tdata  │  .tbss   │tib│
 *     └───┴──────────┴──────────┼───┘
 *                               │
 *                  Windows/Mac %gs
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
 *       __tls_enabled = false;
 *       // do stuff
 *     }
 *
 * This is useful if you want to wrestle back control of %fs using the
 * arch_prctl() function. However, such programs might not be portable
 * and your `errno` variable also won't be thread safe anymore.
 */
void __enable_tls(void) {
  int tid;
  size_t siz;
  struct CosmoTib *tib;
  char *mem, *tls;

  siz = ROUNDUP(_TLSZ + _TIBZ, _Alignof(__static_tls));
  if (siz <= sizeof(__static_tls)) {
    // if tls requirement is small then use the static tls block
    // which helps avoid a system call for appes with little tls
    // this is crucial to keeping life.com 16 kilobytes in size!
    mem = __static_tls;
  } else {
    // if this binary needs a hefty tls block then we'll bank on
    // malloc() being linked, which links _mapanon().  otherwise
    // if you exceed this, you need to STATIC_YOINK("_mapanon").
    // please note that it's probably too early to call calloc()
    _npassert(_weaken(_mapanon));
    siz = ROUNDUP(siz, FRAMESIZE);
    mem = _weaken(_mapanon)(siz);
    _npassert(mem);
  }

  if (IsAsan()) {
    // poison the space between .tdata and .tbss
    __asan_poison(mem + (intptr_t)_tdata_size,
                  (intptr_t)_tbss_offset - (intptr_t)_tdata_size,
                  kAsanProtected);
  }

  // initialize main thread tls memory
  tib = (struct CosmoTib *)(mem + siz - _TIBZ);
  tls = mem + siz - _TIBZ - _TLSZ;
  tib->tib_self = tib;
  tib->tib_self2 = tib;
  tib->tib_errno = __errno;
  tib->tib_strace = __strace;
  tib->tib_ftrace = __ftrace;
  tib->tib_pthread = (pthread_t)&_pthread_main;
  if (IsLinux()) {
    // gnu/systemd guarantees pid==tid for the main thread so we can
    // avoid issuing a superfluous system call at startup in program
    tid = __pid;
  } else {
    tid = sys_gettid();
  }
  atomic_store_explicit(&tib->tib_tid, tid, memory_order_relaxed);

  // initialize posix threads
  _pthread_main.tib = tib;
  _pthread_main.flags = PT_STATIC;
  _pthread_main.list.prev = _pthread_main.list.next =  //
      _pthread_list = VEIL("r", &_pthread_main.list);
  _pthread_main.list.container = &_pthread_main;
  atomic_store_explicit(&_pthread_main.ptid, tid, memory_order_relaxed);

  // copy in initialized data section
  __repmovsb(tls, _tdata_start, _TLDZ);

  // ask the operating system to change the x86 segment register
  __set_tls(tib);

  // rewrite the executable tls opcodes in memory
  __morph_tls();

  // we are now allowed to use tls
  __tls_enabled = true;
}
