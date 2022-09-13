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
#include "libc/calls/calls.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/asancodes.h"
#include "libc/intrin/bits.h"
#include "libc/intrin/weaken.h"
#include "libc/log/libfatal.internal.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/msr.h"
#include "libc/nt/thread.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdalign.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/nrlinux.h"
#include "libc/thread/tls.h"
#include "third_party/xed/x86.h"

#define __NR_sysarch     0x000000a5  // freebsd+netbsd
#define AMD64_SET_GSBASE 131         // freebsd
#define AMD64_SET_FSBASE 129         // freebsd
#define X86_SET_GSBASE   16          // netbsd
#define X86_SET_FSBASE   17          // netbsd

#define __NR___set_tcb                    0x00000149
#define __NR__lwp_setprivate              0x0000013d
#define __NR_thread_fast_set_cthread_self 0x03000003

#define _TLSZ ((intptr_t)_tls_size)
#define _TLDZ ((intptr_t)_tdata_size)
#define _TIBZ sizeof(struct CosmoTib)

int sys_enable_tls();

typedef char xmm_t __attribute__((__vector_size__(16), __aligned__(1)));

__msabi extern typeof(TlsAlloc) *const __imp_TlsAlloc;

extern unsigned char __tls_mov_nt_rax[];
extern unsigned char __tls_add_nt_rax[];
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
privileged void __enable_tls(void) {
  size_t siz;
  struct CosmoTib *tib;
  char *mem, *tls;
  siz = ROUNDUP(_TLSZ + _TIBZ, alignof(__static_tls));
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
    assert(_weaken(_mapanon));
    siz = ROUNDUP(siz, FRAMESIZE);
    mem = _weaken(_mapanon)(siz);
    assert(mem);
  }
  if (IsAsan()) {
    // poison the space between .tdata and .tbss
    __asan_poison(mem + (intptr_t)_tdata_size,
                  (intptr_t)_tbss_offset - (intptr_t)_tdata_size,
                  kAsanProtected);
  }
  tib = (struct CosmoTib *)(mem + siz - _TIBZ);
  tls = mem + siz - _TIBZ - _TLSZ;
  tib->tib_self = tib;
  tib->tib_self2 = tib;
  tib->tib_errno = __errno;
  if (IsLinux()) {
    // gnu/systemd guarantees pid==tid for the main thread so we can
    // avoid issuing a superfluous system call at startup in program
    tib->tib_tid = __pid;
  } else {
    tib->tib_tid = sys_gettid();
  }
  __repmovsb(tls, _tdata_start, _TLDZ);

  // ask the operating system to change the x86 segment register
  int ax, dx;
  if (IsWindows()) {
    __tls_index = __imp_TlsAlloc();
    assert(0 <= __tls_index && __tls_index < 64);
    asm("mov\t%1,%%gs:%0" : "=m"(*((long *)0x1480 + __tls_index)) : "r"(tib));
  } else if (IsFreebsd()) {
    sys_enable_tls(AMD64_SET_FSBASE, tib);
  } else if (IsLinux()) {
    sys_enable_tls(ARCH_SET_FS, tib);
  } else if (IsNetbsd()) {
    // netbsd has sysarch(X86_SET_FSBASE) but we can't use that because
    // signal handlers will cause it to be reset due to not setting the
    // _mc_tlsbase field in struct mcontext_netbsd.
    sys_enable_tls(tib);
  } else if (IsOpenbsd()) {
    sys_enable_tls(tib);
  } else if (IsXnu()) {
    // thread_fast_set_cthread_self has a weird ABI
    int e = errno;
    sys_enable_tls((intptr_t)tib - 0x30);
    errno = e;
  } else {
    uint64_t val = (uint64_t)tib;
    asm volatile("wrmsr"
                 : /* no outputs */
                 : "c"(MSR_IA32_FS_BASE), "a"((uint32_t)val),
                   "d"((uint32_t)(val >> 32)));
  }

  // We need to rewrite SysV _Thread_local code. You MUST use the
  // -mno-tls-direct-seg-refs flag which generates code like this
  //
  //     64 48 8b 0R4 25 00 00 00 00   mov %fs:0,%R
  //     64 48 03 0R4 25 00 00 00 00   add %fs:0,%R
  //
  // Which on Mac we can replace with this:
  //
  //     65 48 8b 0R4 25 30 00 00 00   mov %gs:0x30,%R
  //
  // Since we have no idea where the TLS instructions exist in the
  // binary, we need to disassemble the whole program image. This'll
  // potentially take a few milliseconds for some larger programs.
  //
  // We check `_tls_content` which is generated by the linker script
  // since it lets us determine ahead of time if _Thread_local vars
  // have actually been linked into this program.
  if ((intptr_t)_tls_content && (IsWindows() || IsXnu())) {
    int n;
    uint64_t w;
    unsigned m, dis;
    unsigned char *p;
    __morph_begin();

    if (IsXnu()) {
      // Apple is quite straightforward to patch. We basically
      // just change the segment register, and the linear slot
      // address 0x30 was promised to us, according to Go team
      // https://github.com/golang/go/issues/23617
      dis = 0x30;
    } else {
      // MSVC __declspec(thread) generates binary code for this
      // %gs:0x1480 abi. So long as TlsAlloc() isn't called >64
      // times we should be good.
      dis = 0x1480 + __tls_index * 8;
    }

    // iterate over modifiable code looking for 9 byte instruction
    // this would take 30 ms using xed to enable tls on python.com
    for (p = _ereal; p + 9 <= __privileged_start; p += n) {

      // use sse to zoom zoom to fs register prefixes
      // that way it'll take 1 ms to morph python.com
      while (p + 9 + 16 <= __privileged_start) {
        if ((m = __builtin_ia32_pmovmskb128(
                 *(xmm_t *)p == (xmm_t){0144, 0144, 0144, 0144, 0144, 0144,
                                        0144, 0144, 0144, 0144, 0144, 0144,
                                        0144, 0144, 0144, 0144}))) {
          m = __builtin_ctzll(m);
          p += m;
          break;
        } else {
          p += 16;
        }
      }

      // we're checking for the following expression:
      //   0144 == p[0] &&           // %fs
      //   0110 == (p[1] & 0373) &&  // rex.w (and ignore rex.r)
      //   (0213 == p[2] ||          // mov reg/mem → reg (word-sized)
      //   0003 == p[2]) &&          // add reg/mem → reg (word-sized)
      //   0004 == (p[3] & 0307) &&  // mod/rm (4,reg,0) means sib → reg
      //   0045 == p[4] &&           // sib (5,4,0) → (rbp,rsp,0) → disp32
      //   0000 == p[5] &&           // displacement (von Neumann endian)
      //   0000 == p[6] &&           // displacement
      //   0000 == p[7] &&           // displacement
      //   0000 == p[8]              // displacement
      w = READ64LE(p) & READ64LE("\377\373\377\307\377\377\377\377");
      if ((w == READ64LE("\144\110\213\004\045\000\000\000") ||
           w == READ64LE("\144\110\003\004\045\000\000\000")) &&
          !p[8]) {

        // now change the code
        p[0] = 0145;                       // change %fs to %gs
        p[5] = (dis & 0x000000ff) >> 000;  // displacement
        p[6] = (dis & 0x0000ff00) >> 010;  // displacement
        p[7] = (dis & 0x00ff0000) >> 020;  // displacement
        p[8] = (dis & 0xff000000) >> 030;  // displacement

        // advance to the next instruction
        n = 9;
      } else {
        n = 1;
      }
    }

    __morph_end();
  }

  // we are now allowed to use tls
  __tls_enabled = true;
}
