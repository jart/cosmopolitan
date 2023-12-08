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
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/ucontext.h"
#include "libc/dce.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/sysconf.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"
#include "third_party/xed/x86.h"

volatile int gotsig;
volatile int gotcode;

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

void SetUp(void) {
  gotsig = 0;
}

void OnSig(int sig, siginfo_t *si, void *arg) {
  ucontext_t *ctx = arg;
  gotsig = sig;
  gotcode = si->si_code;
#ifdef __aarch64__
  ctx->uc_mcontext.pc += 4;
#elif defined(__x86_64__)
  struct XedDecodedInst xedd;
  xed_decoded_inst_zero_set_mode(&xedd, XED_MACHINE_MODE_LONG_64);
  xed_instruction_length_decode(&xedd, (void *)ctx->uc_mcontext.rip, 15);
  ctx->uc_mcontext.rip += xedd.length;
#else
#error "unsupported architecture"
#endif /* __x86_64__ */
}

TEST(sigbus, test) {

  if (IsWindows()) {
    // TODO(jart): Just write a kernel style memory manager from scratch
    //             for WIN32 and don't depend on their mapping functions
    return;
  }

  // create a file with less than one page of content
  ASSERT_SYS(0, 3, open("foo", O_CREAT | O_RDWR, 0666));
  ASSERT_SYS(0, 2, write(3, "hi", 2));

  // map two pages of the file into memory
  char *map;
  long pagesz = getauxval(AT_PAGESZ);
  ASSERT_NE(MAP_FAILED,
            (map = mmap(0, pagesz * 2, PROT_READ, MAP_PRIVATE, 3, 0)));

  // verify the file content is there
  ASSERT_EQ('h', map[0]);
  ASSERT_EQ('i', map[1]);

  // memory reads past file eof on first page get zero-filled
  ASSERT_EQ(0, map[2]);

  // listen for sigbus
  struct sigaction sa;
  struct sigaction oldsa;
  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = OnSig;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGBUS, &sa, &oldsa);

  // memory reads past file eof on subsequent pages cause page fault to sigbus
  volatile char x = map[pagesz];
  (void)x;

  // verify the signal was raised
  EXPECT_EQ(SIGBUS, gotsig);
  if (IsXnuSilicon()) {
    // TODO: Why does it say it's an alignment error?
    EXPECT_EQ(BUS_ADRALN, gotcode);
  } else {
    EXPECT_EQ(BUS_ADRERR, gotcode);
  }

  // clean up
  sigaction(SIGBUS, &oldsa, 0);
  munmap(map, pagesz * 2);
}
