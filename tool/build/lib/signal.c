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
#include "libc/calls/calls.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "tool/build/lib/bits.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/machine.h"
#include "tool/build/lib/memory.h"
#include "tool/build/lib/signal.h"
#include "tool/build/lib/xlat.h"

#define SIGCHLD_LINUX  17
#define SIGURG_LINUX   23
#define SIGWINCH_LINUX 28

void OpRestore(struct Machine *m) {
  union {
    struct fpstate_bits fp;
    struct ucontext_bits uc;
  } u;
  VirtualSendRead(m, &u.uc, m->siguc, sizeof(u.uc));
  m->ip = Read64(u.uc.rip);
  m->flags = Read64(u.uc.eflags);
  memcpy(m->r8, u.uc.r8, 8);
  memcpy(m->r9, u.uc.r9, 8);
  memcpy(m->r10, u.uc.r10, 8);
  memcpy(m->r11, u.uc.r11, 8);
  memcpy(m->r12, u.uc.r12, 8);
  memcpy(m->r13, u.uc.r13, 8);
  memcpy(m->r14, u.uc.r14, 8);
  memcpy(m->r15, u.uc.r15, 8);
  memcpy(m->di, u.uc.rdi, 8);
  memcpy(m->si, u.uc.rsi, 8);
  memcpy(m->bp, u.uc.rbp, 8);
  memcpy(m->bx, u.uc.rbx, 8);
  memcpy(m->dx, u.uc.rdx, 8);
  memcpy(m->ax, u.uc.rax, 8);
  memcpy(m->cx, u.uc.rcx, 8);
  memcpy(m->sp, u.uc.rsp, 8);
  VirtualSendRead(m, &u.fp, m->sigfp, sizeof(u.fp));
  m->fpu.cw = Read16(u.fp.cwd);
  m->fpu.sw = Read16(u.fp.swd);
  m->fpu.tw = Read16(u.fp.ftw);
  m->fpu.op = Read16(u.fp.fop);
  m->fpu.ip = Read64(u.fp.rip);
  m->fpu.dp = Read64(u.fp.rdp);
  memcpy(m->fpu.st, u.fp.st, 128);
  memcpy(m->xmm, u.fp.xmm, 256);
  m->sig = 0;
}

int DeliverSignal(struct Machine *m, int sig, int code) {
  uint64_t sp, siaddr;
  static struct siginfo_bits si;
  static struct fpstate_bits fp;
  static struct ucontext_bits uc;
  switch (Read64(m->sighand[sig - 1].handler)) {
    case 1:  // SIG_IGN
      return 0;
    case 0:  // SIG_DFL
      if (sig == SIGCHLD_LINUX || sig == SIGURG_LINUX ||
          sig == SIGWINCH_LINUX) {
        return 0;
      }
      raise(sig);
      _exit(128 + sig);
    default:
      break;
  }
  Write32(si.si_signo, sig);
  Write32(si.si_code, code);
  memcpy(uc.r8, m->r8, 8);
  memcpy(uc.r9, m->r9, 8);
  memcpy(uc.r10, m->r10, 8);
  memcpy(uc.r11, m->r11, 8);
  memcpy(uc.r12, m->r12, 8);
  memcpy(uc.r13, m->r13, 8);
  memcpy(uc.r14, m->r14, 8);
  memcpy(uc.r15, m->r15, 8);
  memcpy(uc.rdi, m->di, 8);
  memcpy(uc.rsi, m->si, 8);
  memcpy(uc.rbp, m->bp, 8);
  memcpy(uc.rbx, m->bx, 8);
  memcpy(uc.rdx, m->dx, 8);
  memcpy(uc.rax, m->ax, 8);
  memcpy(uc.rcx, m->cx, 8);
  memcpy(uc.rsp, m->sp, 8);
  Write64(uc.rip, m->ip);
  Write64(uc.eflags, m->flags);
  Write16(fp.cwd, m->fpu.cw);
  Write16(fp.swd, m->fpu.sw);
  Write16(fp.ftw, m->fpu.tw);
  Write16(fp.fop, m->fpu.op);
  Write64(fp.rip, m->fpu.ip);
  Write64(fp.rdp, m->fpu.dp);
  memcpy(fp.st, m->fpu.st, 128);
  memcpy(fp.xmm, m->xmm, 256);
  sp = Read64(m->sp);
  sp = ROUNDDOWN(sp - sizeof(si), 16);
  VirtualRecvWrite(m, sp, &si, sizeof(si));
  siaddr = sp;
  sp = ROUNDDOWN(sp - sizeof(fp), 16);
  VirtualRecvWrite(m, sp, &fp, sizeof(fp));
  m->sigfp = sp;
  Write64(uc.fpstate, sp);
  sp = ROUNDDOWN(sp - sizeof(uc), 16);
  VirtualRecvWrite(m, sp, &uc, sizeof(uc));
  m->siguc = sp;
  m->sig = sig;
  sp -= 8;
  VirtualRecvWrite(m, sp, m->sighand[sig - 1].restorer, 8);
  Write64(m->sp, sp);
  Write64(m->di, sig);
  Write64(m->si, siaddr);
  Write64(m->dx, m->siguc);
  m->ip = Read64(m->sighand[sig - 1].handler);
  return 0;
}

void EnqueueSignal(struct Machine *m, int sig, int code) {
  if (m->signals.n < ARRAYLEN(m->signals.p)) {
    m->signals.p[m->signals.n].code = UnXlatSicode(sig, code);
    m->signals.p[m->signals.n].sig = UnXlatSignal(sig);
    m->signals.n++;
  }
}

int ConsumeSignal(struct Machine *m) {
  int sig, code;
  sig = m->signals.p[m->signals.i].sig;
  code = m->signals.p[m->signals.i].code;
  if (!m->sig ||
      ((sig != m->sig || (Read64(m->sighand[m->sig - 1].flags) & 0x40000000)) &&
       !(Read64(m->sighand[m->sig - 1].mask) & (1ull << (m->sig - 1))))) {
    if (++m->signals.i == m->signals.n) m->signals.i = m->signals.n = 0;
    return DeliverSignal(m, sig, code);
  }
  return 0;
}

void TerminateSignal(struct Machine *m, int sig) {
  if (m->isfork) {
    _exit(28 + sig);
  } else {
    exit(128 + sig);
  }
}
