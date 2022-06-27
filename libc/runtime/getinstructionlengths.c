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
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "third_party/xed/x86.h"

/**
 * Returns lengths of x86 ops in binary.
 *
 * The first decoded instruction is at `_ereal`. Lengths can be 1 to 15
 * bytes. Each byte in the return value is in that range, and the array
 * is NUL terminated. The returned memory is memoized, since this costs
 * some time. For example, for a 10mb Python binary, it takes 20 millis
 * so the basic idea is is you can use this output multiple times which
 * is a faster way to iterate over the binary than calling Xed.
 *
 * @return nul-terminated length array on success, or null
 */
privileged unsigned char *GetInstructionLengths(void) {
  static bool once;
  int i, n, err, len, rem;
  static unsigned char *res;
  struct XedDecodedInst xedd;
  unsigned char *p, *mem, *code;
  if (!once) {
    if ((mem = mmap(0, ROUNDUP(__privileged_addr - _ereal + 1, FRAMESIZE),
                    PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1,
                    0)) != MAP_FAILED) {
      for (p = mem, code = _ereal; code < __privileged_addr; code += len) {
        rem = __privileged_addr - code;
        xed_decoded_inst_zero_set_mode(&xedd, XED_MACHINE_MODE_LONG_64);
        err = xed_instruction_length_decode(&xedd, code, rem);
        *p++ = len = !err ? xedd.length : 1;
      }
      res = mem;
    }
    once = true;
  }
  return res;
}
