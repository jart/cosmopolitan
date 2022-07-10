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
#include "libc/bits/bits.h"
#include "libc/calls/calls.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/threaded.h"
#include "libc/nt/thread.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/nrlinux.h"
#include "libc/thread/thread.h"
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
#define _TIBZ sizeof(struct cthread_descriptor_t)

extern int __threadcalls_end[];
extern int __threadcalls_start[];
extern unsigned char __get_tls_nt_rax[];
__msabi extern typeof(TlsAlloc) *const __imp_TlsAlloc;

privileged void __enable_tls(void) {
  assert(!__threaded);
  assert(!__tls_enabled);

  // allocate tls memory for main process
  //
  //                              %fs Linux/BSDs
  //                               │
  //            _Thread_local      │ __get_tls()
  //     ┌───┬──────────┬──────────┼───┐
  //     │pad│  .tdata  │  .tbss   │tib│
  //     └───┴──────────┴──────────┼───┘
  //                               │
  //                  Windows/Mac %gs
  //
  size_t siz;
  cthread_t tib;
  char *mem, *tls;
  siz = ROUNDUP(_TLSZ + _TIBZ, FRAMESIZE);
  mem = _mapanon(siz);
  tib = (cthread_t)(mem + siz - _TIBZ);
  tls = mem + siz - _TIBZ - _TLSZ;
  tib->self = tib;
  tib->self2 = tib;
  tib->err = __errno;
  tib->tid = sys_gettid();
  memmove(tls, _tdata_start, _TLDZ);

  // ask the operating system to change the x86 segment register
  int ax, dx;
  if (IsWindows()) {
    __tls_index = __imp_TlsAlloc();
    asm("mov\t%1,%%gs:%0" : "=m"(*((long *)0x1480 + __tls_index)) : "r"(tib));
  } else if (IsFreebsd()) {
    asm volatile("syscall"
                 : "=a"(ax)
                 : "0"(__NR_sysarch), "D"(AMD64_SET_FSBASE), "S"(tib)
                 : "rcx", "r11", "memory", "cc");
  } else if (IsNetbsd()) {
    asm volatile("syscall"
                 : "=a"(ax), "=d"(dx)
                 : "0"(__NR_sysarch), "D"(X86_SET_FSBASE), "S"(tib)
                 : "rcx", "r11", "memory", "cc");
  } else if (IsXnu()) {
    asm volatile("syscall"
                 : "=a"(ax)
                 : "0"(__NR_thread_fast_set_cthread_self),
                   "D"((intptr_t)tib - 0x30)
                 : "rcx", "r11", "memory", "cc");
  } else if (IsOpenbsd()) {
    asm volatile("syscall"
                 : "=a"(ax)
                 : "0"(__NR___set_tcb), "D"(tib)
                 : "rcx", "r11", "memory", "cc");
  } else {
    asm volatile("syscall"
                 : "=a"(ax)
                 : "0"(__NR_linux_arch_prctl), "D"(ARCH_SET_FS), "S"(tib)
                 : "rcx", "r11", "memory");
  }

  /*
   * We need to rewrite SysV _Thread_local code. You MUST use the
   * -mno-tls-direct-seg-refs flag which generates code like this
   *
   *     64 48 8b 0R4 25 00 00 00 00   mov %fs:0,%R
   *
   * Which on Mac we can replace with this:
   *
   *     65 48 8b 0R4 25 30 00 00 00   mov %gs:0x30,%R
   *
   * Whereas on Windows we'll replace it with this:
   *
   *     0f 1f 40 00     fatnop4
   *     e8 xx xx xx xx  call __get_tls_nt_%R
   *
   * Since we have no idea where the TLS instructions exist in the
   * binary, we need to disassemble the whole program image. This'll
   * potentially take a few milliseconds for some larger programs.
   *
   * TODO(jart): compute probability this is just overkill
   */
  if (IsWindows() || IsXnu()) {
    int n, reg, dis;
    unsigned char *p;
    struct XedDecodedInst xedd;
    __morph_begin();

    // The most expensive part of this process is we need to compute the
    // byte length of each instruction in our program. We'll use Intel's
    // disassembler for this purpose.
    for (p = _ereal; p < __privileged_start; p += n) {
      xed_decoded_inst_zero_set_mode(&xedd, XED_MACHINE_MODE_LONG_64);
      if (!xed_instruction_length_decode(&xedd, p, 15)) {

        // We now know p[0] is most likely the first byte of an x86 op.
        // Let's check and see if it's the GCC linear TIB address load.
        // We hope and pray GCC won't generate TLS stores to %r8..%r15.
        if (xedd.length == 9 &&       //
            0144 == p[0] &&           // fs
            0110 == p[1] &&           // rex.w (64-bit operand size)
            0213 == p[2] &&           // mov reg/mem → reg (word-sized)
            0004 == (p[3] & 0307) &&  // mod/rm (4,reg,0) means sib → reg
            0045 == p[4] &&           // sib (5,4,0) → (rbp,rsp,0) → disp32
            0000 == p[5] &&           // displacement (von Neumann endian)
            0000 == p[6] &&           // displacement
            0000 == p[7] &&           // displacement
            0000 == p[8]) {           // displacement

          // Apple is quite straightforward to patch. We basically
          // just change the segment register, and the linear slot
          if (IsXnu()) {
            p[0] = 0145;  // this changes gs segment to fs segment
            p[5] = 0x30;  // tib slot index for tib linear address
          }

          // Windows is kind of complicated. We need to replace the
          // segment mov instruction with a function call, that (a)
          // won't clobber registers, and (b) has a return register
          // that's the same as the mov destination. When setting
          // function displacement, &CALL+5+DISP must equal &FUNC.
          else {
            reg = (p[3] & 070) >> 3;
            dis = (__get_tls_nt_rax + reg * 18) - (p + 9);
            p[0] = 0017;                       // map1
            p[1] = 0037;                       // nopl (onl if reg=0)
            p[2] = 0100;                       // mod/rm (%rax)+disp8
            p[3] = 0000;                       // displacement
            p[4] = 0350;                       // call
            p[5] = (dis & 0x000000ff) >> 000;  // displacement
            p[6] = (dis & 0x0000ff00) >> 010;  // displacement
            p[7] = (dis & 0x00ff0000) >> 020;  // displacement
            p[8] = (dis & 0xff000000) >> 030;  // displacement
          }
        }

        // Move to the next instruction.
        n = xedd.length;
      } else {
        // If Xed failed to decode the instruction, then we'll just plow
        // through memory one byte at a time until Xed's morale improves
        n = 1;
      }
    }

    __morph_end();
  }

  // we are now allowed to use tls
  __tls_enabled = true;
}

privileged void __enable_threads(void) {
  assert(!__threaded);
  __threaded = gettid();
  __morph_begin();
  /*
   * _NOPL("__threadcalls", func)
   *
   * The big ugly macro above is used by Cosmopolitan Libc to unser
   * locking primitive (e.g. flockfile, funlockfile) have zero impact on
   * performance and binary size when threads aren't actually in play.
   *
   * we have this
   *
   *     0f 1f 05 b1 19 00 00  nopl func(%rip)
   *
   * we're going to turn it into this
   *
   *     67 67 e8 b1 19 00 00  addr32 addr32 call func
   *
   * This is cheap and fast because the big ugly macro stored in the
   * binary the offsets of all the instructions we need to change.
   */
  for (int *p = __threadcalls_start; p < __threadcalls_end; ++p) {
    _base[*p + 0] = 0x67;
    _base[*p + 1] = 0x67;
    _base[*p + 2] = 0xe8;
  }
  __morph_end();
}
