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
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/ucontext.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "third_party/xed/x86.h"

#define OUTPATH "vdso.elf"

volatile bool finished;

void OnSegmentationFault(int sig, siginfo_t *si, void *vctx) {
  struct XedDecodedInst xedd;
  ucontext_t *ctx = vctx;
  xed_decoded_inst_zero_set_mode(&xedd, XED_MACHINE_MODE_LONG_64);
  xed_instruction_length_decode(&xedd, (void *)ctx->uc_mcontext.rip, 15);
  ctx->uc_mcontext.rip += xedd.length;
  finished = true;
}

int main(int argc, char *argv[]) {
  FILE *f;
  int byte;
  volatile unsigned char *vdso, *p;

  vdso = (unsigned char *)getauxval(AT_SYSINFO_EHDR);
  if (vdso) {
    fprintf(stderr, "vdso found at address %p\n", vdso);
  } else {
    fprintf(stderr, "error: AT_SYSINFO_EHDR was not in auxiliary values\n");
    return 1;
  }

  f = fopen(OUTPATH, "wb");
  if (!f) {
    fprintf(stderr, "error: fopen(%`'s) failed\n", OUTPATH);
    return 1;
  }

  struct sigaction sa = {
      .sa_sigaction = OnSegmentationFault,
      .sa_flags = SA_SIGINFO,
  };
  sigaction(SIGSEGV, &sa, 0);
  sigaction(SIGBUS, &sa, 0);

  p = vdso;
  for (;;) {
    byte = *p++;
    if (!finished) {
      fputc(byte, f);
    } else {
      break;
    }
  }

  fclose(f);
  fprintf(stderr, "%zu bytes dumped to %s\n", p - vdso, OUTPATH);

  return 0;
}
