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
#include "libc/dce.h"
#include "libc/mem/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

TEST(ftrace, test) {
  if (1) {
    // TODO(jart)
    return;
  }
  const char *ftraceasm;
  testlib_extract("/zip/ftraceasm.txt", "ftraceasm.txt", 0755);
  ftraceasm = gc(xslurp("ftraceasm.txt", 0));
#ifdef __x86_64__
  if (strstr(ftraceasm, "%xmm") ||  //
      strstr(ftraceasm, "%ymm") ||  //
      strstr(ftraceasm, "%zmm")) {
#elif defined(__aarch64__)
  if (strstr(ftraceasm, "\td0,") ||  //
      strstr(ftraceasm, "\tv0.") ||  //
      strstr(ftraceasm, "\tq0.") ||  //
      strstr(ftraceasm, "\td0,") ||  //
      strstr(ftraceasm, "\tv0,") ||  //
      strstr(ftraceasm, "\tq0,")) {
#else
  if (0) {
#endif
    fprintf(stderr,
            "error: ftrace_hook() depends on floating point code\n"
            "you need to objdump -d o//test/libc/runtime/prog/ftraceasm.elf\n"
            "then compile the guilty module with -mgeneral-regs-only\n");
    exit(1);
  }
}
