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
#include "libc/calls/calls.h"
#include "libc/calls/pledge.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/kprintf.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"

void CheckPlatform(void) {
  if (IsOpenbsd()) return;                       // openbsd is ok
  if (IsLinux() && __is_linux_2_6_23()) return;  // non-ancient linux is ok
  kprintf("skipping openbsd_test\n");
  exit(0);
}

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

void SetUp(void) {
  CheckPlatform();
}

TEST(pledge, promisedSyscallsCanBeCalled_ieEnosysIsIgnored) {
  SPAWN(fork);
  EXPECT_SYS(0, 0, pledge("stdio", 0));
  EXPECT_SYS(0, 0, read(0, 0, 0));
  EXITS(0);
}

TEST(unveil, pathBecomeVisible_ieEnosysIsIgnored) {
  SPAWN(fork);
  EXPECT_SYS(0, 0, touch("foo", 0644));
  EXPECT_SYS(0, 0, unveil("foo", "r"));
  EXPECT_SYS(0, 0, unveil(0, 0));
  EXPECT_SYS(0, 3, open("foo", O_RDONLY));
  EXITS(0);
}
