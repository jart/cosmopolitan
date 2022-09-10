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
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/fsgsbase.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/segmentation.h"
#include "libc/thread/tls.h"
#include "libc/nt/version.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"

void SetUpOnce(void) {
  __tls_enabled = false;
  ASSERT_SYS(0, 0, pledge("stdio rpath", 0));
}

void OnTrap(int sig, struct siginfo *si, void *vctx) {
  struct ucontext *ctx = vctx;
}

void TriggerSignal(void) {
  struct sigaction old;
  struct sigaction sig = {.sa_sigaction = OnTrap, .sa_flags = SA_SIGINFO};
  sched_yield();
  sigaction(SIGTRAP, &sig, &old);
  asm("int3");
  sigaction(SIGTRAP, &old, 0);
  sched_yield();
}

TEST(arch_prctl, fs) {
  if (IsLinux() || IsFreebsd() || IsNetbsd() || IsOpenbsd()) {
    uint64_t n, x;
    x = 0xdeadbeef;
    arch_prctl(ARCH_SET_FS, &x);
    ASSERT_NE(-1, arch_prctl(ARCH_GET_FS, (intptr_t)&n));
    ASSERT_EQ((intptr_t)&x, n);
    ASSERT_EQ(0xdeadbeef, fs((int64_t *)0));
    TriggerSignal();
    ASSERT_EQ(0xdeadbeef, fs((int64_t *)0));
  }
}

TEST(arch_prctl, pointerRebasingFs) {
  if (IsLinux() || IsFreebsd() || IsOpenbsd() || IsNetbsd()) {
    unsigned long s[] = {0x0706050403020100, 0x0f0e0d0c0b0a0908};
    ASSERT_EQ(0x0706050403020100, s[0]);
    ASSERT_EQ(0, arch_prctl(ARCH_SET_FS, 1));
    ASSERT_EQ(0x0807060504030201, fs(&s[0]));
    ASSERT_EQ(0, arch_prctl(ARCH_SET_FS, 2));
    ASSERT_EQ(0x0908070605040302, fs(&s[0]));
    intptr_t fs;
    ASSERT_EQ(0, arch_prctl(ARCH_GET_FS, &fs));
    ASSERT_EQ(2, fs);
  }
}

TEST(arch_prctl, gs) {
  if (IsLinux() || IsFreebsd() || IsNetbsd() || IsXnu()) {
    uint64_t n, x;
    x = 0xdeadbeef;
    arch_prctl(ARCH_SET_GS, &x);
    if (!IsXnu()) {
      ASSERT_NE(-1, arch_prctl(ARCH_GET_GS, (intptr_t)&n));
      ASSERT_EQ((intptr_t)&x, n);
    }
    ASSERT_EQ(0xdeadbeef, gs((int64_t *)0));
    TriggerSignal();
    ASSERT_EQ(0xdeadbeef, gs((int64_t *)0));
  }
}

TEST(arch_prctl, pointerRebasing) {
  if (IsLinux() || IsFreebsd() || IsNetbsd() || IsXnu()) {
    unsigned long s[] = {0x0706050403020100, 0x0f0e0d0c0b0a0908};
    ASSERT_EQ(0x0706050403020100, s[0]);
    ASSERT_EQ(0, arch_prctl(ARCH_SET_GS, 1));
    ASSERT_EQ(0x0807060504030201, gs(&s[0]));
    ASSERT_EQ(0, arch_prctl(ARCH_SET_GS, 2));
    ASSERT_EQ(0x0908070605040302, gs(&s[0]));
    if (!IsXnu()) {
      intptr_t gs;
      ASSERT_EQ(0, arch_prctl(ARCH_GET_GS, &gs));
      ASSERT_EQ(2, gs);
    }
  }
}

TEST(fsgsbase, fs) {
  if (!_have_fsgsbase()) return;
  int64_t mem = 0xdeadbeef;
  _wrfsbase(&mem);
  ASSERT_EQ(&mem, _rdfsbase());
  ASSERT_EQ(0xdeadbeef, fs((int64_t *)0));
  TriggerSignal();
  ASSERT_EQ(0xdeadbeef, fs((int64_t *)0));
}

TEST(fsgsbase, gs) {
  if (!_have_fsgsbase()) return;
  int64_t mem = 0xdeadbeef;
  _wrgsbase(&mem);
  ASSERT_EQ(&mem, _rdgsbase());
  ASSERT_EQ(0xdeadbeef, gs((int64_t *)0));
  TriggerSignal();
  ASSERT_EQ(0xdeadbeef, gs((int64_t *)0));
}
