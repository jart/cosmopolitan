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
#include "libc/bits/segmentation.h"
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/testlib/testlib.h"

TEST(arch_prctl, fs) {
  if (IsLinux() || IsOpenbsd()) {
    uint64_t n, x;
    x = 0xdeadbeef;
    arch_prctl(ARCH_SET_FS, &x);
    ASSERT_NE(-1, arch_prctl(ARCH_GET_FS, (intptr_t)&n));
    ASSERT_EQ((intptr_t)&x, n);
    ASSERT_EQ(0xdeadbeef, fs((int64_t *)0));
  }
}

TEST(arch_prctl, pointerRebasingFs) {
  if (IsLinux() || IsOpenbsd()) {
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
  if (IsLinux()) {
    uint64_t n, x;
    x = 0xdeadbeef;
    arch_prctl(ARCH_SET_GS, &x);
    ASSERT_NE(-1, arch_prctl(ARCH_GET_GS, (intptr_t)&n));
    ASSERT_EQ((intptr_t)&x, n);
    ASSERT_EQ(0xdeadbeef, gs((int64_t *)0));
  }
}

TEST(arch_prctl, pointerRebasing) {
  if (IsLinux()) {
    unsigned long s[] = {0x0706050403020100, 0x0f0e0d0c0b0a0908};
    ASSERT_EQ(0x0706050403020100, s[0]);
    ASSERT_EQ(0, arch_prctl(ARCH_SET_GS, 1));
    ASSERT_EQ(0x0807060504030201, gs(&s[0]));
    ASSERT_EQ(0, arch_prctl(ARCH_SET_GS, 2));
    ASSERT_EQ(0x0908070605040302, gs(&s[0]));
    intptr_t gs;
    ASSERT_EQ(0, arch_prctl(ARCH_GET_GS, &gs));
    ASSERT_EQ(2, gs);
  }
}
